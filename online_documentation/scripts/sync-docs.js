const fs = require('fs');
const path = require('path');

console.log('Sincronizando documentacion...');

const sourceDir = path.join(__dirname, '../../docs');
const targetDir = path.join(__dirname, '../public/docs');

if (!fs.existsSync(targetDir)) {
  fs.mkdirSync(targetDir, { recursive: true });
}

if (!fs.existsSync(sourceDir)) {
  console.error('Error: No se encontro la carpeta docs/');
  process.exit(1);
}

const files = fs.readdirSync(sourceDir).filter(file => file.endsWith('.md'));

files.forEach(file => {
  const sourcePath = path.join(sourceDir, file);
  const targetPath = path.join(targetDir, file);
  fs.copyFileSync(sourcePath, targetPath);
  console.log('Copiado:', file);
});

console.log('Completado:', files.length, 'archivos');
