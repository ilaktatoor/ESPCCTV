# CctvFrontend

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 18.2.10.

## Development server

Run `ng serve` for a dev server. Navigate to `http://localhost:4200/`. The application will automatically reload if you change any of the source files.

## Code scaffolding

Run `ng generate component component-name` to generate a new component. You can also use `ng generate directive|pipe|service|class|guard|interface|enum|module`.

## Build

Run `ng build` to build the project. The build artifacts will be stored in the `dist/` directory.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via a platform of your choice. To use this command, you need to first add a package that implements end-to-end testing capabilities.

## Further help

To get more help on the Angular CLI use `ng help` or go check out the [Angular CLI Overview and Command Reference](https://angular.dev/tools/cli) page.

## Backend CORS support

The backend Flask server uses [flask-cors](https://flask-cors.readthedocs.io/) to allow cross-origin requests. This is required for correct operation when deploying frontend and backend in separate containers or domains (e.g., with Docker).

To install the dependency:

```
pip install flask-cors
```

No extra configuration is needed: CORS is enabled globally in `main.py`.

## CCTV Micro Frontend (Angular)

Este proyecto es un micro frontend moderno y responsivo para un sistema de CCTV, desarrollado en Angular + TailwindCSS. Permite:

- Drag & drop de cámaras desde una toolbar (lateral en desktop, inferior en móvil/tablet).
- Canvas principal tipo grid donde los elementos son componentes draggeables y editables.
- Modal para editar propiedades de cada cámara (nombre y selección de ID desde el backend).
- Integración con backend Python (Flask) que expone streams MJPEG por ID y lista de cámaras activas.
- Visualización en tiempo real del stream MJPEG de cada cámara seleccionada.
- Soporte completo para desktop, móvil y tablet.

### Estructura principal
- `src/app/app.component.html` — Interfaz principal, toolbar, grid y modal.
- `src/app/app.component.ts` — Lógica de drag & drop, edición y consumo de backend.
- `src/app/cameras.service.ts` — Servicio Angular para consultar la lista de cámaras activas.
- `proxy.conf.json` — Proxy para desarrollo, permite consumir el backend Flask en otro puerto sin CORS.

### Flujo de trabajo
1. El usuario arrastra una cámara al canvas o la agrega desde el botón en móvil.
2. Puede editar el nombre y seleccionar el ID de la cámara desde el modal.
3. El frontend consulta `/cameras` al backend para mostrar los IDs activos.
4. El stream MJPEG se muestra automáticamente en el grid usando el ID seleccionado.

### Requisitos
- Node.js y Angular CLI para el frontend.
- Python, Flask, flask-cors, OpenCV y numpy para el backend.

---

## Docker deployment

### Estructura de contenedores

- **frontend**: Angular + Nginx (sirve la SPA y hace proxy a la API del backend)
- **backend**: Flask (API y MJPEG streaming)

### Archivos Docker

- `Dockerfile.frontend`: Construye la imagen del frontend Angular y la sirve con Nginx.
- `cctv-frontend/nginx.conf`: Configuración de Nginx para servir la SPA y hacer proxy a la API.
- `Dockerfile.backend`: Construye la imagen del backend Flask.
- `docker-compose.yml`: Orquesta ambos servicios en una red compartida.

### Comandos para build y deploy

1. Construir y levantar todo el stack:
   ```sh
   docker-compose up --build
   ```
   El frontend estará disponible en [http://localhost:8080](http://localhost:8080)

2. Parar y eliminar contenedores:
   ```sh
   docker-compose down
   ```

### Consideraciones importantes

- El frontend **no necesita configuración de IP/puerto del backend**: Nginx hace el proxy automáticamente a través de los paths `/cameras`, `/video`, `/video-frame`.
- El backend Flask debe exponer CORS (ya está configurado con flask-cors).
- Si usas cámaras reales, asegúrate de que los dispositivos puedan enviar frames al backend en el puerto 5050.
- Para producción, puedes agregar variables de entorno, certificados SSL, o ajustar la configuración de Nginx según tus necesidades.

### Estructura de archivos relevante

```
ESPCCTV/
├── Dockerfile.frontend
├── Dockerfile.backend
├── docker-compose.yml
├── requirements.txt
├── main.py
├── cctv-frontend/
│   ├── nginx.conf
│   └── ...
└── ...
```
