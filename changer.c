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
#include <sys/stat.h>

#include "config.h"
#include "flags.h"

typedef struct llname{
    struct llname* next;
    char* name;
} _LL_;



int file_exists(char* path);
void change_background(void);
void run_as_daemon(void);
void useage(void);
void version(void);



int main(int argc, char** argv) {
    srand(time(NULL));
    parse_flags(argv+1, argc-1);

    if (get_flag("-help") || get_flag("h")) {
        useage();
        return 0;
    }
    
    if (get_flag("v") || get_flag("-version")) {
        version();
        return 0;
    }

    if (get_flag("d") || get_flag("-daemon")) {
        run_as_daemon();
        //exit called in r_a_d() no need for return
    }


    change_background();

    return 0;
}


void useage(void) {
    puts("-h --help    : this information");
    puts("-v --version : version informaiton");
    puts("-d --daemon  : run as a daemon");
}

void version(void) {
    puts("rwpcd  ->  random wallpaper changer daemon");
    puts("version 1.0");
}




void run_as_daemon(void)
{
    pid_t pid, sid;
    pid = fork();

    //printf("%i\n", pid);

    // fork off for daemonisation
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // so we can read files
    umask(0);

    // get a unique sid
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // get out of any home-folder nonsense
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    // isolate oneself
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (!file_exists(STOPFILE_PATH)) {
        sleep(DAEMON_DELAY);
        change_background();
    }

    // delete the stopfile
    unlink(STOPFILE_PATH);
}








void change_background() {
    struct dirent* dir;
    _LL_* names;
    int size = 0;

    DIR* pwd = opendir(WALLPAPER_DIRECTORY);

    if (pwd) {
        while ((dir = readdir(pwd)) != NULL) {
            char* name = dir->d_name;
            if (*name != '.') {
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
    while(r-- > 0) {
        touse = touse -> next;
    }

    char* pname = WALLPAPER_DIRECTORY;
    char* fname = touse -> name;
    char* flags = TILE_STYLE;

    // meh, hopefully long enough
    char exec_command[2048];
    snprintf(exec_command, sizeof exec_command, "feh %s %s/%s", flags, pname, fname);
    system(exec_command);
}






int file_exists(char* path) {
    return fopen(path, "r") != NULL;
}
