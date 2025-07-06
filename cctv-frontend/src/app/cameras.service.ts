import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';
import { map } from 'rxjs/operators';

@Injectable({ providedIn: 'root' })
export class CamerasService {
  constructor(private http: HttpClient) {}

  getActiveCameras(): Observable<string[]> {
    // El backend responde { cameras: [...] }
    return this.http.get<{ cameras: string[] }>('/cameras').pipe(
      map(res => res.cameras)
    );
  }
}
