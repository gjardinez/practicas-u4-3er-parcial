# Prácticas 3er Parcial — Unidad 4: Elementos Complementarios

Cinco ejercicios en **C++17** sobre excepciones, archivos, base de datos y librerías externas.

| Carpeta | Ejercicio |
|---|---|
| `ej1_excepciones/` | Validación de edad y correo con excepciones propias |
| `ej2_archivo/` | Guardar y leer una lista de tareas en `tareas.txt` |
| `ej3_base_datos/` | Tabla `productos` en SQLite: inserción y consulta |
| `ej4_libreria_externa/` | Ticket de compra leyendo/escribiendo JSON con nlohmann/json |
| `ej5_integracion/` | Importar tareas de un `.txt` a SQLite con manejo de errores |
| `lib/` | Librerías externas (SQLite y nlohmann/json) |

## Compilar y ejecutar

Requisito: g++ (MinGW-w64) con soporte C++17.

```
build.bat
cd ej3_base_datos
..\bin\ej3.exe
```

Cada programa se ejecuta **desde su carpeta**, porque ahí crea/lee sus archivos (`tareas.txt`, `inventario.db`, `pedido.json`, etc.).

## Librerías externas y cómo se incluyeron

- **SQLite 3.46.1** (dominio público). Se descargó el *amalgamation* oficial
  (`sqlite-amalgamation-3460100.zip` de sqlite.org/download.html) y se copiaron `sqlite3.c` y `sqlite3.h` a `lib/sqlite/`.
  `build.bat` compila `sqlite3.c` una vez a `bin/sqlite3.o` y lo enlaza en los ejercicios 3 y 5.
- **nlohmann/json 3.11.3** (licencia MIT). Es una librería *header-only*: se descargó `json.hpp`
  del release oficial (github.com/nlohmann/json/releases/tag/v3.11.3) a `lib/nlohmann/json.hpp`
  y se incluye con `#include "nlohmann/json.hpp"` compilando con `-Ilib`.

Ambas quedan dentro del proyecto con versión fija; no se instaló nada de forma global.
