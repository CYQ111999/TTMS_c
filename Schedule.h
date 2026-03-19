#pragma once
#ifndef SCHEDULE_SRV_H
#define SCHEDULE_SRV_H

#include "Common.h"

int Schedule_Srv_FetchByID(int id, schedule_t* buf);
int Schedule_Srv_FetchByPlay(int play_id, schedule_list_t list);

#endif