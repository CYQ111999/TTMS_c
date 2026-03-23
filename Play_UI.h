#pragma once
#ifndef PLAY_UI_H
#define PLAY_UI_H
#include "Play.h"   
void Play_UI_MgtEntry(void);
int Play_UI_Add(void);
int Play_UI_Modify(int id);
int Play_UI_Delete(int id);
int Play_UI_Query(int id);
int Play_UI_ListAll(void);  // 显示所有剧目信息
#endif