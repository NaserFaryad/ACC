# ACC

### Startup
[Run GUI Programs on Startup With Autostart ](https://www.makeuseof.com/how-to-run-a-raspberry-pi-program-script-at-startup/)

1) First run this command.
   ```
   sudo nano /etc/xdg/autostart/display.desktop
   ```
2) In the .desktop file, add the following lines of code:
   replace "ProgrameName" with the name of the program and replace "/path/to/program" with the absolute path of the program
  ```
  [Desktop Entry]
  Name=ProgramName
  Exec=/path/to/program
  ```
### Samba
[Setup samba](https://www.makeuseof.com/set-up-network-shared-folder-ubuntu-with-samba/)

#### Run this command.
```
sudo apt install samba
sudo apt install winbind
```

change this line to the interface name you want to bind.
```
interfaces = eth0
```

### SPI
#### add this line at the end of  `/boot/config.txt` file to enable aux spi (spi 6).
```
dtoverlay=spi6-1cs,cs0_pin=2
```

### Disable blutooth
#### add this line at the end of  `/boot/config.txt`
```
# Disable Bluetooth
dtoverlay=disable-bt
```

[bcm2835 library](https://www.airspayce.com/mikem/bcm2835/)

#### Config NTP
[https://raspberrytips.com/time-sync-raspberry-pi/](https://raspberrytips.com/time-sync-raspberry-pi/)

### Qt dependencies
##### First uncomment this line:
```
#deb-src http://raspbian.raspberrypi.org/raspbian/ buster main contrib non-free rpi
```
##### Then run `qt.sh` script.
