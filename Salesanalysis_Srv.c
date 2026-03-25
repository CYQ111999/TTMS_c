// Salesanalysis_Srv.c
// cyq写

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "List.h"
#include "Play_Persist.h"
#include "Schedule_Persist.h"
#include "Ticket_Persist.h"
#include "Sale_Persist.h"
#include "Salesanalysis.h"
#define _CRT_SECURE_NO_WARNINGS

// 按剧目统计票房 - 主函数
int Salesanalysis_Srv_StatByPlay(sales_analysis_list_t* list) {
    play_list_t play_list = NULL;
    schedule_list_t sched_list = NULL;
    ticket_list_t ticket_list = NULL;
    sales_analysis_list_t result_list = NULL;
    int play_count = 0;
    int result_count = 0;

    if (list == NULL) {
        return 0;
    }

    // 初始化返回的统计结果链表list
    List_Init(result_list, sales_analysis_list_node_t);
    if (result_list == NULL) {
        return 0;
    }

    // 初始化剧目链表
    List_Init(play_list, play_list_node_t);
    if (play_list == NULL) {
        List_Destroy(result_list, sales_analysis_list_node_t);
        return 0;
    }

    // 调用 Play_Perst_SelectAll，获取所有剧目信息链表B
    play_count = Play_Perst_SelectAll(&play_list);
    if (play_count <= 0) {
        List_Destroy(play_list, play_list_node_t);
        List_Destroy(result_list, sales_analysis_list_node_t);
        return 0;
    }

    // 遍历每个剧目，计算其票房
    play_list_node_t* play_node;
    List_ForEach(play_list, play_node, play_list_node_t) {
        long total_sales = 0;     // 累计销售额
        long total_tickets = 0;   // 累计票数

        // 初始化演出计划链表
        sched_list = NULL;
        List_Init(sched_list, schedule_list_node_t);
        if (sched_list == NULL) {
            continue;  // 跳过这个剧目
        }

        // 针对剧目链表B中某个剧目，调用 Schedule_Perst_SelectByPlay
        // 获取该剧目的所有演出计划，存入链表C
        Schedule_Perst_SelectByPlay(&sched_list, play_node->data.id);

        // 遍历该剧目的每个演出计划
        schedule_list_node_t* sched_node;
        List_ForEach(sched_list, sched_node, schedule_list_node_t) {
            // 初始化票链表
            ticket_list = NULL;
            List_Init(ticket_list, ticket_list_node_t);
            if (ticket_list == NULL) {
                continue;  // 跳过这个演出计划
            }

            // 针对演出计划链表C中某个演出计划，调用 Ticket_Perst_SelectByScheduleID
            // 获取该计划的所有票，存入链表D
            Ticket_Perst_SelectByScheduleID(sched_node->data.id, &ticket_list);

            // 遍历该计划下的每张票
            ticket_list_node_t* ticket_node;
            List_ForEach(ticket_list, ticket_node, ticket_list_node_t) {
                // 针对票链表D中状态为"已售"的票，调用 Sale_Perst_SelByTicket(票ID)
                // 获取该票的销售记录
                if (ticket_node->data.status == TICKET_SOLD) {
                    sale_t sale_rec;
                    // 假设 Sale_Perst_SelByTicket 函数存在，返回销售记录
                    if (Sale_Perst_SelByTicket(ticket_node->data.id, &sale_rec) > 0) {
                        // 累计销售额
                        total_sales += sale_rec.value; // sale_rec.value 可能为正（售票）或负（退票）
                        total_tickets++;
                    }
                }
            }

            // 清理票链表D，为下一个演出计划做准备
            if (ticket_list) {
                List_Destroy(ticket_list, ticket_list_node_t);
                ticket_list = NULL;
            }
        }

        // 清理演出计划链表C，为下一个剧目做准备
        if (sched_list) {
            List_Destroy(sched_list, schedule_list_node_t);
            sched_list = NULL;
        }

        // 生成一个 salesanalysis_t 节点，填入剧目信息和累计值，插入链表list
        if (total_tickets > 0) { // 只统计有销售的剧目，可根据需求调整
            sales_analysis_list_node_t* stat_node =
                (sales_analysis_list_node_t*)malloc(sizeof(sales_analysis_list_node_t));
            if (stat_node) {
                stat_node->data.play_id = play_node->data.id;
                strncpy(stat_node->data.name, play_node->data.name, sizeof(stat_node->data.name) - 1);
                strncpy(stat_node->data.area, play_node->data.area, sizeof(stat_node->data.area) - 1);
                stat_node->data.duration = play_node->data.duration;
                stat_node->data.price = play_node->data.price;
                stat_node->data.start_date = play_node->data.start_date;
                stat_node->data.end_date = play_node->data.end_date;
                stat_node->data.totaltickets = total_tickets;
                stat_node->data.sales = total_sales;
                List_AddTail(result_list, stat_node);
                result_count++;
            }
        }
    }

    // 清理资源
    if (play_list) {
        List_Destroy(play_list, play_list_node_t);
    }
    if (sched_list) {
        List_Destroy(sched_list, schedule_list_node_t);
    }
    if (ticket_list) {
        List_Destroy(ticket_list, ticket_list_node_t);
    }

    // 返回结果
    if (result_count > 0) {
        *list = result_list;
        return result_count;
    }
    else {
        List_Destroy(result_list, sales_analysis_list_node_t);
        *list = NULL;
        return 0;
    }
}

