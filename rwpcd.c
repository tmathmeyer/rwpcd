/*
 * Author: Ted Meyer @ 2014-05-07
 * random wallpaper changing daemon
 * GPL v2
 *
 * contributors:
 *  -Joe Jevnik @ 2014-05-08
 *  -Nilesh Patel @ 2014-05-09
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
void usage(char* name);
void version(void);

static int TIMEOUT = 0;


int main(int argc, char** argv) {
    srand(time(NULL));
    char* arg_flags[] = {"t", "-timeout"};
    set_flags_with_args(arg_flags, 2);
    parse_flags(argv+1, argc-1);


    if (get_flag("-help") || get_flag("h")) {
        usage(argv[0]);
        return 0;
    }

    if (get_flag("v") || get_flag("-version")) {
        version();
        return 0;
    }

    if (get_flag("d") || get_flag("-daemon")) {
        char* l = get_flag("t");
        char* s = get_flag("-timeout");
        TIMEOUT = l==NULL?s==NULL?0:atoi(s):atoi(l);
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

/*
 * prints usage information
 */
void usage(char* name){
    printf("%s [OPTIONS]\n", name);
    puts("\t-h --help    : this information");
    puts("\t-v --version : version informaiton");
    puts("\t-d --daemon  : run as a daemon");
    puts("\t-t --timeout : the timeout to use if run as daemon");
    puts("\t-s --stop    : stop the daemon if it is running");
}

/*
 * prints version information
 */
void version(){
    puts("rwpcd  ->  random wallpaper changer daemon");
    puts("version 1.1");
}

/*
 * Starts rwpcd as in daemon mode.
 */
void run_as_daemon(){
    pid_t pid, sid;
    pid = fork();


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
        sleep(TIMEOUT?TIMEOUT:DAEMON_DELAY);
        change_background();
    }

    // delete the stopfile.
    unlink(STOPFILE_PATH);
    exit(EXIT_SUCCESS);
}

/*
 * Changes the wallpaper to a random wallpaper in your WALLPAPER_DIRECTORY,
 * as defined in config.h
 */
void change_background() {
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
        closedir(pwd);
    } else {
        puts("please fix your config.h and rebuild");
        exit(EXIT_FAILURE);
    }

    char* fname;
    if (size)
    {
        int r = rand() % size;
        _LL_* touse = names;
        while(r-- > 0) {
            touse = touse -> next;
        }
    } else {
        fname = FALLBACK_IMAGE;
    }

    fname = touse -> name;

    char exec[2048];
    snprintf(exec, sizeof exec,"feh %s %s/%s",
             TILE_STYLE, WALLPAPER_DIRECTORY, fname);
    system(exec);
}

/*
 * Stops the daemon by creating and closing the STOPFILE
 */
void stop_daemon() {
    close(creat(STOPFILE_PATH, O_RDWR));
}

/*
 * return: 0 if file does not exist, non-zero otherwise.
 */
int file_exists(char* path) {
    return !access(path,F_OK);
}
