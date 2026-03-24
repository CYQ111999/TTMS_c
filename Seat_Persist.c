#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Seat.h"
#include "EntityKey_Persist.h"
#include "List.h"
#define _CRT_SECURE_NO_WARNINGS

#define SEAT_DATA_FILE "Seat.dat"
#define SEAT_DATA_TEMP_FILE "SeatTmp.dat"

// 向文件中存储新座位
int Seat_Perst_Insert(seat_t* data) {
    long key = EntKey_Perst_GetNewKeys("seat", 1);
    if (key <= 0) return 0;
    data->id = (int)key;

    FILE* fp = fopen(SEAT_DATA_FILE, "ab");
    if (!fp) {
        printf("打开文件 %s 失败！\n", SEAT_DATA_FILE);
        return 0;
    }

    int ret = fwrite(data, sizeof(seat_t), 1, fp);
    fclose(fp);
    return ret;
}

// 批量存储座位
int Seat_Perst_InsertBatch(seat_list_t list) {
    int count = 0;
    seat_list_node_t* p;

    List_ForEach(list, p) {
        count++;
    }

    if (count == 0) return 0;

    long startKey = EntKey_Perst_GetNewKeys("seat", count);
    if (startKey <= 0) return 0;

    FILE* fp = fopen(SEAT_DATA_FILE, "ab");
    if (!fp) {
        printf("打开文件 %s 失败！\n", SEAT_DATA_FILE);
        return 0;
    }

    int ret = 0;
    long key = startKey;
    List_ForEach(list, p) {
        p->data.id = (int)key++;
        if (fwrite(&p->data, sizeof(seat_t), 1, fp) == 1) {
            ret++;
        }
    }

    fclose(fp);
    return ret;
}

// 更新座位
int Seat_Perst_Update(const seat_t* data) {
    int found = 0;
    seat_t buf;

    FILE* fp = fopen(SEAT_DATA_FILE, "rb+");
    if (!fp) {
        printf("打开文件 %s 失败！\n", SEAT_DATA_FILE);
        return 0;
    }

    while (fread(&buf, sizeof(seat_t), 1, fp) == 1) {
        if (buf.id == data->id) {
            fseek(fp, -(long)sizeof(seat_t), SEEK_CUR);
            fwrite(data, sizeof(seat_t), 1, fp);
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found;
}
// 根据座位ID删除座位
int Seat_Perst_RemoveByID(int ID) {
    int found = 0;
    seat_t buf;

    if (rename(SEAT_DATA_FILE, SEAT_DATA_TEMP_FILE) != 0) {
        return 0;
    }

    FILE* fpSrc = fopen(SEAT_DATA_TEMP_FILE, "rb");
    FILE* fpDst = fopen(SEAT_DATA_FILE, "wb");

    if (!fpSrc || !fpDst) {
        if (fpSrc) fclose(fpSrc);
        if (fpDst) fclose(fpDst);
        return 0;
    }

    while (fread(&buf, sizeof(seat_t), 1, fpSrc) == 1) {
        if (buf.id != ID) {
            fwrite(&buf, sizeof(seat_t), 1, fpDst);
        }
        else {
            found = 1;
        }
    }

    fclose(fpSrc);
    fclose(fpDst);
    remove(SEAT_DATA_TEMP_FILE);

    return found;
}

// 根据演出厅ID删除所有座位
int Seat_Perst_RemoveAllByRoomID(int roomID) {
    int count = 0;
    seat_t buf;

    if (rename(SEAT_DATA_FILE, SEAT_DATA_TEMP_FILE) != 0) {
        return 0;
    }

    FILE* fpSrc = fopen(SEAT_DATA_TEMP_FILE, "rb");
    FILE* fpDst = fopen(SEAT_DATA_FILE, "wb");

    if (!fpSrc || !fpDst) {
        if (fpSrc) fclose(fpSrc);
        if (fpDst) fclose(fpDst);
        return 0;
    }

    while (fread(&buf, sizeof(seat_t), 1, fpSrc) == 1) {
        if (buf.roomID != roomID) {
            fwrite(&buf, sizeof(seat_t), 1, fpDst);
        }
        else {
            count++;
        }
    }

    fclose(fpSrc);
    fclose(fpDst);
    remove(SEAT_DATA_TEMP_FILE);

    return count;
}

// 载入所有座位
int Seat_Perst_SelectAll(seat_list_t list) {
    int recCount = 0;
    seat_t data;

    FILE* fp = fopen(SEAT_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    List_Free(list, seat_list_node_t);

    while (fread(&data, sizeof(seat_t), 1, fp) == 1) {
        seat_list_node_t* newNode = (seat_list_node_t*)malloc(sizeof(seat_list_node_t));
        if (!newNode) break;

        newNode->data = data;
        List_AddTail(list, newNode);
        recCount++;
    }

    fclose(fp);
    return recCount;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Seat_Perst_SelectByRoomID(seat_list_t list, int roomID) {
    int recCount = 0;
    seat_t data;
    FILE* fp = NULL;

    if (list == NULL) {
        return 0;
    }

    // 清空链表
    //List_Free(list, seat_list_node_t);

    fp = fopen(SEAT_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    while (fread(&data, sizeof(seat_t), 1, fp) == 1) {
        if (data.roomID == roomID) {
            seat_list_node_t* newNode = (seat_list_node_t*)malloc(sizeof(seat_list_node_t));
            if (!newNode) {
                fclose(fp);
                return recCount;
            }

            newNode->data = data;
            List_AddTail(list, newNode);
            recCount++;
        }
    }

    fclose(fp);
    return recCount;
}