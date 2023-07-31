#include <stdio.h>

#include "libber.h"


Stack stack_init() {
    Stack s;
    s.top = -1;
    return s;
}

int stack_empty(Stack *stack) {
    return stack->top == -1;
}

int stack_full(Stack *stack) {
    return stack->top == LIBBER_MAX_SIZE - 1;
}

int stack_pop(Stack *stack) {
    if (stack_empty(stack)) {
        return -1;
    };
    return stack->data[stack->top--];
}

int stack_peek(Stack *stack) {
    if (stack_empty(stack)) {
        return -1;
    };
    return stack->data[stack->top];
}

int stack_push(Stack *stack, uint16_t value) {
    if (stack_full(stack)) {
        return -1;
    }
    return stack->data[++stack->top] = value;
}
