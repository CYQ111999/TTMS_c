// Sale_UI.h
#ifndef SALE_UI_H
#define SALE_UI_H

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

	void Sale_UI_MgtEntry(void);
	void Sale_UI_RefundTicket(void);
	void Sale_UI_ShowScheduler(int playID);
	int Sale_UI_SellTicket(ticket_list_t tickList, seat_list_t seatList);

#ifdef __cplusplus
}
#endif

#endif /* SALE_UI_H */