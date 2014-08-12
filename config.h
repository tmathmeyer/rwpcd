/*
 * Author: Ted Meyer @ 2014-05-07
 * config file for rwpcd
 * random wallpaper changing daemon
 * GPL v2
 *
 * contributors:
 *	-Joe Jevnik @ 2014-05-08
 *
 */


/*
 * the directory in which rwpcd should search for files
 */
#define _WALLPAPER_DIRECTORY "/path/to/your/wallpapers/folder"

/*
 * the fallback image to use in case shit hits the fan
 * as of now, this has no use, but I intend to make this
 * the default image if there is a problem with the 
 * wallpaper directory
 */
#define FALLBACK_IMAGE "/path/to/an/image"

/*
 * the style of tiling to use when applying the wallpaper
 *
 *	--bg-scale  : scale the image to screen size (the most common)
 *  --bg-center : if the image is too small, make equal
 *                size borders on all sides
 *  --bg-fill   : same as scale? not sure
 *  --bg-max    : nobody really knows honestly
 *  --bg-tile   : tiles the wallpaper if it doesnt fit
 *                 (USE THIS FOR MULTIPLE MONITORS)
 */
#define TILE_STYLE "--bg-scale"

/*
 * The interval to change wallpaper (in seconds)
 *
 */
#define DAEMON_DELAY 300

/*
 * The file to touch to stop the daemon
 */
#define STOPFILE_PATH "/path/to/a/file/for/killing/the/daemon"
