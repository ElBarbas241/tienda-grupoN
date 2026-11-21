// tienda_v1.cpp - VERSION 1 (Redisenada con Builder, Composite y State)
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <memory>

using namespace std;

// Constantes de negocio (elimina numeros magicos)
namespace ReglasTienda {
	constexpr double IVA_TASA = 0.19;
	constexpr int STOCK_MINIMO_ALERTA = 5;
}

// ==========================================================
// PATRON ESTRUCTURAL: COMPOSITE
// Permite tratar productos individuales y kits/combos igual
// ==========================================================
class ItemCatalogo {
public:
	virtual ~ItemCatalogo() = default;
	virtual int getCodigo() const = 0;
	virtual string getNombre() const = 0;
	virtual double getPrecio() const = 0;
	virtual int getStock() const = 0;
	virtual int getTipo() const = 0; // 1: Calzado, 2: Ropa, 3: Balon/Accesorio, 4: Kit
	virtual int getGarantiaMeses() const = 0;
	virtual bool descontarStock(int cantidad) = 0;
	virtual void mostrarDetalle() const = 0;
};

class ProductoSimple : public ItemCatalogo {
private:
	int codigo;
	string nombre;
	double precio;
	int stock;
	int tipo;
	int garantiaMeses;
	
	friend class ProductoBuilder;
	
public:
	ProductoSimple(int cod, const string& nom, double pre, int stk, int tip, int gar)
		: codigo(cod), nombre(nom), precio(pre), stock(stk), tipo(tip), garantiaMeses(gar) {}
	
	int getCodigo() const override { return codigo; }
	string getNombre() const override { return nombre; }
	double getPrecio() const override { return precio; }
	int getStock() const override { return stock; }
	int getTipo() const override { return tipo; }
	int getGarantiaMeses() const override { return garantiaMeses; }
	
	bool descontarStock(int cantidad) override {
		if (cantidad <= 0 || cantidad > stock) return false;
		stock -= cantidad;
		return true;
	}
	
	void mostrarDetalle() const override {
		string tipoTxt = (tipo == 1) ? "Calzado" :
			(tipo == 2) ? "Ropa" :
			(tipo == 3) ? "Balon/Accesorio" : "Desconocido";
		cout << "[" << codigo << "] " << nombre
			<< " | " << tipoTxt
			<< " | Precio: $" << precio
			<< " | Stock: " << stock;
		if (tipo == 3) {
			cout << " | Garantia: " << garantiaMeses << " meses";
		}
		cout << "\n";
	}
};

class KitDeportivo : public ItemCatalogo {
private:
	int codigo;
	string nombre;
	double descuentoKit; // ej. 0.05 = 5% por llevar combo
	vector<shared_ptr<ItemCatalogo>> componentes;
	
public:
	KitDeportivo(int cod, const string& nom, double descCombo = 0.05)
		: codigo(cod), nombre(nom), descuentoKit(descCombo) {}
	
	void agregarItem(const shared_ptr<ItemCatalogo>& item) {
		if (item) componentes.push_back(item);
	}
	
	int getCodigo() const override { return codigo; }
	string getNombre() const override { return nombre + " [KIT]"; }
	
	double getPrecio() const override {
		double suma = 0.0;
		for (const auto& c : componentes) {
			suma += c->getPrecio();
		}
		return suma * (1.0 - descuentoKit);
	}
	
	int getStock() const override {
		if (componentes.empty()) return 0;
		int minStock = componentes[0]->getStock();
		for (const auto& c : componentes) {
			if (c->getStock() < minStock) {
				minStock = c->getStock();
			}
		}
		return minStock;
	}
	
	int getTipo() const override { return 4; }
	int getGarantiaMeses() const override { return 0; }
	
	bool descontarStock(int cantidad) override {
		if (cantidad <= 0 || cantidad > getStock()) return false;
		for (const auto& c : componentes) {
			if (!c->descontarStock(cantidad)) return false;
		}
		return true;
	}
	
