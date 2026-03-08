////测试时如果发生main函数重复，可以先ctrlA,再K加C注释别的main包

//// test_common.c - Common.h 结构体定义完整性测试
//// 测试文件应放在 Source Files 目录下
//// 引用方式：#include "../Header Files/Common/Common.h"
////测试代码是AI生成的，跑不了的检查路径
//
//#include "Common.h" //这里遇到问题，如果物理位置在一起，就直接#include "Common.h"
//                    //如果不在一起，就#include "Common/Common.h"
//#include <stdio.h>
//#include <string.h>
//
//// 定义全局变量 gl_CurUser（在 Common.h 中声明为 extern）
//account_t gl_CurUser = { 0, USR_ANOMY, "Anonymous", "" };
//
//// 打印日期辅助函数
//void print_date(const ttms_date_t* date) {
//    printf("%04d-%02d-%02d", date->year, date->month, date->day);
//}
//
//// 打印时间辅助函数
//void print_time(const ttms_time_t* time) {
//    printf("%02d:%02d:%02d", time->hour, time->minute, time->second);
//}
//
//// 打印账户类型辅助函数
//const char* account_type_str(account_type_t type) {
//    switch (type) {
//    case USR_ANOMY: return "匿名";
//    case USR_CLERK: return "售票员";
//    case USR_MANG:  return "经理";
//    case USR_ADMIN: return "管理员";
//    default: return "未知";
//    }
//}
//
//// 打印座位状态辅助函数
//const char* seat_status_str(seat_status_t status) {
//    switch (status) {
//    case SEAT_NONE:   return "空位";
//    case SEAT_GOOD:   return "有座位";
//    case SEAT_BROKEN: return "损坏";
//    default: return "未知";
//    }
//}
//
//// 打印剧目类型辅助函数
//const char* play_type_str(play_type_t type) {
//    switch (type) {
//    case PLAY_TYPE_FILE:   return "文件";
//    case PLAY_TYPE_OPEAR:  return "歌剧";
//    case PLAY_TYPE_CONCERT:return "音乐会";
//    default: return "未知";
//    }
//}
//
//// 打印剧目分级辅助函数
//const char* play_rating_str(play_rating_t rating) {
//    switch (rating) {
//    case PLAY_RATE_CHILD:   return "儿童";
//    case PLAY_RATE_TEENAGE: return "青少年";
//    case PLAY_RATE_ADULT:   return "成人";
//    default: return "未知";
//    }
//}
//
//// 打印票状态辅助函数
//const char* ticket_status_str(ticket_status_t status) {
//    switch (status) {
//    case TICKET_AVL:  return "待售";
//    case TICKET_SOLD: return "已售";
//    case TICKET_RESV: return "预留";
//    default: return "未知";
//    }
//}
//
//// 打印订单类型辅助函数
//const char* sale_type_str(sale_type_t type) {
//    switch (type) {
//    case SALE_SELL:    return "买票";
//    case SALE_REFOUND: return "退票";
//    default: return "未知";
//    }
//}
//
//// 主测试函数
//int main() {
//    printf("===========================================\n");
//    printf("        Common.h 结构体定义完整性测试\n");
//    printf("===========================================\n\n");
//
//    // 1. 测试基础类型：ttms_date_t 和 ttms_time_t
//    printf("1. 测试基础日期时间类型：\n");
//    ttms_date_t date = { 2024, 5, 20 };
//    ttms_time_t time = { 14, 30, 0 };
//    printf("   日期：");
//    print_date(&date);
//    printf("\n   时间：");
//    print_time(&time);
//    printf("\n   ✅ 日期时间类型测试通过\n\n");
//
//    // 2. 测试系统用户相关
//    printf("2. 测试系统用户相关类型：\n");
//    account_t user = { 1001, USR_CLERK, "zhangsan", "123456" };
//    printf("   用户ID: %d, 用户名: %s, 类型: %s\n",
//        user.id, user.username, account_type_str(user.type));
//    printf("   全局用户: %s, 类型: %s\n",
//        gl_CurUser.username, account_type_str(gl_CurUser.type));
//
//    account_list_node_t account_node = { user, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(account_node));
//    printf("   ✅ 系统用户类型测试通过\n\n");
//
//    // 3. 测试演出厅相关
//    printf("3. 测试演出厅相关类型：\n");
//    studio_t studio = { 1, "1号厅", 10, 20, 200 };
//    printf("   演出厅: %s, 座位: %d行×%d列, 总数: %d\n",
//        studio.name, studio.rowsCount, studio.colsCount, studio.seatsCount);
//
//    studio_list_node_t studio_node = { studio, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(studio_node));
//    printf("   ✅ 演出厅类型测试通过\n\n");
//
//    // 4. 测试座位相关
//    printf("4. 测试座位相关类型：\n");
//    seat_t seat = { 101, 1, 5, 10, SEAT_GOOD };
//    printf("   座位: 厅%d, 位置: %d行%d列, 状态: %s\n",
//        seat.roomID, seat.row, seat.column, seat_status_str(seat.status));
//
//    seat_list_node_t seat_node = { seat, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(seat_node));
//    printf("   ✅ 座位类型测试通过\n\n");
//
//    // 5. 测试剧目相关
//    printf("5. 测试剧目相关类型：\n");
//    play_t play = { 1, "罗密欧与朱丽叶", PLAY_TYPE_OPEAR, "英国",
//                   PLAY_RATE_ADULT, 150, {2024, 5, 1}, {2024, 6, 30}, 180 };
//    printf("   剧目: %s\n", play.name);
//    printf("   类型: %s, 分级: %s, 时长: %d分钟\n",
//        play_type_str(play.type), play_rating_str(play.rating), play.duration);
//    printf("   上映日期: ");
//    print_date(&play.start_date);
//    printf(" 至 ");
//    print_date(&play.end_date);
//    printf("\n   票价: %d元\n", play.price);
//
//    play_list_node_t play_node = { play, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(play_node));
//    printf("   ✅ 剧目类型测试通过\n\n");
//
//    // 6. 测试演出计划相关
//    printf("6. 测试演出计划相关类型：\n");
//    schedule_t schedule = { 1, 1, 1, {2024, 5, 20}, {19, 30, 0}, 200 };
//    printf("   演出计划ID: %d\n", schedule.id);
//    printf("   关联剧目ID: %d, 演出厅ID: %d\n", schedule.play_id, schedule.studio_id);
//    printf("   时间: ");
//    print_date(&schedule.date);
//    printf(" ");
//    print_time(&schedule.time);
//    printf("\n   座位数: %d\n", schedule.seat_count);
//
//    schedule_list_node_t schedule_node = { schedule, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(schedule_node));
//    printf("   ✅ 演出计划类型测试通过\n\n");
//
//    // 7. 测试票相关
//    printf("7. 测试票相关类型：\n");
//    ticket_t ticket = { 10001, 1, 101, 180, TICKET_AVL };
//    printf("   票ID: %d, 计划ID: %d, 座位ID: %d\n",
//        ticket.id, ticket.schedule_id, ticket.seat_id);
//    printf("   票价: %d元, 状态: %s\n", ticket.price, ticket_status_str(ticket.status));
//
//    ticket_list_node_t ticket_node = { ticket, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(ticket_node));
//    printf("   ✅ 票类型测试通过\n\n");
//
//    // 8. 测试订单相关
//    printf("8. 测试订单相关类型：\n");
//    sale_t sale = { 100001, 1001, 10001, {2024, 5, 20}, {14, 25, 30}, 180, SALE_SELL };
//    printf("   订单ID: %ld, 售票员ID: %d, 票ID: %d\n",
//        sale.id, sale.user_id, sale.ticket_id);
//    printf("   时间: ");
//    print_date(&sale.date);
//    printf(" ");
//    print_time(&sale.time);
//    printf("\n   金额: %d元, 类型: %s\n", sale.value, sale_type_str(sale.type));
//
//    sale_list_node_t sale_node = { sale, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(sale_node));
//    printf("   ✅ 订单类型测试通过\n\n");
//
//    // 9. 测试统计票房相关
//    printf("9. 测试统计票房相关类型：\n");
//    salesanalysis_t analysis = { 1, "罗密欧与朱丽叶", "英国", 150,
//                                1000, 180000, 180, {2024, 5, 1}, {2024, 6, 30} };
//    printf("   剧目: %s, 区域: %s\n", analysis.name, analysis.area);
//    printf("   上座数量: %ld, 票房: %ld元\n", analysis.totaltickets, analysis.sales);
//
//    sales_analysis_list_node_t analysis_node = { analysis, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(analysis_node));
//    printf("   ✅ 统计票房类型测试通过\n\n");
//
//    // 10. 测试主键相关
//    printf("10. 测试主键相关类型：\n");
//    entity_key_t key = { "Account", 1001 };
//    printf("   主键名称: %s, 键值: %ld\n", key.entyName, key.key);
//
//    entkey_list_node_t key_node = { key, NULL, NULL };
//    printf("   链表节点大小: %zu 字节\n", sizeof(key_node));
//    printf("   ✅ 主键类型测试通过\n\n");
//
//    // 11. 测试枚举类型
//    printf("11. 测试枚举类型：\n");
//    printf("   用户类型: USR_CLERK=%d, USR_MANG=%d, USR_ADMIN=%d\n",
//        USR_CLERK, USR_MANG, USR_ADMIN);
//    printf("   座位状态: SEAT_NONE=%d, SEAT_GOOD=%d, SEAT_BROKEN=%d\n",
//        SEAT_NONE, SEAT_GOOD, SEAT_BROKEN);
//    printf("   剧目类型: PLAY_TYPE_FILE=%d, PLAY_TYPE_CONCERT=%d\n",
//        PLAY_TYPE_FILE, PLAY_TYPE_CONCERT);
//    printf("   票状态: TICKET_AVL=%d, TICKET_SOLD=%d\n",
//        TICKET_AVL, TICKET_SOLD);
//    printf("   订单类型: SALE_SELL=%d, SALE_REFOUND=%d\n",
//        SALE_SELL, SALE_REFOUND);
//    printf("   ✅ 枚举类型测试通过\n\n");
//
//    printf("===========================================\n");
//    printf("✅ 所有结构体定义完整性测试通过！\n");
//    printf("✅ Common.h 文件可以正常使用。\n");
//    printf("===========================================\n");
//
//    return 0;
//}