# Prácticas 3er Parcial — Unidad 4: Elementos Complementarios

Cinco ejercicios en **C++17** sobre excepciones, archivos, base de datos y librerías externas.

| Carpeta | Contenido |
|---|---|
| `ej1_excepciones/` | Validación de edad y correo con excepciones propias |
| `ej2_archivo/` | Guardar y leer una lista de tareas en `tareas.txt` |
| `ej3_base_datos/` | Tabla `productos` en SQLite: inserción y consulta |
| `ej4_libreria_externa/` | Ticket de compra leyendo/escribiendo JSON con nlohmann/json (incluye JSON de prueba con errores) |
| `ej5_integracion/` | Importar tareas de un `.txt` a SQLite con manejo de errores |
| `lib/` | Librerías externas (SQLite y nlohmann/json) |
| `capturas/` | Capturas de ejecución y evidencia de las bases de datos |
| `build.bat` | Compila los 5 ejercicios en `bin\` |

## Compilar y ejecutar

Requisito: g++ (MinGW-w64) con soporte C++17.

```
.\build.bat
cd ej3_base_datos
..\bin\ej3.exe
```

Cada programa se ejecuta **desde su carpeta**, porque ahí crea/lee sus archivos (`tareas.txt`, `inventario.db`, `pedido.json`, etc.).
El ejercicio 4 recibe el archivo JSON como argumento, por ejemplo `..\bin\ej4.exe pedido_roto.json`.

## Librerías externas y cómo se incluyeron

- **SQLite 3.46.1** (dominio público). Se descargó el *amalgamation* oficial
  (`sqlite-amalgamation-3460100.zip`, https://www.sqlite.org/2024/sqlite-amalgamation-3460100.zip) y se copiaron
  `sqlite3.c`, `sqlite3.h` y `shell.c` a `lib/sqlite/`. `build.bat` compila `sqlite3.c` una vez a `bin/sqlite3.o` y lo
  enlaza en los ejercicios 3 y 5; con `shell.c` genera `bin\sqlite3.exe`, la consola usada para revisar las bases de datos.
- **nlohmann/json 3.11.3** (licencia MIT). Es una librería *header-only*: se descargó `json.hpp`
  del release oficial (github.com/nlohmann/json/releases/tag/v3.11.3) a `lib/nlohmann/json.hpp`
  y se incluye con `#include "nlohmann/json.hpp"` compilando con `-Ilib`.

Ambas quedan dentro del proyecto con versión fija; no se instaló nada de forma global.
