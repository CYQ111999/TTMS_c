#pragma once
#ifndef SCHEDULE_PERSIST_H
#define SCHEDULE_PERSIST_H

#include "common.h"

int Schedule_Perst_SelectByID(int id, schedule_t* buf);
int Schedule_Perst_SelectByPlayID(int play_id, schedule_list_t list);

#endif