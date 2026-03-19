#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Play.h"
#include "EntityKey_Persist.h"
#include "List.h"

#define PLAY_DATA_FILE "Play.dat"
#define PLAY_DATA_TEMP_FILE "PlayTmp.dat"

int Play_Perst_SelectAll(play_list_t list) {
    int recCount = 0;
    play_t data;

    FILE* fp = fopen(PLAY_DATA_FILE, "rb");
    if (!fp) {
        return 0;
    }

    List_Free(list, play_list_node_t);

    while (fread(&data, sizeof(play_t), 1, fp) == 1) {
        play_list_node_t* newNode = (play_list_node_t*)malloc(sizeof(play_list_node_t));
        if (!newNode) break;

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
        printf("打开文件 %s 失败！\n", PLAY_DATA_FILE);
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
        printf("打开文件 %s 失败！\n", PLAY_DATA_FILE);
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