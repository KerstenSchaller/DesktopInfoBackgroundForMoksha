# DesktopInfoBackgroundForMoksha
Small hacky application which sets the background to black and embeds info text about nordvpn connection.
It updates the screen every 20 seconds or if the vpn status changes.
Tested on Bodhi Linux

## Build
cmake . -bbuild
cd build
make


## Install:
Copy the executable "DesktopbackgroundInfoScreen" and the script "setWallpaper" to some place of your choice.
Write some startup script for you system calling the executable with full path like this
<full path>/DesktopbackgroundInfoScreen <path temp file>
where <path temp file> is a folder where a an image can be saved which is used as a wallpaper. 

Install and enable module slideshow.



## Depenencies:
1. notify-send , expected path /usr/bin/notify-send
installable with
sudo apt install libnotify-bin

2. On Bodhi Linux it works in conjunction with module slide show.
   The programm itself updated the wallpaper sources while slideshow is responsible for updating the screen.
