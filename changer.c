/*
 * Ted Meyer
 * config file for rwpcd
 * random wallpaper changing daemon
 * GPL v2
 *
 * 2014-05-07
 *
 */


#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"

typedef struct llname{
    struct llname* next;
    char* name;
} _LL_;



int main(int argc, char** argv){
    srand(time(NULL));
    struct dirent* dir;
    _LL_* names;
    int size = 0;
    

    DIR* pwd = opendir(WALLPAPER_DIRECTORY);
    
    if (pwd){
        while ((dir = readdir(pwd)) != NULL){
            char* name = dir->d_name;
            if (*name != '.'){
                _LL_* newname = (_LL_*)malloc(sizeof(struct llname));
                newname -> next = names;
                newname -> name = name;
                names = newname;
                size ++;
            }
        }
    }

    int r = rand() % size;
    _LL_* touse = names;
    while(r-- > 0){
        touse = touse -> next;
    }

    char* pname = WALLPAPER_DIRECTORY;
    char* fname = touse -> name;
    char* flags = TILE_STYLE;

    // meh, hopefully long enough
    char exec_command[2048];
    snprintf(exec_command, sizeof exec_command, "feh %s %s/%s", flags, pname, fname);
    system(exec_command);

    return 0;
}
