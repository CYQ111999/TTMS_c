#pragma once
#ifndef PLAY_H
#define PLAY_H
#define PLAY_PAGE_SIZE 5   
// 分页大小，组内各个成员不同模块可能该值不同，因此在Play头文件中
#include "Common.h"
// 获取全部剧目
int Play_Srv_FetchAll(play_list_t list);

// 添加新剧目
int Play_Srv_Add(play_t* data);

// 修改剧目
int Play_Srv_Modify(const play_t* data);

// 根据ID删除剧目
int Play_Srv_DeleteByID(int id);

// 根据ID获取剧目
int Play_Srv_FetchByID(int id, play_t* buf);



#endif