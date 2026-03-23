// Tickets.h
#ifndef TICKETS_H_
#define TICKETS_H_

#include "Common.h"

// 票务服务接口
int Ticket_Srv_Modify(const ticket_t* data);
int Ticket_Srv_DeleteAllByScheduleID(int schedule_id);
int Ticket_Srv_FetchAll(ticket_list_t head);
int Ticket_Srv_GenBatch(int schedule_id, int studio_id);
int Ticket_Srv_DeleteBatch(int schedule_id);
int Ticket_Srv_FetchBySchID(int schedule_id, ticket_list_t head);
int Ticket_Srv_FetchByID(int id, ticket_t* buf);
int Ticket_SrvFetchByID(int id, ticket_t* buf);  // 解决链接错误
ticket_list_t Ticket_Srv_FetchBySeatID(ticket_list_t list, int seat_id);

#endif