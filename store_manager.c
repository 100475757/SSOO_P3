#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

#define NUM_PRODUCTS 5

// Estructuras para las operaciones y argumentos/resultados de hilos.
typedef struct {
    int product_id; // Identificador del producto.
    int op; // Operación: 0 para compra, 1 para venta.
    int units;  // Unidades involucradas en la operación.
} operation;

typedef struct {
    operation *ops; // Puntero a las operaciones asignadas.
    int start_idx;  // Índice de inicio en el array de operaciones.
    int end_idx;    // Índice de fin en el array de operaciones.
} producer_arg;

typedef struct {
    int profit; // Beneficio acumulado por el consumidor.
    int *stock; // Stock de productos actualizado por el consumidor.
} consumer_result;

// Variables globales para la cola y sincronización.
queue *buffer;  // Cola circular para las operaciones.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex para sincronización.
pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER;  // Variable de condición para producción.
pthread_cond_t can_consume = PTHREAD_COND_INITIALIZER;  // Variable de condición para consumo.

// Prototipos de funciones.
operation *load_operations(const char *filename, int *total_ops);
void process_task(consumer_result *result, element *task);


// Cargar operaciones desde un archivo.
operation *load_operations(const char *filename, int *total_ops) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo '%s'\n", filename);
        return NULL;
    }

    fscanf(file, "%d", total_ops);
    operation *ops = malloc(*total_ops * sizeof(operation));

    char operation_type[10];
    for (int i = 0; i < *total_ops; i++) {
        fscanf(file, "%d %s %d", &ops[i].product_id, operation_type, &ops[i].units);
        if (strcmp(operation_type, "PURCHASE") == 0) {
            ops[i].op = 0;
        } else if (strcmp(operation_type, "SALE") == 0) {
            ops[i].op = 1;
        }
    }

    fclose(file);
    return ops;
}


// Función del hilo productor.
void *producer(void *arg) {
    // Convierte el argumento a un puntero de tipo 'producer_arg'.
    producer_arg *p_arg = (producer_arg *)arg;

    // Bucle que recorre un rango de índices desde 'start_idx' hasta 'end_idx'.
    for (int i = p_arg->start_idx; i <= p_arg->end_idx; i++) {
        // Asigna memoria para un nuevo elemento.
        element *elem = malloc(sizeof(element));
        // Establece los valores del elemento según las operaciones en 'p_arg'.
        elem->product_id = p_arg->ops[i].product_id;
        elem->op = p_arg->ops[i].op;
        elem->units = p_arg->ops[i].units;

        // Bloquea el mutex antes de acceder a la cola compartida.
        pthread_mutex_lock(&mutex);
        // Espera mientras la cola esté llena.
        while (queue_full(buffer)) {
            // Espera en la variable de condición 'can_produce'.
            pthread_cond_wait(&can_produce, &mutex);
        }
        // Añade el elemento a la cola.
        queue_put(buffer, elem);
        // Señala a la variable de condición 'can_consume' que hay un nuevo elemento.
        pthread_cond_signal(&can_consume);
        // Desbloquea el mutex después de modificar la cola.
        pthread_mutex_unlock(&mutex);
    }

    // Termina el hilo y devuelve NULL.
    pthread_exit(NULL);
}


// Función del hilo consumidor.
void *consumer(void *arg) {
    // Convierte el argumento a un puntero de tipo 'int' para obtener el número de iteraciones.
    int *iterations = (int *)arg;
    // Asigna memoria para almacenar el resultado del consumidor.
    consumer_result *result = malloc(sizeof(consumer_result));
    if (result == NULL) {
        pthread_exit(NULL); // Si la asignación falla, termina el hilo.
    }

    result->profit = 0; // Inicializa el beneficio acumulado a 0.
    // Asigna memoria para llevar un registro del stock de productos, inicializado a 0.
    result->stock = calloc(NUM_PRODUCTS, sizeof(int));
    if (result->stock == NULL) {
        free(result); // Si la asignación falla, libera la memoria del resultado y termina el hilo.
        pthread_exit(NULL);
    }

    // Bucle que se ejecuta un número determinado de veces.
    for (int i = 0; i < *iterations; i++) {
        pthread_mutex_lock(&mutex); // Bloquea el mutex antes de acceder a la cola compartida.
        while (queue_empty(buffer)) { // Espera mientras la cola esté vacía.
            pthread_cond_wait(&can_consume, &mutex); // Espera en la variable de condición 'can_consume'.
        }
        element *task = queue_get(buffer); // Obtiene el primer elemento de la cola.
        process_task(result, task); // Procesa la tarea obtenida.
        
        // No se libera la memoria de 'task' aquí, ya que es un puntero a un elemento en el buffer.
        pthread_cond_signal(&can_produce); // Señala que hay espacio para producir más elementos.
        pthread_mutex_unlock(&mutex); // Desbloquea el mutex.
    }

    pthread_exit(result); // Termina el hilo y devuelve el resultado del consumidor.
}


