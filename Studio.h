#pragma once
//Studio.h
//zyp
#ifndef STUDIO_UI_H
#define STUDIO_UI_H

//函数功能：用于添加一个新演出厅数据
int Studio_Srv_Add(studio_t* data);

//函数功能：用于修改一个演出厅数据
int Studio_Srv_Modify(const studio_t* data);

//函数功能：用于删除一个演出厅的数据
int Studio_Srv_DeleteByID(int id);

//函数功能：根据演出厅ID获取一个演出厅的数据
int Studio_Srv_FetchByID(int ID, studio_t* buf);

//函数功能：获取所有演出厅的数据，形成以 list 为头指针的演出厅链表
int Studio_Srv_FetchAll(studio_list_t* list);

//函数功能：根据演出厅ID和链表头指针获取该链表上相应演出厅的数据
studio_list_node_t* Studio_Srv_FindByID(studio_list_t list, int ID);

#endif