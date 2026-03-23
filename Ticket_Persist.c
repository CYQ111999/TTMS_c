#include "Ticket_Persist.h"
#include "Common.h"
#include "Schedule_Persist.h"
#include "Play_Persist.h"
#include "EntityKey_Persist.h"  // 假设主键获取函数在此
#include <stdio.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS

// 根据 ID 载入演出票
int Ticket_Perst_SelByID(int id, ticket_t* buf) {
    FILE* fp = fopen("Ticket.dat", "rb");
    if (fp == NULL) return 0;
    ticket_t data;
    while (fread(&data, sizeof(ticket_t), 1, fp) == 1) {
        if (data.id == id) {
            *buf = data;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// 获取所有演出票，构建链表
int Ticket_Perst_SelectAll(ticket_list_t head) {
    FILE* fp = fopen("Ticket.dat", "rb");
    if (fp == NULL) return -1;
    ticket_t data;
    int count = 0;
    while (fread(&data, sizeof(ticket_t), 1, fp) == 1) {
        ticket_list_node_t* node = (ticket_list_node_t*)malloc(sizeof(ticket_list_node_t));
        if (!node) { fclose(fp); return -1; }
        node->data = data;
        // 插入链表尾部（双向循环链表）
        node->node.prev = head->node.prev;
        node->node.next = &head->node;
        head->node.prev->next = &node->node;
        head->node.prev = &node->node;
        count++;
    }
    fclose(fp);
    return count;
}

// 根据演出计划 ID 获取所有票，构建链表
int Ticket_Perst_SelectByScheduleID(int schedule_id, ticket_list_t head) {
    FILE* fp = fopen("Ticket.dat", "rb");
    if (fp == NULL) return -1;
    ticket_t data;
    int count = 0;
    while (fread(&data, sizeof(ticket_t), 1, fp) == 1) {
        if (data.schedule_id == schedule_id) {
            ticket_list_node_t* node = (ticket_list_node_t*)malloc(sizeof(ticket_list_node_t));
            if (!node) { fclose(fp); return -1; }
            node->data = data;
            node->node.prev = head->node.prev;
            node->node.next = &head->node;
            head->node.prev->next = &node->node;
            head->node.prev = &node->node;
            count++;
        }
    }
    fclose(fp);
    return count;
}

// 批量插入票记录（生成演出票）
int Ticket_Perst_Insert(int schedule_id, seat_list_t list) {
    FILE* fp = fopen("Ticket.dat", "ab");
    if (fp == NULL) return -1;
    schedule_t sch;
    if (Schedule_Perst_SelectByID(schedule_id, &sch) != 1) {
        fclose(fp);
        return -1;
    }
    play_t play;
    if (Play_Perst_SelectByID(sch.play_id, &play) != 1) {
        fclose(fp);
        return -1;
    }
    // 统计座位数量
    int count = 0;
    list_node_t* cur;
    for (cur = ((list_node_t*)list)->next; cur != (list_node_t*)list; cur = cur->next)
        count++;
    // 获取主键
// 获取起始主键
    long startKey = EntKey_Perst_GetNewKeys("ticket", count);
    if (startKey <= 0) {
        fclose(fp);
        return -1;
    }
    int i = 0, rtn = 0;
    for (cur = ((list_node_t*)list)->next; cur != (list_node_t*)list; cur = cur->next) {
        seat_list_node_t* seat_node = (seat_list_node_t*)cur;
        ticket_t t;
        t.id = startKey + i;   // 依次分配连续的主键
        t.schedule_id = schedule_id;
        t.seat_id = seat_node->data.id;
        t.price = play.price;
        t.status = TICKET_AVL;
        if (fwrite(&t, sizeof(ticket_t), 1, fp) != 1) {
            rtn = -1;
            break;
        }
        i++;
    }
    free(startKey);
    fclose(fp);
    return (rtn == 0) ? count : -1;
}

// 根据演出计划 ID 批量删除票记录（Ticket_Perst_Rem）
int Ticket_Perst_Rem(int schedule_id) {
    // 重命名原文件
    if (rename("Ticket.dat", "TicketTmp.dat") != 0)
        return 0;  // 原文件不存在，视为删除0条

    FILE* fp_tmp = fopen("TicketTmp.dat", "rb");
    FILE* fp_new = fopen("Ticket.dat", "wb");
    if (fp_tmp == NULL || fp_new == NULL) {
        if (fp_tmp) fclose(fp_tmp);
        if (fp_new) fclose(fp_new);
        rename("TicketTmp.dat", "Ticket.dat");
        return -1;
    }

    ticket_t buf;
    int found = 0;
    while (fread(&buf, sizeof(ticket_t), 1, fp_tmp) == 1) {
        if (buf.schedule_id == schedule_id) {
            found++;
        }
        else {
            if (fwrite(&buf, sizeof(ticket_t), 1, fp_new) != 1) {
                fclose(fp_tmp);
                fclose(fp_new);
                remove("Ticket.dat");
                rename("TicketTmp.dat", "Ticket.dat");
                return -1;
            }
        }
    }
    fclose(fp_tmp);
    fclose(fp_new);
    remove("TicketTmp.dat");
    return found;
}

// 更新票信息
int Ticket_Perst_Update(const ticket_t* data) {
    FILE* fp = fopen("Ticket.dat", "rb+");
    if (fp == NULL) return 0;
    ticket_t buf;
    while (fread(&buf, sizeof(ticket_t), 1, fp) == 1) {
        if (buf.id == data->id) {
            fseek(fp, -sizeof(ticket_t), SEEK_CUR);
            fwrite(data, sizeof(ticket_t), 1, fp);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}