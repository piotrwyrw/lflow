#ifndef LFLOW_XSTRING_H
#define LFLOW_XSTRING_H

typedef struct {
    char *str;
    unsigned int length;
} XString;

XString *XString_Create();
void XString_Destroy(XString *);
void XString_Append(XString *, char);
char XString_Last(XString *);

#endif