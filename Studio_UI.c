// View/Studio_UI.c
#include "Studio_UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "Studio.h"
#include "List.h"
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
        case 4: {  // 查询演出厅
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
                case 0:
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
int Studio_UI_Add(void)
{
    int rtn = 0;
    studio_t new_studio = { 0 };
    char input_buffer[100];
    int rows, cols;
    system("cls");
    printf("========================================\n");
    printf("              添加新演出厅\n");
    printf("========================================\n");
    // 1. 输入演出厅名称
    printf("请输入演出厅名称: ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
        printf("输入错误。\n");
        getchar();
        return 0;
    }
    input_buffer[strcspn(input_buffer, "\n")] = '\0';
    if (strlen(input_buffer) == 0) {
        printf("错误：演出厅名称不能为空！\n");
        printf("按任意键返回...");
        getchar();
        return 0;
    }
    strncpy(new_studio.name, input_buffer, sizeof(new_studio.name) - 1);
    new_studio.name[sizeof(new_studio.name) - 1] = '\0';
    // 2. 输入行数
    printf("请输入行数: ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL ||
        sscanf(input_buffer, "%d", &rows) != 1 || rows <= 0) {
        printf("错误：行数必须为正整数！\n");
        printf("按任意键返回...");
        getchar();
        return 0;
    }
    new_studio.rowsCount = rows;
    // 3. 输入列数
    printf("请输入列数: ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL ||
        sscanf(input_buffer, "%d", &cols) != 1 || cols <= 0) {
        printf("错误：列数必须为正整数！\n");
        printf("按任意键返回...");
        getchar();
        return 0;
    }
    new_studio.colsCount = cols;
    // 4. 计算座位数
    new_studio.seatsCount = rows * cols;
    new_studio.id = 0; // 明确置0，表示等待分配
    // 6. 显示确认信息 (此时ID尚未分配)
    printf("\n-----------------------------------------------------------------------------\n");
    printf("即将添加的演出厅信息：\n");
    printf("  名称: %s\n", new_studio.name);
    printf("  行数: %d\n", new_studio.rowsCount);
    printf("  列数: %d\n", new_studio.colsCount);
    printf("  座位数: %d\n", new_studio.seatsCount);
    printf("  ID: (等待系统分配)\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("\n确认添加此演出厅？(Y/N): ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL ||
        (input_buffer[0] != 'Y' && input_buffer[0] != 'y')) {
        printf("添加操作已取消。\n");
        printf("按任意键返回...");
        getchar();
        return 0;
    }
    rtn = Studio_Perst_Insert(&new_studio);
    if (rtn) {
        printf("\n 演出厅添加成功！\n");
        // 此时 new_studio.id 已被 Studio_Perst_Insert 函数更新
        printf("  系统分配的主键ID: %d\n", new_studio.id);
        printf("  名称: %s\n", new_studio.name);
        printf("  行数/列数/座位数: %d / %d / %d\n",
            new_studio.rowsCount, new_studio.colsCount, new_studio.seatsCount);
    }
    else {
        printf("\n 演出厅添加失败！\n");
        // 可以提示用户检查数据文件权限或磁盘空间
    }

    printf("按任意键返回...");
    getchar(); // 等待用户按键
    return rtn;
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
    getchar();
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
            List_Destroy(studioList, studio_list_node_t);
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
        List_Destroy(studioList, studio_list_node_t);
    }
    return 1;
}