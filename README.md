# ACC

#### Run this command.
```
sudo apt install winbind
```

#### add this line at the end of  `/boot/config.txt` file to enable aux spi (spi 6).
```
dtoverlay=spi6-1cs,cs0_pin=2
```
