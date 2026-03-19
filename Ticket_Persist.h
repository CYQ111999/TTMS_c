#pragma once
#ifndef TICKET_PERSIST_H
#define TICKET_PERSIST_H

#include "Common.h"

int Ticket_Perst_Insert(int schedule_id, seat_list_t list);
int Ticket_Perst_Rem(int schedule_id);
int Ticket_Perst_SelBySchID(int id, ticket_list_t list);
int Ticket_Perst_Update(const ticket_t* data);
int Ticket_Perst_SelectByID(int id, ticket_t* buf);

#endif