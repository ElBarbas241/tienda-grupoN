Informe de Antipatrones y Olores de Código (Versión 0)
 

---

1. Introducción
El análisis estricto del código fuente de la Versión 0 (`v0/tienda_v0.cpp`) permitió identificar vicios de diseño, acoplamiento excesivo y violación de principios sólidos de ingeniería de software . Este documento detalla los cinco principales antipatrones encontrados, los cuales servirán de base conceptual para justificar la migración hacia la Versión 1 rediseñada con patrones.

---

2. Diagnóstico de Antipatrones en la Versión 0

A. Antipatrón 1: Clase "Dios" (God Class)
Ubicación exacta: `v0/tienda_v0.cpp`, clase `Tienda` (Líneas 28 a 292).
Descripción del problema: La clase `Tienda` concentra absolutamente toda la lógica del sistema: administra los vectores en memoria del inventario y las ventas, implementa las seis funciones obligatorias (F1 a F6), procesa las entradas y salidas por consola, valida reglas de negocio y calcula operaciones matemáticas.
Impacto y mantenimiento: Viola el Principio de Responsabilidad Única (SRP). Cualquier cambio mínimo en la visualización, en las reglas de cálculo o en la estructura de almacenamiento obliga a modificar esta única clase, volviéndola frágil y propensa a errores.

B. Antipatrón 2: Condicionales Complejos por Tipo (Switch Statement Smell)
Ubicación exacta: `v0/tienda_v0.cpp`, método `registrarVenta()` (Líneas 165 a 198 y 219 a 240).
Descripción del problema: Se utilizan estructuras `switch` y bloques condicionales anidados evaluando el atributo entero `p.tipo` (1: Calzado, 2: Ropa, 3: Balones/Accesorios) para determinar de forma imperativa los descuentos y los cálculos del IVA.
Impacto y mantenimiento: Viola el Principio de Abierto/Cerrado (OCP). Introducir un nuevo tipo de producto deportivo (por ejemplo, suplementos o equipos) exige modificar directamente la estructura condicional en múltiples lugares del código fuente.

C. Antipatrón 3: Números Mágicos (Magic Numbers)
Ubicación exacta: `v0/tienda_v0.cpp`, métodos `registrarProducto()` y `registrarVenta()` (Líneas 169, 171, 173, 178, 186, 248).
Descripción del problema: Se emplean valores numéricos literales "quemados" directamente en la lógica operativa (como `0.19` para el IVA, porcentajes fijos de descuento como `0.10` o `0.08`, y el límite numérico `5` para controlar las alertas de stock bajo).
Impacto y mantenimiento: Reduce drásticamente la legibilidad del código y complica el mantenimiento, ya que cualquier modificación en las políticas fiscales o comerciales requiere buscar manualmente los números en todo el archivo.

D. Antipatrón 4: Código Duplicado (Duplicated Code)
Ubicación exacta: `v0/tienda_v0.cpp`, método `registrarVenta()` (Líneas 165–198 duplicadas de forma idéntica en las líneas 219–240).
Descripción del problema: La lógica completa para calcular los subtotales, los descuentos según la forma de pago y el IVA se programó dos veces consecutivas: la primera para acumular los valores numéricos de la transacción y la segunda para generar la impresión formateada de la factura en pantalla.
Impacto y mantenimiento: Duplicación innecesaria de líneas de código. Si una regla comercial cambia (por ejemplo, el tratamiento de un descuento), debe actualizarse de manera sincronizada en ambos bloques, aumentando el riesgo de inconsistencias.

E. Antipatrón 5: Variables Globales y Exposición de Estructuras
Ubicación exacta: `v0/tienda_v0.cpp`, ámbito global (Líneas 9 a 26).
Descripción del problema: Los indicadores transaccionales `contadorVentas` y `totalRecaudado` están declarados como variables globales libres. Asimismo, los registros `Producto` y `Venta` exponen todos sus atributos como públicos sin encapsulamiento ni métodos de acceso seguros.
Impacto y mantenimiento: Rompe la encapsulación orientada a objetos, permitiendo que cualquier función o sección del programa modifique el estado financiero o de inventario de forma descontrolada.
