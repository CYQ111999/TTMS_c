#ifndef TICKET_PERSIST_H
#define TICKET_PERSIST_H

#include "Common.h"

int Ticket_Perst_SelByID(int id, ticket_t* buf);
int Ticket_Perst_SelectAll(ticket_list_t head);
int Ticket_Perst_SelectByScheduleID(int schedule_id, ticket_list_t head);
int Ticket_Perst_Insert(int schedule_id, seat_list_t list);
int Ticket_Perst_Rem(int schedule_id);
int Ticket_Perst_Update(const ticket_t* data);

#endif