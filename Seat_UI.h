#pragma once
#ifndef SEAT_UI_H
#define SEAT_UI_H
#include "Seat.h"
void Seat_UI_MgtEntry(void);
int Seat_UI_Add(seat_list_t list, int roomID, int rowsCount, int colsCount);
int Seat_UI_Modify(seat_list_t list, int rowsCount, int colsCount);
int Seat_UI_Delete(seat_list_t list, int rowsCount, int colsCount);
char Seat_UI_Status2Char(seat_status_t status);
seat_status_t Seat_UI_Char2Status(char statusChar);
#endif