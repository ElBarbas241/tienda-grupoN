Informe de Justificación de Patrones de Software (Versión 1)
Proyecto: Sistema de Gestión para Tienda Deportiva  

Responsable de Diseño: Juan Naranjo  

---

1. Introducción
El análisis detallado del código fuente de la Versión 0 (`v0/tienda_v0.cpp`) evidenció problemas críticos de acoplamiento, rigidez ante la adición de nuevos tipos de elementos y mezcla de responsabilidades en la clase principal ("Clase Dios"). Para resolver estos problemas sin alterar el dominio del negocio, se ha rediseñado la arquitectura incorporando tres patrones de software: "Builder" (Creacional), "Composite" (Estructural) y "State" (De Comportamiento), asegurando que todos participen de forma integrada en el flujo principal de la venta.

---

2. Selección e Integración de los Patrones

A. Patrón Creacional: Builder
Antipatrón corregido en la Versión 0: Inicialización y construcción rígida de objetos en el método `registrarProducto()`, donde la creación de un artículo deportivo depende de múltiples validaciones condicionales y campos opcionales (como `garantiaMeses` condicionado al tipo 3) directamente dentro de la lógica principal.
Justificación de diseño: El patrón Builder separa la construcción de un objeto complejo (un artículo deportivo con especificaciones variables, tallas, accesorios o garantías) de su representación final, permitiendo un armado paso a paso, limpio y sin constructores saturados.

B. Patrón Estructural: Composite
Antipatrón corregido en la Versión 0: Manejo plano y limitado del inventario exclusivamente a nivel de productos individuales (`vector<Producto>`), lo que imposibilita gestionar de manera natural paquetes, combos o kits deportivos (por ejemplo, un "Kit de Fútbol" que agrupa balón, espinilleras y pantaloneta) sin recurrir a estructuras condicionales repetitivas.
Justificación de diseño: El patrón "Composite" permite componer objetos en estructuras de árbol para representar jerarquías de todo-parte. Gracias a esto, el sistema trata a los productos individuales y a los kits compuestos de manera totalmente uniforme a través de una interfaz común.

C. Patrón de Comportamiento: State
Antipatrón corregido en la Versión 0:Control rígido del flujo de las transacciones mediante condicionales o estados implícitos mezclados dentro de la clase "Dios" al momento de registrar y procesar una venta.
Justificación de diseño:** El patrón "State" permite que un objeto (en este caso, el Pedido o la Venta) altere su comportamiento cuando su estado interno cambia. Así, los estados como *Abierto*, *Pagado* o *Anulado* se manejan mediante clases separadas, evitando bloques `switch` o `if-else` gigantes en el flujo de la caja.

---

3. Integración en el Flujo Principal de Venta
Los tres patrones seleccionados trabajan de forma colaborativa durante el flujo principal de la aplicación (`registrarVenta`):
1. Builderinterviene preparando y ensamblando los ítems o detalles del pedido que ingresan al sistema de forma estructurada.
2.Composite procesa de manera uniforme tanto los artículos deportivos individuales como los kits o combos agregados a la factura.
3. State gestiona las transiciones de la venta (pasando de un estado abierto a pagado o anulado) de forma limpia y desacoplada.
