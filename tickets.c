#include "Ticket.h"
#include "Ticket_Persist.h"
#include "Seat.h"
#include "Schedule.h"
#include "Common.h"
#include <stdlib.h>

int Ticket_Srv_GenBatch(int schedule_id, int stuID) {
    seat_list_t seat_head = NULL;
    schedule_t sch;

    if (!Schedule_Srv_FetchByID(schedule_id, &sch))
        return -1;

    int count = Seat_Srv_FetchValidByRoomID(sch.studio_id, &seat_head);
    if (count <= 0) return 0;

    int result = Ticket_Perst_Insert(schedule_id, seat_head);
    SeatList_Destroy(seat_head);
    return result >= 0 ? count : -1;
}

int Ticket_Srv_DeleteBatch(int schedule_id) {
    return Ticket_Perst_Rem(schedule_id);
}

int Ticket_Srv_FetchBySchID(int id, ticket_list_t* list) {
    return Ticket_Perst_SelBySchID(id, list);
}

ticket_list_t Ticket_Srv_FetchBySeatID(ticket_list_t list, int seat_id) {
    ticket_list_t p = list;
    while (p) {
        if (p->data.seat_id == seat_id)
            return p;
        p = (ticket_list_t)(p->node.next);
    }
    return NULL;
}

int Ticket_Srv_Modify(const ticket_t* data) {
    return Ticket_Perst_Update(data);
}

int Ticket_Srv_FetchByID(int id, ticket_t* buf) {
    return Ticket_Perst_SelectByID(id, buf);
}