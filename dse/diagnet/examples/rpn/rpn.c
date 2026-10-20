// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <dse/clib/collections/vector.h>
#include <dse/diagnet/examples/rpn/rpn.h>


static Vector stack;

__attribute__((constructor)) void rpn_init(void)
{
    stack = vector_make(sizeof(int), 0, NULL);
}

static void _print(const char* op)
{
    printf("%-8s [", op);
    for (size_t i = 0; i < vector_len(&stack); i++) {
        printf("%s%d", i ? " " : "", *(int*)vector_at(&stack, i, NULL));
    }
    printf("]\n");
}

static int _pop(void)
{
    int v = 0;
    vector_pop(&stack, &v);
    _print("pop");
    return v;
}

void rpn_push(int value)
{
    vector_push(&stack, &value);
    _print("push");
}

int rpn_pop(void)
{
    int v = _pop();
    _print("pop");
    return v;
}

void rpn_add(void)
{
    int b = _pop();
    int a = _pop();
    vector_push(&stack, &(int){ a + b });
    _print("add");
}

void rpn_minus(void)
{
    int b = _pop();
    int a = _pop();
    vector_push(&stack, &(int){ a - b });
    _print("minus");
}

void rpn_multiply(void)
{
    int b = _pop();
    int a = _pop();
    vector_push(&stack, &(int){ a * b });
    _print("multiply");
}
