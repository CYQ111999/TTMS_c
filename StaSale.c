#include "StaSales.h"
#include "Common.h"
#include <stdio.h>
#include <string.h>

sale_list_t sale_list = NULL;

int StaSales_Srv_CompSaleVal(int usrID, ttms_date_t stDate, ttms_date_t endDate) {
    // 1. 输入参数基本校验
    if (usrID <= 0) {
        return 0; // 无效的用户ID
    }

    // 检查日期顺序：结束日期不应早于开始日期
    if (stDate.year > endDate.year ||
        (stDate.year == endDate.year && stDate.month > endDate.month) ||
        (stDate.year == endDate.year && stDate.month == endDate.month && stDate.day > endDate.day)) {
        return 0; // 日期区间无效
    }

    int totalSales = 0;              // 销售总额
    sale_list_t saleCur = NULL;      // 用于遍历销售链表的指针
    int isRefunded = 0;             // 标志位，标记当前销售的票是否已被退

    // 2. 检查用户是否存在（从data/account.dat文件）
    // 由于usrID是传入的参数，我们需要验证这个用户是否存在
    if (!isUserExist(usrID)) {
        printf("错误：用户ID %d 不存在！\n", usrID);
        return 0;
    }


    // 3. 检查销售记录链表是否为空
    if (List_IsEmpty(sale_list)) {
        return 0; // 没有销售记录，直接返回0
    }

    // 4. 遍历销售记录链表
    List_ForEach(sale_list, saleCur) {
        isRefunded = 0; // 为每条销售记录重置退票标志

        // 条件a: 检查售票员ID是否匹配
        if (saleCur->data.user_id != usrID) {
            continue; // 不匹配，检查下一条
        }

        // 条件b: 检查销售日期是否在指定区间内
        if (!isDateInRange(&(saleCur->data.date), &stDate, &endDate)) {
            continue; // 日期不在区间内，检查下一条
        }

        // 条件c: 检查该票是否已被退票
        // 根据sale_t的定义，type字段表示交易类型：SALE_SELL=1(买票), SALE_REFOUND=-1(退票)
        // 如果当前记录是买票记录(SALE_SELL)，需要检查是否有对应的退票记录
        if (saleCur->data.type == SALE_SELL) {
            // 检查是否存在对应的退票记录
            if (isTicketRefunded(saleCur->data.ticket_id)) {
                continue; // 票已退，跳过
            }
        }
        else if (saleCur->data.type == SALE_REFOUND) {
            // 如果是退票记录，直接跳过（不计入销售额）
            continue;
        }

        // 所有条件满足，累加销售金额
        totalSales += saleCur->data.value;
    }

    return totalSales;
}

static int isUserExist(int usrID) {
    FILE* fp = NULL;
    account_t account;

    fp = fopen("data/account.dat", "rb");
    if (fp == NULL) {
        // 文件不存在或无法打开，可能还没有创建用户文件
        return 0;
    }

    // 遍历账户文件，查找匹配的用户ID
    while (fread(&account, sizeof(account_t), 1, fp) == 1) {
        if (account.id == usrID) {
            fclose(fp);
            return 1;  // 用户存在
        }
    }

    fclose(fp);
    return 0;  // 用户不存在
}

static int isDateInRange(const ttms_date_t* date, const ttms_date_t* stDate, const ttms_date_t* endDate) {
    // 检查是否早于起始日期
    if (date->year < stDate->year ||
        (date->year == stDate->year && date->month < stDate->month) ||
        (date->year == stDate->year && date->month == stDate->month && date->day < stDate->day)) {
        return 0;
    }

    // 检查是否晚于结束日期
    if (date->year > endDate->year ||
        (date->year == endDate->year && date->month > endDate->month) ||
        (date->year == endDate->year && date->month == endDate->month && date->day > endDate->day)) {
        return 0;
    }

    return 1;  // 日期在区间内
}

static int isTicketRefunded(int ticketID) {
    sale_list_t saleCur = NULL;

    if (((sale_list) == ((void*)0) ? 1 : (((list_node_t*)(sale_list))->next == ((list_node_t*)(sale_list)) && ((list_node_t*)(sale_list))->prev == ((list_node_t*)(sale_list))))) {
        return 0; // 链表为空，没有退票记录
    }

    // 遍历销售记录链表，查找该票的退票记录
    List_ForEach(sale_list, saleCur) {
        // 如果是退票记录，并且退的就是这个票
        if (saleCur->data.type == SALE_REFOUND && saleCur->data.ticket_id == ticketID) {
            return 1;  // 票已被退
        }
    }

    return 0;  // 票未被退
}