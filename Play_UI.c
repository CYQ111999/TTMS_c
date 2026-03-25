#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Play.h"
#include "Play_Persist.h"
#include "List.h"
#include "Common.h"
#define _CRT_SECURE_NO_WARNINGS

static play_list_t gl_playList = NULL;

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void DisplayPlayInfo(play_list_node_t* node) {
    printf("%-5d %-20s ", node->data.id, node->data.name);
    switch (node->data.type) {
    case PLAY_TYPE_FILE:    printf("%-8s ", "影片"); break;
    case PLAY_TYPE_OPEAR:   printf("%-8s ", "歌剧"); break;
    case PLAY_TYPE_CONCERT: printf("%-8s ", "音乐会"); break;
    default:                 printf("%-8s ", "未知");
    }
    printf("%-8s ", node->data.area);
    switch (node->data.rating) {
    case PLAY_RATE_CHILD:   printf("%-8s ", "儿童"); break;
    case PLAY_RATE_TEENAGE: printf("%-8s ", "青少年"); break;
    case PLAY_RATE_ADULT:   printf("%-8s ", "成人"); break;
    default:                 printf("%-8s ", "未知");
    }
    printf("%4d分钟 %4d元 %04d-%02d-%02d至%04d-%02d-%02d\n",
        node->data.duration, node->data.price,
        node->data.start_date.year, node->data.start_date.month, node->data.start_date.day,
        node->data.end_date.year, node->data.end_date.month, node->data.end_date.day);
}

void Play_UI_MgtEntry(void) {
    if (gl_playList == NULL) {
        List_Init(gl_playList, play_list_node_t);
    }
    int totalRecords = Play_Srv_FetchAll(gl_playList);
    paging_t paging;
    paging.pageSize = PLAY_PAGE_SIZE;
    paging.totalRecords = totalRecords;
    Paging_Locate_FirstPage(gl_playList, paging);
    char choice;
    do {
        //clearScreen();
        printf("================================================================\n");
        printf("                        剧目管理\n");
        printf("================================================================\n");
        printf("%-5s %-20s %-8s %-8s %-8s %-8s %-12s\n",
            "ID", "名称", "类型", "地区", "分级", "价格", "上映-下映");
        printf("----------------------------------------------------------------\n");
        play_list_node_t* pos;
        int i;
        Paging_ViewPage_ForEach(gl_playList, paging, play_list_node_t, pos, i) {
            DisplayPlayInfo(pos);
        }
        printf("----------------------------------------------------------------\n");
        printf("共 %d 条记录  第 %d/%d 页\n",
            paging.totalRecords, Pageing_CurPage(paging), Pageing_TotalPages(paging));
        printf("A:添加  M:修改  D:删除  Q:查询  F:第一页  P:上一页  N:下一页  L:最后一页  R:返回\n");
        printf("请选择操作：");
        scanf(" %c", &choice);
        int id;
        switch (choice) {
        case 'A':
        case 'a':
            Play_UI_Add();
            List_Free(gl_playList, play_list_node_t);
            totalRecords = Play_Srv_FetchAll(gl_playList);
            paging.totalRecords = totalRecords;
            Paging_Locate_FirstPage(gl_playList, paging);
            break;
        case 'M':
        case 'm':
            printf("请输入要修改的剧目ID: ");
            if (scanf("%d", &id) != 1) {
                printf("输入无效，请返回后重试。\n");
                getchar(); // 清空缓冲区
                break;
            }
            if (Play_UI_Modify(id)) {
                List_Free(gl_playList, play_list_node_t);
                totalRecords = Play_Srv_FetchAll(gl_playList);
                paging.totalRecords = totalRecords;
                Paging_Locate_FirstPage(gl_playList, paging);
            }
            break;
        case 'D':
        case 'd':
            printf("请输入要删除的剧目ID: ");
            if (scanf("%d", &id) != 1) {
                printf("输入无效，请返回后重试。\n");
                getchar();
                break;
            }
            if (Play_UI_Delete(id)) {
                List_Free(gl_playList, play_list_node_t);
                totalRecords = Play_Srv_FetchAll(gl_playList);
                paging.totalRecords = totalRecords;
                Paging_Locate_FirstPage(gl_playList, paging);
            }
            break;
        case 'Q':
        case 'q':
            printf("请输入要查询的剧目ID: ");
            if (scanf("%d", &id) != 1) {
                printf("输入无效，请返回后重试。\n");
                getchar();
                break;
            }
            Play_UI_Query(id);
            break;
        case 'F':
        case 'f':
            Paging_Locate_FirstPage(gl_playList, paging);
            break;
        case 'P':
        case 'p':
            if (!Pageing_IsFirstPage(paging)) {
                Paging_Locate_OffsetPage(gl_playList, paging, -1, play_list_node_t);
            }
            break;
        case 'N':
        case 'n':
            if (!Pageing_IsLastPage(paging)) {
                Paging_Locate_OffsetPage(gl_playList, paging, 1, play_list_node_t);
            }
            break;
        case 'L':
        case 'l':
            Paging_Locate_LastPage(gl_playList, paging, play_list_node_t);
            break;
        }
    } while (choice != 'R' && choice != 'r');
}

