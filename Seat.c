#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Seat.h"
#include "Seat_Persist.h"
#include "EntityKey_Persist.h"
#include "List.h"

#define _CRT_SECURE_NO_WARNINGS

// 假设 SEAT_GOOD 的定义
#ifndef SEAT_GOOD
#define SEAT_GOOD 0
#endif

// 初始化演出厅所有座位
int Seat_Srv_RoomInit(seat_list_t list, int roomID, int rowsCount, int colsCount) {
    seat_list_node_t* node;
    for (int i = 1; i <= rowsCount; i++) {
        for (int j = 1; j <= colsCount; j++) {
            node = (seat_list_node_t*)malloc(sizeof(seat_list_node_t));
            if (!node) return 0;
            node->data.roomID = roomID;
            node->data.row = i;
            node->data.column = j;
            node->data.status = SEAT_GOOD;  // 默认有效
            node->data.id = 0;  // 暂不分配ID，批量持久化时统一分配
            List_AddTail(list, node);
        }
    }
    // 批量持久化
    return Seat_Perst_InsertBatch(list);
}

// 添加新座位
int Seat_Srv_Add(seat_t* data) {
    return Seat_Perst_Insert(data);
}

// 修改座位
int Seat_Srv_Modify(const seat_t* data) {
    return Seat_Perst_Update(data);
}

// 根据座位ID删除座位
int Seat_Srv_DeleteByID(int ID) {
    return Seat_Perst_RemoveByID(ID);
}

// 根据演出厅ID删除所有座位
int Seat_Srv_DeleteAllByRoomID(int roomID) {
    return Seat_Perst_RemoveAllByRoomID(roomID);
}

// 根据演出厅ID获取所有座位
int Seat_Srv_FetchByRoomID(seat_list_t* list, int roomID) {
    int count = Seat_Perst_SelectByRoomID(list, roomID);
    if (count > 0 && *list != NULL) {
        Seat_Srv_SortSeatList(*list);
    }
    return count;
}

// 根据演出厅ID获取有效座位
int Seat_Srv_FetchValidByRoomID(seat_list_t* list, int roomID) {
    int count = Seat_Perst_SelectByRoomID(list, roomID);

    if (count > 0 && *list != NULL) {
        seat_list_t tempList = *list;

        // 安全遍历删除无效节点
        seat_list_node_t* p = NULL;
        seat_list_node_t* next = NULL;

        // 使用安全遍历，防止删除节点导致遍历中断
        for (p = (seat_list_node_t*)LIST_NODE(tempList)->next;
            p != (seat_list_node_t*)LIST_NODE(tempList);
            p = next) {

            next = (seat_list_node_t*)LIST_NODE(p)->next;  // 先保存下一个节点

            if (p->data.status != SEAT_GOOD) {
                List_FreeNode(p);
                count--;
            }
        }

        if (count > 0) {
            Seat_Srv_SortSeatList(tempList);
        }
        else {
            // 如果没有有效座位，清理链表
            List_Destroy(*list, seat_list_node_t);
            *list = NULL;
        }
    }
    return count;
}

// 根据行列号获取座位节点
seat_list_node_t* Seat_Srv_FindByRowCol(seat_list_t list, int row, int column) {
    if (List_IsEmpty(list)) return NULL;
    seat_list_node_t* p = NULL;
    List_ForEach(list, p) {
        if (p->data.row == row && p->data.column == column) {
            return p;
        }
    }
    return NULL;
}

// 对座位链表进行按行按列排序
void Seat_Srv_SortSeatList(seat_list_t list) {
    if (List_IsEmpty(list)) return;

    // 断开循环链表：将最后一个节点的next置NULL
    LIST_NODE(list)->prev->next = NULL;   // 尾节点的next原指向头，现在置NULL
    seat_list_node_t* listLeft = (seat_list_node_t*)LIST_NODE(list)->next;  // 第一个数据节点
    seat_list_node_t* p = NULL;

    // 清空原链表（重新初始化头结点）
    LIST_NODE(list)->prev = LIST_NODE(list);
    LIST_NODE(list)->next = LIST_NODE(list);

    // 重新插入排序
    while (listLeft != NULL) {
        p = listLeft;
        listLeft = (seat_list_node_t*)LIST_NODE(listLeft)->next;  // 保存下一个节点
        Seat_Srv_AddToSortedList(list, p);
    }
}

// 将新节点加入到已排序链表
void Seat_Srv_AddToSortedList(seat_list_t list, seat_list_node_t* node) {
    if (List_IsEmpty(list)) {
        List_AddTail(list, node);
        return;
    }

    seat_list_node_t* p = NULL;
    List_ForEach(list, p) {
        // 按行、列排序：先按行，行相同按列
        if (p->data.row > node->data.row ||
            (p->data.row == node->data.row && p->data.column > node->data.column)) {
            // 插入到p之前
            List_InsertBefore(p, node);
            return;
        }
    }
    // 如果比所有节点都大，插入尾部
    List_AddTail(list, node);
}