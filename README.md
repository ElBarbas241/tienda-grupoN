# Proyecto Tienda Deportiva - Grupo N

## Integrantes y Roles
- **Juan Naranjo**: Responsable de diseño.
- **Esteban Duque**: Responsable de código.
- **Valerie Espinosa**: Responsable de modelado.

## Descripción
Aplicación de consola en C++ para la gestión de una **tienda deportiva** (inventario, kits deportivos, ventas, control de stock y reportes), desarrollada para la asignatura de Patrones de Software.

## Patrones de Diseño Aplicados (`v1/`)
- **Builder**: Construcción estructurada de productos deportivos (`ProductoSimple`) según tipo/categoría.
- **Composite**: Representación uniforme de productos individuales y kits deportivos (`ItemCatalogo` / `KitDeportivo`) para consulta, precio y gestión de stock.
- **State**: Gestión del ciclo de vida de la venta (`VentaContexto` y `EstadoVenta`, con transiciones como `EstadoAbierta` -> `EstadoPagada` al confirmar el pago).

## Compilación y Ejecución

### Opción 1: Desde ZinjaI (IDE)
1. **Versión 0 (sin modificar)**: Abrir el archivo principal (o proyecto `.zpr`) dentro de la carpeta de la versión 0 (`v0/`) en **ZinjaI** y presionar **F9** (o **Ejecutar -> Compilar y Ejecutar**).
2. **Versión 1 (completa)**: Abrir el proyecto `.zpr` (o `main.cpp` junto con los módulos `.h` y `.cpp` de `v1/`) en **ZinjaI** y presionar **F9** (o **Ejecutar -> Compilar y Ejecutar**).

### Opción 2: Desde consola (`g++`)
```bash
# Versión 0
g++ -std=c++17 v0/*.cpp -o v0/tienda_v0
./v0/tienda_v0

# Versión 1
g++ -std=c++17 v1/*.cpp -o v1/tienda_v1
./v1/tienda_v1
