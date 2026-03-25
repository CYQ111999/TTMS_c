// Play.h
#pragma once
#ifndef PLAY_H
#define PLAY_H

#include "Common.h"

int Play_Perst_SelectByName(play_list_t* list, char condt[]);
int Play_Perst_SelectAll(play_list_t* list);
int Play_Perst_Insert(play_t* data);
int Play_Perst_Update(const play_t* data);
int Play_Perst_RemByID(int id);
int Play_Perst_SelectByID(int id, play_t* buf);

#endif