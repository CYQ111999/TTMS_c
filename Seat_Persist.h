#pragma once
#ifndef SEAT_PERSIST_H
#define SEAT_PERSIST_H
#include "Seat.h"
int Seat_Perst_Insert(seat_t* data);
int Seat_Perst_InsertBatch(seat_list_t list);
int Seat_Perst_Update(const seat_t* data);
int Seat_Perst_RemoveByID(int ID);
int Seat_Perst_RemoveAllByRoomID(int roomID);
int Seat_Perst_SelectAll(seat_list_t list);
// 修改函数签名，使用双重指针
int Seat_Perst_SelectByRoomID(seat_list_t* list, int roomID);
#endif