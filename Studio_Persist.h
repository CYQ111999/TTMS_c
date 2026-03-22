#pragma once
//Studio_Persist.h
//zyp
#ifndef STUDIO_UI_H
#define STUDIO_UI_H
#include "Common.h"
// 函数功能：用于向文件中添加一个新演出厅数据
int Studio_Perst_Insert(studio_t* data);

//函数功能：用于在文件中更新一个演出厅数据
int Studio_Perst_Update(const studio_t* data);

//函数功能：用于从文件中载入一个演出厅的数据
int Studio_Perst_SelectByID(int ID, studio_t* buf);

// 函数功能：用于从文件中载入所有演出厅数据
int Studio_Perst_SelectAll(studio_list_t* list);

//函数功能：用于在文件中删除指定ID的演出厅数据
int Studio_Perst_RemoveByID(int ID);

#endif