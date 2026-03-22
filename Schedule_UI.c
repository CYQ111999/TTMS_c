//zyp
//Schedule_UI.c
#include "Common.h"
#include "Schedule.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include "Schedule_UI.h"  // 添加头文件声明
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>  // 添加ctype.h用于toupper
#define _CRT_SECURE_NO_WARNINGS
#ifndef DEFAULT_PAGE_SIZE
#define DEFAULT_PAGE_SIZE 10
#endif

// 日期解析函数
static int parse_date_from_string(const char* date_str, ttms_date_t* date_ptr)
{
    if (date_str == NULL || date_ptr == NULL) {
        return 0;
    }

    int year, month, day;
    if (sscanf(date_str, "%4d-%2d-%2d", &year, &month, &day) == 3) {
        // 验证日期范围
        if (year < 2000 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) {
            return 0;
        }

        // 将数值转换为字符串
        snprintf(date_ptr->year, sizeof(date_ptr->year), "%04d", year);
        snprintf(date_ptr->month, sizeof(date_ptr->month), "%02d", month);
        snprintf(date_ptr->day, sizeof(date_ptr->day), "%02d", day);

        return 1;
    }

    return 0;
}

// 时间解析函数
static int parse_time_from_string(const char* time_str, ttms_time_t* time_ptr)
{
    if (time_str == NULL || time_ptr == NULL) {
        return 0;
    }

    int hour, minute;
    if (sscanf(time_str, "%2d:%2d", &hour, &minute) == 2) {
        // 验证时间范围
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
            return 0;
        }

        // 将数值转换为字符串
        snprintf(time_ptr->hour, sizeof(time_ptr->hour), "%02d", hour);
        snprintf(time_ptr->minute, sizeof(time_ptr->minute), "%02d", minute);
        strcpy(time_ptr->second, "00");

        return 1;
    }

    return 0;
}

