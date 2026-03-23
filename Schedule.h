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

//根据参数中的剧目ID号，查找与该剧目相关的演出计划
int Schedule_Srv_FetchByPlay(schedule_list_t list, int play_id);
// 函数功能：将参数data作为实参调用持久化层存储新演出计划函数，并将持久化层函数的返回值传递给界面层函数
int Schedule_Srv_Add(schedule_t* data);
// 函数功能：用参数data中的演出计划数据作为实参，通过调用持久化层函数来修改文件中记录的旧的演出计划信息
int Schedule_Srv_Modify(const schedule_t* data);
//函数功能：根据参数id记录的演出计划ID号，调用持久化层函数删除相应演出计划
int Schedule_Srv_DeleteByID(int id);
//将界面层建立的演出计划链表的头指针传递给持久化层函数
int Schedule_Srv_FetchAll(schedule_list_t list);
//*函数功能：按剧目名称获取剧目函数，将参数中的剧目名称 condt 作为实际参数，调用持久
//* 化层函数获取剧目信息，将获取到的所有满足条件的剧目信息按顺序保存在链表list 上
int Play_Srv_FetchByName(play_list_t list, char condt[]);
//根据演出计划ID，获取对应的演出计划详细信息
int Schedule_Srv_FetchByID(int id, schedule_t* buf);

#endif