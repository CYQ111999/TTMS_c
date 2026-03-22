// Sale_UI.c
#ifndef SALE_UI_C
#define SALE_UI_C
#define _CRT_SECURE_NO_WARNINGS

// 添加 SALESANALYSIS_PAGE_SIZE 的定义
#ifndef SALESANALYSIS_PAGE_SIZE
#define SALESANALYSIS_PAGE_SIZE 5
#endif

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "Common.h"
#include "Play.h"
#include "Schedule.h"
#include "Seat.h"
#include "Ticket.h"
#include "Sale.h"
#include "Sale_UI.h"

// 售票管理入口函数
void Sale_UI_MgtEntry(void) {
    int choice = 0;

    do {
        system("cls");
        printf("\n=================== 售票管理 ===================\n");
        printf("1. 售票\n");
        printf("2. 退票\n");
        printf("3. 查看票务信息\n");
        printf("0. 返回上级菜单\n");
        printf("===============================================\n");
        printf("请选择操作: ");

        if (scanf("%d", &choice) != 1) {
            printf("输入错误！\n");
            getchar(); getchar();
            continue;
        }
        getchar();

        switch (choice) {
        case 1:
            printf("售票功能\n");
            // 调用售票功能
            break;
        case 2:
            Sale_UI_RefundTicket();
            break;
        case 3:
        {
            int playID = 0;
            printf("请输入剧目ID: ");
            scanf("%d", &playID);
            getchar();
            Sale_UI_ShowScheduler(playID);
        }
        break;
        case 0:
            printf("返回上级菜单...\n");
            break;
        default:
            printf("无效选择！\n");
            getchar();
            break;
        }
    } while (choice != 0);
}

// 售票界面
int Sale_UI_SellTicket(ticket_list_t tickList, seat_list_t seatList) {
    int row, col;
    seat_t* seat = NULL;
    ticket_t* ticket = NULL;
    int seat_id = -1;

    system("cls");
    printf("\n=================== 售票 ===================\n");

    // 如果传入了ticket和seat链表，使用它们，否则新建
    ticket_list_t localTicketList = tickList;
    seat_list_t localSeatList = seatList;

    if (!localTicketList) {
        // TODO: 获取票务列表
    }

    if (!localSeatList) {
        // TODO: 获取座位列表
    }

    while (1) {
        printf("输入行和列（格式: 行 列）: ");
        if (scanf("%d %d", &row, &col) != 2) {
            printf("输入格式错误！\n");
            getchar();
            continue;
        }
        getchar();

        // 查找座位
        seat = Seat_Srv_FindByRowCol(localSeatList, row, col);
        if (!seat) {
            printf("座位不存在！\n");
            continue;
        }

        // 查找票
        ticket = Ticket_Srv_FetchBySeatID(localTicketList, seat->id);
        if (!ticket) {
            printf("该座位无对应票！\n");
            continue;
        }

        if (ticket->status != 0) {  // 0表示可售
            if (ticket->status == 1) {
                printf("该票已售出！\n");
            }
            else if (ticket->status == 2) {
                printf("该票已退票！\n");
            }
            else {
                printf("该票不可售！\n");
            }
            continue;
        }

        // 显示确认信息
        printf("\n座位信息：第%d行 第%d列\n", row, col);
        printf("票价：%d元\n", ticket->price);
        printf("确认购票？(y/n): ");
        int ch = tolower(getchar());
        getchar();

        if (ch == 'y') {
            // 更新票状态为已售
            ticket->status = 1;  // 1表示已售
            if (Ticket_Srv_Modify(ticket)) {
                printf("售票成功！\n");
                seat_id = seat->id;
                break;
            }
            else {
                printf("售票失败！\n");
            }
        }
        else {
            printf("取消购票。\n");
        }
    }
    return seat_id;
}

