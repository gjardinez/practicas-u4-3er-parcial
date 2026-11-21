// Ejercicio 1 - Validación con excepciones
// Pide edad y correo; lanza y captura excepciones propias si los datos no son válidos.

#include <iostream>
#include <string>
#include <regex>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>  // para mostrar acentos en la consola
#endif

using namespace std;

// Excepciones propias para distinguir cada tipo de error
class EdadInvalidaException : public invalid_argument {
public:
    explicit EdadInvalidaException(const string& msg) : invalid_argument(msg) {}
};

class CorreoInvalidoException : public invalid_argument {
public:
    explicit CorreoInvalidoException(const string& msg) : invalid_argument(msg) {}
};

string recortar(const string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == string::npos) return "";
    size_t fin = s.find_last_not_of(" \t\r\n");
    return s.substr(ini, fin - ini + 1);
}

int validarEdad(const string& texto) {
    string t = recortar(texto);
    if (t.empty())
        throw EdadInvalidaException("La edad no puede estar vacía.");

    size_t usados = 0;
    int edad;
    try {
        edad = stoi(t, &usados);
    } catch (const invalid_argument&) {
        throw EdadInvalidaException("\"" + t + "\" no es un número.");
    } catch (const out_of_range&) {
        throw EdadInvalidaException("\"" + t + "\" es un número demasiado grande.");
    }

    // stoi acepta "25abc" y se queda con 25, por eso reviso que se haya leído todo
    if (usados != t.size())
        throw EdadInvalidaException("\"" + t + "\" contiene caracteres que no son dígitos.");
    if (edad < 0 || edad > 120)
        throw EdadInvalidaException("La edad debe estar entre 0 y 120 (se recibió " + to_string(edad) + ").");
    return edad;
}

string validarCorreo(const string& texto) {
    string c = recortar(texto);
    if (c.empty())
        throw CorreoInvalidoException("El correo no puede estar vacío.");

    // usuario@dominio.ext  (ext de al menos 2 letras)
    static const regex patron(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)*\.[A-Za-z]{2,}$)");
    if (!regex_match(c, patron))
        throw CorreoInvalidoException("\"" + c + "\" no tiene el formato usuario@dominio.com");
    return c;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    cout << "=== Registro de usuario (validación con excepciones) ===\n";
    int edad = -1;
    string correo;

    // Se repite hasta que el dato sea válido; el programa nunca termina por un error
    while (true) {
        cout << "Edad: ";
        string linea;
        if (!getline(cin, linea)) { cout << "\nEntrada terminada.\n"; return 1; }
        try {
            edad = validarEdad(linea);
            break;
        } catch (const EdadInvalidaException& e) {
            cout << "  [EdadInvalidaException] " << e.what() << "\n";
        }
    }

    while (true) {
        cout << "Correo electrónico: ";
        string linea;
        if (!getline(cin, linea)) { cout << "\nEntrada terminada.\n"; return 1; }
        try {
            correo = validarCorreo(linea);
            break;
        } catch (const CorreoInvalidoException& e) {
            cout << "  [CorreoInvalidoException] " << e.what() << "\n";
        }
    }

    cout << "\nDatos válidos registrados:\n"
         << "  Edad:   " << edad << " años\n"
         << "  Correo: " << correo << "\n";
    return 0;
}
