// Salesanalysis_UI.c
// 销售统计分析界面层
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "List.h"
#include "Salesanalysis_UI.h"
#include "Salesanalysis.h"
#include "Play.h"
#include "Schedule.h"
#define _CRT_SECURE_NO_WARNINGS
#ifndef DEFAULT_PAGE_SIZE
#define DEFAULT_PAGE_SIZE 10
#endif

// 内部函数声明
static void show_sales_by_play(void);
static void show_sales_by_schedule(void);
static void show_sales_by_time_period(void);
static void show_box_office_report(void);

// 销售统计分析主界面
void Salesanalysis_UI_Report(void)
{
    int choice = 0;

    do {
        system("cls");
        printf("\n=================== 销售统计分析 ===================\n");
        printf("1. 按剧目统计销售额\n");
        printf("2. 按演出计划统计销售额\n");
        printf("3. 按时间段统计销售额\n");
        printf("4. 票房统计报表\n");
        printf("5. 返回上级菜单\n");
        printf("===================================================\n");
        printf("请选择统计方式: ");

        if (scanf("%d", &choice) != 1) {
            printf("输入错误！\n");
            getchar(); // 清除错误输入
            getchar(); // 等待按键
            continue;
        }
        getchar(); // 清除回车

        switch (choice) {
        case 1:
            show_sales_by_play();      // 按剧目统计
            break;
        case 2:
            show_sales_by_schedule();  // 按演出计划统计
            break;
        case 3:
            show_sales_by_time_period(); // 按时间段统计
            break;
        case 4:
            show_box_office_report();   // 票房统计报表
            break;
        case 5:
            printf("返回上级菜单...\n");
            break;
        default:
            printf("无效选择，请重新输入！\n");
            printf("按任意键继续...");
            getchar();
            break;
        }
    } while (choice != 5);
}

// 按剧目统计销售额 - 使用分页显示
static void show_sales_by_play(void)
{
    sales_analysis_list_t list = NULL;
    paging_t paging;
    char choice;
    int totalCount = 0;

    system("cls");
    printf("\n=================== 按剧目统计销售额 ===================\n");

    // 调用业务逻辑层函数进行统计
    totalCount = Salesanalysis_Srv_StatByPlay(&list);
    if (totalCount <= 0 || list == NULL) {
        printf("\n当前没有票房统计数据。\n");
        printf("=====================================================\n");
        printf("按任意键返回...");
        getchar();
        getchar(); // 等待按键
        return;
    }

    // 初始化分页器
    paging.totalRecords = totalCount;
    paging.pageSize = DEFAULT_PAGE_SIZE;
    paging.offset = 0;
    paging.curPos = NULL;

    // 主显示循环
    do {
        system("cls");
        printf("\n=================== 按剧目统计销售额 (共%d条) ===================\n", totalCount);

        // 表头
        printf(" 剧目ID  剧目名称                地区  时长(分)  上座数      票房(元)  上映日期\n");
        printf("------------------------------------------------------------------------\n");
        // 使用分页宏遍历当前页
        sales_analysis_list_node_t* pos;
        int i = 0;
        Paging_ViewPage_ForEach(list, paging, sales_analysis_list_node_t, pos, i) {
            printf(" %-8d %-22s %-6s %-10d %-10ld %-12ld  %04d-%02d-%02d\n",
                pos->data.play_id,
                pos->data.name,
                pos->data.area,
                pos->data.duration,
                pos->data.totaltickets,
                pos->data.sales,
                pos->data.start_date.year,
                pos->data.start_date.month,
                pos->data.start_date.day);
        }
        // 页脚信息
        printf("------------------------------------------------------------------------\n");
        printf(" 第 [%d/%d] 页", Pageing_CurPage(paging), Pageing_TotalPages(paging));
        printf("        [P]上一页  [N]下一页  [R]返回\n");
        printf("请选择操作: ");

        // 获取用户输入
        choice = getchar();
        getchar();

        switch (choice) {
        case 'P':
        case 'p':
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, -1, sales_analysis_list_node_t);
            }
            else {
                printf("已经是第一页。\n");
                getchar();
            }
            break;
        case 'N':
        case 'n':
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, 1, sales_analysis_list_node_t);
            }
            else {
                printf("已经是最后一页。\n");
                getchar();
            }
            break;
        case 'R':
        case 'r':
            // 退出循环，返回上一级
            break;
        default:
            printf("无效输入，请重新选择。\n");
            getchar();
            break;
        }
    } while (choice != 'R' && choice != 'r');
    // 释放链表内存
    if (list != NULL) {
        List_Destroy(list, sales_analysis_list_node_t);
    }
}

