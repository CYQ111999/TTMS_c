#pragma once
#ifndef TICKET_SRV_H
#define TICKET_SRV_H

#include "Common.h"

int Ticket_Srv_GenBatch(int schedule_id, int stuID);
int Ticket_Srv_DeleteBatch(int schedule_id);
int Ticket_Srv_FetchBySchID(int id, ticket_list_t list);
ticket_list_t* Ticket_Srv_FetchBySeatID(ticket_list_t list, int seat_id);
int Ticket_Srv_Modify(const ticket_t* data);
int Ticket_Srv_FetchByID(int id, ticket_t* buf);

#endif