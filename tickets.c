#include "Tickets.h"
#include "Ticket_Persist.h"
#include "Schedule_Persist.h"
#include "Play_Persist.h"
#include "Seat_Persist.h"
#include "List.h"
#define _CRT_SECURE_NO_WARNINGS

int Ticket_Srv_Modify(const ticket_t* data) {
    // 需要持久化层提供更新函数
    return Ticket_Perst_Update(data);
}

int Ticket_Srv_DeleteAllByScheduleID(int schedule_id) {
    return Ticket_Srv_DeleteBatch(schedule_id);  // 调用已实现的删除批处理函数
}

int Ticket_Srv_FetchAll(ticket_list_t head) {
    return Ticket_Perst_SelectAll(head);
}

int Ticket_Srv_GenBatch(int schedule_id, int studio_id) {
    seat_list_node_t head;                     // 座位链表头节点
    // 手动初始化链表头节点
    head.node.prev = &head.node;
    head.node.next = &head.node;

    int count = Seat_Srv_FetchValidByRoomID(studio_id, &head);
    if (count <= 0) {
        return -1;                              // 获取座位失败或无有效座位
    }

    int result = Ticket_Perst_Insert(schedule_id, &head);

    // 释放座位链表节点
    list_node_t* cur = head.node.next;
    list_node_t* next;
    while (cur != &head.node) {
        next = cur->next;
        free((seat_list_node_t*)cur);
        cur = next;
    }

    return result;                               // 返回插入结果
}

int Ticket_Srv_DeleteBatch(int schedule_id) {
    return Ticket_Perst_Rem(schedule_id);
}

int Ticket_Srv_FetchBySchID(int schedule_id, ticket_list_t head) {
    return Ticket_Perst_SelectByScheduleID(schedule_id, head);
}

// 添加缺失的 Ticket_Srv_FetchByID 函数
int Ticket_Srv_FetchByID(int id, ticket_t* buf) {
    return Ticket_Perst_SelectByScheduleID(id, buf);
}

// 关键修复：添加 Ticket_SrvFetchByID 函数
int Ticket_SrvFetchByID(int id, ticket_t* buf) {
    // 这个函数只是为了解决链接错误
    return Ticket_Srv_FetchByID(id, buf);
}

ticket_list_t Ticket_Srv_FetchBySeatID(ticket_list_t list, int seat_id) {
    if (!list) return NULL;
    list_node_t* cur = ((list_node_t*)list)->next;
    while (cur != (list_node_t*)list) {
        ticket_list_node_t* node = (ticket_list_node_t*)cur;
        if (node->data.seat_id == seat_id) {
            return node;  // 返回找到的节点指针
        }
        cur = cur->next;
    }
    return NULL;
}