// 函数功能：显示演出计划管理界面，用户需输入剧目ID，然后显示与该剧目相关的所有演出计划，并提供增、删、改演出计划的功能
void Schedule_UI_MgtEntry(void)
{
    int play_id;                    // 剧目ID，由用户输入
    play_t play_info;               // 剧目信息
    schedule_list_t schedule_list = NULL;  // 演出计划链表
    paging_t paging;                // 分页器
    char choice;                    // 用户选择
    int schedule_id;                // 演出计划ID
    schedule_t schedule_data;       // 演出计划数据
    int operation_success;          // 操作成功标志
    int input_valid;                // 输入验证标志
    // 1. 获取用户输入的剧目ID
    do {
        system("cls");
        printf("========================================\n");
        printf("        演出计划管理 - 输入剧目ID        \n");
        printf("========================================\n");
        printf("请输入要管理的剧目ID（输入0返回上级菜单）：");
        input_valid = scanf("%d", &play_id);
        // 清除输入缓冲区
        { int c; while ((c = getchar()) != '\n' && c != EOF); }
        if (!input_valid) {
            printf("输入无效！请输入一个有效的数字。\n");
            printf("按任意键继续...");
            getchar();
            continue;
        }
        // 检查用户是否想返回
        if (play_id == 0) {
            printf("返回上级菜单...\n");
            return;
        }
        // 验证剧目ID是否有效
        if (play_id <= 0) {
            printf("剧目ID必须是一个正整数！\n");
            printf("按任意键继续...");
            getchar();
            continue;
        }
        // 2. 调用Play_Srv_FetchByID()函数获取剧目信息
        if (!Play_Srv_FetchByID(play_id, &play_info)) {
            // 获取不到记录，提示错误
            printf("获取剧目信息失败！剧目ID: %d 不存在。\n", play_id);
            printf("按任意键重新输入...");
            getchar();
            continue;
        }
        // 剧目信息获取成功，跳出循环
        break;
    } while (1);
    // 3. 显示剧目信息
    system("cls");
    printf("========================================\n");
    printf("        安排演出 - 剧目信息\n");
    printf("========================================\n");
    printf("剧目ID: %d\n", play_info.id);
    printf("剧目名称: %s\n", play_info.name);
    printf("剧目类型: %d\n", play_info.type);
    // 检查是否有area字段（根据之前定义）
    if (strlen(play_info.area) > 0) {
        printf("地区: %s\n", play_info.area);
    }
    printf("时长: %d分钟\n", play_info.duration);
    printf("========================================\n\n");
    // 4. 调用List_Init()函数初始化演出计划信息链表
    // 注意：从图片看，可能需要使用宏初始化链表
    schedule_list = NULL;
    // 5. 初始化分页参数paging
    paging.totalRecords = 0;
    paging.pageSize = DEFAULT_PAGE_SIZE;
    paging.offset = 0;
    paging.curPos = NULL;
    // 6. 调用Schedule_Srv_FetchByPlay()函数获取与编号为play_id的剧目相关的演出计划
    int totalCount = Schedule_Srv_FetchByPlay(play_id, &schedule_list);
    paging.totalRecords = totalCount;
    // 7. 设置显示第一页
    Paging_Locate_FirstPage(schedule_list, paging);

    // 主循环
    while (1) {
        system("cls");

        // 8. 显示当前页的剧目信息及菜单项
        printf("========================================\n");
        printf("        安排演出 - 剧目: %s\n", play_info.name);
        printf("========================================\n");
        printf("剧目ID: %d\n", play_info.id);
        printf("剧目名称: %s\n", play_info.name);
        printf("剧目类型: %d\n", play_info.type);

        if (strlen(play_info.area) > 0) {
            printf("地区: %s\n", play_info.area);
        }

        printf("时长: %d分钟\n", play_info.duration);
        printf("========================================\n\n");

        // 显示演出计划列表
        if (totalCount > 0) {
            printf("演出计划列表 (共 %d 个):\n", totalCount);
            printf("┌─────┬────────────┬────────────┬────────────┬─────────┐\n");
            printf("│ ID  │ 演出厅ID   │ 放映日期   │ 放映时间   │ 座位数  │\n");
            printf("├─────┼────────────┼────────────┼────────────┼─────────┤\n");

            // 使用分页宏遍历当前页
            schedule_list_node_t* pos = NULL;
            int i = 0;
            Paging_ViewPage_ForEach(schedule_list, paging, schedule_list_node_t, pos, i) {
                if (pos != schedule_list) {  // 跳过链表头
                    printf("│ %-3d │ %-10d │ %-10s │ %-10s │ %-7d │\n",
                        pos->data.id,
                        pos->data.studio_id,
                        "",  // 日期显示需要格式化，这里用空字符串占位
                        "",  // 时间显示需要格式化，这里用空字符串占位
                        pos->data.seat_count);
                }
            }

            printf("└─────┴────────────┴────────────┴────────────┴─────────┘\n");

            // 显示分页信息
            printf("\n--- 第 %d/%d 页，共 %d 条记录 ---\n",
                Pageing_CurPage(paging),
                Pageing_TotalPages(paging),
                totalCount);
        }
        else {
            printf("暂无演出计划数据。\n");
        }

        // 显示菜单
        printf("\n请选择操作：\n");
        printf("  [T] 生成演出票  [A] 添加演出计划  [U] 修改演出计划  [D] 删除演出计划\n");
        printf("  [P] 上一页      [N] 下一页        [R] 返回上级菜单\n");
        printf("请输入选项: ");

        scanf(" %c", &choice);
        getchar();  // 清除换行符

        // 处理用户选择
        switch (toupper(choice)) {
        case 'T':
            // 生成演出票
            printf("请输入演出计划ID: ");
            if (scanf("%d", &schedule_id) != 1) {
                printf("输入无效！\n");
                getchar();
                break;
            }
            getchar();

            if (Schedule_Srv_FetchByID(schedule_id, &schedule_data))
            {
                Ticket_UI_MgtEntry(schedule_id);

                // 重新获取数据
                List_FreeNode(schedule_list, schedule_list_node_t);
                schedule_list = NULL;
                totalCount = Schedule_Srv_FetchByPlay(play_id, &schedule_list);
                paging.totalRecords = totalCount;
                Paging_Locate_FirstPage(schedule_list, paging);
            }
            else {
                printf("演出计划不存在！\n");
            }
            break;

        case 'A':
            operation_success = Schedule_UI_Add(play_id);
            if (operation_success) {
                // 重新获取数据
                List_FreeNode(schedule_list, schedule_list_node_t);
                schedule_list = NULL;
                totalCount = Schedule_Srv_FetchByPlay(play_id, &schedule_list);
                paging.totalRecords = totalCount;
                Paging_Locate_FirstPage(schedule_list, paging);
            }
            break;

        case 'U':
            printf("请输入要修改的演出计划ID: ");
            if (scanf("%d", &schedule_id) != 1) {
                printf("输入无效！\n");
                getchar();
                break;
            }
            getchar();

            if (Schedule_Srv_FetchByID(schedule_id, &schedule_data)) {
                operation_success = Schedule_UI_Modify(schedule_id);
                if (operation_success) {
                    // 重新获取数据
                    List_FreeNode(schedule_list, schedule_list_node_t);
                    schedule_list = NULL;
                    totalCount = Schedule_Srv_FetchByPlay(play_id, &schedule_list);
                    paging.totalRecords = totalCount;
                    Paging_Locate_FirstPage(schedule_list, paging);
                }
            }
            else {
                printf("演出计划不存在！\n");
            }
            break;

        case 'D':
            printf("请输入要删除的演出计划ID: ");
            if (scanf("%d", &schedule_id) != 1) {
                printf("输入无效！\n");
                getchar();
                break;
            }
            getchar();

            if (Schedule_Srv_FetchByID(schedule_id, &schedule_data)) {
                operation_success = Schedule_UI_Delete(schedule_id);
                if (operation_success) {
                    // 重新获取数据
                    List_FreeNode(schedule_list, schedule_list_node_t);
                    schedule_list = NULL;
                    totalCount = Schedule_Srv_FetchByPlay(play_id, &schedule_list);
                    paging.totalRecords = totalCount;
                    Paging_Locate_FirstPage(schedule_list, paging);
                }
            }
            else {
                printf("演出计划不存在！\n");
            }
            break;

        case 'P':
            // 上一页
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(schedule_list, paging, -1, schedule_list_node_t);
            }
            else {
                printf("已经是第一页了！\n");
            }
            break;

        case 'N':
            // 下一页
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(schedule_list, paging, 1, schedule_list_node_t);
            }
            else {
                printf("已经是最后一页了！\n");
            }
            break;

        case 'R':
            // 销毁链表
            List_FreeNode(schedule_list, schedule_list_node_t);
            printf("返回上级菜单...\n");
            return;

        default:
            printf("无效的选项，请重新输入\n");
            break;
        }

        printf("按任意键继续...");
        getchar();
    }
}

