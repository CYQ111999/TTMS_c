// View/Studio_UI.c
#include "Studio_UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
// 管理演出厅入口函数
void Studio_UI_MgtEntry(void) {
    int choice = 0;

    do {
        system("cls");
        printf("\n========== 演出厅管理 ==========\n");
        printf("1. 添加演出厅\n");
        printf("2. 修改演出厅\n");
        printf("3. 删除演出厅\n");
        printf("4. 查询演出厅\n");
        printf("0. 返回\n");
        printf("==============================\n");
        printf("请选择: ");

        if (scanf("%d", &choice) != 1) {
            printf("输入错误！\n");
            getchar(); getchar();
            continue;
        }
        getchar();

        switch (choice) {
        case 1:
            Studio_UI_Add();
            break;
        case 2: {
            int id;
            printf("请输入要修改的演出厅ID: ");
            scanf("%d", &id);
            getchar();
            Studio_UI_Modify(id);
            break;
        }
        case 3: {
            int id;
            printf("请输入要删除的演出厅ID: ");
            scanf("%d", &id);
            getchar();
            Studio_UI_Delete(id);
            break;
        }
        case 4:
            printf("查询演出厅功能暂未实现\n");
            break;
        case 0:
            printf("返回主菜单...\n");
            return;
        default:
            printf("无效选择！\n");
        }

        printf("按任意键继续...");
        getchar();
    } while (1);
}

// 添加新演出厅界面
int Studio_UI_Add(void) {
    printf("添加演出厅功能暂未实现\n");
    printf("按任意键继续...");
    getchar();
    return 1;
}

// 修改演出厅界面
int Studio_UI_Modify(int id) {
    printf("修改演出厅 %d 功能暂未实现\n", id);
    printf("按任意键继续...");
    getchar();
    return 1;
}

// 删除演出厅界面
int Studio_UI_Delete(int id) {
    printf("删除演出厅 %d 功能暂未实现\n", id);
    printf("按任意键继续...");
    getchar();
    return 1;
}