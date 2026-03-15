//cyq写
//主菜单同时实现对登陆者的分级
//接口比较多可能和具体模块实现的组员定义不同，大家写完自己的函数之后记得来
//这个文件检查接口，可以注释掉其他为实现的功能然后运行测试 
#include <stdio.h>
#include "Common.h" 
#include "Studio_UI.h"
#include "Play_UI.h"
#include "Schedule_UI.h"
#include "Ticket.h"
#include "Sale_UI.h"
#include "Account_UI.h"
#include "MaiAccount_UI.h"
#include "Queries_Menu.h"
#include "Seat_UI.h"
#include "StaSales_UI.h"
#include "Salesanalysis_UI.h"

static void DisplayMenuByRole(void);
static int  HandleUserChoice(int choice);

int Main_Menu(void) {
    int userChoice = 0;
    int inputValid = 0;
    while (!inputValid) {
        //根据用户角色显示菜单
        DisplayMenuByRole();
        //获取用户选择
        if (scanf("%d", &userChoice) == 1) {
            getchar(); // 清空输入缓冲区中的换行符
            //验证输入范围 (1-9)
            if (userChoice >= 1 && userChoice <= 9) {
                inputValid = 1;

                //处理用户选择，如果是注销(9)则直接返回
                if (userChoice == 9) {
                    return 9; /* 明确返回注销代码 */
                }
                HandleUserChoice(userChoice);
            }
            else {
                printf("输入无效，请输入1-9之间的数字。\n");
            }
        }
        else {
            printf("输入错误，请重新输入。\n");
            while (getchar() != '\n');
        }
    }
    return userChoice;
}

static void DisplayMenuByRole(void) {
    printf("\n========== 剧院票务管理系统主菜单 ==========\n");
    // 显示当前用户信息
    printf("当前用户：[%s] %s\n",
        (gl_CurUser.type == USR_ADMIN) ? "管理员" :
        (gl_CurUser.type == USR_MANG) ? "经理" : "售票员",
        gl_CurUser.username);
    printf("------------------------------------------\n");
    // 根据用户角色显示不同的菜单选项
    switch (gl_CurUser.type) {
    case USR_ADMIN:
        printf("  1. 管理演出厅\n");
        printf("  2. 管理系统用户\n");
        printf("  3. 设置座位\n");
        printf("  8. 系统信息\n");
        printf("  9. 注销登录\n");
        break;
    case USR_MANG:
        printf("  1. 管理剧目\n");
        printf("  2. 安排演出\n");
        printf("  3. 查询演出\n");
        printf("  4. 统计销售额\n");
        printf("  5. 统计票房\n");
        printf("  8. 系统信息\n");
        printf("  9. 注销登录\n");
        break;
    case USR_CLERK:
        printf("  1. 查询演出\n");
        printf("  2. 售票\n");
        printf("  3. 退票\n");
        printf("  4. 统计销售额\n");
        printf("  8. 系统信息\n");
        printf("  9. 注销登录\n");
        break;
    default:
        printf("  错误：未知用户类型！\n");
        printf("  9. 注销登录\n");
        break;
    }
    printf("------------------------------------------\n");
    printf("请选择操作 (1-9): ");
}

static int HandleUserChoice(int choice) {
    // 系统信息对所有角色开放
    if (choice == 8) {
        printf("\n=== 系统信息 ===\n");
        printf("TTMS 剧院票务管理系统 v1.0\n");
        printf("架构: 三层架构 (View-Service-Persistence)\n");
        printf("开发团队: 五人小组项目\n");
        printf("按任意键返回...");
        getchar();
        return 1;
    }
    // 根据用户角色处理不同选择
    switch (gl_CurUser.type) {
    case USR_ADMIN: // 系统管理员
        switch (choice) {
        case 1: // 管理演出厅
            Studio_UI_MainMenu(); 
            break;
        case 2: // 管理系统用户
            Account_UI_MgtEntry();
            break;
        case 3: // 设置座位
            Seat_UI_MainMenu();
            break;
        default:
            printf("无效选择。\n");
        }
        break;
    case USR_MANG: //剧院经理 
        switch (choice) {
        case 1: // 管理剧目
            Play_UI_MainMenu(); 
            break;
        case 2: // 安排演出
            Schedule_UI_Add();
            break;
        case 3: // 查询演出
            Queries_Menu_Schedule(); 
            break;
        case 4: //统计销售额
            StaSales_UI_Analysis();
            break;
        case 5: // 统计票房
            Salesanalysis_UI_Report();
            break;
        default:
            printf("无效选择。\n");
        }
        break;
    case USR_CLERK: //售票员
        switch (choice) {
        case 1: 
            Queries_Menu_Schedule(); 
            break;
        case 2: // 售票
            Ticket_UI_Sell(); 
            break;
        case 3: // 退票 
            Ticket_UI_Refund(); 
            break;
        case 4: // 统计销售额
            Sale_UI_Analysis();
            break;
        default:
            printf("无效选择。\n");
        }
        break;
    default:
        printf("未知用户类型，无法处理选择。\n");
    }
    return 1; //继续显示菜单
}