//函数功能：接收键盘输入的演出计划信息，通过调用业务逻辑层函数添加新演出计划
int Schedule_UI_Add(int play_id)
{
    schedule_t new_schedule;   // 新演出计划数据
    int studio_id;             // 演出厅ID
    int seats_count;           // 座位数
    int continue_add = 1;      // 是否继续添加的标志
    int success_count = 0;     // 成功添加的计数

    // 验证剧目ID是否有效
    if (play_id <= 0) {
        printf("剧目ID无效！\n");
        return 0;
    }

    do {
        printf("\n══════════════════════════════════════════════════════\n");
        printf("                    添加新演出计划                      \n");
        printf("══════════════════════════════════════════════════════\n");

        // 显示剧目信息
        play_t play_info;
        if (Play_Srv_FetchByID(play_id, &play_info)) {
            printf("剧目信息：\n");
            printf("  ID: %d\n", play_info.id);
            printf("  名称: %s\n", play_info.name);
            printf("  类型: %d\n", play_info.type);
            printf("──────────────────────────────────────────────────\n");
        }

        // 1. 输入演出厅ID
        printf("请输入演出厅ID: ");
        if (scanf("%d", &studio_id) != 1 || studio_id <= 0) {
            printf("演出厅ID无效！请输入一个正整数。\n");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }

        // 验证演出厅是否存在
        studio_t studio_info;
        if (!Studio_Srv_FetchByID(studio_id, &studio_info)) {
            printf(" 演出厅(ID: %d)不存在！\n", studio_id);
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }

        // 2. 输入演出日期
        printf("请输入演出日期(yyyy-mm-dd): ");
        char date_str[11];  // 日期字符串
        if (scanf("%10s", date_str) != 1) {
            printf("日期输入无效！\n");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }

        // 3. 输入演出时间
        printf("请输入演出时间(hh:mm): ");
        char time_str[6];  // 时间字符串
        if (scanf("%5s", time_str) != 1) {
            printf(" 时间输入无效！\n");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }
        // 4. 输入座位数
        printf("请输入座位数: ");
        if (scanf("%d", &seats_count) != 1 || seats_count <= 0) {
            printf(" 座位数无效！请输入一个正整数。\n");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }
        // 验证座位数不超过演出厅最大容量
        if (seats_count > studio_info.seatsCount) {
            printf(" 座位数(%d)超过演出厅最大容量(%d)！\n", seats_count, studio_info.seatsCount);
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }

        // 内联实现清除输入缓冲区
        { int c; while ((c = getchar()) != '\n' && c != EOF); }

        // 5. 填充演出计划数据
        new_schedule.play_id = play_id;
        new_schedule.studio_id = studio_id;

        // 解析并复制日期到 date 结构体
        if (!parse_date_from_string(date_str, &new_schedule.date)) {
            printf("日期格式无效！请输入 yyyy-mm-dd 格式的日期。\n");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }
        // 解析并复制时间到 time 结构体
        if (!parse_time_from_string(time_str, &new_schedule.time)) {
            printf("时间格式无效！请输入 hh:mm 格式的时间。\n");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            printf("\n是否继续添加？(Y/N): ");
            char continue_choice = getchar();
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            if (continue_choice != 'Y' && continue_choice != 'y') {
                break;
            }
            continue;
        }
        new_schedule.seat_count = seats_count;    // 设置座位数

        // 6. 显示确认信息
        printf("\n即将添加的演出计划信息：\n");
        printf("══════════════════════════════════════════════════════\n");
        printf("剧目ID: %d\n", new_schedule.play_id);
        printf("演出厅ID: %d\n", new_schedule.studio_id);
        printf("演出厅名称: %s\n", studio_info.name);
        printf("放映日期: %s\n", new_schedule.date);
        printf("放映时间: %s\n", new_schedule.time);
        printf("座位数: %d\n", new_schedule.seat_count);
        printf("══════════════════════════════════════════════════════\n");
        // 7. 确认添加
        printf("\n确认添加此演出计划？(Y/N): ");
        char confirm = getchar();
        // 内联实现清除输入缓冲区
        { int c; while ((c = getchar()) != '\n' && c != EOF); }

        if (confirm == 'Y' || confirm == 'y') {
            // 8. 调用业务逻辑层函数 Schedule_Srv_Add
            if (Schedule_Srv_Add(&new_schedule)) {
                printf("演出计划添加成功！\n");
                success_count++;
            }
            else {
                printf(" 演出计划添加失败！\n");
            }
        }
        else {
            printf(" 添加操作已取消。\n");
        }

        // 9. 询问是否继续添加
        printf("\n是否继续添加其他演出计划？(Y/N): ");
        char add_more = getchar();
        // 内联实现清除输入缓冲区
        { int c; while ((c = getchar()) != '\n' && c != EOF); }

        if (add_more != 'Y' && add_more != 'y') {
            continue_add = 0;
        }

    } while (continue_add);

    // 10. 返回结果
    if (success_count > 0) {
        printf("\n成功添加了 %d 个演出计划。\n", success_count);
        return 1;
    }
    else {
        return 0;
    }
}

