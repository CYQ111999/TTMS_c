//cyq写
//这个文件里用来存放公用的结构体和变量声明
// 大家在引用该文件后可以直接创建相关的结构体变量
//引用方式为#include "Common/Common.h"或者#include "Common.h"，详见测试代码
//测试代码为text_common.c
//未经讨论不许修改该文件的内容

#pragma once
#ifndef COMMON_H
#define COMMON_H

// 日期类型的定义
typedef struct {
    int year;   // 年
    int month;  // 月
    int day;    // 日
} ttms_date_t;

// 时间类型的定义
typedef struct {
    int hour;    // 时
    int minute;  // 分
    int second;  // 秒
} ttms_time_t;

// 注意：教材2.5节不要求单独的list_node_t定义
// 链表节点结构改为外嵌式，直接在各个链表节点中定义prev和next

// 系统用户类型的定义
typedef enum {
    USR_ANOMY = 0,   // 匿名类型,初始化账号时使用
    USR_CLERK = 1,   // 销售员类型
    USR_MANG = 2,    // 经理类型
    USR_ADMIN = 9    // 系统管理员类型
} account_type_t;

// 系统用户实体数据类型的定义
typedef struct {
    int id;                   // 用户id
    account_type_t type;      // 用户类型
    char username[30];        // 用户名
    char password[30];        // 用户密码
} account_t;

// 定义全局变量gl_CurUser存储系统用户信息
extern account_t gl_CurUser;

// 系统用户链表节点的定义（修改为教材规范格式）
typedef struct account_node {
    account_t data;           // 实体数据
    struct account_node* prev;  // 前向指针
    struct account_node* next;  // 后向指针
} account_list_node_t, * account_list_t;

// 演出厅实体数据类型的定义
typedef struct {
    int id;            // 演出厅ID
    char name[30];     // 演出厅名称
    int rowsCount;     // 座位行数
    int colsCount;     // 座位列数
    int seatsCount;    // 座位个数
} studio_t;

// 演出厅链表结点及演出厅链表类型的定义（修改）
typedef struct studio_node {
    studio_t data;           // 实体数据
    struct studio_node* prev;  // 前向指针
    struct studio_node* next;  // 后向指针
} studio_list_node_t, * studio_list_t;

// 座位状态类型的定义
typedef enum {
    SEAT_NONE = 0,     // 空位
    SEAT_GOOD = 1,     // 有座位
    SEAT_BROKEN = 9    // 损坏的座位
} seat_status_t;

// 座位实体数据类型的定义
typedef struct {
    int id;                   // 座位ID
    int roomID;               // 所在演出厅ID
    int row;                  // 座位行号
    int column;               // 座位列号
    seat_status_t status;     // 座位状态
} seat_t;

// 座位链表结点的定义（修改）
typedef struct seat_node {
    seat_t data;           // 实体数据
    struct seat_node* prev;  // 前向指针
    struct seat_node* next;  // 后向指针
} seat_list_node_t, * seat_list_t;

// 剧目类型的定义
typedef enum {
    PLAY_TYPE_FILE = 1,    // 文件
    PLAY_TYPE_OPEAR = 2,   // 歌剧
    PLAY_TYPE_CONCERT = 3  // 音乐会
} play_type_t;

// 剧目分级类型的定义
typedef enum {
    PLAY_RATE_CHILD = 1,    // 儿童
    PLAY_RATE_TEENAGE = 2,  // 青少年
    PLAY_RATE_ADULT = 3     // 成人
} play_rating_t;

// 剧目实体数据类型的定义
typedef struct {
    int id;                 // 剧目ID
    char name[31];          // 剧目名称
    play_type_t type;       // 剧目类型
    char area[9];           // 剧目出品地区
    play_rating_t rating;   // 剧目等级
    int duration;           // 时长，以分钟为单位
    ttms_date_t start_date; // 开始放映日期
    ttms_date_t end_date;   // 放映结束日期
    int price;              // 票价
} play_t;

// 剧目链表节点的定义（修改）
typedef struct play_node {
    play_t data;           // 实体数据
    struct play_node* prev;  // 前向指针
    struct play_node* next;  // 后向指针
} play_list_node_t, * play_list_t;

// 演出计划数据类型的定义
typedef struct {
    int id;                // 演出计划ID
    int play_id;           // 上映剧目ID
    int studio_id;         // 演出厅ID
    ttms_date_t date;      // 放映日期
    ttms_time_t time;      // 放映时间
    int seat_count;        // 座位数
} schedule_t;

// 演出计划链表节点的定义（修改）
typedef struct schedule_node {
    schedule_t data;           // 实体数据
    struct schedule_node* prev;  // 前向指针
    struct schedule_node* next;  // 后向指针
} schedule_list_node_t, * schedule_list_t;

// 票类型的定义
typedef enum {
    TICKET_AVL = 0,    // 待售
    TICKET_SOLD = 1,   // 已售
    TICKET_RESV = 9    // 预留
} ticket_status_t;

// 票实体数据类型的定义
typedef struct {
    int id;              // 票ID
    int schedule_id;     // 演出计划ID
    int seat_id;         // 座位ID
    int price;           // 票价
    ticket_status_t status;  // 票状态
} ticket_t;

// 票链表节点的定义（修改）
typedef struct ticket_node {
    ticket_t data;           // 实体数据
    struct ticket_node* prev;  // 前向指针
    struct ticket_node* next;  // 后向指针
} ticket_list_node_t, * ticket_list_t;

// 订单类型的定义
typedef enum {
    SALE_SELL = 1,      // 买票
    SALE_REFOUND = -1   // 退票
} sale_type_t;

// 订单信息实体数据类型的定义
typedef struct {
    long id;            // 销售记录ID
    int user_id;        // 售票员ID
    int ticket_id;      // 票ID
    ttms_date_t date;   // 处理日期
    ttms_time_t time;   // 处理时间
    int value;          // 票价
    sale_type_t type;   // 交易类型
} sale_t;

// 订单信息链表节点的定义（修改）
typedef struct sale_node {
    sale_t data;           // 实体数据
    struct sale_node* prev;  // 前向指针
    struct sale_node* next;  // 后向指针
} sale_list_node_t, * sale_list_t;

// 统计票房实体数据类型的定义
typedef struct {
    int play_id;             // 剧目编号
    char name[31];           // 剧目名称
    char area[9];            // 剧目区域
    int duration;            // 剧目播放时长
    long totaltickets;       // 剧目上座数量
    long sales;              // 剧目票房(销售额统计)
    int price;               // 剧目票价
    ttms_date_t start_date;  // 剧目上映日期
    ttms_date_t end_date;    // 剧目下映日期
} salesanalysis_t;

// 统计票房链表节点的定义（修改）
typedef struct salesanalysis_node {
    salesanalysis_t data;           // 实体数据
    struct salesanalysis_node* prev;  // 前向指针
    struct salesanalysis_node* next;  // 后向指针
} sales_analysis_list_node_t, * sales_analysis_list_t;

// 主键实体数据类型的定义（注意：按照教材例2.11，字段名应为name，不是entyName）
typedef struct {
    char name[41];  // 主键名称（教材例2.11使用name）
    long key;       // 主键键值
} entity_key_t;

// 主键链表结点的定义（修改）
typedef struct entity_key_node {
    entity_key_t data;           // 实体数据
    struct entity_key_node* prev;  // 前向指针
    struct entity_key_node* next;  // 后向指针
} entkey_list_node_t, * entkey_list_t;

#endif /* COMMON_H */