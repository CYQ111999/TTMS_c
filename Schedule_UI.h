#pragma once
//zyp
//Schedule_UI.h
#ifndef SCHEDULE_UI_H
#define SCHEDULE_UI_H

#include <stdio.h>
#include <stdlib.h>
#include "Schedule.h"
#include "Play.h"

//函数功能：显示与ID号为play_id的剧目相关联的所有演出计划，并提供增、删、改演出计划的功能
void Schedule_UI_MgtEntry(void);

//函数功能：接收键盘输入的演出计划信息，通过调用业务逻辑层函数添加新演出计划
int Schedule_UI_Add(int play_id);

//函数功能：根据演出计划的ID值，调用业务逻辑层函数修改演出计划数据
int Schedule_UI_Modify(int id);

//函数功能：根据参数中的演出计划ID号，通过调用业务逻辑层函数删除演出计划
int Schedule_UI_Delete(int id);
//函数功能：以列表形式显示所有的演出计划信息，列表上方显示表头信息，表头下方显示与所有演出计划中第一页的信息，列表下方显示查询功能选项。
void Schedule_UI_ListAll(void);
//函数功能：显示与参数关联的演出计划的信息。
int Schedule_UI_Query(const char* play_name);

#endif