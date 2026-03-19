#pragma once
#ifndef SEAT_H
#define SEAT_H

#include "Common.h"

// 座位业务逻辑层函数声明

// 初始化演出厅所有座位
int Seat_Srv_RoomInit(seat_list_t list, int roomID, int rowsCount, int colsCount);

// 添加新座位
int Seat_Srv_Add(seat_t* data);

// 修改座位
int Seat_Srv_Modify(const seat_t* data);

// 根据座位ID删除座位
int Seat_Srv_DeleteByID(int ID);

// 根据演出厅ID删除所有座位
int Seat_Srv_DeleteAllByRoomID(int roomID);

// 根据演出厅ID获取所有座位
int Seat_Srv_FetchByRoomID(seat_list_t list, int roomID);

// 根据演出厅ID获取有效座位
int Seat_Srv_FetchValidByRoomID(seat_list_t list, int roomID);

// 根据行列号获取座位节点
seat_list_node_t* Seat_Srv_FindByRowCol(seat_list_t list, int row, int column);

// 对座位链表进行按行按列排序
void Seat_Srv_SortSeatList(seat_list_t list);

// 将新节点加入到已排序链表
void Seat_Srv_AddToSortedList(seat_list_t list, seat_list_node_t* node);

#endif