	void mostrarDetalle() const override {
		cout << "[" << codigo << "] " << getNombre()
			<< " | Combo Deportivo"
			<< " | Precio: $" << getPrecio()
			<< " | Stock disponible: " << getStock() << "\n";
		for (const auto& c : componentes) {
			cout << "    -> Incluye: " << c->getNombre() << " ($" << c->getPrecio() << ")\n";
		}
	}
};

// ==========================================================
// PATRON CREACIONAL: BUILDER
// Construye ProductoSimple paso a paso validando campos
// ==========================================================
class ProductoBuilder {
private:
	int codigo = 0;
	string nombre = "";
	double precio = 0.0;
	int stock = 0;
	int tipo = 1;
	int garantiaMeses = 0;
	
public:
	ProductoBuilder& setCodigo(int c) { codigo = c; return *this; }
	ProductoBuilder& setNombre(const string& n) { nombre = n; return *this; }
	ProductoBuilder& setPrecio(double p) { precio = p; return *this; }
	ProductoBuilder& setStock(int s) { stock = s; return *this; }
	ProductoBuilder& setTipo(int t) { tipo = t; return *this; }
	ProductoBuilder& setGarantia(int g) {
		garantiaMeses = (tipo == 3) ? g : 0;
		return *this;
	}
	
	shared_ptr<ProductoSimple> build() const {
		return make_shared<ProductoSimple>(codigo, nombre, precio, stock, tipo, garantiaMeses);
	}
};

// ==========================================================
// PATRON DE COMPORTAMIENTO: STATE
// Gestiona el estado de una Venta (Abierta, Pagada, Anulada)
// ==========================================================
class VentaContexto;

class EstadoVenta {
public:
	virtual ~EstadoVenta() = default;
	virtual string getNombreEstado() const = 0;
	virtual void pagar(VentaContexto& contexto) = 0;
	virtual void anular(VentaContexto& contexto) = 0;
};

class VentaContexto {
private:
	int id;
	string detalle;
	string pago;
	double subtotal;
	double descuento;
	double iva;
	double total;
	shared_ptr<EstadoVenta> estadoActual;
	
public:
	VentaContexto(int idVenta, const string& det, const string& formaPago,
				  double sub, double desc, double imp, double tot,
				  shared_ptr<EstadoVenta> estadoInicial)
		: id(idVenta), detalle(det), pago(formaPago),
		subtotal(sub), descuento(desc), iva(imp), total(tot),
		estadoActual(move(estadoInicial)) {}
	
	void setEstado(shared_ptr<EstadoVenta> nuevoEstado) {
		estadoActual = move(nuevoEstado);
	}
	
	string getEstadoNombre() const {
		return estadoActual ? estadoActual->getNombreEstado() : "Sin estado";
	}
	
	int getId() const { return id; }
	string getDetalle() const { return detalle; }
	string getPago() const { return pago; }
	double getTotal() const { return total; }
	
	void confirmarPago() {
		if (estadoActual) estadoActual->pagar(*this);
	}
	
	void anularVenta() {
		if (estadoActual) estadoActual->anular(*this);
	}
};

class EstadoPagada : public EstadoVenta {
public:
	string getNombreEstado() const override { return "Pagada"; }
	void pagar(VentaContexto&) override {
		cout << "La venta ya se encuentra pagada.\n";
	}
	void anular(VentaContexto& contexto) override;
};

class EstadoAnulada : public EstadoVenta {
public:
	string getNombreEstado() const override { return "Anulada"; }
	void pagar(VentaContexto&) override {
		cout << "No se puede pagar una venta anulada.\n";
	}
	void anular(VentaContexto&) override {
		cout << "La venta ya esta anulada.\n";
	}
};

class EstadoAbierta : public EstadoVenta {
public:
	string getNombreEstado() const override { return "Abierta"; }
	void pagar(VentaContexto& contexto) override {
		contexto.setEstado(make_shared<EstadoPagada>());
		cout << "Venta #" << contexto.getId() << " marcada como PAGADA.\n";
	}
	void anular(VentaContexto& contexto) override {
		contexto.setEstado(make_shared<EstadoAnulada>());
		cout << "Venta #" << contexto.getId() << " marcada como ANULADA.\n";
	}
};

