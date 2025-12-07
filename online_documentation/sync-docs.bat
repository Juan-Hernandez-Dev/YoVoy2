@echo off
echo Sincronizando documentacion...
if not exist "public\docs" mkdir "public\docs"
copy /Y "..\docs\*.md" "public\docs\"
echo Completado!