// 按演出计划统计销售额
static void show_sales_by_schedule(void)
{
    int play_id = 0;
    system("cls");
    printf("\n========== 按演出计划统计销售额 ==========\n");
    // 输入剧目ID
    printf("请输入剧目ID (输入0查看所有剧目): ");
    if (scanf("%d", &play_id) != 1) {
        printf("输入错误！\n");
        getchar(); getchar();
        return;
    }
    getchar();
    if (play_id > 0) {
        // 获取剧目名称
        play_t play;
        if (!Play_Srv_FetchByID(play_id, &play)) {
            printf("剧目ID %d 不存在！\n", play_id);
            printf("按任意键返回...");
            getchar();
            return;
        }
        printf("剧目: %s\n", play.name);
    }
    // 获取演出计划列表
    schedule_list_t schedule_list = NULL;
    int total_schedules = 0;
    if (play_id > 0) {
        // 获取指定剧目的演出计划
        total_schedules = Schedule_Srv_FetchByPlay(play_id, &schedule_list);
    }
    else {
        // 获取所有演出计划
        total_schedules = Schedule_Srv_FetchAll(&schedule_list);
    }
    if (total_schedules <= 0) {
        printf("暂无演出计划数据。\n");
        printf("按任意键返回...");
        getchar();
        return;
    }
    printf("\n%-8s %-30s %-12s %-8s %-8s %-10s\n",
        "计划ID", "剧目名称", "日期", "时间", "演出厅", "售票数");
    printf("----------------------------------------------------------------\n");
    int total_sales = 0;
    int total_tickets = 0;
    schedule_list_node_t* schedule_node = (schedule_list_node_t*)LIST_NODE(schedule_list)->next;
    int count = 0;
    while (schedule_node != schedule_list) {
        int schedule_id = schedule_node->data.id;

        // 获取剧目名称
        play_t play;
        Play_Srv_FetchByID(schedule_node->data.play_id, &play);

        // 统计该演出计划的销售情况
        int ticket_count = 0;
        int sales_amount = 0;

        // 这里需要调用一个统计函数
        // Salesanalysis_Srv_StatBySchedule(schedule_id, &ticket_count, &sales_amount);

        printf("%-8d %-30s %04d-%02d-%02d %02d:%02d  %-8d %-8d\n",
            schedule_id,
            play.name,
            schedule_node->data.date.year,
            schedule_node->data.date.month,
            schedule_node->data.date.day,
            schedule_node->data.time.hour,
            schedule_node->data.time.minute,
            schedule_node->data.studio_id,
            ticket_count);

        total_sales += sales_amount;
        total_tickets += ticket_count;

        schedule_node = (schedule_list_node_t*)schedule_node->node.next;
        count++;

        // 分页显示
        if (count % DEFAULT_PAGE_SIZE == 0) {
            printf("\n--- 第 %d 页，按任意键继续...", count / DEFAULT_PAGE_SIZE);
            getchar();
        }
    }

    printf("----------------------------------------------------------------\n");
    printf("总计: 演出计划数=%d, 总票数=%d, 总额=%d元\n",
        total_schedules, total_tickets, total_sales);

    // 释放链表
    if (schedule_list) {
        List_Destroy(schedule_list, schedule_list_node_t);
    }

    printf("\n按任意键返回...");
    getchar();
}

