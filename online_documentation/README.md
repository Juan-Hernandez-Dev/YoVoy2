# Traffic Simulation Documentation Site

Sitio web de documentación para el Sistema de Simulación de Tráfico Urbano, construido con Next.js, React Markdown y Tailwind CSS.

## 🚀 Características

- ✅ Navegación por tabs automática basada en `docs-config.json`
- ✅ Renderizado de Markdown con soporte para GitHub Flavored Markdown (tablas, listas de tareas, etc.)
- ✅ Diseño responsive con Tailwind CSS
- ✅ Syntax highlighting para bloques de código
- ✅ Optimizado para Vercel
- ✅ Carga dinámica de documentos desde la carpeta `docs/`

## 📁 Estructura del Proyecto

```
online_documentation/
├── app/
│   ├── api/
│   │   └── docs/
│   │       └── [slug]/
│   │           └── route.ts          # API endpoint para cargar MD
│   ├── layout.tsx                    # Layout principal
│   ├── page.tsx                      # Página principal con tabs
│   └── globals.css                   # Estilos globales
├── public/
│   └── docs/                         # Archivos MD copiados aquí
│       ├── overview.md
│       ├── estructura-archivos.md
│       └── ...
├── docs-config.json                  # Configuración de documentos
├── package.json
├── tailwind.config.js
├── tsconfig.json
└── README.md
```

## 🛠️ Instalación

### 1. Instalar dependencias

```bash
cd online_documentation
npm install
```

### 2. Ejecutar en desarrollo

```bash
npm run dev
```

Abre [http://localhost:3000](http://localhost:3000) en tu navegador.

## 📝 Agregar Nueva Documentación

### Paso 1: Agregar archivo MD

1. Coloca tu archivo `.md` en `public/docs/`
2. Edita `docs-config.json`:

```json
{
  "titulo": "Mi Nuevo Doc",
  "endpoint": "mi-nuevo-doc",
  "archivo": "mi-nuevo-doc.md"
}
```

3. El tab aparecerá automáticamente en la navbar

### Sincronizar desde carpeta docs/ externa

Si tienes archivos MD en una carpeta `docs/` fuera de `online_documentation/`:

```bash
# Windows
Copy-Item -Path "../docs/*.md" -Destination "public/docs/" -Force

# Linux/Mac
cp ../docs/*.md public/docs/
```

## 🎨 Personalización

### Cambiar colores

Edita `tailwind.config.js` para modificar el tema:

```js
theme: {
  extend: {
    colors: {
      primary: '#3b82f6',
      secondary: '#10b981',
    }
  }
}
```

### Modificar estilos de Markdown

Edita los componentes personalizados en `app/page.tsx`:

```tsx
components={{
  h1: ({ node, ...props }) => (
    <h1 className="text-4xl font-bold text-blue-600" {...props} />
  ),
  // ... más componentes
}}
```

## 🚀 Deploy en Vercel

### Método 1: Desde GitHub

1. Sube tu proyecto a GitHub
2. Ve a [vercel.com](https://vercel.com)
3. Importa tu repositorio
4. Configura el directorio raíz como `online_documentation`
5. Deploy automático

### Método 2: Desde CLI

```bash
npm install -g vercel
cd online_documentation
vercel
```

### Configuración de Vercel

Si necesitas configurar el directorio raíz, crea `vercel.json`:

```json
{
  "buildCommand": "npm run build",
  "outputDirectory": ".next",
  "devCommand": "npm run dev",
  "installCommand": "npm install"
}
```

## 📦 Dependencias Principales

- **Next.js 14**: Framework React con SSR
- **React Markdown**: Renderizado de Markdown
- **remark-gfm**: Soporte para GitHub Flavored Markdown
- **Tailwind CSS**: Framework de utilidades CSS
- **TypeScript**: Tipado estático

## 🔧 Scripts Disponibles

```bash
npm run dev      # Servidor de desarrollo
npm run build    # Build de producción
npm run start    # Servidor de producción
npm run lint     # Linter
```

## 📖 Formato de docs-config.json

```json
[
  {
    "titulo": "Nombre visible en tab",
    "endpoint": "url-slug",
    "ruta": "../docs/archivo.md"
  }
]
```

- **titulo**: Texto que aparece en el tab de navegación
- **endpoint**: Slug para la URL (sin espacios, minúsculas)
- **ruta**: Ruta relativa al archivo MD desde `online_documentation/`

## 🎯 Características de Markdown Soportadas

- ✅ Encabezados (H1-H6)
- ✅ Listas (ordenadas y desordenadas)
- ✅ Tablas
- ✅ Bloques de código con syntax highlighting
- ✅ Código inline
- ✅ Enlaces
- ✅ Imágenes
- ✅ Citas (blockquotes)
- ✅ Líneas horizontales
- ✅ Listas de tareas (GitHub style)
- ✅ Emojis

## 🐛 Troubleshooting

### Error: Cannot find module '../docs/...'

Asegúrate de que la ruta en `docs-config.json` sea correcta y relativa a `online_documentation/`.

### Los estilos no se aplican

Ejecuta:

```bash
npm run build
```

Y verifica que `tailwind.config.js` tenga las rutas correctas en `content`.

### El markdown no se renderiza

Verifica que `raw-loader` esté instalado:

```bash
npm install --save-dev raw-loader
```

## 📄 Licencia

Este proyecto es parte del curso de Estructuras de Datos II de la Universidad Autónoma de Aguascalientes.

## 👥 Contribuciones

Para agregar documentación:

1. Crea tu archivo `.md` en `docs/`
2. Actualiza `docs-config.json`
3. Haz commit y push
4. Vercel desplegará automáticamente

## 🔗 Enlaces Útiles

- [Next.js Documentation](https://nextjs.org/docs)
- [React Markdown](https://github.com/remarkjs/react-markdown)
- [Tailwind CSS](https://tailwindcss.com/docs)
- [Vercel Deployment](https://vercel.com/docs)
