#ifndef QUEUE_H
#define QUEUE_H

// Incluye la biblioteca estándar para el tipo de dato bool (verdadero/falso).
#include <stdbool.h>

// Define la estructura 'element' que representa un elemento en la cola.
typedef struct {
    int product_id; // Identificador del producto.
    int op;         // Operación a realizar con el elemento.
    int units;      // Unidades del producto.
} element;

// Define la estructura 'queue' que representa la cola.
typedef struct {
    element *elems; // Puntero a un arreglo de elementos 'element'.
    int size;       // Tamaño actual de la cola.
    int capacity;   // Capacidad máxima de la cola.
    int front;      // Índice del primer elemento de la cola.
    int rear;       // Índice del último elemento de la cola.
} queue;

// Declaraciones de las funciones para operar sobre la cola.
queue *queue_init(int capacity); // Inicializa una nueva cola con una capacidad dada.
void queue_destroy(queue *q);    // Destruye la cola y libera la memoria.
void queue_put(queue *q, element *elem); // Añade un elemento a la cola.
element *queue_get(queue *q);    // Obtiene y elimina el primer elemento de la cola.
bool queue_empty(queue *q);      // Verifica si la cola está vacía.
bool queue_full(queue *q);       // Verifica si la cola está llena.

#endif /* QUEUE_H */
