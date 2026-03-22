#pragma once
#ifndef PLAY_PERSIST_H
#define PLAY_PERSIST_H
#include "Play.h" 
int Play_Perst_SelectAll(play_list_t list);
int Play_Perst_Insert(play_t* data);
int Play_Perst_Update(const play_t* data);
int Play_Perst_RemByID(int id);
int Play_Perst_SelectByID(int id, play_t* buf);
//从剧目信息文件Play.dat中载入剧目名称包含指定字符串的剧目信息
int Play_Perst_SelectByName(play_list_t list, char condt[]);
#endif