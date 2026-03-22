#include "Sale.h"
#include "Sale_Persist.h"
#define _CRT_SECURE_NO_WARNINGS
int Sale_Srv_Add(const sale_t* data) {
    return Sale_Perst_Insert(data);
}