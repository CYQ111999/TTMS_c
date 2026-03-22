#ifndef SALE_PERSIST_H
#define SALE_PERSIST_H

#include "Common.h"

int Sale_Perst_Insert(const sale_t* data);
int Sale_Perst_SelByTicket(int ticket_id, sale_list_t list);

#endif