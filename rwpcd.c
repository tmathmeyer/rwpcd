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
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curses.h>
#include <menu.h>

#include "config.h"
#include "flags.h"

typedef struct llname{
    struct llname* next;
    char* name;
} name_list;



int file_exists(char* path);
void free_file_name_list(name_list * list);
void _change_background(const char * filename);
void change_background(void);
void run_as_daemon(void);
void stop_daemon(void);
void usage(char* name);
void do_visual_mode();
void version(void);
name_list * get_file_list(int * size, char * directory);
MENU * generate_menu (name_list * list, int length);

static int TIMEOUT = 0;
static char* WALLPAPER_DIRECTORY = _WALLPAPER_DIRECTORY;

int main(int argc, char** argv) {
    srand(time(NULL));
    char* arg_flags[] = {"t", "-timeout", "p", "-path"};
    set_flags_with_args(arg_flags, 4);
    parse_flags(argv+1, argc-1);


    if (get_flag("-help") || get_flag("h")) {
        usage(argv[0]);
        return 0;
    }

    char * path = get_flag("-path");
           path = path==NULL?get_flag("p"):path;
    if (path != NULL) {
        WALLPAPER_DIRECTORY=path;
    }


    if (get_flag("v") || get_flag("-version")) {
        version();
        return 0;
    }

    if (get_flag("i") || get_flag("-interactive")) {
        do_visual_mode();
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
    puts("\t-h --help        : this information");
    puts("\t-v --version     : version informaiton");
    puts("\t-d --daemon      : run as a daemon with default timeout");
    puts("\t-t --timeout     : the timeout to use if run as daemon");
    puts("\t-s --stop        : stop the daemon if it is running");
    puts("\t-p --path        : provide a path other than the config");
    puts("\t-i --interactive : select an image in interactive mode");
}

/*
 * prints version information
 */
void version(){
    puts("rwpcd  ->  random wallpaper changer daemon");
    puts("version 2.0");
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
        change_background();
        sleep(TIMEOUT?TIMEOUT:DAEMON_DELAY);
    }

    // delete the stopfile.
    unlink(STOPFILE_PATH);
    exit(EXIT_SUCCESS);
}

/*
 * Changes the wallpaper to a random wallpaper in your WALLPAPER_DIRECTORY,
 * as defined in config.h
 */
name_list * names;
void change_background() {
    int list_size;
    if (names == NULL || PICKUP_CHANGES) {
        names = get_file_list(&list_size, WALLPAPER_DIRECTORY);
    }

    if (list_size == 0) {
        puts("please fix your config.h and rebuild");
        exit(EXIT_FAILURE);
    }

    int random_name = rand() % list_size;
    name_list * selector = names;
    while(random_name --> 0) {
        selector = selector -> next;
    }

    if (PICKUP_CHANGES) {
        free_file_name_list(names);
    }

    _change_background(selector -> name);
}

/*
 * change the background to any image supplied
 */
void _change_background(const char * filename) {
    char exec[2048];
    snprintf(exec, sizeof exec,"feh %s %s/%s",
             TILE_STYLE, WALLPAPER_DIRECTORY, filename);
    system(exec);
}


/*
 * get the files that are in the image directory
 */
name_list * get_file_list(int * size, char * directory) {
    name_list * names = NULL;
    struct dirent * dir;
    DIR * pwd = opendir(directory);
    * size = 0;

    if (pwd) {
        while((dir = readdir(pwd)) != NULL) {
            char* name = dir->d_name;
            if (*name != '.') {
                name_list * newname = malloc(sizeof(struct llname));
                newname -> name = malloc(sizeof(name));
                memcpy(newname -> name, name, strlen(name)+1);
                newname -> next = names;
                names = newname;
                (* size) ++;
            }
        }
        closedir(pwd);
    }
    return names;
}

/*
 * Free the list of files generated (can be toggled on in config)
 */
void free_file_name_list(name_list * list) {
    while(list != NULL) {
        name_list * next = list -> next;
        free(list);
        free(list -> name);
        list = next;
    }
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

/*
 * Generate an NCurses-Menu array
 */
MENU * generate_menu (name_list * list, int length) {
    ITEM ** items = calloc(length+1, sizeof(ITEM *));
    items[length] = NULL;

    int i;
    while(list != NULL) {
        items[i] = new_item(list -> name, "");
        list = list -> next;
        i++;
    }
    return new_menu(items);
}

/*
 * enter the NCurses visual image select mode
 */
void do_visual_mode() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    int n_choices=0;
    int c;

    name_list * files = get_file_list(&n_choices, WALLPAPER_DIRECTORY);

    MENU * my_menu = generate_menu(files, n_choices);
    mvprintw(LINES - 2, 0, "Enter (Return) to Exit");
    post_menu(my_menu);
    refresh();

    while((c = getch()) != 10) {
        switch(c) {
            case KEY_DOWN:
                menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(my_menu, REQ_UP_ITEM);
                break;
        }
        move(20, 0);
        clrtoeol();
        mvprintw(20, 0, "Item selected is : %s",
                item_name(current_item(my_menu)));
        pos_menu_cursor(my_menu);
        _change_background(item_name(current_item(my_menu)));
    }

    free_menu(my_menu);
    endwin();
}
