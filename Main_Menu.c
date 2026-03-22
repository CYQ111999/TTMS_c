// Main_Menu.c
// 剧院票务管理系统主菜单
// 严格按照教材规范编写

#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Studio_UI.h"
#include "Play_UI.h"
#include "Schedule_UI.h"
#include "Sale_UI.h"           // 销售模块
#include "Account_UI.h"        // 账户管理
#include "Queries_Menu.h"      // 查询菜单
#include "Seat_UI.h"           // 座位管理
#include "Salesanalysis_UI.h"  // 统计分析
#include "Main_Menu.h"
#define _CRT_SECURE_NO_WARNINGS

// 内部函数声明
static void show_menu(void);
static int process_choice(int choice);

// 全局用户变量（需要在Common.h中定义）
// 假设在Common.h中有：extern account_t gl_CurUser;

// 主菜单入口函数
int Main_Menu(void)
{
    int choice = 0;
    int ok = 0;  // 输入是否有效的标志

    while (!ok)
    {
        system("cls");  // 清屏
        show_menu();    // 显示菜单

        if (scanf("%d", &choice) == 1)
        {
            getchar();  // 清掉回车

            // 检查是不是1-9
            if (choice >= 1 && choice <= 9)
            {
                ok = 1;

                // 9是注销，直接返回
                if (choice == 9)
                {
                    printf("注销成功！\n");
                    return 9;
                }

                // 处理其他选择
                process_choice(choice);
            }
            else
            {
                printf("请输入1-9的数字\n");
                printf("按任意键继续...");
                getchar();
            }
        }
        else
        {
            printf("输入格式不对\n");
            while (getchar() != '\n');  // 清掉错误的输入
        }
    }
    return choice;
}

// 显示菜单
static void show_menu(void)
{
    printf("\n========================================\n");
    printf("        剧院票务管理系统 (TTMS)         \n");
    printf("========================================\n");

    // 显示当前用户
    const char* role = "";
    if (gl_CurUser.type == USR_ADMIN)
        role = "系统管理员";
    else if (gl_CurUser.type == USR_MANG)
        role = "经理";
    else if (gl_CurUser.type == USR_CLERK)
        role = "售票员";
    else
        role = "未知角色";

    printf("当前用户：[%s] %s\n", role, gl_CurUser.username);
    printf("----------------------------------------\n");

    // 根据角色显示不同菜单
    switch (gl_CurUser.type)
    {
    case USR_ADMIN:  // 管理员
        printf("  1. 管理系统用户\n");
        printf("  2. 管理演出厅\n");
        printf("  3. 管理座位\n");
        break;

    case USR_MANG:   // 经理
        printf("  1. 管理剧目\n");
        printf("  2. 管理演出计划\n");
        printf("  3. 查询演出信息\n");
        printf("  4. 统计销售额\n");
        printf("  5. 统计票房\n");
        break;

    case USR_CLERK:  // 售票员
        printf("  1. 查询演出信息\n");
        printf("  2. 售票\n");
        printf("  3. 退票\n");
        printf("  4. 查看销售统计\n");
        break;

    default:
        printf("  错误：未知用户类型\n");
    }

    // 公共选项
    printf("\n  8. 系统信息\n");
    printf("  9. 注销登录\n");
    printf("----------------------------------------\n");
    printf("请选择 (1-9): ");
}

// 处理用户的选择
static int process_choice(int choice)
{
    // 系统信息（所有角色都一样）
    if (choice == 8)
    {
        printf("\n========================================\n");
        printf("        系统信息：TTMS v1.0             \n");
        printf("        五人小组项目                    \n");
        printf("        版本：2024                      \n");
        printf("========================================\n");
        printf("按回车键返回...");
        getchar();
        return 1;
    }

    // 根据角色处理
    switch (gl_CurUser.type)
    {
    case USR_ADMIN:  // 管理员
        switch (choice)
        {
        case 1:
            Account_UI_MgtEntry();   // 管理系统用户
            break;
        case 2:
            Studio_UI_MgtEntry();    // 管理演出厅
            break;
        case 3:
            Seat_UI_MgtEntry();     // 管理座位
            break;
        default:
            printf("无效选择\n");
            getchar();
        }
        break;

    case USR_MANG:   // 经理
        switch (choice)
        {
        case 1:
            Play_UI_MgtEntry();          // 管理剧目
            break;
        case 2:
            Schedule_UI_MgtEntry();     // 管理演出计划
            break;
        case 3:
            Queries_Menu_Schedule();     // 查询演出
            break;
        case 4:
            Salesanalysis_UI_Report();   // 统计销售额
            break;
        case 5:
            Salesanalysis_UI_Report();   // 统计票房
            break;
        default:
            printf("无效选择\n");
            getchar();
        }
        break;

    case USR_CLERK:  // 售票员
        switch (choice)
        {
        case 1:
            Queries_Menu_Schedule();     // 查询演出
            break;
        case 2:
            Sale_UI_MgtEntry();          // 售票
            break;
        case 3:
            Sale_UI_RefundTicket();     // 退票
            break;
        case 4:
            Salesanalysis_UI_Report();   // 查看销售统计
            break;
        default:
            printf("无效选择\n");
            getchar();
        }
        break;

    default:
        printf("用户类型错误\n");
        getchar();
    }

    return 1;
}