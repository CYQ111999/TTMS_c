#ifndef SALE_UI_H
#define SALE_UI_H

#include "Common.h"

void Sale_UI_MgtEntry(void);
void Sale_UI_ShowScheduler(int playID);
int Sale_UI_SellTicket(ticket_list_t tickList, seat_list_t seatList);
void Sale_UI_RetfundTicket(void);

#endif