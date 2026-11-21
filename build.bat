@echo off
setlocal
rem Compila los 5 ejercicios. Requiere g++ y gcc (MinGW-w64) en el PATH.
cd /d "%~dp0"
if not exist bin mkdir bin

rem MinGW falla al enlazar si su ruta tiene espacios (p. ej. "C:\Users\Nombre Apellido\...").
rem Se le indica su carpeta interna con la ruta corta 8.3, que no tiene espacios.
set "GPP=g++"
set "GCC=gcc"
set "GBIN="
for %%G in (g++.exe) do set "GBIN=%%~dps$PATH:G"
if "%GBIN%"=="" (echo No se encontro g++ en el PATH & exit /b 1)
set "GCC_EXEC_PREFIX=%GBIN%..\lib\gcc\"

set FLAGS=-std=c++17 -Wall -O2

if not exist bin\sqlite3.o (
    echo Compilando SQLite ^(solo la primera vez, tarda un poco^)...
    %GCC% -O1 -c lib\sqlite\sqlite3.c -o bin\sqlite3.o || goto :error
)
if not exist bin\sqlite3.exe (
    echo Compilando consola sqlite3.exe para revisar las bases de datos...
    %GCC% -O1 lib\sqlite\shell.c bin\sqlite3.o -o bin\sqlite3.exe || goto :error
)

echo [1/5] ej1_excepciones
%GPP% %FLAGS% ej1_excepciones\main.cpp -o bin\ej1.exe || goto :error
echo [2/5] ej2_archivo
%GPP% %FLAGS% ej2_archivo\main.cpp -o bin\ej2.exe || goto :error
echo [3/5] ej3_base_datos
%GPP% %FLAGS% -Ilib\sqlite ej3_base_datos\main.cpp bin\sqlite3.o -o bin\ej3.exe || goto :error
echo [4/5] ej4_libreria_externa
%GPP% %FLAGS% -Ilib ej4_libreria_externa\main.cpp -o bin\ej4.exe || goto :error
echo [5/5] ej5_integracion
%GPP% %FLAGS% -Ilib\sqlite ej5_integracion\main.cpp bin\sqlite3.o -o bin\ej5.exe || goto :error

echo Listo. Ejecutables en bin\
exit /b 0

:error
echo Error de compilacion.
exit /b 1
