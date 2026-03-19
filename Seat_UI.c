#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Seat.h"
#include "Studio.h"       
#include "Seat_Persist.h"
#include "List.h"
#include "Common.h"

// 清屏函数
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 根据座位状态获取界面显示符号
char Seat_UI_Status2Char(seat_status_t status) {
    switch (status) {
    case SEAT_GOOD:    return '#';
    case SEAT_BROKEN:  return '~';
    case SEAT_NONE:    return ' ';
    default:           return '?';
    }
}

// 根据输入符号获取座位状态
seat_status_t Seat_UI_Char2Status(char statusChar) {
    switch (statusChar) {
    case '#': return SEAT_GOOD;
    case '~': return SEAT_BROKEN;
    case ' ': return SEAT_NONE;
    default:  return SEAT_NONE;
    }
}

// 管理座位界面
void Seat_UI_MgtEntry(int roomID) {
    if (roomID <= 0) {
        printf("无效的演出厅ID！\n");
        return;
    }
    // 获取演出厅信息
    studio_t studio;
    // 假设有外部函数
    extern int Studio_Srv_FetchByID(int id, studio_t * buf);
    if (!Studio_Srv_FetchByID(roomID, &studio)) {
        printf("演出厅不存在！\n");
        return;
    }
    seat_list_t list;
    List_Init(list, seat_list_node_t);
    int seatCount = Seat_Srv_FetchByRoomID(list, roomID);
    if (seatCount == 0) {
        printf("该演出厅尚无座位数据，正在初始化座位...\n");
        seatCount = Seat_Srv_RoomInit(list, roomID, studio.rowsCount, studio.colsCount);
        if (seatCount > 0) {
            studio.seatsCount = seatCount;
            // 更新演出厅信息
            extern int Studio_Srv_Modify(const studio_t * data);
            Studio_Srv_Modify(&studio);
            printf("座位初始化完成！共 %d 个座位\n", seatCount);
        }
        else {
            printf("座位初始化失败！\n");
            List_Destroy(list, seat_list_node_t);
            return;
        }
    }
    Seat_Srv_SortSeatList(list);
    char choice;
    do {
        clearScreen();

        printf("================================================================\n");
        printf("                演出厅座位管理 [%s]\n", studio.name);
        printf("                行数: %d, 列数: %d, 座位数: %d\n",
            studio.rowsCount, studio.colsCount, seatCount);
        printf("================================================================\n");
        printf("座位布局图：\n\n");
        printf("     ");
        for (int j = 1; j <= studio.colsCount; j++) {
            printf("%3d ", j);
        }
        printf("\n");
        for (int i = 1; i <= studio.rowsCount; i++) {
            printf("行%2d ", i);
            for (int j = 1; j <= studio.colsCount; j++) {
                seat_list_node_t* node = Seat_Srv_FindByRowCol(list, i, j);
                if (node) {
                    printf("  %c ", Seat_UI_Status2Char(node->data.status));
                }
                else {
                    printf("  ? ");
                }
            }
            printf("\n");
        }
        printf("\n符号说明：'#'有效座位  '~'损坏座位  ' '过道\n");
        printf("================================================================\n");
        printf("A:添加新座位  M:修改座位  D:删除座位  R:返回\n");
        printf("请选择操作：");
        scanf(" %c", &choice);
        switch (choice) {
        case 'A':
        case 'a':
            Seat_UI_Add(list, roomID, studio.rowsCount, studio.colsCount);
            break;
        case 'M':
        case 'm':
            Seat_UI_Modify(list, studio.rowsCount, studio.colsCount);
            break;
        case 'D':
        case 'd':
            Seat_UI_Delete(list, studio.rowsCount, studio.colsCount);
            break;
        }
    } while (choice != 'R' && choice != 'r');
    List_Destroy(list, seat_list_node_t);
}

// 添加新座位界面
int Seat_UI_Add(seat_list_t list, int roomID, int rowsCount, int colsCount) {
    int row, col;
    char statusChar;
    printf("\n===== 添加新座位 =====\n");
    printf("请输入行号(1-%d): ", rowsCount);
    scanf("%d", &row);
    printf("请输入列号(1-%d): ", colsCount);
    scanf("%d", &col);
    if (row < 1 || row > rowsCount || col < 1 || col > colsCount) {
        printf("行号或列号无效！\n");
        return 0;
    }
    if (Seat_Srv_FindByRowCol(list, row, col) != NULL) {
        printf("该位置已有座位！\n");
        return 0;
    }
    printf("请输入座位状态（#有效 ~损坏 空格过道）: ");
    scanf(" %c", &statusChar);
    seat_status_t status = Seat_UI_Char2Status(statusChar);
    seat_t newSeat;
    newSeat.roomID = roomID;
    newSeat.row = row;
    newSeat.column = col;
    newSeat.status = status;
    if (Seat_Srv_Add(&newSeat)) {
        seat_list_node_t* newNode = (seat_list_node_t*)malloc(sizeof(seat_list_node_t));
        newNode->data = newSeat;
        Seat_Srv_AddToSortedList(list, newNode);
        printf("座位添加成功！座位ID: %d\n", newSeat.id);
        return 1;
    }
    else {
        printf("座位添加失败！\n");
        return 0;
    }
}

// 修改座位界面
int Seat_UI_Modify(seat_list_t list, int rowsCount, int colsCount) {
    int row, col;
    char statusChar;
    printf("\n===== 修改座位 =====\n");
    printf("请输入要修改座位的行号(1-%d): ", rowsCount);
    scanf("%d", &row);
    printf("请输入要修改座位的列号(1-%d): ", colsCount);
    scanf("%d", &col);
    seat_list_node_t* node = Seat_Srv_FindByRowCol(list, row, col);
    if (node == NULL) {
        printf("该位置没有座位！\n");
        return 0;
    }
    printf("当前座位状态: %c\n", Seat_UI_Status2Char(node->data.status));
    printf("请输入新座位状态（#有效 ~损坏 空格过道）: ");
    scanf(" %c", &statusChar);
    node->data.status = Seat_UI_Char2Status(statusChar);
    if (Seat_Srv_Modify(&node->data)) {
        printf("座位修改成功！\n");
        return 1;
    }
    else {
        printf("座位修改失败！\n");
        return 0;
    }
}

// 删除座位界面
int Seat_UI_Delete(seat_list_t list, int rowsCount, int colsCount) {
    int row, col;
    printf("\n===== 删除座位 =====\n");
    printf("请输入要删除座位的行号(1-%d): ", rowsCount);
    scanf("%d", &row);
    printf("请输入要删除座位的列号(1-%d): ", colsCount);
    scanf("%d", &col);
    seat_list_node_t* node = Seat_Srv_FindByRowCol(list, row, col);
    if (node == NULL) {
        printf("该位置没有座位！\n");
        return 0;
    }
    if (Seat_Srv_DeleteByID(node->data.id)) {
        List_FreeNode(node);
        printf("座位删除成功！\n");
        return 1;
    }
    else {
        printf("座位删除失败！\n");
        return 0;
    }
}