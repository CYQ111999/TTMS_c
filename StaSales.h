#pragma once
#pragma once
#ifndef STASALES_H
#define STASALES_H
#include "Common.h"

extern sale_list_t sale_list;

//根据ID获取销售额统计
int StaSales_Srv_CompSaleVal(int usrID, ttms_date_t stDate, ttms_date_t endDate);

#endif