int Play_UI_Add(void) {
    int newCount = 0;
    play_t newPlay;
    char choice;
    do {
        printf("\n===== 添加新剧目 =====\n");
        printf("剧目名称: ");
        if (scanf("%s", newPlay.name) != 1) {
            printf("输入无效，操作取消。\n");
            getchar();
            return newCount;
        }

        printf("剧目类型 (1-影片 2-歌剧 3-音乐会): ");
        int type;
        if (scanf("%d", &type) != 1 || type < 1 || type > 3) {
            printf("输入无效，操作取消。\n");
            getchar();
            return newCount;
        }
        newPlay.type = (play_type_t)type;

        printf("出品地区: ");
        if (scanf("%s", newPlay.area) != 1) {
            printf("输入无效，操作取消。\n");
            getchar();
            return newCount;
        }

        printf("剧目分级 (1-儿童 2-青少年 3-成人): ");
        int rating;
        if (scanf("%d", &rating) != 1 || rating < 1 || rating > 3) {
            printf("输入无效，操作取消。\n");
            getchar();
            return newCount;
        }
        newPlay.rating = (play_rating_t)rating;

        printf("时长(分钟): ");
        if (scanf("%d", &newPlay.duration) != 1 || newPlay.duration <= 0) {
            printf("输入无效，操作取消。\n");
            getchar();
            return newCount;
        }

        printf("票价: ");
        if (scanf("%d", &newPlay.price) != 1 || newPlay.price <= 0) {
            printf("输入无效，操作取消。\n");
            getchar();
            return newCount;
        }

        printf("上映日期(年 月 日): ");
        int y, m, d;
        if (scanf("%d %d %d", &y, &m, &d) != 3) {
            printf("日期格式错误，操作取消。\n");
            getchar();
            return newCount;
        }
        // 简单验证日期合法性
        if (y <= 0 || m < 1 || m > 12 || d < 1 || d > 31) {
            printf("日期值超出合理范围，操作取消。\n");
            return newCount;
        }
        newPlay.start_date.year = y;
        newPlay.start_date.month = m;
        newPlay.start_date.day = d;

        printf("下映日期(年 月 日): ");
        if (scanf("%d %d %d", &y, &m, &d) != 3) {
            printf("日期格式错误，操作取消。\n");
            getchar();
            return newCount;
        }
        if (y <= 0 || m < 1 || m > 12 || d < 1 || d > 31) {
            printf("日期值超出合理范围，操作取消。\n");
            return newCount;
        }
        newPlay.end_date.year = y;
        newPlay.end_date.month = m;
        newPlay.end_date.day = d;

        if (Play_Srv_Add(&newPlay)) {
            play_list_node_t* newNode = (play_list_node_t*)malloc(sizeof(play_list_node_t));
            newNode->data = newPlay;
            List_AddTail(gl_playList, newNode);
            printf("剧目添加成功！剧目ID: %d\n", newPlay.id);
            newCount++;
        }
        else {
            printf("剧目添加失败！\n");
        }
        printf("\n[A]继续添加 [R]返回: ");
        scanf(" %c", &choice);
    } while (choice == 'A' || choice == 'a');
    return newCount;
}

int Play_UI_Modify(int id) {
    play_t buf;
    if (!Play_Srv_FetchByID(id, &buf)) {
        printf("剧目ID %d 不存在！\n", id);
        return 0;
    }
    printf("\n===== 修改剧目(ID:%d) =====\n", id);
    printf("原剧目名称: %s\n", buf.name);
    printf("新剧目名称(直接回车不修改): ");
    char input[31];
    fgets(input, sizeof(input), stdin);  // 清空缓冲区
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strcpy(buf.name, input);
    }

    printf("原剧目类型: %d\n", buf.type);
    printf("新剧目类型(1-影片 2-歌剧 3-音乐会, 0不修改): ");
    int type;
    if (scanf("%d", &type) != 1) {
        printf("输入无效，修改取消。\n");
        getchar();
        return 0;
    }
    if (type != 0 && (type < 1 || type > 3)) {
        printf("无效类型，修改取消。\n");
        return 0;
    }
    if (type != 0) {
        buf.type = (play_type_t)type;
    }

    printf("原出品地区: %s\n", buf.area);
    printf("新出品地区(直接回车不修改): ");
    fgets(input, sizeof(input), stdin);
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strcpy(buf.area, input);
    }

    printf("原剧目分级: %d\n", buf.rating);
    printf("新剧目分级(1-儿童 2-青少年 3-成人, 0不修改): ");
    int rating;
    if (scanf("%d", &rating) != 1) {
        printf("输入无效，修改取消。\n");
        getchar();
        return 0;
    }
    if (rating != 0 && (rating < 1 || rating > 3)) {
        printf("无效分级，修改取消。\n");
        return 0;
    }
    if (rating != 0) {
        buf.rating = (play_rating_t)rating;
    }

    printf("原时长: %d分钟\n", buf.duration);
    printf("新时长(0不修改): ");
    int duration;
    if (scanf("%d", &duration) != 1) {
        printf("输入无效，修改取消。\n");
        getchar();
        return 0;
    }
    if (duration < 0) {
        printf("时长不能为负数，修改取消。\n");
        return 0;
    }
    if (duration > 0) {
        buf.duration = duration;
    }

    printf("原票价: %d元\n", buf.price);
    printf("新票价(0不修改): ");
    int price;
    if (scanf("%d", &price) != 1) {
        printf("输入无效，修改取消。\n");
        getchar();
        return 0;
    }
    if (price < 0) {
        printf("票价不能为负数，修改取消。\n");
        return 0;
    }
    if (price > 0) {
        buf.price = price;
    }

    if (Play_Srv_Modify(&buf)) {
        play_list_node_t* p;
        List_ForEach(gl_playList, p) {
            if (p->data.id == id) {
                p->data = buf;
                break;
            }
        }
        printf("剧目修改成功！\n");
        return 1;
    }
    else {
        printf("剧目修改失败！\n");
        return 0;
    }
}

