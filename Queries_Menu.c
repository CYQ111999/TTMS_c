// Queries_Menu.c
// 演出计划查询模块 - 界面层

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "List.h"
#include "Schedule.h"
#include "Play.h"
#define _CRT_SECURE_NO_WARNINGS
#ifndef QUERIES_PAGE_SIZE
#define QUERIES_PAGE_SIZE 10
#endif

// 内部函数：格式化显示单个演出计划
static void display_schedule_detail(const schedule_t* sched, const play_t* play) {
    if (!sched) return;

    printf("┌─────────────── 演出计划详情 ───────────────┐\n");
    printf("│ 计划ID: %-34d │\n", sched->id);

    if (play) {
        printf("│ 剧目  : %-34s │\n", play->name);
    }
    else {
        printf("│ 剧目  : %-34s │\n", "未知剧目");
    }

    printf("│ 日期  : %04d-%02d-%02d                     │\n",
        sched->date.year, sched->date.month, sched->date.day);
    printf("│ 时间  : %02d:%02d:%02d                     │\n",
        sched->time.hour, sched->time.minute, sched->time.second);
    printf("│ 演出厅: %-34d │\n", sched->studio_id);
    printf("│ 座位数: %-34d │\n", sched->seat_count);
    printf("└─────────────────────────────────────────────┘\n");
}

// 内部函数：显示演出计划列表（分页）- 使用分页宏
static void display_schedule_list(schedule_list_t list, paging_t* paging) {
    if (List_IsEmpty(list) || paging->totalRecords <= 0) {
        printf("暂无演出计划数据。\n");
        return;
    }

    printf("\n%-8s %-20s %-12s %-8s %-10s %-8s\n",
        "计划ID", "剧目", "日期", "时间", "演出厅", "座位数");
    printf("────────────────────────────────────────────────────────────────────\n");

    schedule_list_node_t* pos = NULL;
    int i = 0;

    // 使用分页宏遍历当前页
    Paging_ViewPage_ForEach(list, *paging, schedule_list_node_t, pos, i) {
        // 跳过链表头节点
        if (pos == list) {
            continue;
        }

        // 获取关联的剧目信息
        play_t play_info = { 0 };
        const char* play_name = "未知剧目";
        if (Play_Srv_FetchByID(pos->data.play_id, &play_info)) {
            play_name = play_info.name;
        }

        // 显示演出计划信息
        printf("%-8d %-20s %04d-%02d-%02d %02d:%02d  %-10d %-8d\n",
            pos->data.id,
            play_name,
            pos->data.date.year, pos->data.date.month, pos->data.date.day,
            pos->data.time.hour, pos->data.time.minute,
            pos->data.studio_id,
            pos->data.seat_count);
    }

    printf("────────────────────────────────────────────────────────────────────\n");
}

// 演出计划查询主界面 (被 Main_Menu 调用)
void Queries_Menu_Schedule(void) {
    schedule_list_t sched_list = NULL;
    paging_t paging = { 0 };
    char choice;
    int total = 0;

    system("cls");
    printf("\n=================== 演出计划查询 ===================\n");

    // 1. 获取所有演出计划数据
    total = Schedule_Srv_FetchAll(&sched_list);
    if (total <= 0 || List_IsEmpty(sched_list)) {
        printf("\n当前没有演出计划。\n");
        printf("===================================================\n");
        printf("按任意键返回...");
        getchar();
        return;
    }

    // 2. 初始化分页器
    paging.totalRecords = total;
    paging.pageSize = QUERIES_PAGE_SIZE;
    paging.offset = 0;
    paging.curPos = (void*)sched_list;  // 初始化为链表头

    // 3. 定位到第一页
    Paging_Locate_FirstPage(sched_list, paging);

    // 4. 主查询界面循环
    do {
        system("cls");
        printf("\n=================== 演出计划查询 (共%d条) ===================\n", total);

        // 显示当前页
        display_schedule_list(sched_list, &paging);

        // 计算分页信息
        int current_page = Pageing_CurPage(paging);
        int total_pages = Pageing_TotalPages(paging);

        // 显示分页信息
        printf("\n 第 [%d/%d] 页", current_page, total_pages);

        // 显示操作菜单
        printf("        [D]查看详情  [P]上一页  [N]下一页  [R]返回\n");
        printf("请选择操作: ");

        choice = getchar();
        getchar(); // 清除回车

        switch (choice) {
        case 'D':
        case 'd': {
            int sched_id;
            printf("请输入要查看的演出计划ID: ");
            if (scanf("%d", &sched_id) == 1) {
                getchar();

                // 查找并显示详情
                schedule_list_node_t* pos = NULL;
                int found = 0;

                // 遍历整个链表查找
                List_ForEach(sched_list, pos) {
                    if (pos != sched_list) {  // 跳过链表头
                        schedule_list_node_t* node = (schedule_list_node_t*)pos;
                        if (node->data.id == sched_id) {
                            play_t play_info = { 0 };
                            play_t* p_play = NULL;
                            if (Play_Srv_FetchByID(node->data.play_id, &play_info)) {
                                p_play = &play_info;
                            }
                            display_schedule_detail(&node->data, p_play);
                            found = 1;
                            break;
                        }
                    }
                }

                if (!found) {
                    printf("未找到ID为 %d 的演出计划。\n", sched_id);
                }

                printf("按任意键继续...");
                getchar();
            }
            else {
                printf("输入格式错误！\n");
                getchar();
            }
            break;
        }

        case 'P':
        case 'p':
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(sched_list, paging, -1, schedule_list_node_t);
                printf("跳转到上一页...\n");
            }
            else {
                printf("已经是第一页。\n");
                printf("按任意键继续...");
                getchar();
            }
            break;

        case 'N':
        case 'n':
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(sched_list, paging, 1, schedule_list_node_t);
                printf("跳转到下一页...\n");
            }
            else {
                printf("已经是最后一页。\n");
                printf("按任意键继续...");
                getchar();
            }
            break;

        case 'R':
        case 'r':
            // 退出循环
            break;

        default:
            printf("无效选择。\n");
            printf("按任意键继续...");
            getchar();
            break;
        }

    } while (choice != 'R' && choice != 'r');

    // 5. 清理资源
    if (sched_list) {
        // 使用您提供的宏销毁链表
        List_Destroy(sched_list, schedule_list_node_t);
    }

    printf("\n查询结束，返回主菜单。\n");
}