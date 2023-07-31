#include <stdint.h>


#define LIBBER_MAX_SIZE 1000


/* -------------------------- STACK ----------------------------- */

typedef struct {
    uint16_t data[LIBBER_MAX_SIZE];
    int top;
} Stack;

Stack   stack_init();
int     stack_empty(Stack *stack);
int     stack_full(Stack *stack);
int     stack_pop(Stack *stack);
int     stack_peek(Stack *stack);
int     stack_push(Stack *stack, uint16_t value);
