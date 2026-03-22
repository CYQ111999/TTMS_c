#pragma once
//zyp
#ifndef SCHEDULE_UI_H
#define SCHEDULE_UI_H
#include "Common.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//函数功能：从演出计划数据文件中载入与指定剧目关联的演出计划信息，构成链表list
int Schedule_Perst_SelectByPlay(schedule_list_t list, int play_id);

//函数功能：向演出计划文件Schedule.dat的末尾添加一条新的演出计划
int Schedule_Perst_Insert(schedule_t* data);

//函数功能：更新演出计划数据文件Schedule.dat中的一条演出计划信息
int Schedule_Perst_Update(const schedule_t* data);

//函数功能：去除演出计划数据文件Schedule.dat中指定ID的演出计划
int Schedule_Perst_RemByID(int id);
//从演出计划数据文件Schedule.dat中载入所有演出计划，加载到链表list上
int Schedule_Perst_SelectAll(schedule_list_t list);

//从文件Schedule.dat中载入指定ID的演出计划
int Schedule_Perst_SelectByID(int id, schedule_t* buf);
#endif /* SCHEDULE_PERSIST_H */