// 按时间段统计销售额
static void show_sales_by_time_period(void)
{
    ttms_date_t start_date, end_date;

    system("cls");
    printf("\n========== 按时间段统计销售额 ==========\n");

    // 输入起始日期
    printf("请输入起始日期 (格式: 年 月 日): ");
    if (scanf("%d %d %d", &start_date.year, &start_date.month, &start_date.day) != 3) {
        printf("日期格式错误！\n");
        getchar(); getchar();
        return;
    }
    getchar();

    // 输入结束日期
    printf("请输入结束日期 (格式: 年 月 日): ");
    if (scanf("%d %d %d", &end_date.year, &end_date.month, &end_date.day) != 3) {
        printf("日期格式错误！\n");
        getchar(); getchar();
        return;
    }
    getchar();

    // 验证日期有效性
    if (start_date.year > end_date.year ||
        (start_date.year == end_date.year && start_date.month > end_date.month) ||
        (start_date.year == end_date.year && start_date.month == end_date.month && start_date.day > end_date.day)) {
        printf("错误：起始日期不能晚于结束日期！\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    // 调用业务逻辑层统计
    sales_analysis_list_t result_list = NULL;
    int total_sales = 0;
    int total_tickets = 0;

    int result = Salesanalysis_Srv_StatByTimePeriod(start_date, end_date, &result_list, &total_sales, &total_tickets);

    printf("\n统计时间段: %04d-%02d-%02d 至 %04d-%02d-%02d\n",
        start_date.year, start_date.month, start_date.day,
        end_date.year, end_date.month, end_date.day);

    if (result <= 0) {
        printf("该时间段内无销售记录。\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    printf("\n%-8s %-30s %-10s %-10s\n",
        "剧目ID", "剧目名称", "上座数", "销售额");
    printf("-------------------------------------------------\n");

    sales_analysis_list_node_t* node = (sales_analysis_list_node_t*)LIST_NODE(result_list)->next;
    int count = 0;

    while (node != result_list) {
        printf("%-8d %-30s %-10ld %-10ld\n",
            node->data.play_id,
            node->data.name,
            node->data.totaltickets,
            node->data.sales);

        node = (sales_analysis_list_node_t*)node->node.next;
        count++;

        // 分页显示
        if (count % DEFAULT_PAGE_SIZE == 0) {
            printf("\n--- 第 %d 页，按任意键继续...", count / DEFAULT_PAGE_SIZE);
            getchar();
        }
    }

    printf("-------------------------------------------------\n");
    printf("总计: 销售剧目数=%d, 总票数=%d, 总销售额=%d元\n",
        result, total_tickets, total_sales);

    // 释放链表
    if (result_list) {
        List_Destroy(result_list, sales_analysis_list_node_t);
    }

    printf("\n按任意键返回...");
    getchar();
}

// 票房统计报表 - 专门用于经理的"统计票房"功能
static void show_box_office_report(void)
{
    sales_analysis_list_t list = NULL;
    paging_t paging;
    char choice;
    int totalCount = 0;

    system("cls");
    printf("\n=================== 票房统计报表 ===================\n");

    // 调用业务逻辑层函数进行统计
    totalCount = Salesanalysis_Srv_StatByPlay(&list);
    if (totalCount <= 0 || list == NULL) {
        printf("\n当前没有票房统计数据。\n");
        printf("=====================================================\n");
        printf("按任意键返回...");
        getchar();
        getchar(); // 等待按键
        return;
    }

    // 初始化分页器
    paging.totalRecords = totalCount;
    paging.pageSize = DEFAULT_PAGE_SIZE;
    paging.offset = 0;
    paging.curPos = NULL;

    // 主显示循环
    do {
        system("cls");
        printf("\n=================== 票房统计报表 (共%d条) ===================\n", totalCount);

        // 表头
        printf(" 剧目ID  剧目名称                地区  时长(分)  上座数      票房(元)  上映日期\n");
        printf("------------------------------------------------------------------------\n");

        // 使用分页宏遍历当前页
        sales_analysis_list_node_t* pos;
        int i = 0;
        Paging_ViewPage_ForEach(list, paging, sales_analysis_list_node_t, pos, i) {
            printf(" %-8d %-22s %-6s %-10d %-10ld %-12ld  %04d-%02d-%02d\n",
                pos->data.play_id,
                pos->data.name,
                pos->data.area,
                pos->data.duration,
                pos->data.totaltickets,
                pos->data.sales,
                pos->data.start_date.year,
                pos->data.start_date.month,
                pos->data.start_date.day);
        }

        // 页脚信息
        printf("------------------------------------------------------------------------\n");
        printf(" 第 [%d/%d] 页", Pageing_CurPage(paging), Pageing_TotalPages(paging));
        printf("        [P]上一页  [N]下一页  [R]返回\n");
        printf("请选择操作: ");

        // 获取用户输入
        choice = getchar();
        getchar();

        switch (choice) {
        case 'P':
        case 'p':
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, -1, sales_analysis_list_node_t);
            }
            else {
                printf("已经是第一页。\n");
                getchar();
            }
            break;
        case 'N':
        case 'n':
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, 1, sales_analysis_list_node_t);
            }
            else {
                printf("已经是最后一页。\n");
                getchar();
            }
            break;
        case 'R':
        case 'r':
            // 退出循环，返回上一级
            break;
        default:
            printf("无效输入，请重新选择。\n");
            getchar();
            break;
        }
    } while (choice != 'R' && choice != 'r');

    // 释放链表内存
    if (list != NULL) {
        List_Destroy(list, sales_analysis_list_node_t);
    }
}