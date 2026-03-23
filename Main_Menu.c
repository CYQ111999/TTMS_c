// Main_Menu.c
// 剧院票务管理系统主菜单

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void show_menu(void);
static int process_choice(int choice);

// 主菜单入口函数
int Main_Menu(void)
{
    int choice = 0;
    int ok = 0;  
    while (!ok)
    {
        system("cls"); 
        show_menu();  // 显示菜单

        if (scanf("%d", &choice) == 1)
        {
            getchar();  
            // 根据用户类型检查
            int min_choice = 1;
            int max_choice = 0;
            switch (gl_CurUser.type) {
            case USR_ADMIN:  // 管理员
                max_choice = 3;
                break;
            case USR_MANG:   // 经理
                max_choice = 5;
                break;
            case USR_CLERK:  // 售票员
                max_choice = 4;
                break;
            case USR_ANOMY:  // 匿名用户
                max_choice = 6;  // 匿名用户有6个选项
                break;
            default:
                max_choice = 0;
            }
            // 检查选择是否有效
            if (choice == 8 || choice == 9) {
                ok = 1;
            }
            else if (choice >= min_choice && choice <= max_choice) {
                ok = 1;
            }
            else {
                printf("请输入%d-%d的数字\n", min_choice, max_choice);
                printf("按任意键继续...");
                getchar();
                continue;
            }
            // 处理9选项 注销/退出
            if (choice == 9) {
                if (gl_CurUser.type == USR_ANOMY) {
                    printf("谢谢使用，再见！\n");
                    printf("按任意键退出...");
                    getchar();
                    return 0;  // 返回0表示退出系统
                }
                else {
                    printf("注销成功！\n");
                    printf("按任意键继续...");
                    getchar();
                    // 重置当前用户为匿名用户
                    gl_CurUser.type = USR_ANOMY;
                    strcpy(gl_CurUser.username, "Guest");
                    ok = 0;  // 重置ok标志，重新显示菜单
                    continue;  // 重新开始循环
                }
            }
            // 处理其他选择
            int result = process_choice(choice);
            if (result == 0) {
                // 如果返回0，表示退出系统
                return 0;
            }
            else if (result == 2) {
                // 如果返回2，表示登录成功，需要重新进入循环
                ok = 0;  // 重置ok标志
                continue;  // 重新开始循环
            }
        }
        else
        {
            printf("输入格式不对\n");
            while (getchar() != '\n'); 
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
    switch (gl_CurUser.type) {
    case USR_ADMIN: role = "系统管理员"; break;
    case USR_MANG:  role = "经理"; break;
    case USR_CLERK: role = "售票员"; break;
    case USR_ANOMY: role = "匿名用户"; break;  
    default:        role = "未知角色";
    }
    if (gl_CurUser.type == USR_ANOMY) {
        printf("当前用户： 匿名用户\n"); 
    }
    else {
        printf("当前用户：[%s] %s\n", role, gl_CurUser.username);
    }
    printf("----------------------------------------\n");
    // 根据角色显示不同菜单
    switch (gl_CurUser.type) {
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
    case USR_ANOMY:  // 匿名用户
        printf("  1. 查询剧目信息\n");
        printf("  2. 查询演出计划\n");
        printf("  3. 查询演出厅信息\n");
        printf("  4. 查询座位信息\n");
        printf("  5. 登录系统\n");
        printf("  6. 注册新用户\n");
        break;
    default:
        printf("  错误：未知用户类型\n");
    }
    printf("\n  8. 系统信息\n");
    printf("  9. %s\n", (gl_CurUser.type == USR_ANOMY) ? "退出系统" : "注销登录");
    printf("----------------------------------------\n");
    printf("请选择 (1-9): ");
}

static int process_choice(int choice)
{
    // 系统信息
    if (choice == 8) {
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
    switch (gl_CurUser.type) {
    case USR_ADMIN:  // 管理员
        switch (choice) {
        case 1:
            Account_UI_MgtEntry();   // 管理系统用户
            break;
        case 2:
            Studio_UI_MgtEntry();    // 管理演出厅
            break;
        case 3:
            Seat_UI_MgtEntry();      // 管理座位
            break;
        default:
            printf("无效选择\n");
            getchar();
        }
        break;
    case USR_MANG:   // 经理
        switch (choice) {
        case 1:
            Play_UI_MgtEntry();      // 管理剧目
            break;
        case 2:
            Schedule_UI_MgtEntry();  // 管理演出计划
            break;
        case 3:
            Queries_Menu_Schedule(); // 查询演出
            break;
        case 4:
            Salesanalysis_UI_Report(); // 统计销售额
            break;
        case 5:
            Salesanalysis_UI_Report(); // 统计票房
            break;
        default:
            printf("无效选择\n");
            getchar();
        }
        break;
    case USR_CLERK:  // 售票员
        switch (choice) {
        case 1:
            Queries_Menu_Schedule(); // 查询演出
            break;
        case 2:
            Sale_UI_MgtEntry();      // 售票
            break;
        case 3:
            Sale_UI_RefundTicket();  // 退票
            break;
        case 4:
            Salesanalysis_UI_Report(); // 查看销售统计
            break;
        default:
            printf("无效选择\n");
            getchar();
        }
        break;
    case USR_ANOMY:  // 匿名用户
        switch (choice) {
        case 1:  // 查询剧目信息
            Play_UI_ListAll();      // 查询剧目信息
            break;
        case 2:  // 查询演出计划
            Schedule_UI_ListAll();  // 查询演出计划
            break;
        case 3:  // 查询演出厅信息
            Studio_UI_ListAll();    // 查询演出厅信息
            break;
        case 4:  // 查询座位信息
            Seat_UI_MgtEntry();     // 查询座位信息
            break;
        case 5:  // 登录系统
            Account_UI_Login();     // 用户登录
            return 2;               // 登录后需要刷新菜单
        case 6:  // 注册新用户
            Account_UI_Add(NULL);   // 用户注册
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