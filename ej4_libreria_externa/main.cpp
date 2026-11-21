// Ejercicio 4 - Uso de una librería externa: nlohmann/json v3.11.3
// Problema: leer un pedido en JSON, calcular subtotal, IVA y total, y guardar un ticket en JSON.

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include "nlohmann/json.hpp"

#ifdef _WIN32
#include <windows.h>  // para mostrar acentos en la consola
#endif

using namespace std;
using json = nlohmann::json;

const double IVA = 0.16;

// Redondea a centavos para que el JSON no guarde cosas como 886.704
double centavos(double x) { return std::round(x * 100.0) / 100.0; }

// Excepciones propias para los errores que no vienen de la librería
class ArchivoNoEncontradoException : public runtime_error {
public:
    explicit ArchivoNoEncontradoException(const string& ruta)
        : runtime_error("No se encontró el archivo " + ruta) {}
};

class PedidoInvalidoException : public runtime_error {
public:
    explicit PedidoInvalidoException(const string& msg) : runtime_error(msg) {}
};

json cargarJson(const string& ruta) {
    ifstream in(ruta);
    if (!in)
        throw ArchivoNoEncontradoException(ruta);
    return json::parse(in);  // lanza json::parse_error si el JSON está mal escrito
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    string entrada = argc > 1 ? argv[1] : "pedido.json";
    cout << "=== Ticket de compra con nlohmann/json " << NLOHMANN_JSON_VERSION_MAJOR << "."
         << NLOHMANN_JSON_VERSION_MINOR << "." << NLOHMANN_JSON_VERSION_PATCH << " ===\n";
    cout << "Leyendo " << entrada << "...\n\n";

    try {
        json pedido = cargarJson(entrada);

        // .at() lanza json::out_of_range si falta la clave; get<>() lanza type_error si el tipo no coincide
        string cliente = pedido.at("cliente").get<string>();
        double subtotal = 0;
        json renglones = json::array();

        cout << left << setw(20) << "Producto" << right << setw(6) << "Cant" << setw(10) << "Precio"
             << setw(11) << "Importe" << "\n" << string(47, '-') << "\n" << fixed << setprecision(2);

        for (const json& art : pedido.at("articulos")) {
            string nombre = art.at("nombre").get<string>();
            long long cantidad = art.at("cantidad").get<long long>();
            double precio = art.at("precio").get<double>();
            if (cantidad <= 0 || cantidad > 100000 || precio < 0)
                throw PedidoInvalidoException("El artículo \"" + nombre + "\" tiene cantidad o precio inválido.");
            double importe = cantidad * precio;
            subtotal += importe;
            renglones.push_back({{"nombre", nombre}, {"cantidad", cantidad}, {"importe", centavos(importe)}});
            cout << left << setw(20) << nombre << right << setw(6) << cantidad << setw(10) << precio
                 << setw(11) << importe << "\n";
        }

        double iva = subtotal * IVA;
        double total = subtotal + iva;
        cout << string(47, '-') << "\n"
             << right << setw(36) << "Subtotal:" << setw(11) << subtotal << "\n"
             << setw(36) << "IVA 16%:" << setw(11) << iva << "\n"
             << setw(36) << "Total:" << setw(11) << total << "\n";

        json ticket = {
            {"cliente", cliente},
            {"renglones", renglones},
            {"subtotal", centavos(subtotal)},
            {"iva", centavos(iva)},
            {"total", centavos(total)}
        };
        ofstream out("ticket.json");
        out << setw(2) << ticket << "\n";
        cout << "\nTicket de " << cliente << " guardado en ticket.json\n";

    } catch (const json::parse_error& e) {
        cout << "[json::parse_error] El archivo no es JSON válido: " << e.what() << "\n";
        return 1;
    } catch (const json::out_of_range& e) {
        cout << "[json::out_of_range] Falta un campo obligatorio: " << e.what() << "\n";
        return 1;
    } catch (const json::type_error& e) {
        cout << "[json::type_error] Un campo tiene un tipo incorrecto: " << e.what() << "\n";
        return 1;
    } catch (const PedidoInvalidoException& e) {
        cout << "[PedidoInvalido] " << e.what() << "\n";
        return 1;
    } catch (const ArchivoNoEncontradoException& e) {
        cout << "[ArchivoNoEncontrado] " << e.what() << "\n";
        return 1;
    }
    return 0;
}