//函数功能：根据演出计划的ID值，调用业务逻辑层函数修改演出计划数据
int Schedule_UI_Modify(int id)
{
    int rtn = 0;                        // a) 成功修改演出计划的标志 rtn 置 0
    schedule_t schedule;                // 存储演出计划数据
    char new_date_str[11];              // 新日期字符串
    char new_time_str[6];               // 新时间字符串
    int new_seat_count;                 // 新座位数
    char confirm;                       // 确认修改
    // b) 调用 Schedule_Srv_FetchByID() 函数，根据待修改的演出计划 ID 查看是否存在
    if (!Schedule_Srv_FetchByID(id, &schedule)) {
        // c) 若不存在，则输出演出计划不存在，返回 0
        printf("演出计划(ID: %d)不存在！\n", id);
        return 0;
    }
    // 显示当前演出计划信息
    printf("\n══════════════════════════════════════════════════════\n");
    printf("                    修改演出计划                     \n");
    printf("══════════════════════════════════════════════════════\n");
    printf("当前演出计划信息：\n");
    printf("──────────────────────────────────────────────────\n");
    printf("演出计划ID: %d\n", schedule.id);
    printf("剧目ID: %d\n", schedule.play_id);
    printf("演出厅ID: %d\n", schedule.studio_id);
    printf("放映日期: %s-%s-%s\n",
        schedule.date.year, schedule.date.month, schedule.date.day);
    printf("放映时间: %s:%s:%s\n",
        schedule.time.hour, schedule.time.minute, schedule.time.second);
    printf("座位数: %d\n", schedule.seat_count);
    printf("──────────────────────────────────────────────────\n");

    // 获取剧目信息用于显示
    play_t play_info;
    if (Play_Srv_FetchByID(schedule.play_id, &play_info)) {
        printf("剧目名称: %s\n", play_info.name);
    }

    // 获取演出厅信息用于显示
    studio_t studio_info;
    if (Studio_Srv_FetchByID(schedule.studio_id, &studio_info)) {
        printf("演出厅名称: %s (容量: %d)\n", studio_info.name, studio_info.seatsCount);
    }

    // 检查该演出计划是否已经有票售出
    int has_tickets = 0;  // 假设有检查票务的函数
    // 这里可以调用票务服务层函数检查是否有票售出
    // has_tickets = Ticket_Srv_CheckSoldCount(id);

    printf("\n请修改演出计划信息（直接按回车键保持原值）：\n");

    // 获取新的日期
    printf("新的放映日期(yyyy-mm-dd)[%s-%s-%s]: ",
        schedule.date.year, schedule.date.month, schedule.date.day);
    getchar();  // 清除之前的换行符
    fgets(new_date_str, sizeof(new_date_str), stdin);
    new_date_str[strcspn(new_date_str, "\n")] = '\0';  // 去除换行符

    // 如果用户输入了新的日期
    if (strlen(new_date_str) > 0) {
        ttms_date_t new_date;
        if (parse_date_from_string(new_date_str, &new_date)) {
            schedule.date = new_date;  // 更新日期
        }
        else {
            printf("日期格式无效！请使用 yyyy-mm-dd 格式。\n");
            printf("按回车键返回...");
            getchar();
            return 0;
        }
    }
    // 获取新的时间
    printf("新的放映时间(hh:mm)[%s:%s]: ",
        schedule.time.hour, schedule.time.minute);
    fgets(new_time_str, sizeof(new_time_str), stdin);
    new_time_str[strcspn(new_time_str, "\n")] = '\0';  // 去除换行符
    // 如果用户输入了新的时间
    if (strlen(new_time_str) > 0) {
        ttms_time_t new_time;
        if (parse_time_from_string(new_time_str, &new_time)) {
            schedule.time = new_time;  // 更新时间
        }
        else {
            printf(" 时间格式无效！请使用 hh:mm 格式。\n");
            printf("按回车键返回...");
            getchar();
            return 0;
        }
    }

    // 获取新的座位数
    printf("新的座位数[%d]: ", schedule.seat_count);
    char seat_count_str[20];
    fgets(seat_count_str, sizeof(seat_count_str), stdin);
    // 如果用户输入了新的座位数
    if (strlen(seat_count_str) > 1) {  // 不仅仅是换行符
        new_seat_count = atoi(seat_count_str);
        // 验证座位数
        if (new_seat_count <= 0) {
            printf("座位数必须大于0！\n");
            printf("按回车键返回...");
            getchar();
            return 0;
        }
        // 验证座位数是否超过演出厅容量
        if (Studio_Srv_FetchByID(schedule.studio_id, &studio_info)) {
            if (new_seat_count > studio_info.seatsCount) {
                printf("座位数(%d)超过演出厅最大容量(%d)！\n",
                    new_seat_count, studio_info.seatsCount);
                printf("按回车键返回...");
                getchar();
                return 0;
            }
        }
        // 如果已经有票售出，座位数只能增加，不能减少
        if (has_tickets && new_seat_count < schedule.seat_count) {
            printf("该演出计划已有票售出，座位数不能减少！\n");
            printf("按回车键返回...");
            getchar();
            return 0;
        }
        schedule.seat_count = new_seat_count;
    }
    // 显示修改后的信息
    printf("\n修改后的演出计划信息：\n");
    printf("──────────────────────────────────────────────────\n");
    printf("演出计划ID: %d\n", schedule.id);
    printf("剧目ID: %d\n", schedule.play_id);
    printf("演出厅ID: %d\n", schedule.studio_id);
    printf("放映日期: %s-%s-%s\n",
        schedule.date.year, schedule.date.month, schedule.date.day);
    printf("放映时间: %s:%s:%s\n",
        schedule.time.hour, schedule.time.minute, schedule.time.second);
    printf("座位数: %d\n", schedule.seat_count);
    printf("──────────────────────────────────────────────────\n");
    // 确认修改
    printf("\n确定要修改此演出计划信息吗？(Y/N): ");
    confirm = getchar();
    getchar();  // 清除换行符
    if (confirm != 'Y' && confirm != 'y') {
        printf("修改操作已取消。\n");
        return 0;
    }
    // g) 调用 Schedule_Srv_Modify() 函数，修改演出计划
    if (Schedule_Srv_Modify(&schedule)) {
        // h) 若修改成功，则输出修改成功，rtn=1
        printf("演出计划修改成功！\n");
        rtn = 1;
    }
    else {
        // i) 若修改失败，则输出修改失败
        printf("演出计划修改失败！\n");
    }

    // j) 返回 rtn
    return rtn;
}
//函数功能：根据参数中的演出计划ID号，通过调用业务逻辑层函数删除演出计划
int Schedule_UI_Delete(int id)
{
    int rtn = 0;                //  成功删除演出计划的标志 rtn 置 0
    schedule_t schedule;        // 存储演出计划信息
    char confirm;               // 确认删除标志
    // 先检查演出计划是否存在
    if (!Schedule_Srv_FetchByID(id, &schedule)) {
        printf(" 演出计划(ID: %d)不存在！\n", id);
        return 0;
    }
    // 显示演出计划信息
    printf("\n══════════════════════════════════════════════════════\n");
    printf("                    删除演出计划                     \n");
    printf("══════════════════════════════════════════════════════\n");
    printf("演出计划信息：\n");
    printf("──────────────────────────────────────────────────\n");
    printf("演出计划ID: %d\n", schedule.id);
    printf("剧目ID: %d\n", schedule.play_id);
    printf("演出厅ID: %d\n", schedule.studio_id);
    printf("放映日期: %s-%s-%s\n",
        schedule.date.year, schedule.date.month, schedule.date.day);
    printf("放映时间: %s:%s:%s\n",
        schedule.time.hour, schedule.time.minute, schedule.time.second);
    printf("座位数: %d\n", schedule.seat_count);
    printf("──────────────────────────────────────────────────\n");
    // 获取剧目信息用于显示
    play_t play_info;
    if (Play_Srv_FetchByID(schedule.play_id, &play_info)) {
        printf("剧目名称: %s\n", play_info.name);
    }
    // 获取演出厅信息用于显示
    studio_t studio_info;
    if (Studio_Srv_FetchByID(schedule.studio_id, &studio_info)) {
        printf("演出厅名称: %s\n", studio_info.name);
    }
    // 检查该演出计划是否已经有票售出
    int has_tickets = 0;  // 假设有检查票务的函数
    // 这里可以调用票务服务层函数检查是否有票售出
    // has_tickets = Ticket_Srv_CheckSoldCount(id);
    if (has_tickets) {
        printf(" 警告：该演出计划已有票售出，删除将同时删除所有相关票务！\n");
    }
    printf("\n确定要删除此演出计划吗？(Y/N): ");
    scanf(" %c", &confirm);
    if (confirm != 'Y' && confirm != 'y') {
        printf("删除操作已取消。\n");
        return 0;
    }
    // b) 调用Schedule_Srv_DeleteByID()函数，根据待删除的演出计划ID删除演出计划
    if (Schedule_Srv_DeleteByID(id)) {
        printf("演出计划删除成功！\n");
        rtn = 1;
        // 同时调用Ticket_Srv_DeleteAllByScheduleID()函数删除该演出计划的所有票
        // 注意：这里假设有Ticket_Srv_DeleteAllByScheduleID函数
        if (Ticket_Srv_DeleteAllByScheduleID(id)) {
            printf("该演出计划的所有票务也已成功删除。\n");
        }
        else {
            printf(" 演出计划已删除，但票务删除失败或没有票务数据。\n");
        }
    }
    else {
        printf("演出计划删除失败！\n");
    }
    return rtn;
}

