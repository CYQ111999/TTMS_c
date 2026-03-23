#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Play.h"
#include "EntityKey_Persist.h"
#include "List.h"
#define _CRT_SECURE_NO_WARNINGS

#define PLAY_DATA_FILE "Play.dat"
#define PLAY_DATA_TEMP_FILE "PlayTmp.dat"

// 从剧目信息文件Play.dat中载入剧目名称包含指定字符串的剧目信息
int Play_Perst_SelectByName(play_list_t list, char condt[])
{
    int recCount = 0;        // 记录计数器
    FILE* fp = NULL;         // 文件指针
    play_t data;             // 临时存储读取的数据
    play_list_node_t* newNode = NULL;  // 新节点指针

    // 参数验证
    if (list == NULL) {
        return 0;
    }
    if (condt == NULL || condt[0] == '\0') {
        return 0;
    }

    recCount = 0;

    // 清空链表
    List_Free(list, play_list_node_t);

    // 判断剧目数据文件Play.dat是否存在
    fp = fopen(PLAY_DATA_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    // 循环读取文件
    while (1) {
        // 从文件中读取数据
        size_t read_count = fread(&data, sizeof(play_t), 1, fp);
        if (read_count != 1) {
            break;
        }

        // 判断data.name是否包含待查字符串condt
        if (strstr(data.name, condt) != NULL) {
            // 分配新节点内存
            newNode = (play_list_node_t*)malloc(sizeof(play_list_node_t));
            if (newNode == NULL) {
                fclose(fp);
                return recCount;
            }

            // 复制数据
            newNode->data = data;

            // 正确添加到链表尾部
            List_AddTail(list, newNode);
            recCount++;
        }
    }

    fclose(fp);
    return recCount;
}

int Play_Perst_SelectAll(play_list_t list) {
    int recCount = 0;
    play_t data;

    // 参数验证
    if (list == NULL) {
        return 0;
    }

    FILE* fp = fopen(PLAY_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    // 清空链表
    List_Free(list, play_list_node_t);

    // 读取文件并构建链表
    while (fread(&data, sizeof(play_t), 1, fp) == 1) {
        play_list_node_t* newNode = (play_list_node_t*)malloc(sizeof(play_list_node_t));
        if (!newNode) {
            break;
        }
        newNode->data = data;
        List_AddTail(list, newNode);
        recCount++;
    }

    fclose(fp);
    return recCount;
}

int Play_Perst_Insert(play_t* data) {
    long key = EntKey_Perst_GetNewKeys("play", 1);
    if (key <= 0) return 0;
    data->id = (int)key;
    FILE* fp = fopen(PLAY_DATA_FILE, "ab");
    if (!fp) {
        return 0;
    }
    int ret = fwrite(data, sizeof(play_t), 1, fp);
    fclose(fp);
    return ret;
}

int Play_Perst_Update(const play_t* data) {
    int found = 0;
    play_t buf;
    FILE* fp = fopen(PLAY_DATA_FILE, "rb+");
    if (!fp) {
        return 0;
    }

    while (fread(&buf, sizeof(play_t), 1, fp) == 1) {
        if (buf.id == data->id) {
            fseek(fp, -(long)sizeof(play_t), SEEK_CUR);
            fwrite(data, sizeof(play_t), 1, fp);
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

int Play_Perst_RemByID(int id) {
    int found = 0;
    play_t buf;

    if (rename(PLAY_DATA_FILE, PLAY_DATA_TEMP_FILE) != 0) {
        return 0;
    }

    FILE* fpSrc = fopen(PLAY_DATA_TEMP_FILE, "rb");
    FILE* fpDst = fopen(PLAY_DATA_FILE, "wb");

    if (!fpSrc || !fpDst) {
        if (fpSrc) fclose(fpSrc);
        if (fpDst) fclose(fpDst);
        return 0;
    }

    while (fread(&buf, sizeof(play_t), 1, fpSrc) == 1) {
        if (buf.id != id) {
            fwrite(&buf, sizeof(play_t), 1, fpDst);
        }
        else {
            found = 1;
        }
    }

    fclose(fpSrc);
    fclose(fpDst);
    remove(PLAY_DATA_TEMP_FILE);

    return found;
}

int Play_Perst_SelectByID(int id, play_t* buf) {
    int found = 0;
    play_t data;

    FILE* fp = fopen(PLAY_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    while (fread(&data, sizeof(play_t), 1, fp) == 1) {
        if (data.id == id) {
            *buf = data;
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found;
}