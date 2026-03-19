#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "Common.h"
#include "Play.h"
#include "Schedule.h"
#include "Seat.h"
#include "Ticket.h"
#include "Sale.h"

#ifndef SALESANALYSIS_PAGE_SIZE
#define SALESANALYSIS_PAGE_SIZE 5
#endif

#define PAGE_SIZE SALESANALYSIS_PAGE_SIZE

// 辅助函数：显示剧目列表（分页）
static void display_plays(play_list_t list, int start, int size) {
    play_list_t p = list;
    // 跳过前面的 start 个节点
    for (int i = 0; p && i < start; i++) {
        p = (play_list_t)(p->node.next);
    }
    printf("剧目列表：\n");
    for (int i = 0; p && i < size; i++) {
        printf("%d: %s\n", p->data.id, p->data.name);
        p = (play_list_t)(p->node.next);
    }
}

// 辅助函数：显示演出计划列表
static void display_schedules(schedule_list_t list) {
    schedule_list_t p = list;
    printf("演出计划：\n");
    while (p) {
        printf("ID:%d 日期:%s 时间:%s 厅:%d\n",
            p->data.id, p->data.date, p->data.time, p->data.studio_id);
        p = (schedule_list_t)(p->node.next);
    }
}

// 售票管理主界面
void Sale_UI_MgtEntry(void) {
    play_list_t playList = NULL;
    int choice, currentPage = 0, totalRecords = 0;
    totalRecords = Play_Srv_FetchAll(&playList); // 假设 Play_Srv_FetchAll 返回总数并填充链表
    while (1) {
        display_plays(playList, currentPage * PAGE_SIZE, PAGE_SIZE);
        printf("总记录数：%d，当前页：%d\n", totalRecords, currentPage + 1);
        printf("操作：[c]显示演出计划 [s]查询 [f]筛选 [p]上一页 [n]下一页 [r]返回\n");
        choice = tolower(getchar());
        while (getchar() != '\n'); // 清空输入缓冲区

        if (choice == 'r') break;

        switch (choice) {
        case 'c': {
            int pid;
            printf("请输入剧目ID：");
            scanf("%d", &pid);
            while (getchar() != '\n');
            Sale_UI_ShowScheduler(pid);
            break;
        }
        case 's': {
            char name[50];
            printf("请输入剧目名称：");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';
            // 查询功能待实现
            printf("查询功能未实现\n");
            break;
        }
        case 'f': {
            char filter[50];
            printf("筛选关键词：");
            fgets(filter, sizeof(filter), stdin);
            filter[strcspn(filter, "\n")] = '\0';
            Play_Srv_FilterByName(playList, filter); // 假设此函数存在
            // 筛选后可能需要重置分页并重新计算总数
            currentPage = 0;
            totalRecords = Play_Srv_GetCount(playList);
            break;
        }
        case 'p':
            if (currentPage > 0) currentPage--;
            break;
        case 'n':
            if ((currentPage + 1) * PAGE_SIZE < totalRecords) currentPage++;
            break;
        default:
            printf("无效输入！\n");
            break;
        }
    }

    PlayList_Destroy(playList);
}

// 根据剧目显示演出计划界面
void Sale_UI_ShowScheduler(int playID) {
    play_t play;
    if (!Play_Srv_FetchByID(playID, &play)) {
        printf("剧目不存在！\n");
        return;
    }

    schedule_list_t schList = NULL;
    int total = Schedule_Srv_FetchByPlay(playID, &schList); // 假设此函数存在
    int choice;

    while (1) {
        printf("剧目：%s\n", play.name);
        display_schedules(schList);
        printf("[t]查看演出票 [r]返回\n");
        choice = tolower(getchar());
        while (getchar() != '\n');

        if (choice == 'r') break;

        if (choice == 't') {
            int sid;
            printf("请输入演出计划ID：");
            scanf("%d", &sid);
            while (getchar() != '\n');

            ticket_list_t tList = NULL;
            seat_list_t sList = NULL;
            Ticket_Srv_FetchBySchID(sid, &tList);
            schedule_t sch;
            if (Schedule_Srv_FetchByID(sid, &sch)) {
                Seat_Srv_FetchByRoomID(sch.studio_id, &sList); // 假设此函数存在
            }

            int seat_id = Sale_UI_SellTicket(tList, sList);
            if (seat_id > 0) {
                printf("售出座位ID：%d\n", seat_id);
            }

            TicketList_Destroy(tList);
            SeatList_Destroy(sList);
        }
    }

    ScheduleList_Destroy(schList);
}

// 售票界面
int Sale_UI_SellTicket(ticket_list_t tickList, seat_list_t seatList) {
    int row, col;
    seat_t* seat = NULL;
    ticket_list_t tNode = NULL;
    int seat_id = -1;
    while (1) {
        printf("输入行和列：");
        scanf("%d %d", &row, &col);
        while (getchar() != '\n');

        seat = Seat_Srv_FindByRC(seatList, row, col); // 假设存在
        if (!seat) {
            printf("座位不存在！\n");
            continue;
        }

        tNode = Ticket_Srv_FetchBySeatID(tickList, seat->id); // 假设存在
        if (!tNode) {
            printf("无对应票！\n");
            continue;
        }
        if (tNode->data.status != 0) {
            printf("该票已售或不可售！\n");
            continue;
        }

        printf("票ID=%d 价格=%d 确认售票？(y/n): ", tNode->data.id, tNode->data.price);
        int ch = tolower(getchar());
        while (getchar() != '\n');
        if (ch != 'y') continue;

        ticket_t newt = tNode->data;
        newt.status = 1; // 已售
        if (Ticket_Srv_Modify(&newt)) {
            sale_t s = { 0, newt.id, time(NULL), 1 }; // 根据 common.h 的字段顺序
            if (Sale_Srv_Add(&s)) {
                printf("售票成功！\n");
                seat_id = seat->id;
                break;
            }
            else {
                printf("销售记录保存失败！\n");
            }
        }
        else {
            printf("修改票状态失败！\n");
        }
    }
    return seat_id;
}

// 退票界面
void Sale_UI_RetfundTicket(void) {
    int tid;
    ticket_t t;
    printf("输入票ID：");
    scanf("%d", &tid);
    while (getchar() != '\n');

    if (!Ticket_Srv_FetchByID(tid, &t)) {
        printf("票不存在！\n");
        return;
    }
    if (t.status != 1) {
        printf("票未售或已退！\n");
        return;
    }
    printf("票信息：ID=%d 演出计划=%d 座位=%d 价格=%d\n", t.id, t.schedule_id, t.seat_id, t.price);
    printf("确认退票？(y/n): ");
    int ch = tolower(getchar());
    while (getchar() != '\n');
    if (ch != 'y') return;

    t.status = 2; // 已退
    if (Ticket_Srv_Modify(&t)) {
        printf("退票成功！\n");
    }
    else {
        printf("退票失败！\n");
    }
}