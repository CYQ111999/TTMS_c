// Salesanalysis_Srv.h
#ifndef SALESANALYSIS_SRV_H
#define SALESANALYSIS_SRV_H

#include "Common.h"

// 按剧目统计票房
int Salesanalysis_Srv_StatByPlay(sales_analysis_list_t* list);

// 按时间段统计销售额
int Salesanalysis_Srv_StatByTimePeriod(ttms_date_t start_date,
    ttms_date_t end_date,
    sales_analysis_list_t* result_list,
    int* total_sales,
    int* total_tickets);

#endif