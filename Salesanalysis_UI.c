// Salesanalysis_UI.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "List.h"
#include "Salesanalysis.h"

// 统计票房报表界面
void Salesanalysis_UI_Report(void) {
    sales_analysis_list_t list = NULL; // 票房统计链表
    paging_t paging;                   // 分页器
    char choice;                       // 用户选择
    int totalCount = 0;                // 总记录数
    int pageSize = 10;                 // 每页显示条数，可按需调整
    system("cls"); // Windows清屏
    printf("\n=================== 票房统计报表 ===================\n");
    // 调用业务逻辑层函数进行统计
    totalCount = Salesanalysis_Srv_StatByPlay(&list);
    if (totalCount <= 0 || list == NULL) {
        printf("\n当前没有票房统计数据。\n");
        printf("=====================================================\n");
        printf("按任意键返回...");
        getchar();
        getchar(); // 等待按键
        return;
    }
    // 2. 初始化分页器 (教材表2.4)
    paging.totalRecords = totalCount;
    paging.pageSize = pageSize;
    paging.offset = 0;
    paging.curPos = NULL;
    // 3. 主显示循环
    do {
        system("cls");
        printf("\n=================== 票房统计报表 (共%d条) ===================\n", totalCount);
        // 表头 (教材图4.84样式)
        printf(" 剧目ID  剧目名称                地区  时长(分)  上座数      票房(元)  上映日期\n");
        printf("------------------------------------------------------------------------\n");
        // 使用分页宏遍历当前页 (教材表2.4，宏5)
        sales_analysis_list_node_t* pos;
        int i = 0;
        Paging_ViewPage_ForEach(list, paging, sales_analysis_list_node_t, pos, i) {
            printf(" %-8d %-22s %-6s %-10d %-10ld %-12ld  %04d-%02d-%02d\n",
                pos->data.play_id,
                pos->data.name,
                pos->data.area,
                pos->data.duration,
                pos->data.totaltickets,
                pos->data.sales,
                pos->data.start_date.year,
                pos->data.start_date.month,
                pos->data.start_date.day);
        }
        // 页脚信息
        printf("------------------------------------------------------------------------\n");
        printf(" 第 [%d/%d] 页", Pageing_CurPage(paging), Pageing_TotalPages(paging));
        printf("        [P]上一页  [N]下一页  [R]返回\n");
        printf("请选择操作: ");
        // 获取用户输入
        choice = getchar();
        getchar(); 
        switch (choice) {
        case 'P':
        case 'p':
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, -1, sales_analysis_list_node_t);
            }
            else {
                printf("已经是第一页。\n");
                getchar();
            }
            break;
        case 'N':
        case 'n':
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, 1, sales_analysis_list_node_t);
            }
            else {
                printf("已经是最后一页。\n");
                getchar();
            }
            break;
        case 'R':
        case 'r':
            // 退出循环，返回上一级
            break;
        default:
            printf("无效输入，请重新选择。\n");
            getchar();
            break;
        }
    } while (choice != 'R' && choice != 'r');
    // 4. 退出前释放链表内存
    if (list != NULL) {
        List_Destroy(list, sales_analysis_list_node_t);
    }
}