// Ejercicio 2 - Archivo de texto
// Modo 1 guarda una lista de tareas (mínimo 5) en tareas.txt; modo 2 las lee y las muestra.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>  // para mostrar acentos en la consola
#endif

using namespace std;

const string ARCHIVO = "tareas.txt";
const size_t MINIMO = 5;

void guardarTareas(const vector<string>& tareas, const string& ruta) {
    ofstream out(ruta);  // sobrescribe el archivo anterior
    if (!out)
        throw runtime_error("No se pudo crear/abrir \"" + ruta + "\" para escritura.");
    for (const string& t : tareas)
        out << t << '\n';
    if (!out)
        throw runtime_error("Ocurrió un error al escribir en \"" + ruta + "\".");
}

vector<string> leerTareas(const string& ruta) {
    ifstream in(ruta);
    if (!in)
        throw runtime_error("El archivo \"" + ruta + "\" no existe todavía. Usa primero la opción 1.");
    vector<string> tareas;
    string linea;
    while (getline(in, linea))
        if (!linea.empty()) tareas.push_back(linea);
    return tareas;
}

void modoGuardar() {
    vector<string> tareas;
    cout << "Escribe tus tareas (una por línea). Línea vacía para terminar, mínimo " << MINIMO << ".\n";
    while (true) {
        cout << "  Tarea " << tareas.size() + 1 << ": ";
        string t;
        if (!getline(cin, t)) break;
        if (t.empty()) {
            if (tareas.size() >= MINIMO) break;
            cout << "  Faltan " << MINIMO - tareas.size() << " tarea(s) para el mínimo.\n";
            continue;
        }
        tareas.push_back(t);
    }
    if (tareas.size() < MINIMO) {
        cout << "No se guardó nada: se necesitan al menos " << MINIMO << " tareas.\n";
        return;
    }
    guardarTareas(tareas, ARCHIVO);
    cout << "Se guardaron " << tareas.size() << " tareas en " << ARCHIVO << ".\n";
}

void modoLeer() {
    vector<string> tareas = leerTareas(ARCHIVO);
    cout << "Tareas guardadas en " << ARCHIVO << " (" << tareas.size() << "):\n";
    for (size_t i = 0; i < tareas.size(); ++i)
        cout << "  " << i + 1 << ". " << tareas[i] << '\n';
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    cout << "=== Lista de tareas en archivo de texto ===\n";
    while (true) {
        cout << "\n1) Guardar tareas   2) Leer tareas   3) Salir\nOpción: ";
        string op;
        if (!getline(cin, op) || op == "3") break;
        try {
            if (op == "1") modoGuardar();
            else if (op == "2") modoLeer();
            else cout << "Opción no válida.\n";
        } catch (const runtime_error& e) {
            cout << "  [Error de archivo] " << e.what() << "\n";
        }
    }
    cout << "Hasta luego.\n";
    return 0;
}
