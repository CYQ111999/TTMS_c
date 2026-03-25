//zyp
#include "Common.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _CRT_SECURE_NO_WARNINGS
// 根据剧目ID获取演出计划
int Schedule_Srv_FetchByPlay(int play_id, schedule_list_t* list)
{
    int rtn = 0;
    if (list == NULL || play_id <= 0) {
        printf("错误：Schedule_Srv_FetchByPlay 参数无效\n");
        return 0;
    }
    // 将参数list和play_id传递给持久化层根据剧目ID载入演出计划函数
    rtn = Schedule_Perst_SelectByPlay(list, play_id);
    return rtn;
}
// 添加新演出计划
int Schedule_Srv_Add(schedule_t* data)
{
    int rtn = 0;
    if (data == NULL) {
        printf("错误：Schedule_Srv_Add 参数为NULL\n");
        return 0;
    }
    if (data->play_id <= 0) {
        printf("错误：剧目ID无效\n");
        return 0;
    }
    if (data->studio_id <= 0) {
        printf("错误：演出厅ID无效\n");
        return 0;
    }
    if (data->seat_count <= 0) {
        printf("错误：座位数无效\n");
        return 0;
    }
    if (data->date.year < 1900 || data->date.year > 2100 ||
        data->date.month < 1 || data->date.month > 12 ||
        data->date.day < 1 || data->date.day > 31) {
        printf("错误：日期无效\n");
        return 0;
    }
    if (data->time.hour < 0 || data->time.hour > 23 ||
        data->time.minute < 0 || data->time.minute > 59) {
        printf("错误：时间无效\n");
        return 0;
    }
    printf("调试：调用持久层插入数据...\n");
    rtn = Schedule_Perst_Insert(data);
    if (rtn) {
        printf("调试：插入成功，分配的ID=%d\n", data->id);
    }
    else {
        printf("调试：插入失败\n");
    }
    return rtn;
}

// 修改演出计划
int Schedule_Srv_Modify(const schedule_t* data)
{
    int rtn = 0;
    if (data == NULL) {
        printf("错误：演出计划数据为空！\n");
        return 0;
    }
    if (data->id <= 0) {
        printf("错误：演出计划ID无效！\n");
        return 0;
    }
    if (data->play_id <= 0) {
        printf("错误：剧目ID无效！\n");
        return 0;
    }
    if (data->studio_id <= 0) {
        printf("错误：演出厅ID无效！\n");
        return 0;
    }
    if (data->seat_count <= 0) {
        printf("错误：座位数必须大于0！\n");
        return 0;
    }
    // 验证日期格式 - 修改为整型验证
    if (data->date.year < 2000 || data->date.year > 2100) {
        printf("错误：年份必须在2000-2100之间！\n");
        return 0;
    }
    if (data->date.month < 1 || data->date.month > 12) {
        printf("错误：月份必须在1-12之间！\n");
        return 0;
    }
    if (data->date.day < 1 || data->date.day > 31) {
        printf("错误：日期必须在1-31之间！\n");
        return 0;
    }
    int valid_day = 1;
    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int is_leap_year = 0;
    if ((data->date.year % 4 == 0 && data->date.year % 100 != 0) || (data->date.year % 400 == 0)) {
        is_leap_year = 1;
    }
    if (data->date.month == 2) {
        int feb_days = is_leap_year ? 29 : 28;
        if (data->date.day > feb_days) {
            valid_day = 0;
        }
    }
    else {
        if (data->date.day > days_in_month[data->date.month - 1]) {
            valid_day = 0;
        }
    }
    if (!valid_day) {
        printf("错误：日期无效！%d年%d月最多有%d天\n",
            data->date.year,
            data->date.month,
            (data->date.month == 2) ? (is_leap_year ? 29 : 28) :
            days_in_month[data->date.month - 1]);
        return 0;
    }
    if (data->time.hour < 0 || data->time.hour >= 24) {
        printf("错误：小时必须在0-23之间！\n");
        return 0;
    }
    if (data->time.minute < 0 || data->time.minute >= 60) {
        printf("错误：分钟必须在0-59之间！\n");
        return 0;
    }
    if (data->time.second < 0 || data->time.second >= 60) {
        printf("错误：秒必须在0-59之间！\n");
        return 0;
    }
    printf("调试：验证通过，准备更新演出计划\n");
    printf("  ID: %d, 剧目ID: %d, 演出厅ID: %d\n",
        data->id, data->play_id, data->studio_id);
    printf("  日期: %04d-%02d-%02d, 时间: %02d:%02d:%02d\n",
        data->date.year, data->date.month, data->date.day,
        data->time.hour, data->time.minute, data->time.second);
    printf("  座位数: %d\n", data->seat_count);
    // 调用持久化层更新演出计划函数Schedule_Perst_Update对数据进行修改
    rtn = Schedule_Perst_Update(data);
    if (rtn) {
        printf(" 演出计划更新成功！\n");
    }
    else {
        printf(" 演出计划更新失败！\n");
    }
    return rtn;
}

//根据ID删除演出计划
int Schedule_Srv_DeleteByID(int id)
{
    int rtn = 0;
    // 参数验证
    if (id <= 0) {
        printf("错误：演出计划ID无效！\n");
        return 0;
    }
    // 根据要删除的演出计划ID值，调用持久化层根据ID去除演出计划函数
    //Schedule_Perst_RemByID删除相应的数据，随后接收持久化层函数的返回值rtn
    rtn = Schedule_Perst_RemByID(id);
    return rtn;
}

//将界面层建立的演出计划链表的头指针传递给持久化层函数
int Schedule_Srv_FetchAll(schedule_list_t list)
{
    int rtn = 0;
    if (list == NULL) {
        return 0;
    }
    //  将参数 list (界面层初始化的空链表头指针，用来保存演出计划信息) 作为实参调用
    rtn = Schedule_Perst_SelectAll(list);
    return rtn;
}

//调用持久化层函数获取剧目信息，将获取到的所有满足条件的剧目信息按顺序保存在链表list 上
int Play_Srv_FetchByName(play_list_t list, char condt[])
{
    int rtn = 0;
    if (list == NULL || condt == NULL) {
        return 0;
    }
    // 验证条件字符串长度
    if (strlen(condt) == 0) {
        printf("错误：剧目名称不能为空！\n");
        return 0;
    }
    // 将参数 condt 作为实参，调用持久化层根据名称载入剧目
    rtn = Play_Perst_SelectByName(list, condt);
    return rtn;
}

//根据演出计划ID，获取对应的演出计划详细信息
int Schedule_Srv_FetchByID(int id, schedule_t* buf) {
    int rtn = 0;
    if (id <= 0 || buf == NULL) {
        return 0;
    }
    rtn = Schedule_Perst_SelectByID(id, buf);
    return rtn;
}