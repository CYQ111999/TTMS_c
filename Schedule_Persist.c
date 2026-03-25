//zyp
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
#include <io.h>  // 添加这个头文件以使用 _access 函数

 /* 数据文件名常量 */
#define SCHEDULE_DATA_FILE "Schedule.dat"
#define SCHEDULE_TEMP_FILE "ScheduleTmp.dat"
#define PLAY_DATA_FILE "Play.dat"

// 检查文件是否存在
static int fileExists(const char* filename) {
    return _access(filename, 0) == 0;
}

int Schedule_Perst_SelectByPlay(schedule_list_t* list, int play_id)
{
    FILE* fp = NULL;
    schedule_t data = { 0 };  // 使用 {0} 初始化
    schedule_list_node_t* newNode = NULL;
    int recCount = 0;
    // 验证参数
    if (list == NULL) {
        printf("错误：链表指针为NULL\n");
        return 0;
    }

    // 打开文件
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    // 如果链表为空，创建头节点
    if (*list == NULL) {
        *list = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
        if (*list == NULL) {
            printf("错误：无法分配头节点内存\n");
            fclose(fp);
            return 0;
        }
        // 初始化头节点
        (*list)->node.next = (list_node_t*)*list;
        (*list)->node.prev = (list_node_t*)*list;
        printf("调试：创建链表头节点\n");
    }

    // 读取文件
    while (!feof(fp)) {
        // 每次读取前清零
        memset(&data, 0, sizeof(schedule_t));
        if (fread(&data, sizeof(schedule_t), 1, fp) != 1) {
            break;  // 读取失败或到达文件尾
        }
        // 检查是否匹配剧目ID
        if (data.play_id == play_id) {
            printf("调试：找到匹配记录，创建新节点\n");

            // 创建新节点
            newNode = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
            if (newNode == NULL) {
                printf("错误：无法分配节点内存\n");
                break;
            }
            // 复制数据
            newNode->data = data;
            // 将新节点添加到链表尾部
            // 获取尾节点
            schedule_list_node_t* tail = (schedule_list_node_t*)((*list)->node.prev);
            // 设置新节点的指针
            newNode->node.next = (list_node_t*)*list;
            newNode->node.prev = (list_node_t*)tail;
            // 更新尾节点和头节点的指针
            tail->node.next = (list_node_t*)newNode;
            (*list)->node.prev = (list_node_t*)newNode;
            recCount++;
            printf("调试：已添加 %d 条记录\n", recCount);
        }
    }

    fclose(fp);
    printf("调试：共找到 %d 条匹配记录\n", recCount);
    return recCount;
}

// 插入新演出计划
int Schedule_Perst_Insert(schedule_t* data)
{
    FILE* fp = NULL;
    schedule_t last_record;
    int max_id = 0;
    int read_count = 0;

    // 参数验证
    if (data == NULL) {
        printf("错误：数据指针为空！\n");
        return 0;
    }

    printf("调试：开始插入演出计划...\n");

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
        // 如果文件不存在，尝试创建
        fp = fopen(SCHEDULE_DATA_FILE, "wb");
        if (fp) {
            fclose(fp);
            fp = fopen(SCHEDULE_DATA_FILE, "ab");
        }
    }
    if (fp == NULL) {
        printf("错误：无法打开文件 %s\n", SCHEDULE_DATA_FILE);
        return 0;
    }
    // 写入记录
    if (fwrite(data, sizeof(schedule_t), 1, fp) == 1) {
        printf("调试：成功写入演出计划，ID=%d\n", data->id);
        fclose(fp);
        return 1;
    }
    else {
        printf("错误：写入文件失败\n");
        fclose(fp);
        return 0;
    }
}

// 更新演出计划
int Schedule_Perst_Update(const schedule_t* data)
{
    int found = 0;      // 查找标志
    FILE* fp = NULL;    // 文件指针
    schedule_t buf;     // 临时缓冲区
    // 参数验证
    if (data == NULL) {
        return 0;
    }
    // 检查文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;  // 文件不存在，无法更新
    }
    fp = fopen(SCHEDULE_DATA_FILE, "rb+");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (1) {
        long current_pos = ftell(fp);

        // 读出当前文件读写指针所指的演出计划信息
        size_t read_count = fread(&buf, sizeof(schedule_t), 1, fp);
        if (read_count != 1) {
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
                found = 1;  // 更新成功
            }
            break;  // 找到并更新后退出循环
        }
    }

    fclose(fp);
    return found;
}