// 按时间段统计销售额
int Salesanalysis_Srv_StatByTimePeriod(ttms_date_t start_date,
    ttms_date_t end_date,
    sales_analysis_list_t* result_list,
    int* total_sales,
    int* total_tickets) {
    play_list_t play_list = NULL;
    schedule_list_t sched_list = NULL;
    ticket_list_t ticket_list = NULL;
    sales_analysis_list_t temp_list = NULL;
    int play_count = 0;
    int result_count = 0;

    // 初始化总销售额和总票数
    if (total_sales) *total_sales = 0;
    if (total_tickets) *total_tickets = 0;

    if (result_list == NULL) {
        return 0;
    }

    // 初始化返回的统计结果链表
    List_Init(temp_list, sales_analysis_list_node_t);
    if (temp_list == NULL) {
        return 0;
    }

    // 初始化剧目链表
    List_Init(play_list, play_list_node_t);
    if (play_list == NULL) {
        List_Destroy(temp_list, sales_analysis_list_node_t);
        return 0;
    }

    // 获取所有剧目信息
    play_count = Play_Perst_SelectAll(&play_list);
    if (play_count <= 0) {
        List_Destroy(play_list, play_list_node_t);
        List_Destroy(temp_list, sales_analysis_list_node_t);
        return 0;
    }

    // 遍历每个剧目
    play_list_node_t* play_node;
    List_ForEach(play_list, play_node, play_list_node_t) {
        long play_total_sales = 0;     // 剧目累计销售额
        long play_total_tickets = 0;   // 剧目累计票数

        // 初始化演出计划链表
        sched_list = NULL;
        List_Init(sched_list, schedule_list_node_t);
        if (sched_list == NULL) {
            continue;  // 跳过这个剧目
        }

        // 获取该剧目的所有演出计划
        Schedule_Perst_SelectByPlay(&sched_list, play_node->data.id);

        // 遍历该剧目的每个演出计划
        schedule_list_node_t* sched_node;
        List_ForEach(sched_list, sched_node, schedule_list_node_t) {
            // 检查演出日期是否在指定时间段内
            if (isDateInRange(sched_node->data.date, start_date, end_date)) {
                // 初始化票链表
                ticket_list = NULL;
                List_Init(ticket_list, ticket_list_node_t);
                if (ticket_list == NULL) {
                    continue;  // 跳过这个演出计划
                }

                // 获取该计划的所有票
                Ticket_Perst_SelectByScheduleID(sched_node->data.id, &ticket_list);

                // 遍历该计划下的每张票
                ticket_list_node_t* ticket_node;
                List_ForEach(ticket_list, ticket_node, ticket_list_node_t) {
                    // 只统计已售出的票
                    if (ticket_node->data.status == TICKET_SOLD) {
                        sale_t sale_rec;
                        // 获取该票的销售记录
                        if (Sale_Perst_SelByTicket(ticket_node->data.id, &sale_rec) > 0) {
                            // 累计销售额（注意：退票的value是负的）
                            play_total_sales += sale_rec.value;
                            play_total_tickets++;
                        }
                    }
                }
                // 清理票链表
                if (ticket_list) {
                    List_Destroy(ticket_list, ticket_list_node_t);
                    ticket_list = NULL;
                }
            }
        }

        // 清理演出计划链表
        if (sched_list) {
            List_Destroy(sched_list, schedule_list_node_t);
            sched_list = NULL;
        }

        // 如果该剧目在指定时间段内有销售记录，则添加到结果链表
        if (play_total_tickets > 0) {
            sales_analysis_list_node_t* stat_node =
                (sales_analysis_list_node_t*)malloc(sizeof(sales_analysis_list_node_t));
            if (stat_node) {
                // 填充剧目信息
                stat_node->data.play_id = play_node->data.id;
                strncpy(stat_node->data.name, play_node->data.name, sizeof(stat_node->data.name) - 1);
                strncpy(stat_node->data.area, play_node->data.area, sizeof(stat_node->data.area) - 1);
                stat_node->data.duration = play_node->data.duration;
                stat_node->data.price = play_node->data.price;
                stat_node->data.start_date = play_node->data.start_date;
                stat_node->data.end_date = play_node->data.end_date;
                stat_node->data.totaltickets = play_total_tickets;
                stat_node->data.sales = play_total_sales;

                List_AddTail(temp_list, stat_node);
                result_count++;

                // 累加到总计数
                if (total_sales) *total_sales += play_total_sales;
                if (total_tickets) *total_tickets += play_total_tickets;
            }
        }
    }

    // 清理资源
    if (play_list) {
        List_Destroy(play_list, play_list_node_t);
    }

    // 设置返回链表
    if (result_count > 0) {
        *result_list = temp_list;
        return result_count;
    }
    else {
        List_Destroy(temp_list, sales_analysis_list_node_t);
        *result_list = NULL;
        return 0;
    }
}

// 检查日期是否在指定范围内
static int isDateInRange(ttms_date_t date, ttms_date_t start, ttms_date_t end) {
    // 将日期转换为整数比较
    int date_num = date.year * 10000 + date.month * 100 + date.day;
    int start_num = start.year * 10000 + start.month * 100 + start.day;
    int end_num = end.year * 10000 + end.month * 100 + end.day;

    return (date_num >= start_num && date_num <= end_num);
}