import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { CommonModule } from '@angular/common';
import { CamerasService } from './cameras.service';
import { HttpClientModule } from '@angular/common/http';

interface CanvasItem {
  type: string;
  label: string;
  url?: string;
  id?: string; // Nuevo campo para el ID de la cámara
  x?: number;
  y?: number;
}

interface LayoutConfig {
  columns: number;
  rows: number;
  forceHorizontalOnMobile: boolean;
}

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet, CommonModule, HttpClientModule],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  title = 'cctv-frontend';
  showEditModal = false;
  showLayoutModal = false;
  canvasItems: CanvasItem[] = [];
  editingIndex: number | null = null;
  activeCameras: string[] = [];
  camerasLoading = false;
  camerasError = '';
  
  // Configuración del layout
  layoutConfig: LayoutConfig = {
    columns: 4,
    rows: 3,
    forceHorizontalOnMobile: true
  };

  constructor(private camerasService: CamerasService) {
    this.loadFromLocalStorage();
    this.fetchActiveCameras();
  }

  fetchActiveCameras() {
    this.camerasLoading = true;
    this.camerasError = '';
    this.camerasService.getActiveCameras().subscribe({
      next: (resp: any) => {
        const ids = Array.isArray(resp) ? resp : (resp.cameras || []);
        if (ids.length === 0) {
          this.camerasError = 'No se pudo cargar la lista de cámaras.';
        }
        this.activeCameras = ids;
        this.camerasLoading = false;
      },
      error: (err) => {
        this.camerasError = 'No se pudo cargar la lista de cámaras.';
        this.activeCameras = [];
        this.camerasLoading = false;
      }
    });
  }

  // Métodos para persistencia en localStorage
  loadFromLocalStorage() {
    try {
      const savedItems = localStorage.getItem('cctv-canvas-items');
      const savedLayout = localStorage.getItem('cctv-layout-config');
      
      if (savedItems) {
        this.canvasItems = JSON.parse(savedItems);
      }
      
      if (savedLayout) {
        this.layoutConfig = { ...this.layoutConfig, ...JSON.parse(savedLayout) };
      }
    } catch (error) {
      console.error('Error al cargar desde localStorage:', error);
    }
  }

  saveToLocalStorage() {
    try {
      localStorage.setItem('cctv-canvas-items', JSON.stringify(this.canvasItems));
      localStorage.setItem('cctv-layout-config', JSON.stringify(this.layoutConfig));
    } catch (error) {
      console.error('Error al guardar en localStorage:', error);
    }
  }

  // Drag & drop para la barra lateral
  onDragStart(event: DragEvent, type: string) {
    event.dataTransfer?.setData('type', type);
  }

  // Drag & drop para los elementos del grid
  onItemDragStart(event: DragEvent, index: number) {
    event.dataTransfer?.setData('dragIndex', index.toString());
  }

  onDrop(event: DragEvent) {
    event.preventDefault();
    const type = event.dataTransfer?.getData('type');
    const dragIndex = event.dataTransfer?.getData('dragIndex');
    if (type) {
      this.canvasItems.push({ type, label: 'Nueva cámara' });
      this.saveToLocalStorage();
    } else if (dragIndex !== null && dragIndex !== undefined) {
      const from = Number(dragIndex);
      const to = this.getDropIndex(event);
      if (from !== to && to !== -1) {
        const moved = this.canvasItems.splice(from, 1)[0];
        this.canvasItems.splice(to, 0, moved);
        this.saveToLocalStorage();
      }
    }
  }

  onDragOver(event: DragEvent) {
    event.preventDefault();
  }

  getDropIndex(event: DragEvent): number {
    // Encuentra el índice de la celda sobre la que se suelta
    const grid = document.querySelectorAll('.canvas-grid-item');
    let dropIndex = -1;
    grid.forEach((el, i) => {
      const rect = el.getBoundingClientRect();
      if (
        event.clientX >= rect.left &&
        event.clientX <= rect.right &&
        event.clientY >= rect.top &&
        event.clientY <= rect.bottom
      ) {
        dropIndex = i;
      }
    });
    return dropIndex;
  }

  // Modal de edición
  openEditModal(index: number) {
    this.editingIndex = index;
    this.showEditModal = true;
    setTimeout(() => {
      const modal = document.getElementById('editModal');
      if (modal) {
        modal.style.opacity = '1';
        modal.style.pointerEvents = 'auto';
      }
    }, 10);
  }

  closeEditModal() {
    const modal = document.getElementById('editModal');
    if (modal) {
      modal.style.opacity = '0';
      modal.style.pointerEvents = 'none';
    }
    setTimeout(() => {
      this.showEditModal = false;
      this.editingIndex = null;
      this.saveToLocalStorage();
    }, 500);
  }

  // Modal de configuración de layout
  openLayoutModal() {
    this.showLayoutModal = true;
  }

  closeLayoutModal() {
    this.showLayoutModal = false;
    this.saveToLocalStorage();
  }

  isMobile(): boolean {
    return window.innerWidth <= 768;
  }

  getGridColsClass() {
    const isMobile = this.isMobile();
    if (isMobile && !this.layoutConfig.forceHorizontalOnMobile) {
      return 'grid-cols-1';
    }
    const count = this.canvasItems.length;
    if (count === 1) return 'grid-cols-1';
    if (count === 2) return 'grid-cols-2';
    if (count === 3) return 'grid-cols-3';
    if (count >= 4) return 'grid-cols-4';
    return 'grid-cols-1';
  }

  getGridStyle() {
    const count = this.canvasItems.length;
    if (count === 0) {
      return {};
    }
    const isMobile = this.isMobile();
    
    // Si está en móvil y se fuerza horizontal, usar diseño horizontal
    const useHorizontalLayout = isMobile && this.layoutConfig.forceHorizontalOnMobile;
    
    return {
      width: '100%',
      height: '100%',
      minHeight: '300px',
      display: 'grid',
      gap: '1.5rem',
      gridTemplateColumns: useHorizontalLayout ? 
        this.getHorizontalColumns(count) : 
        this.getGridTemplateColumns(),
      gridTemplateRows: useHorizontalLayout ? 
        'minmax(200px, 1fr)' : 
        this.getGridTemplateRows(),
      overflowX: useHorizontalLayout ? 'auto' : 'visible',
      overflowY: useHorizontalLayout ? 'hidden' : 'auto'
    };
  }

  getHorizontalColumns(count: number) {
    return `repeat(${count}, minmax(250px, 1fr))`;
  }

  getGridTemplateColumns() {
    const isMobile = this.isMobile();
    if (isMobile && !this.layoutConfig.forceHorizontalOnMobile) {
      return '1fr';
    }
    return `repeat(${this.layoutConfig.columns}, 1fr)`;
  }

  getGridTemplateRows() {
    const isMobile = this.isMobile();
    if (isMobile && !this.layoutConfig.forceHorizontalOnMobile) {
      const count = this.canvasItems.length;
      return `repeat(${count}, minmax(200px, 1fr))`;
    }
    return `repeat(${this.layoutConfig.rows}, 1fr)`;
  }

  updateLabel(event: Event) {
    if (this.editingIndex !== null) {
      const input = event.target as HTMLInputElement;
      if (input) {
        this.canvasItems[this.editingIndex].label = input.value;
      }
    }
  }

  updateUrl(event: Event) {
    if (this.editingIndex !== null) {
      const input = event.target as HTMLInputElement;
      if (input) {
        this.canvasItems[this.editingIndex].url = input.value;
      }
    }
  }

  updateId(event: Event) {
    if (this.editingIndex !== null) {
      const input = event.target as HTMLInputElement;
      if (input) {
        this.canvasItems[this.editingIndex].id = input.value;
      }
    }
  }

  // Métodos para actualizar configuración del layout
  updateColumns(event: Event) {
    const input = event.target as HTMLInputElement;
    if (input) {
      this.layoutConfig.columns = Math.max(1, Math.min(8, parseInt(input.value) || 1));
    }
  }

  updateRows(event: Event) {
    const input = event.target as HTMLInputElement;
    if (input) {
      this.layoutConfig.rows = Math.max(1, Math.min(8, parseInt(input.value) || 1));
    }
  }

  toggleHorizontalMobile(event: Event) {
    const input = event.target as HTMLInputElement;
    this.layoutConfig.forceHorizontalOnMobile = input.checked;
  }

  // Eliminar cámara
  deleteCamera() {
    if (this.editingIndex !== null) {
      this.canvasItems.splice(this.editingIndex, 1);
      this.closeEditModal();
      this.saveToLocalStorage();
    }
  }

  // Blur dinámico para el canvas cuando el modal está abierto
  getCanvasBlur() {
    return (this.showEditModal || this.showLayoutModal) ? 'blur-sm pointer-events-none select-none' : '';
  }

  addCameraFromButton() {
    this.canvasItems.push({ type: 'camera', label: 'Nueva cámara', url: '' });
    this.saveToLocalStorage();
  }
}
