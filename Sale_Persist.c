#include "Sale_Persist.h"
#include "EntityKey.h"
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS

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

int Sale_Perst_SelByTicket(int ticket_id, sale_list_t list) {
    FILE* fp = fopen("Sale.dat", "rb");
    if (fp == NULL) return -1;
    sale_t data;
    int count = 0;
    while (fread(&data, sizeof(sale_t), 1, fp) == 1) {
        if (data.ticket_id == ticket_id) {
            sale_list_node_t* node = (sale_list_node_t*)malloc(sizeof(sale_list_node_t));
            if (!node) {
                fclose(fp);
                return -1;
            }
            node->data = data;
            List_AddTail(list, node);
            count++;
        }
    }
    fclose(fp);
    return count;
}