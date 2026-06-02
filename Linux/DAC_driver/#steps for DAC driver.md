# steps for DAC driver

1. load the default device tree by updating 
'sudo nano /boot/firmware/config.txt'
'dtparam=i2s=on
dtoverlay=hifiberry-dac'

2. load the driver
3. # Basic test
speaker-test -D hw:0,0 -c 2 -t wav -l 2
