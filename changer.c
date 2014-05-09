/*
 * Ted Meyer, Joe Jevnik
 * rwpcd
 * random wallpaper changing daemon
 * GPL v2
 *
 * 2014-05-08
 *
 */
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "flags.h"

typedef struct llname{
    struct llname* next;
    char* name;
} _LL_;



int file_exists(char* path);
void change_background(void);
void run_as_daemon(void);
void stop_daemon(void);
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
        // exit called in run_as_daemon() no need for return
    }

    if(get_flag("s") || get_flag("-stop")) {
        stop_daemon();
        return 0;
    }


    change_background();

    return 0;
}


void useage(){
    puts("-h --help    : this information");
    puts("-v --version : version informaiton");
    puts("-d --daemon  : run as a daemon");
    puts("-s --stop    : stop the daemon if it is running");
}

void version(){
    puts("rwpcd  ->  random wallpaper changer daemon");
    puts("version 1.1");
}

// Starts rwcd as in daemon mode.
void run_as_daemon(){
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

    // Reset to the FALLBACK_IMAGE.
    system("feh " TILE_STYLE " " FALLBACK_IMAGE);

    // delete the stopfile.
    unlink(STOPFILE_PATH);
}

// Changes the wallpaper to a random wallpaper in your WALLPAPER_DIRECTORY as
// defined in config.h
void change_background(){
    struct dirent* dir;
    _LL_* names;
    int size = 0;

    DIR* pwd = opendir(WALLPAPER_DIRECTORY);

    if (pwd) {
        while ((dir = readdir(pwd)) != NULL) {
            char* name = dir->d_name;
            if (*name != '.') {
                _LL_* newname = malloc(sizeof(struct llname));
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

    char* fname = touse -> name;

    char exec_command[BUFSIZ];
    snprintf(exec_command, sizeof(exec_command), "feh " TILE_STYLE " "
             WALLPAPER_DIRECTORY "/%s", fname);
    system(exec_command);
}

// Stops the daemon by creating and closing the STOPFILE
void stop_daemon(){
    close(creat(STOPFILE_PATH, O_RDWR));
}

// return: 0 if file does not exist, non-zero otherwise.
int file_exists(char* path){
    return !access(path,F_OK);
}
