// Ejercicio 5 - Integración: archivo + base de datos + manejo de excepciones
// Importa tareas desde un archivo de texto a una BD SQLite. Si el archivo no existe
// o la conexión a la BD falla, se muestra un mensaje claro y el programa sigue funcionando.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "sqlite3.h"

#ifdef _WIN32
#include <windows.h>  // para mostrar acentos en la consola
#endif

using namespace std;

class ArchivoNoEncontradoException : public runtime_error {
public:
    explicit ArchivoNoEncontradoException(const string& ruta)
        : runtime_error("El archivo \"" + ruta + "\" no existe o no se puede leer.") {}
};

class ConexionBDException : public runtime_error {
public:
    explicit ConexionBDException(const string& msg) : runtime_error(msg) {}
};

class ConsultaBDException : public runtime_error {
public:
    explicit ConsultaBDException(const string& msg) : runtime_error(msg) {}
};

class RepositorioTareas {
    sqlite3* db = nullptr;

    void exec(const string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            string msg = err ? err : "error desconocido";
            sqlite3_free(err);
            throw ConsultaBDException(msg);
        }
    }

public:
    explicit RepositorioTareas(const string& ruta) {
        // Sin SQLITE_OPEN_CREATE para carpetas inexistentes falla igual; con CREATE crea el archivo
        int rc = sqlite3_open_v2(ruta.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        if (rc != SQLITE_OK) {
            string msg = db ? sqlite3_errmsg(db) : "sin memoria";
            sqlite3_close(db);
            db = nullptr;
            throw ConexionBDException("No se pudo conectar a \"" + ruta + "\": " + msg);
        }
        exec("CREATE TABLE IF NOT EXISTS tareas ("
             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
             " descripcion TEXT NOT NULL,"
             " creada TEXT DEFAULT (datetime('now','localtime')));");
    }
    ~RepositorioTareas() { sqlite3_close(db); }
    RepositorioTareas(const RepositorioTareas&) = delete;
    RepositorioTareas& operator=(const RepositorioTareas&) = delete;

    int insertarVarias(const vector<string>& tareas) {
        exec("BEGIN;");
        sqlite3_stmt* st = nullptr;
        if (sqlite3_prepare_v2(db, "INSERT INTO tareas(descripcion) VALUES(?);", -1, &st, nullptr) != SQLITE_OK) {
            exec("ROLLBACK;");
            throw ConsultaBDException(sqlite3_errmsg(db));
        }
        for (const string& t : tareas) {
            sqlite3_bind_text(st, 1, t.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(st) != SQLITE_DONE) {
                string msg = sqlite3_errmsg(db);
                sqlite3_finalize(st);
                exec("ROLLBACK;");  // si falla una, no se guarda ninguna
                throw ConsultaBDException(msg);
            }
            sqlite3_reset(st);
        }
        sqlite3_finalize(st);
        exec("COMMIT;");
        return static_cast<int>(tareas.size());
    }

    void listar() {
        sqlite3_stmt* st = nullptr;
        if (sqlite3_prepare_v2(db, "SELECT id, descripcion, creada FROM tareas ORDER BY id;", -1, &st, nullptr) != SQLITE_OK)
            throw ConsultaBDException(sqlite3_errmsg(db));
        int n = 0;
        while (sqlite3_step(st) == SQLITE_ROW) {
            cout << "  #" << sqlite3_column_int(st, 0) << "  "
                 << reinterpret_cast<const char*>(sqlite3_column_text(st, 1)) << "   ("
                 << reinterpret_cast<const char*>(sqlite3_column_text(st, 2)) << ")\n";
            ++n;
        }
        sqlite3_finalize(st);
        cout << "  Total: " << n << " tarea(s)\n";
    }
};

vector<string> leerArchivo(const string& ruta) {
    ifstream in(ruta);
    if (!in) throw ArchivoNoEncontradoException(ruta);
    vector<string> lineas;
    string l;
    while (getline(in, l)) {
        if (!l.empty() && l.back() == '\r') l.pop_back();
        if (!l.empty()) lineas.push_back(l);
    }
    return lineas;
}

string preguntar(const string& msg) {
    cout << msg;
    string s;
    if (!getline(cin, s)) throw runtime_error("entrada terminada");
    return s;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    cout << "=== Importador de tareas: archivo .txt -> SQLite ===\n";
    unique_ptr<RepositorioTareas> repo;

    try {
        while (true) {
            cout << "\nBD actual: " << (repo ? "conectada" : "SIN CONEXIÓN") << "\n"
                 << "1) Conectar a BD   2) Importar archivo   3) Ver tareas   4) Salir\n";
            string op = preguntar("Opción: ");
            if (op == "4") break;

            try {
                if (op == "1") {
                    string ruta = preguntar("Ruta de la BD (ej. tareas.db): ");
                    repo.reset();  // cierra la conexión anterior
                    repo = make_unique<RepositorioTareas>(ruta);
                    cout << "  Conexión exitosa a " << ruta << "\n";
                } else if (op == "2" || op == "3") {
                    if (!repo) throw ConexionBDException("Primero conecta a una base de datos (opción 1).");
                    if (op == "2") {
                        string ruta = preguntar("Archivo a importar: ");
                        vector<string> tareas = leerArchivo(ruta);
                        if (tareas.empty()) { cout << "  El archivo está vacío.\n"; continue; }
                        cout << "  Se importaron " << repo->insertarVarias(tareas) << " tarea(s).\n";
                    } else {
                        repo->listar();
                    }
                } else {
                    cout << "  Opción no válida.\n";
                }
            } catch (const ArchivoNoEncontradoException& e) {
                cout << "  [ArchivoNoEncontrado] " << e.what() << "\n";
            } catch (const ConexionBDException& e) {
                cout << "  [ConexionBD] " << e.what() << "\n";
            } catch (const ConsultaBDException& e) {
                cout << "  [ConsultaBD] " << e.what() << "\n";
            }
        }
    } catch (const exception& e) {
        cout << "\n[Error inesperado] " << e.what() << "\n";
        return 1;
    }
    cout << "Programa terminado correctamente.\n";
    return 0;
}
