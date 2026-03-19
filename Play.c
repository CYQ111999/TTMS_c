#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Play.h"
#include "Play_Persist.h"
#include "EntityKey_Persist.h"
#include "List.h"

int Play_Srv_FetchAll(play_list_t list) {
    return Play_Perst_SelectAll(list);
}

int Play_Srv_Add(play_t* data) {
    return Play_Perst_Insert(data);
}

int Play_Srv_Modify(const play_t* data) {
    return Play_Perst_Update(data);
}

int Play_Srv_DeleteByID(int id) {
    return Play_Perst_RemByID(id);
}

int Play_Srv_FetchByID(int id, play_t* buf) {
    return Play_Perst_SelectByID(id, buf);
}