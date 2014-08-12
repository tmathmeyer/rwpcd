#RWPCD

random wallpaper changing daemon is a lightweight daemon that changes wallpaper
using feh (https://github.com/derf/feh).


## Usage
| flag | long flag | description|
|------|-----------|------------|
| -h   | --help        | show the help dialog (this) |
| -v   | --version     | version informaiton |
| -d   | --daemon      | run as a daemon with timeout from config.h |
| -t   | --timeout     | the timeout to use if run as daemon |
| -s   | --stop        | stop the daemon if it is running |
| -p   | --path        | provide a path other than the one in config.h |
| -i   | --interactive | select an image in interactive mode |

##Configuration
| Variable | What it does |
|----------|--------------|
| _WALLPAPER_DIRECTORY | the directory that rwpcd looks on your computer for the images. It has to be a folder |
| FALLBACK_IMAGE | If something screws up, or you kill the daemon, the wallpaper will be set to this |
| TILE_STYLE | the argument for feh |
| DAEMON_DELAY | how long to sleep in seconds before changing the wallpaper |
| STOPFILE_PATH | this is how the daemon gets killed; put it somewhere in your file system you have rw permissions |
| PICKUP_CHANGES | whether the daemon should refresh it's internal representation of the directory every time it changes wallpaper |
