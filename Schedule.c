#include "Schedule_UI.h"
#include <stdio.h>
#include "common.h"

void Schedule_UI_ListAll(void) {
    FILE* fp = fopen("Schedule.dat", "rb");
    if (!fp) {
        printf("无法打开演出计划文件！\n");
        return;
    }
    schedule_t sch;
    printf("ID\t剧目ID\t演出厅\t日期\t时间\n");
    printf("----------------------------------------\n");
    while (fread(&sch, sizeof(schedule_t), 1, fp) == 1) {
        printf("%d\t%d\t%d\t%s\t%s\n", sch.id, sch.play_id, sch.studio_id, sch.date, sch.time);
    }
    fclose(fp);
}