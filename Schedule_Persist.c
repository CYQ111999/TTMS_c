#define _CRT_SECURE_NO_WARNINGS
#include "Common.h"
#include "List.h"
#include "Utils.h"
#include "Studio.h"
#include "Seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <io.h>
#include "Schedule_Persist.h"

#define SCHEDULE_DATA_FILE "Schedule.dat"
#define SCHEDULE_TEMP_FILE "ScheduleTmp.dat"

// 检查文件是否存在
static int fileExists(const char* filename) {
    return _access(filename, 0) == 0;
}

int Schedule_Perst_SelectByPlay(schedule_list_t* list, int play_id)
{
    FILE* fp = NULL;
    schedule_t data = { 0 };
    int recCount = 0;

    // 参数验证
    if (list == NULL) {
        return 0;
    }

    // 打开文件
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    // 初始化链表
    if (*list != NULL) {
        List_Free(*list, schedule_list_node_t);
    }
    else {
        List_Init(*list, schedule_list_node_t);
    }

    if (*list == NULL) {
        fclose(fp);
        return 0;
    }

    // 读取文件
    while (fread(&data, sizeof(schedule_t), 1, fp) == 1) {
        // 检查是否匹配剧目ID
        if (data.play_id == play_id) {
            // 创建新节点
            schedule_list_node_t* newNode = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
            if (newNode == NULL) {
                break;
            }

            // 复制数据
            newNode->data = data;

            // 将新节点添加到链表尾部
            List_AddTail(*list, newNode);
            recCount++;
        }
    }

    fclose(fp);

    // 如果没有数据，清理链表
    if (recCount == 0 && *list != NULL) {
        List_Destroy(*list, schedule_list_node_t);
        *list = NULL;
    }

    return recCount;
}

int Schedule_Perst_Insert(schedule_t* data)
{
    FILE* fp = NULL;
    schedule_t last_record;
    int max_id = 0;

    // 参数验证
    if (data == NULL) {
        return 0;
    }

    // 读取现有记录，找出最大的ID
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp != NULL) {
        while (fread(&last_record, sizeof(schedule_t), 1, fp) == 1) {
            if (last_record.id > max_id) {
                max_id = last_record.id;
            }
        }
        fclose(fp);
    }

    // 为新记录分配ID
    data->id = max_id + 1;

    // 打开文件，追加写入新记录
    fp = fopen(SCHEDULE_DATA_FILE, "ab");
    if (fp == NULL) {
        return 0;
    }

    // 写入记录
    int result = fwrite(data, sizeof(schedule_t), 1, fp);
    fclose(fp);

    return result;
}

int Schedule_Perst_Update(const schedule_t* data)
{
    int found = 0;
    FILE* fp = NULL;
    schedule_t buf;

    // 参数验证
    if (data == NULL) {
        return 0;
    }

    // 检查文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;
    }

    fp = fopen(SCHEDULE_DATA_FILE, "rb+");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (1) {
        long current_pos = ftell(fp);

        // 读出当前文件读写指针所指的演出计划信息
        if (fread(&buf, sizeof(schedule_t), 1, fp) != 1) {
            if (feof(fp)) {
                break;
            }
            continue;
        }

        // 判断 buf.id 是否和 data.id 相等
        if (buf.id == data->id) {
            // 回退读写指针到该记录开始的位置
            fseek(fp, current_pos, SEEK_SET);

            // 将参数 data 所保存的演出计划写入文件覆盖旧数据
            if (fwrite(data, sizeof(schedule_t), 1, fp) == 1) {
                found = 1;
            }
            break;
        }
    }

    fclose(fp);
    return found;
}

int Schedule_Perst_RemByID(int id)
{
    int found = 0;
    FILE* fp_src = NULL;
    FILE* fp_dst = NULL;
    schedule_t buf;

    // 检查原文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;
    }

    // 重命名原文件
    if (rename(SCHEDULE_DATA_FILE, SCHEDULE_TEMP_FILE) != 0) {
        return 0;
    }

    // 打开临时文件（读取）和新文件（写入）
    fp_src = fopen(SCHEDULE_TEMP_FILE, "rb");
    if (fp_src == NULL) {
        rename(SCHEDULE_TEMP_FILE, SCHEDULE_DATA_FILE);
        return 0;
    }

    fp_dst = fopen(SCHEDULE_DATA_FILE, "wb");
    if (fp_dst == NULL) {
        fclose(fp_src);
        rename(SCHEDULE_TEMP_FILE, SCHEDULE_DATA_FILE);
        return 0;
    }

    // 循环读取临时文件
    while (fread(&buf, sizeof(schedule_t), 1, fp_src) == 1) {
        if (buf.id == id) {
            found = 1;
        }
        else {
            fwrite(&buf, sizeof(schedule_t), 1, fp_dst);
        }
    }

    fclose(fp_src);
    fclose(fp_dst);

    // 删除临时文件
    remove(SCHEDULE_TEMP_FILE);
    return found;
}

int Schedule_Perst_SelectAll(schedule_list_t* list)
{
    int recCount = 0;
    FILE* fp = NULL;
    schedule_t data;

    // 参数验证
    if (list == NULL) {
        return 0;
    }

    // 初始化链表
    if (*list != NULL) {
        List_Free(*list, schedule_list_node_t);
    }
    else {
        List_Init(*list, schedule_list_node_t);
    }

    if (*list == NULL) {
        return 0;
    }

    // 检查文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;
    }

    // 打开文件
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (fread(&data, sizeof(schedule_t), 1, fp) == 1) {
        // 创建新结点
        schedule_list_node_t* newNode = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
        if (newNode == NULL) {
            fclose(fp);
            return recCount;
        }

        // 初始化新节点
        newNode->data = data;

        // 使用 List_AddTail 宏添加到链表尾部
        List_AddTail(*list, newNode);
        recCount++;
    }

    fclose(fp);

    // 如果没有数据，清理链表
    if (recCount == 0 && *list != NULL) {
        List_Destroy(*list, schedule_list_node_t);
        *list = NULL;
    }

    return recCount;
}

int Schedule_Perst_SelectByID(int id, schedule_t* buf)
{
    int found = 0;
    FILE* fp = NULL;
    schedule_t data;

    // 验证参数
    if (id <= 0 || buf == NULL) {
        return 0;
    }

    // 检查文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;
    }

    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (fread(&data, sizeof(schedule_t), 1, fp) == 1) {
        if (data.id == id) {
            found = 1;
            *buf = data;
            break;
        }
    }

    fclose(fp);
    return found;
}