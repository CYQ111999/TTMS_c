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
#include <stdlib.h> 
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
            Sale_UI_SellTicket();
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
void Sale_UI_SellTicket(void) {
    int schedule_id = 0;
    int row, col;
    seat_t* seat = NULL;
    ticket_t* ticket = NULL;

    system("cls");
    printf("\n=================== 售票 ===================\n");

    // 用户选择演出计划
    schedule_list_t schedule_list = NULL;
    List_Init(schedule_list, schedule_list_node_t);

    int schedule_count = Schedule_Srv_FetchAll(schedule_list);
    if (schedule_count <= 0) {
        printf("暂无演出计划！\n");
        List_Destroy(schedule_list, schedule_list_node_t);
        printf("按任意键返回...");
        getchar();
        return;
    }

    // 显示演出计划列表
    printf("\n============= 可选的演出计划 =============\n");
    printf("序号 计划ID 剧目ID 演出厅 日期        时间     座位数\n");
    printf("---------------------------------------------------\n");

    int schedule_ids[100];
    int index = 0;
    schedule_list_node_t* schedule_node = (schedule_list_node_t*)schedule_list->node.next;

    while (schedule_node != (schedule_list_node_t*)schedule_list) {
        schedule_ids[index] = schedule_node->data.id;

        printf("%-4d %-6d %-6d %-6d %04d-%02d-%02d %02d:%02d  %d\n",
            index + 1,
            schedule_node->data.id,
            schedule_node->data.play_id,
            schedule_node->data.studio_id,
            schedule_node->data.date.year,
            schedule_node->data.date.month,
            schedule_node->data.date.day,
            schedule_node->data.time.hour,
            schedule_node->data.time.minute,
            schedule_node->data.seat_count);

        schedule_node = (schedule_list_node_t*)schedule_node->node.next;
        index++;
    }
    printf("---------------------------------------------------\n");
    printf("共 %d 个演出计划\n", index);

    if (index == 0) {
        printf("没有可用的演出计划！\n");
        List_Destroy(schedule_list, schedule_list_node_t);
        printf("按任意键返回...");
        getchar();
        return;
    }

    // 让用户选择演出计划
    int selection = 0;
    printf("\n请选择演出计划（序号）: ");
    if (scanf("%d", &selection) != 1 || selection < 1 || selection > index) {
        printf("选择无效！\n");
        getchar();
        List_Destroy(schedule_list, schedule_list_node_t);
        printf("按任意键返回...");
        getchar();
        return;
    }
    getchar();

    schedule_id = schedule_ids[selection - 1];

    // 查找选中的演出计划详情
    schedule_t selected_schedule;
    int found = 0;
    schedule_node = (schedule_list_node_t*)schedule_list->node.next;
    while (schedule_node != (schedule_list_node_t*)schedule_list) {
        if (schedule_node->data.id == schedule_id) {
            memcpy(&selected_schedule, &(schedule_node->data), sizeof(schedule_t));
            found = 1;
            break;
        }
        schedule_node = (schedule_list_node_t*)schedule_node->node.next;
    }

    // 释放演出计划链表
    List_Destroy(schedule_list, schedule_list_node_t);

    if (!found) {
        printf("找不到指定的演出计划！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    //获取该演出计划的票务列表
    ticket_list_t ticketList = NULL;
    List_Init(ticketList, ticket_list_node_t);

    int ticket_count = Ticket_Perst_SelectByScheduleID(schedule_id, ticketList);
    if (ticket_count <= 0) {
        printf("该演出计划暂无票务信息！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    //获取演出厅的座位列表
    seat_list_t seatList = NULL;
    List_Init(seatList, seat_list_node_t);

    int seat_count = Seat_Srv_FetchByRoomID(seatList, selected_schedule.studio_id);
    if (seat_count <= 0) {
        printf("该演出厅暂无座位信息！\n");
        List_Destroy(ticketList, ticket_list_node_t);
        printf("按任意键返回...");
        getchar();
        return;
    }

    //获取剧目信息
    play_t play_info;
    if (Play_Srv_FetchByID(selected_schedule.play_id, &play_info) != 1) {
        printf("获取剧目信息失败！\n");
        List_Destroy(ticketList, ticket_list_node_t);
        List_Destroy(seatList, seat_list_node_t);
        printf("按任意键返回...");
        getchar();
        return;
    }

    //显示演出计划信息
    system("cls");
    printf("\n=================== 演出计划信息 ===================\n");
    printf("演出计划ID: %d\n", selected_schedule.id);
    printf("剧目: %s\n", play_info.name);
    printf("演出厅: %d号厅\n", selected_schedule.studio_id);
    printf("演出时间: %04d-%02d-%02d %02d:%02d\n",
        selected_schedule.date.year,
        selected_schedule.date.month,
        selected_schedule.date.day,
        selected_schedule.time.hour,
        selected_schedule.time.minute);
    printf("座位总数: %d\n", selected_schedule.seat_count);
    printf("======================================================\n");

    //计算最大行和列
    int max_row = 0, max_col = 0;
    seat_list_node_t* seat_node = (seat_list_node_t*)seatList->node.next;
    while (seat_node != (seat_list_node_t*)seatList) {
        if (seat_node->data.row > max_row) max_row = seat_node->data.row;
        if (seat_node->data.column > max_col) max_col = seat_node->data.column;
        seat_node = (seat_list_node_t*)seat_node->node.next;
    }

    //显示座位图
    printf("\n=================== 座位图 ===================\n");
    printf("座位图说明：1=可售，0=已售，X=退票\n");
    printf("\n  ");
    for (int col_num = 1; col_num <= max_col; col_num++) {
        printf("%2d", col_num);
    }
    printf("\n");

    for (int r = 1; r <= max_row; r++) {
        printf("%2d ", r);
        for (int c = 1; c <= max_col; c++) {
            int seat_found = 0;
            int ticket_found = 0;
            int ticket_status = 0;

            // 查找座位
            seat_node = (seat_list_node_t*)seatList->node.next;
            while (seat_node != (seat_list_node_t*)seatList) {
                if (seat_node->data.row == r && seat_node->data.column == c) {
                    seat_found = 1;
                    break;
                }
                seat_node = (seat_list_node_t*)seat_node->node.next;
            }

            if (!seat_found) {
                printf("   ");  // 座位不存在
                continue;
            }

            // 查找票的状态
            ticket_list_node_t* ticket_node = (ticket_list_node_t*)ticketList->node.next;
            while (ticket_node != (ticket_list_node_t*)ticketList) {
                if (ticket_node->data.seat_id == seat_node->data.id) {
                    ticket_found = 1;
                    ticket_status = ticket_node->data.status;
                    break;
                }
                ticket_node = (ticket_list_node_t*)ticket_node->node.next;
            }

            // 显示座位状态
            if (ticket_found) {
                if (ticket_status == 0) {  // 可售
                    printf(" 1");
                }
                else if (ticket_status == 1) {  // 已售
                    printf(" 0");
                }
                else {  // 退票
                    printf(" X");
                }
            }
            else {
                printf(" 1");  // 有座位但无票信息，默认可售
            }
        }
        printf("\n");
    }
    printf("==============================================\n");

    //售票循环
    int continue_sell = 1;
    int sold_count = 0;
    float total_amount = 0.0;

    while (continue_sell) {
        printf("\n请输入要购买的座位（格式: 行 列），输入0 0结束购票: ");
        if (scanf("%d %d", &row, &col) != 2) {
            printf("输入格式错误！请重新输入。\n");
            getchar();
            continue;
        }
        getchar();

        if (row == 0 && col == 0) {
            printf("购票结束。\n");
            printf("已售票数: %d\n", sold_count);
            printf("总金额: %.2f元\n", total_amount);
            break;
        }

        if (row < 1 || row > max_row || col < 1 || col > max_col) {
            printf("座位坐标超出范围！行数应在1-%d之间，列数应在1-%d之间。\n", max_row, max_col);
            continue;
        }

        // 查找座位
        seat = NULL;
        seat_node = (seat_list_node_t*)seatList->node.next;
        while (seat_node != (seat_list_node_t*)seatList) {
            if (seat_node->data.row == row && seat_node->data.column == col) {
                seat = &(seat_node->data);
                break;
            }
            seat_node = (seat_list_node_t*)seat_node->node.next;
        }

        if (seat == NULL) {
            printf("该座位不存在！请重新输入。\n");
            continue;
        }

        // 查找对应的票
        ticket = NULL;
        ticket_list_node_t* ticket_node = (ticket_list_node_t*)ticketList->node.next;
        while (ticket_node != (ticket_list_node_t*)ticketList) {
            if (ticket_node->data.seat_id == seat->id) {
                ticket = &(ticket_node->data);
                break;
            }
            ticket_node = (ticket_list_node_t*)ticket_node->node.next;
        }
        if (ticket == NULL) {
            printf("错误：该座位没有对应的票务信息！\n");
            continue;
        }
        // 检查票状态
        if (ticket->status == 1) {
            printf("该座位已售出，请选择其他座位。\n");
            continue;
        }
        else if (ticket->status == 2) {
            printf("该座位已退票，暂时不可售。\n");
            continue;
        }
        // 确认购票
        printf("\n=== 购票确认 ===\n");
        printf("座位：第%d行第%d列\n", row, col);
        printf("票价：%d元\n", ticket->price);
        printf("确认购票吗?(y/n):");
        char confirm;
        confirm = getchar(); 
        getchar();
        if (confirm == 'y' || confirm == 'Y') {
            // 修改票状态
            ticket->status = 1;  // 已售
            // 创建销售记录
            sale_t sale;
            // 根据Common.h中的结构体定义赋值
            sale.ticket_id = ticket->id;        
            sale.user_id = gl_CurUser.id;         // 当前用户ID
            sale.value = ticket->price;           // 票价
            sale.type = SALE_SELL;                // 交易类型

            // 设置销售时间
            time_t now = time(NULL);
            struct tm* local_time = localtime(&now);
            sale.date.year = local_time->tm_year + 1900; 
            sale.date.month = local_time->tm_mon + 1;    
            sale.date.day = local_time->tm_mday;      
            sale.time.hour = local_time->tm_hour;           
            sale.time.minute = local_time->tm_min;          
            sale.time.second = local_time->tm_sec;          

            // 调用销售服务添加记录
            if (Sale_Srv_Add(&sale) == 1) {  // 注意：这里原来是 Sale_Srv Add
                // 更新票状态
                if (Ticket_Perst_Update(ticket) == 1) {
                    printf("购票成功！座位号: %d\n", seat->id);
                    sold_count++;
                    total_amount += ticket->price;
                }
                else {
                    printf("错误：更新票状态失败！\n");
                }
            }
            else {
                printf("错误：创建销售记录失败！\n");
            }
        }
        else {
            printf("已取消购票。\n");
        }
        // 询问是否继续购票
        printf("\n是否继续购票？(y/n): ");
        confirm = getchar();
        getchar();

        if (confirm == 'n' || confirm == 'N') {
            continue_sell = 0;
        }
    }
    // 清理资源
    List_Destroy(ticketList, ticket_list_node_t);
    List_Destroy(seatList, seat_list_node_t);
    printf("\n购票结束。\n");
    printf("按任意键返回主菜单...");
    getchar();
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
    t.status = 2;  //表示已退
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