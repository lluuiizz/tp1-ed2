#ifndef __PAG_H__
#define __PAG_H__



#include "reg.h"
#define M 2
#define MM 2*M

typedef struct tipo_pagina* tipo_apontador;
typedef reg_t tipo_registro;

typedef struct tipo_pagina {
    int n;
    tipo_registro r[MM];
    tipo_apontador p[MM+1];
} tipo_pagina_t;





#endif // __PAG_H__
