#include "Ticket_Persist.h"
#include "EntityKey.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

static const char* TICKET_FILE = "Ticket.dat";

extern int Schedule_Perst_SelectByID(int id, schedule_t* buf);
extern int Play_Perst_SelectByID(int id, play_t* buf);

int Ticket_Perst_Insert(int schedule_id, seat_list_t list) {
    FILE* fp = fopen(TICKET_FILE, "ab");
    if (!fp) return 0;

    schedule_t sch;
    if (!Schedule_Perst_SelectByID(schedule_id, &sch)) {
        fclose(fp);
        return 0;
    }

    play_t play;
    if (!Play_Perst_SelectByID(sch.play_id, &play)) {
        fclose(fp);
        return 0;
    }

    int count = 0;
    seat_list_t pos;
    for (pos = list->node.next; pos; pos = (seat_list_t)(((list_node_t*)pos)->next)) {
        count++;
    }

    int* keys = NULL;
    if (count > 0) {
        keys = (int*)malloc(count * sizeof(int));
        if (!keys) { fclose(fp); return 0; }
        if (EntKey_Perst_GetNewKeys(count, keys) < count) {
            free(keys); fclose(fp); return 0;
        }
    }

    int rtn = 0, i = 0;
    pos = list->node.next;
    while (pos) {
        ticket_t data = {
            .id = keys[i],
            .schedule_id = schedule_id,
            .seat_id = pos->data.id,
            .price = 0,
            .status = 0
        };
        if (fwrite(&data, sizeof(ticket_t), 1, fp) == 1) rtn++;
        i++;
        pos = (seat_list_t)(((list_node_t*)pos)->next);
    }

    free(keys);
    fclose(fp);
    return rtn;
}

int Ticket_Perst_Rem(int schedule_id) {
    FILE* fp_src, * fp_dst;
    ticket_t data;
    int found = 0;

    if (rename(TICKET_FILE, "Ticket.tmp") != 0) return 0;

    fp_src = fopen("Ticket.tmp", "rb");
    if (!fp_src) return 0;
    fp_dst = fopen(TICKET_FILE, "wb");
    if (!fp_dst) { fclose(fp_src); return 0; }

    while (fread(&data, sizeof(ticket_t), 1, fp_src) == 1) {
        if (data.schedule_id == schedule_id) found++;
        else fwrite(&data, sizeof(ticket_t), 1, fp_dst);
    }

    fclose(fp_src);
    fclose(fp_dst);
    remove("Ticket.tmp");
    return found;
}

int Ticket_Perst_SelBySchID(int id, ticket_list_t* list) {
    FILE* fp = fopen(TICKET_FILE, "rb");
    if (!fp) return 0;
    ticket_t data;
    int count = 0;
    while (fread(&data, sizeof(ticket_t), 1, fp) == 1) {
        if (data.schedule_id == id) {
            ticket_list_t node = (ticket_list_t)malloc(sizeof(ticket_list_node_t));
            if (!node) break;
            node->data = data;
            node->node.next = NULL;
            if (*list == NULL) {
                *list = node;
            }
            else {
                ticket_list_t p = *list;
                while (p->node.next) p = (ticket_list_t)(p->node.next);
                p->node.next = (list_node_t*)node;
            }
            count++;
        }
    }
    fclose(fp);
    return count;
}

int Ticket_Perst_Update(const ticket_t* data) {
    FILE* fp = fopen(TICKET_FILE, "rb+");
    if (!fp) return 0;
    ticket_t rec;
    int rtn = 0;
    while (fread(&rec, sizeof(ticket_t), 1, fp) == 1) {
        if (rec.id == data->id) {
            fseek(fp, -(long)sizeof(ticket_t), SEEK_CUR);
            if (fwrite(data, sizeof(ticket_t), 1, fp) == 1) rtn = 1;
            break;
        }
    }
    fclose(fp);
    return rtn;
}

int Ticket_Perst_SelectByID(int id, ticket_t* buf) {
    FILE* fp = fopen(TICKET_FILE, "rb");
    if (!fp) return 0;
    ticket_t data;
    int found = 0;
    while (fread(&data, sizeof(ticket_t), 1, fp) == 1) {
        if (data.id == id) {
            *buf = data;
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}