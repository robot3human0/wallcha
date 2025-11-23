# Wallpaper Changer 2.0

## Description

A small C++ daemon for **XFCE** desktop environments that automatically changes the desktop wallpaper at a specified interval.
The program scans a directory (including subdirectories), collects all supported image files, and cycles through them in an infinite loop.

> This tool requires the xfce4-set-wallpaper script (from Linux Mint’s xapp tools) to apply wallpapers without directly dealing with xfconf-query.

### Supported image formats
* jpeg / jpg
* png
* bmp

## Dependencies
* xfce4-set-wallpaper<br>
  Source:<br>
https://github.com/linuxmint/xapp/blob/master/scripts/xfce4-set-wallpaper or take it from **resources** directory.

> the script needs to be installed manually.

## Usage
```
wallcha -d <path/to/directory> -c <minutes>
```

## Options
```
-d, --directory   Path to the wallpaper directory.
-c, --count       Interval between wallpaper changes in minutes.
                  If not provided, the default is 30 minutes.
-h, --help        Show help.
-v, --version     Show version.
```

## Examples

Run with directory and a custom interval:

```
wallpaperd -d ~/Pictures/wallpapers -c 10
```

Run with default interval (30 minutes):
```
wallpaperd -d ~/Pictures/wallpapers
```

### Behavior
* Recursively scans the specified directory and collects all valid image files.
* Validates arguments and directory existence.
* Forks into background and runs as a simple daemon.
* Changes wallpaper in a loop, cycling through the list.
* Does not reload files at runtime (planned for future versions).

### Notes
* Maximum allowed interval: 1440 minutes (24 hours).
* Minimum interval: 1 minute (0 sets default 30 minutes).
* The list of images is kept in memory for the whole runtime.
* Requires XFCE environment.

---
BTW, I put my favorite wallpapers collection to **wallpapers** directory, I choose 24 images special for you stranger =)