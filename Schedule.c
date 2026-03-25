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
    // 参数验证
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

    // 参数验证
    if (data == NULL) {
        printf("错误：Schedule_Srv_Add 参数为NULL\n");
        return 0;
    }

    // 验证必填字段
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

    // 验证日期和时间
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
    rtn = Schedule_Perst_Insert(data);
    return rtn;
}

// 修改演出计划
int Schedule_Srv_Modify(const schedule_t* data)
{
    int rtn = 0; 
    // 参数验证
    if (data == NULL) {
        return 0;
    }
    // 数据验证
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
    // 验证日期和时间格式
    if (strlen(data->date.year) != 4 || strlen(data->date.month) != 2 || strlen(data->date.day) != 2) {
        printf("错误：日期格式无效！\n");
        return 0;
    }
    if (strlen(data->time.hour) != 2 || strlen(data->time.minute) != 2) {
        printf("错误：时间格式无效！\n");
        return 0;
    }
    // 调用持久化层更新演出计划函数Schedule_Perst_Update对数据进行修改，接收其返回值rtn
    rtn = Schedule_Perst_Update(data);
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
    // 参数验证
    if (list == NULL) {
        return 0;
    }
    // a) 将参数 list (界面层初始化的空链表头指针，用来保存演出计划信息) 作为实参调用
    //    持久化层 Schedule_Perst_SelectAll 函数，并接收其返回值 rtn；
    rtn = Schedule_Perst_SelectAll(list);
    return rtn;
}

//调用持久化层函数获取剧目信息，将获取到的所有满足条件的剧目信息按顺序保存在链表list 上
int Play_Srv_FetchByName(play_list_t list, char condt[])
{
    int rtn = 0; 
    // 参数验证
    if (list == NULL || condt == NULL) {
        return 0;
    }
    // 验证条件字符串长度
    if (strlen(condt) == 0) {
        printf("错误：剧目名称不能为空！\n");
        return 0;
    }
    // 将参数 condt 作为实参，调用持久化层根据名称载入剧目
    // Play_Perst_SelectByName 函数，并接受其返回值 rtn；
    rtn = Play_Perst_SelectByName(list, condt);
    return rtn;
}

//根据演出计划ID，获取对应的演出计划详细信息
int Schedule_Srv_FetchByID(int id, schedule_t* buf) {
    int rtn = 0; 
    //  参数基础验证
    if (id <= 0 || buf == NULL) {
        return 0;
    }
    //  调用持久化层对应函数 Schedule_Perst_SelectByID
    rtn = Schedule_Perst_SelectByID(id, buf);
    return rtn;
}