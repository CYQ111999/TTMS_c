// Ticket.c
#include "Ticket.h"
#include "Schedule.h"
#include "Play.h"
#include "Common.h"
#include "List.h"
#include <stdio.h>
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS
int Ticket_UI_ShowTicket(int ticket_id) {
    ticket_t ticket;
    int rtn = 0;

    if (Ticket_Srv_FetchByID(ticket_id, &ticket) == 1) {  // 统一使用 Ticket_Srv_FetchByID
        printf("票ID: %d\n", ticket.id);
        printf("演出计划ID: %d\n", ticket.schedule_id);
        printf("座位ID: %d\n", ticket.seat_id);
        printf("票价: %d元\n", ticket.price);
        printf("状态: %s\n",
            ticket.status == 0 ? "待售" :
            ticket.status == 1 ? "已售" : "已退");
        rtn = 1;
    }
    else {
        printf("未找到ID为 %d 的演出票\n", ticket_id);
    }
    return rtn;
}

void Ticket_UI_Query(void) {
    ticket_list_t head = NULL;  // 使用链表指针

    List_Init(head, ticket_list_node_t);

    int count = Ticket_Srv_FetchAll(&head);
    if (count < 0) {
        printf("查询演出票失败！\n");
        return;
    }

    if (count == 0) {
        printf("暂无演出票信息。\n");
    }
    else {
        printf("共找到 %d 张演出票：\n", count);
        printf("==============================\n");

        ticket_list_node_t* cur = (ticket_list_node_t*)((list_node_t*)head)->next;
        int i = 0;
        while (cur != (ticket_list_node_t*)head) {
            printf("%d. ", ++i);
            Ticket_UI_ShowTicket(cur->data.id);
            printf("---\n");
            cur = (ticket_list_node_t*)cur->node.next;
        }
    }

    // 释放链表
    if (head != NULL) {
        List_Destroy(head, ticket_list_node_t);
    }
}

void Ticket_UI_MgtEntry(int schedule_id) {
    schedule_t sch;
    play_t play;

    system("cls");
    printf("\n=================== 票务管理 ===================\n");

    if (Schedule_Srv_FetchByID(schedule_id, &sch) != 1) {
        printf("获取演出计划失败！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    if (Play_Srv_FetchByID(sch.play_id, &play) != 1) {
        printf("获取剧目信息失败！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    printf("剧目名称: %s\n", play.name);
    printf("演出厅编号: %d\n", sch.studio_id);
    printf("演出日期: %04d-%02d-%02d\n",
        sch.date.year, sch.date.month, sch.date.day);
    printf("演出时间: %02d:%02d:%02d\n",
        sch.time.hour, sch.time.minute, sch.time.second);

    printf("\n请选择操作：\n");
    printf("1. 生成演出票\n");
    printf("2. 重新生成票（删除后重新生成）\n");
    printf("0. 返回上级菜单\n");
    printf("==============================================\n");
    printf("请选择操作: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("输入错误！\n");
        getchar();
        getchar();
        return;
    }
    getchar();

    switch (choice) {
    case 1:
        if (Ticket_Srv_GenBatch(schedule_id, sch.studio_id) > 0) {
            printf("演出票生成完成。\n");
        }
        else {
            printf("演出票生成失败！\n");
        }
        break;
    case 2:
        printf("确定要删除并重新生成票吗？(y/n): ");
        char confirm = tolower(getchar());
        getchar();
        if (confirm == 'y') {
            Ticket_Srv_DeleteBatch(schedule_id);
            if (Ticket_Srv_GenBatch(schedule_id, sch.studio_id) > 0) {
                printf("演出票重新生成完成。\n");
            }
            else {
                printf("演出票重新生成失败！\n");
            }
        }
        else {
            printf("已取消操作。\n");
        }
        break;
    case 0:
        printf("返回上级菜单...\n");
        break;
    default:
        printf("无效选择！\n");
        break;
    }

    printf("按任意键继续...");
    getchar();
}