// Procesar cada tarea de la cola.
void process_task(consumer_result *result, element *task) {
    int price[] = {2, 5, 15, 25, 100};
    int sale_price[] = {3, 10, 20, 40, 125};

    int idx = task->product_id - 1;

    if (task->op == 0) {  // PURCHASE
        result->stock[idx] += task->units;
        result->profit -= task->units * price[idx];
    } else if (task->op == 1) {  // SALE
        result->stock[idx] -= task->units;
        result->profit += task->units * sale_price[idx];
    }
}

// Función principal.
int main(int argc, const char *argv[]) {
    // Verifica que se hayan pasado 4 argumentos al programa.
    if (argc != 5) {
        printf("USO: ./store_manager <archivo_operaciones> <num_productores> <num_consumidores> <tamaño_buffer>\n");

        return -1;
    }

    // Inicializa las variables para los beneficios y el stock de productos.
    int profits = 0;
    int product_stock[NUM_PRODUCTS] = {0};

    // Carga las operaciones desde un archivo y verifica que se hayan cargado correctamente.
    int total_ops = 0;
    operation *ops = load_operations(argv[1], &total_ops);
    if (ops == NULL) {
        printf("Error al cargar las operaciones del archivo '%s'\n", argv[1]);
        return -1;
    }

    // Convierte los argumentos a enteros y verifica que sean válidos.
    int num_producers = atoi(argv[2]);
    if (num_producers < 1) {
        printf("[ERROR] El número de productores debe ser mayor que 0\n");
        return -1;
    }

    int num_consumers = atoi(argv[3]);
    if (num_consumers < 1) {
        printf("[ERROR] El número de consumidores debe ser mayor que 0\n");
        return -1;
    }

    int buffer_size = atoi(argv[4]);
    if (buffer_size < 1) {
        printf("[ERROR] El tamaño del buffer debe ser mayor que 0\n");
        return -1;
    }

    // Crea los hilos para productores y consumidores.
    pthread_t producers[num_producers], consumers[num_consumers];
    producer_arg p_args[num_producers];
    // Calcula la cantidad de operaciones por hilo productor y consumidor.
    int ops_per_prod_thread = total_ops / num_producers;
    int ops_per_cons_thread = total_ops / num_consumers;
    // Calcula las operaciones restantes para el último hilo consumidor.
    int ops_last_const_thread = total_ops % num_consumers;

    // Inicializa el buffer de la cola con el tamaño especificado.
    buffer = queue_init(buffer_size);
    if (buffer == NULL) {
        printf("[ERROR] Fallo al inicializar el buffer\n");
        return -1;
    }

    // Crea los hilos productores y les asigna sus operaciones.
    for (int i = 0; i < num_producers; i++) {
        p_args[i].ops = ops;
        p_args[i].start_idx = i * ops_per_prod_thread;
        p_args[i].end_idx = (i + 1) * ops_per_prod_thread - 1;
        if (i == num_producers - 1) {
            // Asegura que el último hilo productor procese todas las operaciones restantes.
            p_args[i].end_idx = total_ops - 1;
        }
        pthread_create(&producers[i], NULL, producer, (void *)&p_args[i]);
    }

    // Crea los hilos consumidores.
    for (int i = 0; i < num_consumers; i++) {
        if (i == num_consumers - 1 && ops_last_const_thread != 0) {
            // Ajusta las operaciones para el último hilo consumidor si es necesario.
            ops_last_const_thread = ops_per_cons_thread + ops_last_const_thread;
            pthread_create(&consumers[i], NULL, consumer, &ops_last_const_thread);
        } else {
            pthread_create(&consumers[i], NULL, consumer, &ops_per_cons_thread);
        }
    }

    // Espera a que todos los hilos productores terminen.
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

    // Espera a que todos los hilos consumidores terminen y recoge sus resultados.
    consumer_result *results[num_consumers];
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], (void **)&results[i]);
    }

    // Calculamos el beneficio total y actualiza el stock de productos.
    for (int i = 0; i < num_consumers; i++) {
        consumer_result *result = results[i];
        profits += result->profit;
        for (int j = 0; j < NUM_PRODUCTS; j++) {
            product_stock[j] += result->stock[j];
        }
        // Libera la memoria asignada para el stock y el resultado del consumidor.
        free(results[i]->stock);
        free(result);
    }

    // Verificamos que el stock de cada producto no sea negativo.
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        if (product_stock[i] < 0) {
            printf("[ERROR] El stock del producto %d no puede ser negativo\n", i + 1);
            return -1;
        }
    }

    // Imprimimos el beneficio total y el stock de productos.
    printf("Total: %d euros\n", profits);
    printf("Stock:\n");
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("  Product %d: %d\n", i + 1, product_stock[i]);
    }

    // Destruimos los mutex y las variables de condición, y termina el programa.
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_produce);
    pthread_cond_destroy(&can_consume);

    return 0;
}