void EstadoPagada::anular(VentaContexto& contexto) {
	contexto.setEstado(make_shared<EstadoAnulada>());
	cout << "Venta #" << contexto.getId() << " ha sido ANULADA.\n";
}

// ==========================================================
// CLASE GESTORA DE LA TIENDA (sin globales ni calculos duplicados)
// ==========================================================
class TiendaV1 {
private:
	vector<shared_ptr<ItemCatalogo>> catalogo;
	vector<VentaContexto> ventas;
	int contadorVentas;
	double totalRecaudado;
	
	struct LineaCalculada {
		shared_ptr<ItemCatalogo> item;
		int cantidad;
		double bruto;
		double descuento;
		double iva;
	};
	
	shared_ptr<ItemCatalogo> buscarItem(int codigo) const {
		for (const auto& item : catalogo) {
			if (item->getCodigo() == codigo) return item;
		}
		return nullptr;
	}
	
	// Calcula subtotal, descuento e IVA en un solo lugar (sin duplicar logica)
	LineaCalculada calcularLinea(const shared_ptr<ItemCatalogo>& item, int cantidad, int formaPago) const {
		double bruto = item->getPrecio() * cantidad;
		double tasaDesc = 0.0;
		
		switch (item->getTipo()) {
		case 1: // Calzado
			tasaDesc = (formaPago == 1) ? 0.10 : 0.05;
			break;
		case 2: // Ropa
			tasaDesc = (formaPago == 1) ? 0.08 : 0.0;
			break;
		case 3: // Balon/Accesorio
			tasaDesc = (formaPago == 1) ? 0.05 : 0.0;
			break;
		default:
			tasaDesc = 0.0;
			break;
		}
		
		double desc = bruto * tasaDesc;
		double imp = (bruto - desc) * ReglasTienda::IVA_TASA;
		return {item, cantidad, bruto, desc, imp};
	}
	
public:
		TiendaV1() : contadorVentas(0), totalRecaudado(0.0) {}
		
		// F1: Registrar producto usando Builder
		void registrarProducto() {
			int codigo = 0, stock = 0, tipo = 0, garantia = 0;
			double precio = 0.0;
			string nombre;
			
			cout << "\n--- REGISTRAR PRODUCTO (BUILDER) ---\n";
			cout << "Codigo: ";
			if (!(cin >> codigo)) {
				cin.clear(); cin.ignore(10000, '\n');
				cout << "Codigo invalido.\n"; return;
			}
			if (buscarItem(codigo) != nullptr) {
				cout << "Ya existe un item con ese codigo.\n"; return;
			}
			
			cin.ignore(10000, '\n');
			cout << "Nombre: ";
			getline(cin, nombre);
			
			cout << "Precio: ";
			if (!(cin >> precio) || precio <= 0) {
				cin.clear(); cin.ignore(10000, '\n');
				cout << "Precio invalido.\n"; return;
			}
			
			cout << "Stock: ";
			if (!(cin >> stock) || stock < 0) {
				cin.clear(); cin.ignore(10000, '\n');
				cout << "Stock invalido.\n"; return;
			}
			
			cout << "Tipo (1: Calzado, 2: Ropa, 3: Balones/Accesorios): ";
			if (!(cin >> tipo) || tipo < 1 || tipo > 3) {
				cin.clear(); cin.ignore(10000, '\n');
				cout << "Tipo invalido.\n"; return;
			}
			
			if (tipo == 3) {
				cout << "Meses de garantia: ";
				if (!(cin >> garantia) || garantia < 0) {
					cin.clear(); cin.ignore(10000, '\n');
					cout << "Garantia invalida.\n"; return;
				}
			}
			
			ProductoBuilder builder;
			auto nuevoProducto = builder.setCodigo(codigo)
				.setNombre(nombre)
				.setPrecio(precio)
				.setStock(stock)
				.setTipo(tipo)
				.setGarantia(garantia)
				.build();
			
			catalogo.push_back(nuevoProducto);
			cout << "Producto registrado correctamente.\n";
		}
		
