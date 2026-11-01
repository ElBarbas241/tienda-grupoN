// tienda_v0.cpp - VERSION 0 (con antipatrones intencionales)
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// ANTIPATRON 5: variables globales sin encapsulamiento
int contadorVentas = 0;
double totalRecaudado = 0;

// ANTIPATRON 5: atributos publicos expuestos
struct Producto {
	int codigo;
	string nombre;
	double precio;
	int stock;
	int tipo;           // 1: Calzado, 2: Ropa, 3: Balones/Accesorios
	int garantiaMeses;  // solo aplica al tipo 3
};

struct Venta {
	int id;
	string detalle;
	string pago;
	double subtotal;
	double descuento;
	double iva;
	double total;
};

// ANTIPATRON 1: clase "Dios" que lo hace todo
class Tienda {
public:
	vector<Producto> inventario;
	vector<Venta> ventas;
	
	Tienda() {
		// La tienda inicia completamente vacía de forma intencional
	}
	
	// ---------------- F1: Registrar productos ----------------
	void registrarProducto() {
		Producto p = {0, "", 0, 0, 0, 0};
		cout << "\n--- REGISTRAR PRODUCTO ---\n";
		cout << "Codigo: ";
		cin >> p.codigo;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Codigo invalido.\n";
			return;
		}
		for (size_t i = 0; i < inventario.size(); i++) {
			if (inventario[i].codigo == p.codigo) {
				cout << "Ya existe un producto con ese codigo.\n";
				return;
			}
		}
		cin.ignore(10000, '\n');
		cout << "Nombre: ";
		getline(cin, p.nombre);
		cout << "Precio: ";
		cin >> p.precio;
		if (cin.fail() || p.precio <= 0) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Precio invalido.\n";
			return;
		}
		cout << "Stock: ";
		cin >> p.stock;
		if (cin.fail() || p.stock < 0) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Stock invalido.\n";
			return;
		}
		cout << "Tipo (1: Calzado, 2: Ropa, 3: Balones/Accesorios): ";
		cin >> p.tipo;
		if (cin.fail() || p.tipo < 1 || p.tipo > 3) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Tipo invalido.\n";
			return;
		}
		if (p.tipo == 3) {
			cout << "Meses de garantia: ";
			cin >> p.garantiaMeses;
			if (cin.fail() || p.garantiaMeses < 0) {
				cin.clear();
				cin.ignore(10000, '\n');
				cout << "Garantia invalida.\n";
				return;
			}
		}
		inventario.push_back(p);
		cout << "Producto registrado correctamente.\n";
	}
	
	// ---------------- F2: Consultar inventario ----------------
	void consultarInventario() {
		cout << "\n--- INVENTARIO ---\n";
		if (inventario.empty()) {
			cout << "No hay productos registrados.\n";
			return;
		}
		cout << fixed << setprecision(2);
		for (size_t i = 0; i < inventario.size(); i++) {
			string tipoTxt;
			if (inventario[i].tipo == 1) {
				tipoTxt = "Calzado";
			} else if (inventario[i].tipo == 2) {
				tipoTxt = "Ropa";
			} else if (inventario[i].tipo == 3) {
				tipoTxt = "Balon/Accesorio";
			} else {
				tipoTxt = "Desconocido";
			}
			cout << "[" << inventario[i].codigo << "] " << inventario[i].nombre
					   << " | " << tipoTxt
					   << " | Precio: $" << inventario[i].precio
					   << " | Stock: " << inventario[i].stock;
			if (inventario[i].tipo == 3) {
				cout << " | Garantia: " << inventario[i].garantiaMeses << " meses";
			}
			cout << "\n";
		}
	}
	
	// ---------------- F3, F4, F5: Registrar venta ----------------
	void registrarVenta() {
		cout << "\n--- REGISTRAR VENTA ---\n";
		if (inventario.empty()) {
			cout << "No hay productos para vender. Registre productos primero (Opcion 1).\n";
			return;
		}
		
		vector<int> codigos;
		vector<int> cantidades;
		char seguir = 's';
		
		while (seguir == 's' || seguir == 'S') {
			int cod;
			int cant;
			cout << "Codigo del producto: ";
			cin >> cod;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(10000, '\n');
				cout << "Codigo invalido.\n";
			} else {
				int idx = -1;
				for (size_t i = 0; i < inventario.size(); i++) {
					if (inventario[i].codigo == cod) {
						idx = (int)i;
					}
				}
				if (idx == -1) {
					cout << "Producto no encontrado.\n";
				} else {
					cout << "Cantidad de " << inventario[idx].nombre << ": ";
					cin >> cant;
					if (cin.fail() || cant <= 0) {
						cin.clear();
						cin.ignore(10000, '\n');
						cout << "Cantidad invalida.\n";
					} else {
						int yaEnCarrito = 0;
						for (size_t j = 0; j < codigos.size(); j++) {
							if (codigos[j] == cod) {
								yaEnCarrito += cantidades[j];
							}
						}
						if (cant + yaEnCarrito > inventario[idx].stock) {
							cout << "Stock insuficiente. Disponible: "
								<< inventario[idx].stock << "\n";
						} else {
							codigos.push_back(cod);
							cantidades.push_back(cant);
							cout << "Producto agregado a la venta.\n";
						}
					}
				}
			}
			cout << "Agregar otro producto? (s/n): ";
			cin >> seguir;
		}
		
		if (codigos.empty()) {
			cout << "Venta cancelada: no se agrego ningun producto.\n";
			return;
		}
		
		int formaPago;
		cout << "Forma de pago (1: Efectivo, 2: Tarjeta): ";
		cin >> formaPago;
		if (cin.fail() || (formaPago != 1 && formaPago != 2)) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Forma de pago invalida. Venta cancelada.\n";
			return;
		}
		
		double subtotal = 0;
		double descuentoTotal = 0;
		double ivaTotal = 0;
		
		// ANTIPATRON 2 y 3: switch/if anidados y numeros magicos
		for (size_t i = 0; i < codigos.size(); i++) {
			int idx = -1;
			for (size_t k = 0; k < inventario.size(); k++) {
				if (inventario[k].codigo == codigos[i]) {
					idx = (int)k;
				}
			}
			double bruto = inventario[idx].precio * cantidades[i];
			double desc = 0;
			double iva = 0;
			
			switch (inventario[idx].tipo) {
			case 1:
				if (formaPago == 1) {
					desc = bruto * 0.10;
				} else {
					desc = bruto * 0.05;
				}
				iva = (bruto - desc) * 0.19;
				break;
			case 2:
				if (formaPago == 1) {
					desc = bruto * 0.08;
				} else {
					desc = 0;
				}
				iva = (bruto - desc) * 0.19;
				break;
			case 3:
				if (formaPago == 1) {
					desc = bruto * 0.05;
				} else {
					desc = 0;
				}
				iva = (bruto - desc) * 0.19;
				break;
			default:
				desc = 0;
				iva = bruto * 0.19;
				break;
			}
			subtotal += bruto;
			descuentoTotal += desc;
			ivaTotal += iva;
		}
		
		double total = subtotal - descuentoTotal + ivaTotal;
		
		// ANTIPATRON 4: se repite el mismo calculo para imprimir la factura
		cout << fixed << setprecision(2);
		cout << "\n======== FACTURA ========\n";
		string detalle = "";
		for (size_t i = 0; i < codigos.size(); i++) {
			int idx = -1;
			for (size_t k = 0; k < inventario.size(); k++) {
				if (inventario[k].codigo == codigos[i]) {
					idx = (int)k;
				}
			}
			double bruto = inventario[idx].precio * cantidades[i];
			double desc = 0;
			double iva = 0;
			
			if (inventario[idx].tipo == 1) {
				if (formaPago == 1) {
					desc = bruto * 0.10;
				} else {
					desc = bruto * 0.05;
				}
				iva = (bruto - desc) * 0.19;
			} else if (inventario[idx].tipo == 2) {
				if (formaPago == 1) {
					desc = bruto * 0.08;
				} else {
					desc = 0;
				}
				iva = (bruto - desc) * 0.19;
			} else if (inventario[idx].tipo == 3) {
				if (formaPago == 1) {
					desc = bruto * 0.05;
				} else {
					desc = 0;
				}
				iva = (bruto - desc) * 0.19;
			} else {
				iva = bruto * 0.19;
			}
			
			cout << inventario[idx].nombre << " x" << cantidades[i]
					   << " | Bruto: $" << bruto
					   << " | Desc: $" << desc
					   << " | IVA: $" << iva << "\n";
			detalle += inventario[idx].nombre + " x" + to_string(cantidades[i]) + "; ";
		}
		cout << "-------------------------\n";
		cout << "Subtotal:   $" << subtotal << "\n";
		cout << "Descuento:  -$" << descuentoTotal << "\n";
		cout << "IVA (19%):  +$" << ivaTotal << "\n";
		cout << "TOTAL:      $" << total << "\n";
		cout << "=========================\n";
		
		// Descontar stock y verificar alertas (F5)
		for (size_t i = 0; i < codigos.size(); i++) {
			for (size_t k = 0; k < inventario.size(); k++) {
				if (inventario[k].codigo == codigos[i]) {
					inventario[k].stock -= cantidades[i];
					if (inventario[k].stock < 5) {
						cout << "ALERTA: Stock bajo del producto '" << inventario[k].nombre 
							<< "' (Quedan " << inventario[k].stock << " unidades).\n";
					}
				}
			}
		}
		
		contadorVentas++;
		totalRecaudado += total;
		Venta v = {contadorVentas, detalle, (formaPago == 1 ? "Efectivo" : "Tarjeta"), subtotal, descuentoTotal, ivaTotal, total};
		ventas.push_back(v);
		cout << "Venta registrada con exito.\n";
	}
	
	// ---------------- F6: Reporte de ventas ----------------
	void reporteDeVentas() {
		cout << "\n--- REPORTE DE VENTAS ---\n";
		cout << "Total de transacciones: " << contadorVentas << "\n";
		cout << "Dinero total recaudado: $" << totalRecaudado << "\n";
		if (ventas.empty()) {
			cout << "No hay ventas registradas.\n";
			return;
		}
		for (size_t i = 0; i < ventas.size(); i++) {
			cout << "Venta #" << ventas[i].id 
				<< " | Detalle: " << ventas[i].detalle 
				<< " | Pago: " << ventas[i].pago 
				<< " | Total: $" << ventas[i].total << "\n";
		}
	}
};

// ---------------- Menu Principal ----------------
int main() {
	Tienda miTienda;
	int opcion = 0;
	do {
		cout << "\n==================================\n";
		cout << "   SISTEMA TIENDA DEPORTIVA (v0)  \n";
		cout << "==================================\n";
		cout << "1. Registrar producto (F1)\n";
		cout << "2. Consultar inventario (F2)\n";
		cout << "3. Registrar venta (F3, F4, F5)\n";
		cout << "4. Reporte de ventas (F6)\n";
		cout << "5. Salir\n";
		cout << "Seleccione una opcion: ";
		cin >> opcion;
		
		if (cin.fail()) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Opcion invalida.\n";
			continue;
		}
		
		switch (opcion) {
		case 1:
			miTienda.registrarProducto();
			break;
		case 2:
			miTienda.consultarInventario();
			break;
		case 3:
			miTienda.registrarVenta();
			break;
		case 4:
			miTienda.reporteDeVentas();
			break;
		case 5:
			cout << "Saliendo del programa...\n";
			break;
		default:
			cout << "Opcion no valida.\n";
			break;
		}
	} while (opcion != 5);
	
	return 0;
}
