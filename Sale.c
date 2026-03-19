#include "Sale.h"
#include "Sale_Persist.h"

int Sale_Srv_Add(const sale_t* data) {
    return Sale_Perst_Insert(data);
}