// 根据ID删除演出计划
int Schedule_Perst_RemByID(int id)
{
    int found = 0;                    // 查找标志
    FILE* fp_src = NULL;              // 源文件指针（临时文件）
    FILE* fp_dst = NULL;              // 目标文件指针（原文件）
    schedule_t buf;                   // 临时缓冲区

    // 检查原文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;  // 文件不存在，无法删除
    }

    // 重命名原文件
    if (rename(SCHEDULE_DATA_FILE, SCHEDULE_TEMP_FILE) != 0) {
        return 0;
    }

    // 打开临时文件（读取）和新文件（写入）
    fp_src = fopen(SCHEDULE_TEMP_FILE, "rb");
    if (fp_src == NULL) {
        // 恢复原文件名
        rename(SCHEDULE_TEMP_FILE, SCHEDULE_DATA_FILE);
        return 0;
    }

    fp_dst = fopen(SCHEDULE_DATA_FILE, "wb");
    if (fp_dst == NULL) {
        fclose(fp_src);
        // 恢复原文件名
        rename(SCHEDULE_TEMP_FILE, SCHEDULE_DATA_FILE);
        return 0;
    }

    // 循环读取临时文件
    while (1) {
        // 从 ScheduleTmp.dat 文件读出一条记录到临时变量 buf
        size_t read_count = fread(&buf, sizeof(schedule_t), 1, fp_src);
        if (read_count != 1) {
            if (feof(fp_src)) {
                break;
            }
            continue;
        }

        // 判断 buf.id 是否等于参数 id
        if (buf.id == id) {
            found = 1;  // 找到了要删除的记录
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

// 从演出计划数据文件 Schedule.dat 中载入所有演出计划，加载到链表 list 上
int Schedule_Perst_SelectAll(schedule_list_t* list)  // 改为二级指针
{
    int recCount = 0;        // 记录计数器
    FILE* fp = NULL;         // 文件指针
    schedule_t data;         // 临时存储读取的数据

    // 参数验证
    if (list == NULL) {
        return 0;
    }

    // 初始化链表
    List_Init(*list, schedule_list_node_t);
    if (*list == NULL) {
        return 0;
    }

    // 检查文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;  // 文件不存在，没有演出计划
    }

    // 打开文件
    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (1) {
        // 从演出计划数据文件读出当前记录
        size_t read_count = fread(&data, sizeof(schedule_t), 1, fp);
        if (read_count != 1) {
            if (feof(fp)) {
                break;
            }
            continue;
        }

        // 构造新结点
        schedule_list_node_t* newNode = (schedule_list_node_t*)malloc(sizeof(schedule_list_node_t));
        if (newNode == NULL) {
            fclose(fp);
            return recCount;  // 返回已读取的记录数
        }

        // 初始化新节点
        newNode->data = data;

        // 使用 List_AddTail 宏添加到链表尾部
        List_AddTail(*list, newNode);
        recCount++;
    }

    fclose(fp);
    return recCount;
}

// 从文件 Schedule.dat 中载入指定ID的演出计划
int Schedule_Perst_SelectByID(int id, schedule_t* buf)
{
    int found = 0;      // 查找标志
    FILE* fp = NULL;    // 文件指针
    schedule_t data;    // 临时存储读取的数据

    // 验证参数
    if (id <= 0 || buf == NULL) {
        return 0;
    }

    // 检查文件是否存在
    if (!fileExists(SCHEDULE_DATA_FILE)) {
        return 0;  // 文件不存在
    }

    fp = fopen(SCHEDULE_DATA_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (1) {
        // 从 Schedule.dat 文件读出一条记录保存在局部变量 data 中
        size_t read_count = fread(&data, sizeof(schedule_t), 1, fp);
        if (read_count != 1) {
            if (feof(fp)) {
                break;
            }
            continue;
        }

        // 判断 data.id 是否等于参数 id
        if (data.id == id) {
            found = 1;
            *buf = data;  // 将 data 的值复制到 buf 指向的内存
            break;  // 找到后跳出循环
        }
    }

    fclose(fp);
    return found;
}