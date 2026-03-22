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
    if (!sched || !play) return;

    printf("┌─────────────── 演出计划详情 ───────────────┐\n");
    printf("│ 计划ID: %-34d │\n", sched->id);
    printf("│ 剧目  : %-34s │\n", play->name);
    printf("│ 日期  : %04d-%02d-%02d                     │\n",
        sched->date.year, sched->date.month, sched->date.day);
    printf("│ 时间  : %02d:%02d:%02d                     │\n",
        sched->time.hour, sched->time.minute, sched->time.second);
    printf("│ 演出厅: %-34d │\n", sched->studio_id);
    printf("│ 座位数: %-34d │\n", sched->seat_count);
    printf("└─────────────────────────────────────────────┘\n");
}

// 内部函数：显示演出计划列表（分页）
static void display_schedule_list(schedule_list_t list, int start, int pageSize) {
    if (!list) {
        printf("暂无演出计划数据。\n");
        return;
    }

    printf("\n%-8s %-20s %-12s %-8s %-10s %-8s\n",
        "计划ID", "剧目", "日期", "时间", "演出厅", "座位数");
    printf("────────────────────────────────────────────────────────────────────\n");

    schedule_list_node_t* pos = list;
    int count = 0;

    // 移动到起始位置
    while (pos && count < start) {
        pos = (schedule_list_node_t*)pos->node.next;
        count++;
    }

    // 显示一页数据
    count = 0;
    while (pos && count < pageSize && &pos->node != &(list)->node) {
        // 获取关联的剧目信息（这里简化处理，实际应通过剧目ID查询）
        play_t play_info = { 0 };
        Play_Srv_FetchByID(pos->data.play_id, &play_info);

        printf("%-8d %-20s %04d-%02d-%02d %02d:%02d  %-10d %-8d\n",
            pos->data.id,
            play_info.name,
            pos->data.date.year, pos->data.date.month, pos->data.date.day,
            pos->data.time.hour, pos->data.time.minute,
            pos->data.studio_id,
            pos->data.seat_count);

        pos = (schedule_list_node_t*)pos->node.next;
        count++;
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
    if (total <= 0 || !sched_list) {
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

    // 3. 主查询界面循环
    do {
        system("cls");
        printf("\n=================== 演出计划查询 (共%d条) ===================\n", total);

        // 显示当前页
        display_schedule_list(sched_list, paging.offset, paging.pageSize);

        // 显示分页信息
        printf("\n 第 [%d/%d] 页",
            (paging.offset / paging.pageSize) + 1,
            (total + paging.pageSize - 1) / paging.pageSize);

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
                schedule_list_node_t* pos = sched_list;
                int found = 0;

                while (pos && &pos->node != &(sched_list)->node) {
                    if (pos->data.id == sched_id) {
                        play_t play_info = { 0 };
                        Play_Srv_FetchByID(pos->data.play_id, &play_info);
                        display_schedule_detail(&pos->data, &play_info);
                        found = 1;
                        break;
                    }
                    pos = (schedule_list_node_t*)pos->node.next;
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
            if (paging.offset >= paging.pageSize) {
                paging.offset -= paging.pageSize;
            }
            else {
                printf("已经是第一页。\n");
                getchar();
            }
            break;

        case 'N':
        case 'n':
            if (paging.offset + paging.pageSize < total) {
                paging.offset += paging.pageSize;
            }
            else {
                printf("已经是最后一页。\n");
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

    // 4. 清理资源
    if (sched_list) {
        List_Destroy(sched_list, schedule_list_node_t);
    }

    printf("\n查询结束，返回主菜单。\n");
}