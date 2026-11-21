// Ejercicio 3 - Conexión a base de datos (SQLite)
// Crea la tabla productos, inserta registros con sentencias preparadas y los consulta.

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <cmath>
#include "sqlite3.h"

#ifdef _WIN32
#include <windows.h>  // para mostrar acentos en la consola
#endif

using namespace std;

class ErrorBD : public runtime_error {
public:
    explicit ErrorBD(const string& msg) : runtime_error(msg) {}
};

// setw cuenta bytes, y en UTF-8 "á" ocupa 2; esto rellena contando caracteres reales
string rellenar(const string& s, size_t ancho) {
    size_t letras = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80) ++letras;
    return letras >= ancho ? s : s + string(ancho - letras, ' ');
}

// Envoltorio pequeño: abre la conexión en el constructor y la cierra en el destructor
class BaseDatos {
    sqlite3* db = nullptr;
public:
    explicit BaseDatos(const string& ruta) {
        if (sqlite3_open(ruta.c_str(), &db) != SQLITE_OK) {
            string msg = db ? sqlite3_errmsg(db) : "memoria insuficiente";
            sqlite3_close(db);
            throw ErrorBD("No se pudo abrir " + ruta + ": " + msg);
        }
    }
    ~BaseDatos() { sqlite3_close(db); }
    BaseDatos(const BaseDatos&) = delete;
    BaseDatos& operator=(const BaseDatos&) = delete;

    void ejecutar(const string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            string msg = err ? err : "error desconocido";
            sqlite3_free(err);
            throw ErrorBD(msg);
        }
    }

    void insertarProducto(const string& nombre, double precio, int stock) {
        sqlite3_stmt* st = nullptr;
        const char* sql = "INSERT INTO productos(nombre, precio, stock) VALUES(?, ?, ?);";
        if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
            throw ErrorBD(sqlite3_errmsg(db));
        sqlite3_bind_text(st, 1, nombre.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(st, 2, precio);
        sqlite3_bind_int(st, 3, stock);
        int rc = sqlite3_step(st);
        sqlite3_finalize(st);
        if (rc != SQLITE_DONE)
            throw ErrorBD(sqlite3_errmsg(db));
    }

    void listarProductos() {
        sqlite3_stmt* st = nullptr;
        const char* sql = "SELECT id, nombre, precio, stock FROM productos ORDER BY id;";
        if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
            throw ErrorBD(sqlite3_errmsg(db));
        cout << left << setw(4) << "ID" << setw(22) << "Nombre" << right << setw(10) << "Precio"
             << setw(8) << "Stock" << "\n" << string(44, '-') << "\n";
        int filas = 0;
        int rc;
        while ((rc = sqlite3_step(st)) == SQLITE_ROW) {
            cout << left << setw(4) << sqlite3_column_int(st, 0)
                 << rellenar(reinterpret_cast<const char*>(sqlite3_column_text(st, 1)), 22)
                 << right << fixed << setprecision(2) << setw(10) << sqlite3_column_double(st, 2)
                 << setw(8) << sqlite3_column_int(st, 3) << "\n";
            ++filas;
        }
        string msg = sqlite3_errmsg(db);
        sqlite3_finalize(st);
        if (rc != SQLITE_DONE)  // el ciclo también termina si hubo un error de lectura
            throw ErrorBD(msg);
        cout << "(" << filas << " registro(s))\n";
    }
};

double leerDouble(const string& etiqueta) {
    while (true) {
        cout << etiqueta;
        string s;
        if (!getline(cin, s)) throw runtime_error("entrada terminada");
        try {
            size_t usados;
            double v = stod(s, &usados);
            if (usados == s.size() && isfinite(v) && v >= 0) return v;  // isfinite rechaza "inf"
        } catch (const invalid_argument&) {  // no es un número
        } catch (const out_of_range&) {      // número demasiado grande
        }
        cout << "  Valor inválido, escribe un número mayor o igual a 0.\n";
    }
}

int leerEntero(const string& etiqueta) {
    while (true) {
        cout << etiqueta;
        string s;
        if (!getline(cin, s)) throw runtime_error("entrada terminada");
        try {
            size_t usados;
            int v = stoi(s, &usados);
            if (usados == s.size() && v >= 0) return v;
        } catch (const invalid_argument&) {  // no es un número
        } catch (const out_of_range&) {      // número demasiado grande
        }
        cout << "  Valor inválido, escribe un número entero mayor o igual a 0.\n";
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    cout << "=== Inventario con SQLite (" << sqlite3_libversion() << ") ===\n";
    try {
        BaseDatos bd("inventario.db");
        bd.ejecutar("CREATE TABLE IF NOT EXISTS productos ("
                    " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    " nombre TEXT NOT NULL,"
                    " precio REAL NOT NULL CHECK(precio >= 0),"
                    " stock INTEGER NOT NULL DEFAULT 0);");
        cout << "Conectado a inventario.db\n";

        while (true) {
            cout << "\n1) Insertar producto   2) Consultar productos   3) Salir\nOpción: ";
            string op;
            if (!getline(cin, op) || op == "3") break;
            try {
                if (op == "1") {
                    cout << "Nombre: ";
                    string nombre;
                    getline(cin, nombre);
                    if (nombre.empty()) { cout << "  El nombre es obligatorio.\n"; continue; }
                    double precio = leerDouble("Precio: ");
                    int stock = leerEntero("Stock: ");
                    bd.insertarProducto(nombre, precio, stock);
                    cout << "  Producto insertado.\n";
                } else if (op == "2") {
                    bd.listarProductos();
                } else {
                    cout << "Opción no válida.\n";
                }
            } catch (const ErrorBD& e) {
                cout << "  [ErrorBD] " << e.what() << "\n";
            }
        }
    } catch (const ErrorBD& e) {
        cout << "[ErrorBD] " << e.what() << "\n";
        return 1;
    } catch (const exception& e) {
        cout << "[Error] " << e.what() << "\n";
        return 1;
    }
    cout << "Conexión cerrada.\n";
    return 0;
}