//函数以列表形式显示所有的演出计划信息，列表上方显示表头信息，表头下方显示与所有演出计划中第一页的信息，列表下方显示查询功能选项。
void Schedule_UI_ListAll(void)
{
    schedule_list_t list = NULL;      // 演出计划链表
    paging_t paging;                 // 分页器
    char choice;                     // 用户输入
    schedule_list_node_t* pos;       // 当前节点指针
    int index;                       // 循环索引
    int total_count;                 // 总记录数

    // a) 初始化演出计划链表list，设置分页器参数
    List_Init(list, schedule_list_node_t);

    paging.totalRecords = 0;
    paging.pageSize = DEFAULT_PAGE_SIZE;
    paging.offset = 0;
    paging.curPos = (void*)list;

    // b) 调用业务逻辑层获取所有演出计划信息
    total_count = Schedule_Srv_FetchAll(&list);
    if (total_count <= 0) {
        printf("当前没有演出计划数据。\n");
        List_Destroy(list, schedule_list_node_t);
        return;
    }

    paging.totalRecords = total_count;

    // 设置显示第一页
    Paging_Locate_FirstPage(list, paging);

    int exit_flag = 0;

    do {
        system("cls");

        // 显示标题
        printf("=================================================\n");
        printf("               查询演出 - 所有演出计划           \n");
        printf("=================================================\n");

        // 显示表头
        printf("%-6s %-20s %-12s %-10s %-8s %-8s\n",
            "计划ID", "剧目名称", "演出厅", "日期", "时间", "座位数");
        printf("---------------------------------------------------------------\n");

        // c) 显示当前页数据
        index = 0;
        Paging_ViewPage_ForEach(list, paging, schedule_list_node_t, pos, index) {
            if (pos != list) {  // 跳过链表头节点
                // 获取剧目信息
                play_t play_info;
                int has_play = Play_Srv_FetchByID(pos->data.play_id, &play_info);

                // 获取演出厅信息
                studio_t studio_info;
                int has_studio = Studio_Srv_FetchByID(pos->data.studio_id, &studio_info);

                // d) 显示演出计划信息和剧目名称、演出厅名称
                printf("%-6d %-20s %-12s %s-%s-%s %s:%s:%s %-8d\n",
                    pos->data.id,
                    has_play ? play_info.name : "未知",
                    has_studio ? studio_info.name : "未知",
                    pos->data.date.year, pos->data.date.month, pos->data.date.day,
                    pos->data.time.hour, pos->data.time.minute, pos->data.time.second,
                    pos->data.seat_count);
            }
        }

        // 显示分页信息
        printf("\n--- 第 %d/%d 页，共 %d 条记录 ---\n",
            Pageing_CurPage(paging),
            Pageing_TotalPages(paging),
            total_count);

        // 显示功能菜单
        printf("\n请选择操作：\n");
        printf("  [Q] 按剧目名称查询  [P] 上一页  [N] 下一页  [R] 返回\n");
        printf("请输入选项: ");
        // 内联实现清除输入缓冲区
        { int c; while ((c = getchar()) != '\n' && c != EOF); }
        choice = getchar();
        // 处理用户输入
        switch (toupper(choice)) {
        case 'Q':
            // 调用查询函数
        {
            char play_name[100];
            printf("\n请输入剧目名称: ");
            // 内联实现清除输入缓冲区
            { int c; while ((c = getchar()) != '\n' && c != EOF); }
            fgets(play_name, sizeof(play_name), stdin);
            // 去除换行符
            play_name[strcspn(play_name, "\n")] = '\0';

            int query_count = Schedule_UI_Query(play_name);
            printf("查询到 %d 个演出计划。\n", query_count);

            printf("按任意键继续...");
            getchar();
        }
        break;
        case 'P':
            // 上一页
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, -1, schedule_list_node_t);
            }
            else {
                printf("已经是第一页了！\n");
                printf("按任意键继续...");
                getchar();
            }
            break;
        case 'N':
            // 下一页
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(list, paging, 1, schedule_list_node_t);
            }
            else {
                printf("已经是最后一页了！\n");
                printf("按任意键继续...");
                getchar();
            }
            break;
        case 'R':
            // 返回
            exit_flag = 1;
            break;
        default:
            printf("无效的选项，请重新输入！\n");
            printf("按任意键继续...");
            getchar();
            break;
        }
    } while (!exit_flag);
    //  销毁链表
    List_Destroy(list, schedule_list_node_t);
}

