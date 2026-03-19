#include "Sale_Persist.h"
#include "EntityKey.h"
#include <stdio.h>

static const char* SALE_FILE = "Sale.dat";

int Sale_Perst_Insert(const sale_t* data) {
    FILE* fp = fopen(SALE_FILE, "ab");
    if (!fp) return 0;

    sale_t sale = *data;
    int newID;
    if (EntKey_Perst_GetNewKeys(1, &newID) != 1) {
        fclose(fp);
        return 0;
    }
    sale.id = newID;

    int rtn = 0;
    if (fwrite(&sale, sizeof(sale_t), 1, fp) == 1) rtn = 1;
    fclose(fp);
    return rtn;
}