		// Extra: Crear Kit Deportivo usando Composite
		void registrarKitDeportivo() {
			if (catalogo.size() < 2) {
				cout << "Registre al menos 2 productos simples antes de crear un kit.\n";
				return;
			}
			int codigoKit = 0;
			string nombreKit;
			cout << "\n--- CREAR KIT DEPORTIVO (COMPOSITE) ---\n";
			cout << "Codigo del kit: ";
			if (!(cin >> codigoKit) || buscarItem(codigoKit) != nullptr) {
				cin.clear(); cin.ignore(10000, '\n');
				cout << "Codigo invalido o ya existente.\n"; return;
			}
			cin.ignore(10000, '\n');
			cout << "Nombre del kit: ";
			getline(cin, nombreKit);
			
			auto kit = make_shared<KitDeportivo>(codigoKit, nombreKit);
			char agregarMas = 's';
			while (agregarMas == 's' || agregarMas == 'S') {
				int codProd = 0;
				cout << "Codigo de producto a incluir en el kit: ";
				if (cin >> codProd) {
					auto item = buscarItem(codProd);
					if (item && item->getTipo() != 4) {
						kit->agregarItem(item);
						cout << "Agregado al kit.\n";
					} else {
						cout << "Producto simple no encontrado.\n";
					}
				} else {
					cin.clear(); cin.ignore(10000, '\n');
				}
				cout << "Agregar otro producto al kit? (s/n): ";
				cin >> agregarMas;
			}
			
			catalogo.push_back(kit);
			cout << "Kit deportivo registrado.\n";
		}
		
		// F2: Consultar inventario
		void consultarInventario() const {
			cout << "\n--- INVENTARIO ---\n";
			if (catalogo.empty()) {
				cout << "No hay items registrados.\n";
				return;
			}
			cout << fixed << setprecision(2);
			for (const auto& item : catalogo) {
				item->mostrarDetalle();
			}
		}
		