//函数功能：显示与参数关联的演出计划的信息。
int Schedule_UI_Query(const char* play_name)
{
    char search_name[100];           // 搜索的剧目名称
    play_list_t play_list = NULL;    // 剧目链表
    schedule_list_t schedule_list = NULL;  // 演出计划链表
    int total_count = 0;             // 总演出计划数量
    char user_input[100];            // 用户输入
    //  提示用户输入待查的剧目名称
    if (play_name == NULL || strlen(play_name) == 0) {
        printf("请输入要查询的剧目名称: ");
        fgets(user_input, sizeof(user_input), stdin);
        user_input[strcspn(user_input, "\n")] = '\0';
        strncpy(search_name, user_input, sizeof(search_name) - 1);
        search_name[sizeof(search_name) - 1] = '\0';
    }
    else {
        strncpy(search_name, play_name, sizeof(search_name) - 1);
        search_name[sizeof(search_name) - 1] = '\0';
    }

    if (strlen(search_name) == 0) {
        printf("剧目名称不能为空！\n");
        return 0;
    }
    // 调用Play_Srv_FetchByName获取剧目信息
    // 注意：假设有Play_Srv_FetchByName函数，如果没有需要实现
    // 这里我们先使用Play_Srv_FetchAll，然后手动筛选
    play_list_t all_plays = NULL;
    int all_play_count = Play_Srv_FetchAll(&all_plays);
    if (all_play_count <= 0) {
        printf("系统中没有剧目数据。\n");
        return 0;
    }
    system("cls");
    printf("=================================================\n");
    printf("       查询演出 - 剧目: %s\n", search_name);
    printf("=================================================\n");
    // 查找匹配的剧目
    play_list_node_t* play_current = all_plays;
    int play_found_count = 0;
    while (play_current != NULL) {
        if (strstr(play_current->data.name, search_name) != NULL) {
            // 找到匹配的剧目
            play_found_count++;
            printf("\n剧目[%d/%d]:\n", play_found_count, all_play_count);
            printf("-----------------------------------------\n");
            printf("剧目ID: %d\n", play_current->data.id);
            printf("剧目名称: %s\n", play_current->data.name);
            printf("剧目类型: %d\n", play_current->data.type);
            printf("时长: %d分钟\n", play_current->data.duration);
            printf("-----------------------------------------\n");
            // 调用Schedule_Srv_FetchByPlay获取该剧目的演出计划
            schedule_list_t temp_schedule_list = NULL;
            int schedule_count = Schedule_Srv_FetchByPlay(&temp_schedule_list, play_current->data.id);
            if (schedule_count > 0) {
                printf("该剧目有以下演出计划:\n");
                printf("---------------------------------------------------------------\n");
                printf("%-6s %-12s %-10s %-8s %-8s\n",
                    "计划ID", "演出厅", "日期", "时间", "座位数");
                printf("---------------------------------------------------------------\n");
                //  显示每个演出计划的信息
                schedule_list_node_t* schedule_current = temp_schedule_list;
                int schedule_index = 0;
                while (schedule_current != NULL) {
                    // 获取演出厅信息
                    studio_t studio_info;
                    Studio_Srv_FetchByID(schedule_current->data.studio_id, &studio_info);
                    printf("%-6d %-12s %s-%s-%s %s:%s:%s %-8d\n",
                        schedule_current->data.id,
                        studio_info.name,
                        schedule_current->data.date.year,
                        schedule_current->data.date.month,
                        schedule_current->data.date.day,
                        schedule_current->data.time.hour,
                        schedule_current->data.time.minute,
                        schedule_current->data.time.second,
                        schedule_current->data.seat_count);
                    total_count++;
                    schedule_index++;
                    schedule_current = (schedule_list_node_t*)(schedule_current->node.next);
                }
                printf("共 %d 个演出计划\n\n", schedule_count);
                // 释放临时链表
                List_Destroy(temp_schedule_list, schedule_list_node_t);
            }
            else {
                printf("该剧目暂无演出计划。\n\n");
            }
        }
        play_current = (play_list_node_t*)(play_current->node.next);
    }
    // 释放所有剧目链表
    List_Destroy(all_plays, play_list_node_t);
    if (play_found_count == 0) {
        printf("未找到剧目名称包含 '%s' 的剧目。\n", search_name);
    }
    else {
        printf("=================================================\n");
        printf("查询完成！共找到 %d 个剧目，%d 个演出计划。\n",
            play_found_count, total_count);
        printf("=================================================\n");
    }
    // 返回获取到的演出计划数量
    return total_count;
}