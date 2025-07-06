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
  canvasItems: CanvasItem[] = [];
  editingIndex: number | null = null;
  activeCameras: string[] = [];
  camerasLoading = false;
  camerasError = '';

  constructor(private camerasService: CamerasService) {
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
    } else if (dragIndex !== null && dragIndex !== undefined) {
      const from = Number(dragIndex);
      const to = this.getDropIndex(event);
      if (from !== to && to !== -1) {
        const moved = this.canvasItems.splice(from, 1)[0];
        this.canvasItems.splice(to, 0, moved);
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
    }, 500);
  }

  isMobile(): boolean {
    return window.innerWidth <= 768;
  }

  getGridColsClass() {
    if (window.innerWidth <= 768) {
      return 'grid-cols-1';
    }
    if (this.canvasItems.length === 1) return 'grid-cols-1';
    if (this.canvasItems.length === 2) return 'grid-cols-2';
    if (this.canvasItems.length === 3) return 'grid-cols-3';
    if (this.canvasItems.length >= 4) return 'grid-cols-4';
    return 'grid-cols-1';
  }

  getGridStyle() {
    const count = this.canvasItems.length;
    if (count === 0) {
      return {};
    }
    const isMobile = window.innerWidth <= 768;
    return {
      width: '100%',
      height: '100%',
      minHeight: '300px',
      display: 'grid',
      gap: '1.5rem',
      gridTemplateColumns: isMobile ? '1fr' : this.getGridTemplateColumns(),
      gridTemplateRows: isMobile ? this.getGridTemplateRowsMobile() : this.getGridTemplateRows(),
    };
  }

  getGridTemplateColumns() {
    const count = this.canvasItems.length;
    if (count === 1) return '1fr';
    if (count === 2) return '1fr 1fr';
    if (count === 3) return '1fr 1fr 1fr';
    if (count === 4) return '1fr 1fr 1fr 1fr';
    return 'repeat(4, 1fr)';
  }

  getGridTemplateRows() {
    const count = this.canvasItems.length;
    if (count === 0) return '1fr';
    if (count <= 4) return '1fr';
    const rows = Math.ceil(count / 4);
    return `repeat(${rows}, 1fr)`;
  }

  getGridTemplateRowsMobile() {
    const count = this.canvasItems.length;
    if (count === 0) return '1fr';
    return `repeat(${count}, 1fr)`;
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

  // Eliminar cámara
  deleteCamera() {
    if (this.editingIndex !== null) {
      this.canvasItems.splice(this.editingIndex, 1);
      this.closeEditModal();
    }
  }

  // Blur dinámico para el canvas cuando el modal está abierto
  getCanvasBlur() {
    return this.showEditModal ? 'blur-sm pointer-events-none select-none' : '';
  }

  addCameraFromButton() {
    this.canvasItems.push({ type: 'camera', label: 'Nueva cámara', url: '' });
  }
}
