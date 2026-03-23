// View/Studio_UI.c
#include "Studio_UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#define _CRT_SECURE_NO_WARNINGS
// 管理演出厅入口函数
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
        case 4: {  // 查询演出厅 - 使用现有函数
            int queryChoice = 0;

            do {
                system("cls");
                printf("\n========== 查询演出厅 ==========\n");
                printf("1. 按ID查询单个演出厅\n");
                printf("2. 查看所有演出厅\n");
                printf("0. 返回上级菜单\n");
                printf("================================\n");
                printf("请选择: ");

                if (scanf("%d", &queryChoice) != 1) {
                    printf("输入错误！\n");
                    getchar(); getchar();
                    continue;
                }
                getchar();

                switch (queryChoice) {
                case 1: {  // 按ID查询
                    int id;
                    printf("请输入演出厅ID: ");
                    if (scanf("%d", &id) != 1) {
                        printf("输入错误！\n");
                        getchar();
                        break;
                    }
                    getchar();

                    // 调用现有的查询函数
                    Studio_UI_QueryById(id);

                    printf("按回车键返回查询菜单...");
                    getchar();
                    break;
                }
                case 2:  // 查看所有演出厅
                    // 调用现有的列表函数
                    Studio_UI_ListAll();
                    printf("按回车键返回查询菜单...");
                    getchar();
                    break;
                case 0:  // 返回
                    break;
                default:
                    printf("无效选择！\n");
                }
            } while (queryChoice != 0);
            break;
        }
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

// 修改添加演出厅函数
int Studio_UI_Add(void) {
    system("cls");
    printf("\n========================================\n");
    printf("            添加新演出厅                 \n");
    printf("========================================\n");

    studio_t newStudio;

    printf("演出厅名称: ");
    fgets(newStudio.name, sizeof(newStudio.name), stdin);
    newStudio.name[strcspn(newStudio.name, "\n")] = '\0';  // 移除换行符

    printf("行数: ");
    scanf("%d", &newStudio.rowsCount);
    getchar();

    printf("列数: ");
    scanf("%d", &newStudio.colsCount);
    getchar();

    // 计算座位数
    newStudio.seatsCount = newStudio.rowsCount * newStudio.colsCount;

    if (Studio_Srv_Add(&newStudio)) {
        printf("\n演出厅添加成功！\n");
        printf("演出厅ID: %d\n", newStudio.id);
        printf("演出厅名称: %s\n", newStudio.name);
        printf("座位数: %d\n", newStudio.seatsCount);
        return 1;
    }
    else {
        printf("\n演出厅添加失败！\n");
        return 0;
    }
}

// 修改演出厅函数
int Studio_UI_Modify(int id) {
    system("cls");
    printf("\n========================================\n");
    printf("            修改演出厅信息               \n");
    printf("========================================\n");

    studio_t studio;
    if (!Studio_Srv_FetchByID(id, &studio)) {
        printf("演出厅ID %d 不存在！\n", id);
        printf("按回车键返回...");
        getchar();
        return 0;
    }

    printf("当前信息：\n");
    printf("演出厅名称: %s\n", studio.name);
    printf("行数: %d\n", studio.rowsCount);
    printf("列数: %d\n", studio.colsCount);
    printf("座位数: %d\n", studio.seatsCount);
    printf("----------------------------------------\n");

    char input[100];
    printf("新演出厅名称（直接回车保持不变）: ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strcpy(studio.name, input);
    }

    printf("新行数（输入0保持不变）: ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') {
        int rows = atoi(input);
        if (rows > 0) {
            studio.rowsCount = rows;
        }
    }

    printf("新列数（输入0保持不变）: ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') {
        int cols = atoi(input);
        if (cols > 0) {
            studio.colsCount = cols;
        }
    }

    // 重新计算座位数
    studio.seatsCount = studio.rowsCount * studio.colsCount;

    if (Studio_Srv_Modify(&studio)) {
        printf("\n演出厅修改成功！\n");
        return 1;
    }
    else {
        printf("\n演出厅修改失败！\n");
        return 0;
    }
}

