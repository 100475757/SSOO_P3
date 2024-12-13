#include <stdlib.h>
#include "queue.h"

// Inicializa una cola con una capacidad dada.
queue *queue_init(int capacity) {
    // Intenta asignar memoria para la estructura de la cola.
    queue *q = (queue *)malloc(sizeof(queue));
    if (q == NULL) {
        return NULL; // Si la asignación falla, devuelve NULL.
    }
    // Intenta asignar memoria para los elementos de la cola.
    q->elems = (element *)malloc(capacity * sizeof(element));
    if (q->elems == NULL) {
        free(q); // Si la asignación falla, libera la memoria de la cola y devuelve NULL.
        return NULL;
    }
    // Inicializa los atributos de la estructura de la cola.
    q->size = 0; // Tamaño inicial de la cola es 0.
    q->capacity = capacity; // Capacidad establecida según el argumento.
    q->front = 0; // El índice del primer elemento es 0.
    q->rear = -1; // El índice del último elemento es -1 (cola vacía).
    return q; // Devuelve el puntero a la nueva cola.
}

// Destruye la cola y libera la memoria asignada.
void queue_destroy(queue *q) {
    free(q->elems); // Libera la memoria de los elementos.
    free(q); // Libera la memoria de la estructura de la cola.
}

// Añade un elemento a la cola si hay espacio.
void queue_put(queue *q, element *elem) {
    if (!queue_full(q)) { // Verifica si la cola no está llena.
        q->rear = (q->rear + 1) % q->capacity; // Actualiza el índice del último elemento.
        q->elems[q->rear] = *elem; // Añade el nuevo elemento al final de la cola.
        q->size++; // Incrementa el tamaño de la cola.
    }
}

// Obtiene y elimina el primer elemento de la cola.
element *queue_get(queue *q) {
    if (!queue_empty(q)) { // Verifica si la cola no está vacía.
        element *elem = &(q->elems[q->front]); // Obtiene el primer elemento.
        q->front = (q->front + 1) % q->capacity; // Actualiza el índice del primer elemento.
        q->size--; // Decrementa el tamaño de la cola.
        return elem; // Devuelve el elemento obtenido.
    }
    return NULL; // Si la cola está vacía, devuelve NULL.
}

// Verifica si la cola está vacía.
bool queue_empty(queue *q) {
    return q->size == 0; // Devuelve verdadero si el tamaño es 0.
}

// Verifica si la cola está llena.
bool queue_full(queue *q) {
    return q->size == q->capacity; // Devuelve verdadero si el tamaño es igual a la capacidad.
}
