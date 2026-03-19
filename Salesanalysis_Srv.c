// Salesanalysis_Srv.c
//cyq写

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "List.h"
#include "Play_Persist.h"
#include "Schedule_Persist.h"
#include "Ticket_Persist.h"
#include "Sale_Persist.h"

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
    // 调用 Play_Perst_SelectAll，获取所有剧目信息链表B
    play_count = Play_Perst_SelectAll(&play_list);
    if (play_count <= 0) {
        List_Destroy(result_list, sales_analysis_list_node_t);
        return 0;
    }
    // 遍历每个剧目，计算其票房
    play_list_node_t* play_node;
    List_ForEach(play_list, play_node, play_list_node_t) {
        long total_sales = 0;     // 累计销售额
        long total_tickets = 0;   // 累计票数
        //针对剧目链表B中某个剧目，调用 Schedule_Perst_SelectByPlay
        //获取该剧目的所有演出计划，存入链表C
        Schedule_Perst_SelectByPlay(play_node->data.id, &sched_list);
        // 遍历该剧目的每个演出计划
        schedule_list_node_t* sched_node;
        List_ForEach(sched_list, sched_node, schedule_list_node_t) {
            // 针对演出计划链表C中某个演出计划，调用 Ticket_Perst_SelBySchedule
            //    获取该计划的所有票，存入链表D
            Ticket_Perst_SelBySchedule(sched_node->data.id, &ticket_list);
            // 遍历该计划下的每张票
            ticket_list_node_t* ticket_node;
            List_ForEach(ticket_list, ticket_node, ticket_list_node_t) {
                // g. 针对票链表D中状态为“已售”的票，调用 Sale_Perst_SelByTicket(票ID)
                //    获取该票的销售记录
                if (ticket_node->data.status == TICKET_SOLD) {
                    sale_t sale_rec;
                    // 假设 Sale_Perst_SelByTicket 函数存在，返回销售记录
                    if (Sale_Perst_SelByTicket(ticket_node->data.id, &sale_rec) > 0) {
                        // h. 累计销售额
                        total_sales += sale_rec.value; // sale_rec.value 可能为正（售票）或负（退票）
                        total_tickets++;
                    }
                }
            }
            //清理票链表D，为下一个演出计划做准备
            if (ticket_list) {
                List_Destroy(ticket_list, ticket_list_node_t);
                ticket_list = NULL;
            }
        }
        //清理演出计划链表C，为下一个剧目做准备
        if (sched_list) {
            List_Destroy(sched_list, schedule_list_node_t);
            sched_list = NULL;
        }
        //生成一个 salesanalysis_t 节点，填入剧目信息和累计值，插入链表list
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