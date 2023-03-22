#ifndef LFLOW_ARR_H
#define LFLOW_ARR_H

typedef struct {
    void **base;
    unsigned int length;
} Array;

Array *Array_Create();
void Array_Destroy(Array *);
void Array_Push(Array *, void *);
void *Array_At(Array *, int);

#endif