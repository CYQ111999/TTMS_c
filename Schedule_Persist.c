#include "Schedule_Persist.h"
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>

static const char* SCHEDULE_FILE = "Schedule.dat";

int Schedule_Perst_SelectByID(int id, schedule_t* buf) {
    FILE* fp = fopen(SCHEDULE_FILE, "rb");
    if (!fp) return 0;
    schedule_t data;
    int found = 0;
    while (fread(&data, sizeof(schedule_t), 1, fp) == 1) {
        if (data.id == id) {
            *buf = data;
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

int Schedule_Perst_SelectByPlayID(int play_id, schedule_list_t* list) {
    FILE* fp = fopen(SCHEDULE_FILE, "rb");
    if (!fp) return 0;
    schedule_t data;
    int count = 0;

    while (fread(&data, sizeof(schedule_t), 1, fp) == 1) {
        if (data.play_id == play_id) {
            // 分配新节点（使用正确的类型名）
            schedule_list_t node = (schedule_list_t)malloc(sizeof(schedule_list_node_t));
            if (!node) break;
            node->data = data;
            node->node.next = NULL;  // 通过 node.next 访问 next 指针

            if (*list == NULL) {
                *list = node;
            }
            else {
                // 找到链表尾部
                schedule_list_t p = *list;
                while (p->node.next) {
                    p = (schedule_list_t)(p->node.next);
                }
                p->node.next = (list_node_t*)node;
            }
            count++;
        }
    }

    fclose(fp);
    return count;
}