// 删除演出厅函数
int Studio_UI_Delete(int id) {
    system("cls");
    printf("\n========================================\n");
    printf("            删除演出厅                 \n");
    printf("========================================\n");

    studio_t studio;
    if (!Studio_Srv_FetchByID(id, &studio)) {
        printf("演出厅ID %d 不存在！\n", id);
        printf("按回车键返回...");
        getchar();
        return 0;
    }

    printf("确认删除以下演出厅吗？\n");
    printf("演出厅ID: %d\n", studio.id);
    printf("演出厅名称: %s\n", studio.name);
    printf("行数: %d\n", studio.rowsCount);
    printf("列数: %d\n", studio.colsCount);
    printf("座位数: %d\n", studio.seatsCount);
    printf("----------------------------------------\n");
    printf("输入Y确认删除，其他键取消: ");

    char confirm = getchar();
    getchar();  // 清除回车

    if (confirm == 'Y' || confirm == 'y') {
        if (Studio_Srv_DeleteByID(id)) {
            printf("演出厅删除成功！\n");
            return 1;
        }
        else {
            printf("演出厅删除失败！\n");
            return 0;
        }
    }
    else {
        printf("取消删除。\n");
        return 0;
    }
}

// 在 Studio_UI.c 中添加按ID查询的函数
int Studio_UI_QueryById(int id) {
    system("cls");
    printf("\n========================================\n");
    printf("            查询演出厅信息               \n");
    printf("========================================\n");

    studio_t studio;
    if (!Studio_Srv_FetchByID(id, &studio)) {
        printf("演出厅ID %d 不存在！\n", id);
        return 0;
    }

    printf("演出厅ID: %d\n", studio.id);
    printf("演出厅名称: %s\n", studio.name);
    printf("行数: %d\n", studio.rowsCount);
    printf("列数: %d\n", studio.colsCount);
    printf("座位数: %d\n", studio.seatsCount);
    printf("========================================\n");

    return 1;
}

int Studio_UI_ListAll(void) {
    system("cls");
    printf("\n========================================\n");
    printf("            所有演出厅信息               \n");
    printf("========================================\n");

    // 初始化演出厅列表
    studio_list_t studioList = NULL;
    List_Init(studioList, studio_list_node_t);

    // 获取所有演出厅
    int total = Studio_Srv_FetchAll(&studioList);

    if (total == 0) {
        printf("暂无演出厅信息。\n");
        printf("========================================\n");
        if (studioList) {
            List_Free(studioList, studio_list_node_t);
        }
        printf("按回车键返回...");
        getchar();
        return 0;
    }

    // 显示表头
    printf("%-5s %-20s %-8s %-8s %-8s\n",
        "ID", "名称", "行数", "列数", "座位数");
    printf("------------------------------------------------\n");

    // 显示所有演出厅
    studio_list_node_t* pos = NULL;
    int count = 0;

    // 遍历链表
    for (pos = (studio_list_node_t*)studioList->node.next;
        pos != (studio_list_node_t*)studioList;
        pos = (studio_list_node_t*)pos->node.next) {

        printf("%-5d %-20s %-8d %-8d %-8d\n",
            pos->data.id,
            pos->data.name,
            pos->data.rowsCount,
            pos->data.colsCount,
            pos->data.seatsCount);

        count++;

        // 每显示10条记录暂停一下
        if (count % 10 == 0 && count < total) {
            printf("\n已显示 %d 条记录，按回车键继续...", count);
            getchar();
            system("cls");
            printf("\n========================================\n");
            printf("            所有演出厅信息（续）         \n");
            printf("========================================\n");
            printf("%-5s %-20s %-8s %-8s %-8s\n",
                "ID", "名称", "行数", "列数", "座位数");
            printf("------------------------------------------------\n");
        }
    }

    printf("------------------------------------------------\n");
    printf("共 %d 个演出厅\n", count);
    printf("========================================\n");

    // 释放链表
    if (studioList) {
        List_Free(studioList, studio_list_node_t);
    }

    return 1;
}