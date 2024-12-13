# Práctica 3 - Sistemas Operativos 23/24

Este proyecto es la práctica 3 de la asignatura de Sistemas Operativos (SSOO) del curso 2023/2024. El objetivo de esta práctica es implementar un gestor de tienda que simula la producción y consumo de operaciones de compra y venta de productos utilizando hilos, semáforos y una cola circular para la sincronización de los mismos.

# Funcionalidad

El proyecto incluye tres programas:

1. queue.h:
   - Define las estructuras y funciones necesarias para manejar una cola circular de elementos.
   - Proporciona funciones para inicializar, destruir, añadir y obtener elementos de la cola, así como para verificar si la cola está vacía o llena.

2. queue.c:
   - Implementa las funciones definidas en queue.h.
   - Incluye la lógica para inicializar y destruir la cola, añadir elementos a la cola, obtener elementos de la cola, y verificar si la cola está vacía o llena.

3. store_manager.c:
   - Implementa un gestor de tienda que simula la producción y consumo de operaciones de compra y venta de productos.
   - Utiliza hilos y semáforos para sincronizar la producción y consumo de operaciones.
   - Lee las operaciones de compra y venta de un archivo de texto y las añade a una cola circular.
   - Crea un hilo productor que lee las operaciones de la cola y las procesa.
   - Crea un hilo consumidor que lee las operaciones de la cola y las procesa.
   - Imprime el estado de la tienda (número de productos en stock y caja) en cada iteración.

# Compilación

Para compilar el proyecto se proporciona un archivo Makefile que incluye las reglas necesarias para compilar los programas. Para compilar el proyecto, ejecuta el siguiente comando en la terminal:

make 

# Uso 

Una vez compilado el proyecto, puedes ejecutar el programa store_manager con el siguiente comando:

./store_manager <archivo_operaciones> <n_productores> <n_consumidores> <tamaño_buffer>

Donde:
- `<archivo_operaciones>` es el archivo de texto que contiene las operaciones de compra y venta de productos.
- `<n_productores>` es el número de hilos productores que se crearán.
- `<n_consumidores>` es el número de hilos consumidores que se crearán.
- `<tamaño_buffer>` es el tamaño del buffer de la cola circular.

Proporcionamos dos archivos de pruebas para el programa store_manager, solo hace falta especificar el resto de argumentos. 

# Requisitos 

El proyecto ha sido desarrollado y probado en un entorno Linux. Para compilar y ejecutar el proyecto, necesitarás tener instalado un compilador de C (como gcc) y la utilidad make.
