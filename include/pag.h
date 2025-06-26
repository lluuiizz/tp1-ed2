#ifndef __PAG_H__
#define __PAG_H__



#include "reg.h"
#define ORDEM 1000

typedef struct tipo_pagina* tipo_apontador;
typedef reg_t tipo_registro;

struct tipo_pagina {
    int n;
    tipo_registro r[ORDEM];
    tipo_apontador p[ORDEM+1];
};





#endif // __PAG_H__