int Play_UI_Delete(int id) {
    char confirm;
    printf("确定要删除剧目ID: %d 吗？(Y/N): ", id);
    scanf(" %c", &confirm);
    if (confirm != 'Y' && confirm != 'y') {
        printf("取消删除\n");
        return 0;
    }
    if (Play_Srv_DeleteByID(id)) {
        play_list_node_t* p;
        List_ForEach(gl_playList, p) {
            if (p->data.id == id) {
                List_FreeNode(p);
                break;
            }
        }
        printf("剧目删除成功！\n");
        return 1;
    }
    else {
        printf("剧目删除失败！\n");
        return 0;
    }
}

int Play_UI_Query(int id) {
    play_t buf;
    if (Play_Srv_FetchByID(id, &buf)) {
        printf("\n===== 剧目信息(ID:%d) =====\n", id);
        printf("剧目名称: %s\n", buf.name);
        printf("剧目类型: %d\n", buf.type);
        printf("出品地区: %s\n", buf.area);
        printf("剧目分级: %d\n", buf.rating);
        printf("时长: %d分钟\n", buf.duration);
        printf("票价: %d元\n", buf.price);
        printf("上映日期: %04d-%02d-%02d\n", buf.start_date.year, buf.start_date.month, buf.start_date.day);
        printf("下映日期: %04d-%02d-%02d\n", buf.end_date.year, buf.end_date.month, buf.end_date.day);
        return 1;
    }
    else {
        printf("剧目ID %d 不存在！\n", id);
        return 0;
    }
}

// 在 Play_UI.c 文件末尾添加这个函数
int Play_UI_ListAll(void) {
    system("cls");
    printf("\n========================================\n");
    printf("              所有剧目信息               \n");
    printf("========================================\n");

    // 初始化剧目列表
    play_list_t playList = NULL;
    List_Init(playList, play_list_node_t);

    // 获取所有剧目
    int total = Play_Srv_FetchAll(playList);

    if (total == 0) {
        printf("暂无剧目信息。\n");
        printf("========================================\n");
        List_Free(playList, play_list_node_t);
        printf("按回车键返回...");
        getchar();
        return 0;
    }

    // 显示表头
    printf("%-5s %-20s %-8s %-8s %-8s %-8s %-8s %-12s\n",
        "ID", "名称", "类型", "地区", "分级", "时长", "价格", "上映-下映");
    printf("----------------------------------------------------------------------------\n");

    // 显示所有剧目
    play_list_node_t* pos;
    int count = 0;
    List_ForEach(playList, pos) {
        // 显示剧目信息
        printf("%-5d %-20s ", pos->data.id, pos->data.name);

        // 显示类型
        switch (pos->data.type) {
        case PLAY_TYPE_FILE:    printf("%-8s ", "影片"); break;
        case PLAY_TYPE_OPEAR:   printf("%-8s ", "歌剧"); break;
        case PLAY_TYPE_CONCERT: printf("%-8s ", "音乐会"); break;
        default:                printf("%-8s ", "未知");
        }

        // 显示地区
        printf("%-8s ", pos->data.area);

        // 显示分级
        switch (pos->data.rating) {
        case PLAY_RATE_CHILD:   printf("%-8s ", "儿童"); break;
        case PLAY_RATE_TEENAGE: printf("%-8s ", "青少年"); break;
        case PLAY_RATE_ADULT:   printf("%-8s ", "成人"); break;
        default:                printf("%-8s ", "未知");
        }

        // 显示时长、价格和日期
        printf("%-8d %-8d ", pos->data.duration, pos->data.price);
        printf("%04d-%02d-%02d至%04d-%02d-%02d\n",
            pos->data.start_date.year, pos->data.start_date.month, pos->data.start_date.day,
            pos->data.end_date.year, pos->data.end_date.month, pos->data.end_date.day);

        count++;
    }

    printf("----------------------------------------------------------------------------\n");
    printf("共 %d 个剧目\n", count);
    printf("========================================\n");

    // 释放列表
    List_Free(playList, play_list_node_t);

    return 1;
}