		// F3, F4, F5: Registrar venta con State
		void registrarVenta() {
			cout << "\n--- REGISTRAR VENTA ---\n";
			if (catalogo.empty()) {
				cout << "No hay items para vender.\n";
				return;
			}
			
			vector<pair<shared_ptr<ItemCatalogo>, int>> carrito;
			char seguir = 's';
			
			while (seguir == 's' || seguir == 'S') {
				int cod = 0, cant = 0;
				cout << "Codigo del item: ";
				if (!(cin >> cod)) {
					cin.clear(); cin.ignore(10000, '\n');
					cout << "Codigo invalido.\n";
				} else {
					auto item = buscarItem(cod);
					if (!item) {
						cout << "Item no encontrado.\n";
					} else {
						cout << "Cantidad de " << item->getNombre() << ": ";
						if (!(cin >> cant) || cant <= 0) {
							cin.clear(); cin.ignore(10000, '\n');
							cout << "Cantidad invalida.\n";
						} else {
							int enCarrito = 0;
							for (const auto& par : carrito) {
								if (par.first->getCodigo() == cod) enCarrito += par.second;
							}
							if (cant + enCarrito > item->getStock()) {
								cout << "Stock insuficiente. Disponible: " << item->getStock() << "\n";
							} else {
								carrito.push_back({item, cant});
								cout << "Item agregado a la venta.\n";
							}
						}
					}
				}
				cout << "Agregar otro item? (s/n): ";
				cin >> seguir;
			}
			
			if (carrito.empty()) {
				cout << "Venta cancelada: sin items.\n";
				return;
			}
			
			int formaPago = 0;
			cout << "Forma de pago (1: Efectivo, 2: Tarjeta): ";
			if (!(cin >> formaPago) || (formaPago != 1 && formaPago != 2)) {
				cin.clear(); cin.ignore(10000, '\n');
				cout << "Forma de pago invalida. Venta cancelada.\n";
				return;
			}
			
			double subtotal = 0.0, descuentoTotal = 0.0, ivaTotal = 0.0;
			string detalle = "";
			vector<LineaCalculada> lineas;
			
			for (const auto& par : carrito) {
				LineaCalculada lin = calcularLinea(par.first, par.second, formaPago);
				lineas.push_back(lin);
				subtotal += lin.bruto;
				descuentoTotal += lin.descuento;
				ivaTotal += lin.iva;
				detalle += par.first->getNombre() + " x" + to_string(par.second) + "; ";
			}
			
			double total = subtotal - descuentoTotal + ivaTotal;
			
			cout << fixed << setprecision(2);
			cout << "\n======== FACTURA ========\n";
			for (const auto& lin : lineas) {
				cout << lin.item->getNombre() << " x" << lin.cantidad
					<< " | Bruto: $" << lin.bruto
					<< " | Desc: $" << lin.descuento
					<< " | IVA: $" << lin.iva << "\n";
			}
			cout << "-------------------------\n";
			cout << "Subtotal:   $" << subtotal << "\n";
			cout << "Descuento:  -$" << descuentoTotal << "\n";
			cout << "IVA (19%):  +$" << ivaTotal << "\n";
			cout << "TOTAL:      $" << total << "\n";
			cout << "=========================\n";
			
			for (const auto& par : carrito) {
				par.first->descontarStock(par.second);
				if (par.first->getStock() < ReglasTienda::STOCK_MINIMO_ALERTA) {
					cout << "ALERTA: Stock bajo de '" << par.first->getNombre()
						<< "' (Quedan " << par.first->getStock() << " unidades).\n";
				}
			}
			
			contadorVentas++;
			VentaContexto venta(contadorVentas, detalle,
								(formaPago == 1 ? "Efectivo" : "Tarjeta"),
								subtotal, descuentoTotal, ivaTotal, total,
								make_shared<EstadoAbierta>());
			
			char confirmar = 's';
			cout << "Confirmar pago de la venta ahora? (s/n): ";
			cin >> confirmar;
			if (confirmar == 's' || confirmar == 'S') {
				venta.confirmarPago();
				totalRecaudado += total;
			} else {
				cout << "Venta queda en estado: " << venta.getEstadoNombre() << "\n";
			}
			
			ventas.push_back(venta);
			cout << "Venta registrada con exito.\n";
		}
		
		// F6: Reporte de ventas
		void reporteDeVentas() const {
			cout << "\n--- REPORTE DE VENTAS ---\n";
			cout << "Total de transacciones: " << contadorVentas << "\n";
			cout << "Dinero total recaudado (pagadas): $" << totalRecaudado << "\n";
			if (ventas.empty()) {
				cout << "No hay ventas registradas.\n";
				return;
			}
			for (const auto& v : ventas) {
				cout << "Venta #" << v.getId()
					<< " | Estado: " << v.getEstadoNombre()
					<< " | Detalle: " << v.getDetalle()
					<< " | Pago: " << v.getPago()
					<< " | Total: $" << v.getTotal() << "\n";
			}
		}
};

int main() {
	TiendaV1 miTienda;
	int opcion = 0;
	
	do {
		cout << "\n=====================================\n";
		cout << "   SISTEMA TIENDA DEPORTIVA (v1)     \n";
		cout << "=====================================\n";
		cout << "1. Registrar producto \n";
		cout << "2. Crear kit deportivo \n";
		cout << "3. Consultar inventario \n";
		cout << "4. Registrar venta \n";
		cout << "5. Reporte de ventas \n";
		cout << "6. Salir\n";
		cout << "Seleccione una opcion: ";
		
		if (!(cin >> opcion)) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Opcion invalida.\n";
			continue;
		}
		
		switch (opcion) {
		case 1: miTienda.registrarProducto(); break;
		case 2: miTienda.registrarKitDeportivo(); break;
		case 3: miTienda.consultarInventario(); break;
		case 4: miTienda.registrarVenta(); break;
		case 5: miTienda.reporteDeVentas(); break;
		case 6: cout << "Saliendo del programa...\n"; break;
		default: cout << "Opcion no valida.\n"; break;
		}
	} while (opcion != 6);
	
	return 0;
}
