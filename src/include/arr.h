#ifndef LFLOW_ARR_H
#define LFLOW_ARR_H

typedef struct {
    void **base;
    unsigned int length;
} Array;

Array *Array_Create();
void Array_Destroy(Array *);
void Array_DestroyCallBack(Array *, void (*)(void *));
void Array_Push(Array *, void *);
void *Array_At(Array *, unsigned);

#endif