// 退票界面
void Sale_UI_RefundTicket(void) {
    int tid;
    ticket_t t;

    system("cls");
    printf("\n=================== 退票 ===================\n");

    printf("请输入票ID: ");
    if (scanf("%d", &tid) != 1) {
        printf("输入错误！\n");
        getchar(); getchar();
        return;
    }
    getchar();

    if (!Ticket_Srv_FetchByID(tid, &t)) {
        printf("票不存在！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    if (t.status != 1) {  // 1表示已售
        printf("该票未售出或已退票，无法退票！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    printf("\n票信息：\n");
    printf("票ID: %d\n", t.id);
    printf("演出计划ID: %d\n", t.schedule_id);
    printf("座位ID: %d\n", t.seat_id);
    printf("价格: %d元\n", t.price);
    printf("状态: %s\n", t.status == 1 ? "已售" : "可售");

    printf("\n确认退票？(y/n): ");
    int ch = tolower(getchar());
    getchar();

    if (ch != 'y') {
        printf("退票已取消。\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    t.status = 2;  // 2表示已退
    if (Ticket_Srv_Modify(&t)) {
        printf("退票成功！\n");
    }
    else {
        printf("退票失败！\n");
    }

    printf("按任意键返回...");
    getchar();
}

// 查看演出计划
void Sale_UI_ShowScheduler(int playID) {
    play_t play;

    system("cls");
    printf("\n=================== 演出计划查看 ===================\n");

    if (!Play_Srv_FetchByID(playID, &play)) {
        printf("剧目不存在！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    printf("剧目: %s\n\n", play.name);

    schedule_list_t schList = NULL;
    List_Init(schList, schedule_list_node_t);

    int total = Schedule_Srv_FetchByPlay(playID, &schList);
    if (total <= 0) {
        printf("该剧目暂无演出计划。\n");
        printf("按任意键返回...");
        getchar();
        List_Destroy(schList, schedule_list_node_t);
        return;
    }

    paging_t paging;
    paging.pageSize = SALESANALYSIS_PAGE_SIZE;
    paging.totalRecords = total;
    Paging_Locate_FirstPage(schList, paging, schedule_list_node_t);

    char choice;
    do {
        system("cls");
        printf("\n=================== 演出计划查看 ===================\n");
        printf("剧目: %s\n\n", play.name);

        // 显示当前页的演出计划
        schedule_list_node_t* pos;
        int i = 0;

        printf("序号 计划ID 日期        时间     演出厅\n");
        printf("------------------------------------------\n");

        Paging_ViewPage_ForEach(schList, paging, schedule_list_node_t, pos, i) {
            printf("%2d.  %-6d  %04d-%02d-%02d  %02d:%02d     %d\n",
                i + 1,
                pos->data.id,
                pos->data.date.year, pos->data.date.month, pos->data.date.day,
                pos->data.time.hour, pos->data.time.minute,
                pos->data.studio_id);
        }

        printf("------------------------------------------\n");
        printf("当前页 %d/%d\n", Pageing_CurPage(paging), Pageing_TotalPages(paging));
        printf("[t]查看票务  [p]上一页  [n]下一页  [r]返回\n");
        printf("请选择操作: ");

        choice = tolower(getchar());
        getchar();

        switch (choice) {
        case 't': {
            int sid;
            printf("请输入演出计划序号: ");
            if (scanf("%d", &sid) != 1 || sid < 1 || sid > paging.pageSize) {
                printf("序号无效！\n");
                getchar();
                break;
            }
            getchar();

            // 根据序号找到对应的演出计划
            schedule_list_node_t* selected = schList;
            for (int j = 0; j < sid; j++) {
                selected = (schedule_list_node_t*)selected->node.next;
            }

            if (selected) {
                printf("查看演出计划ID=%d的票务信息\n", selected->data.id);
                printf("按任意键继续...");
                getchar();
            }
            break;
        }
        case 'p':
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(schList, paging, -1, schedule_list_node_t);
            }
            else {
                printf("已经是第一页。\n");
                printf("按任意键继续...");
                getchar();
            }
            break;
        case 'n':
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(schList, paging, 1, schedule_list_node_t);
            }
            else {
                printf("已经是最后一页。\n");
                printf("按任意键继续...");
                getchar();
            }
            break;
        case 'r':
            break;
        default:
            printf("无效选项，请重新输入\n");
            printf("按任意键继续...");
            getchar();
        }
    } while (choice != 'r');

    List_Destroy(schList, schedule_list_node_t);
}

#endif /* SALE_UI_C */