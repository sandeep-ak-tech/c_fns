### I2C Touchpad Project
#### User space to kernel driver to hardware (PSOC4) Flow
From userspace we make a call:

` cat /sys/bus/i2c/devices/i2c-1/psoc4-capsense/fw_ver `

is a valid user-space command — it's a standard way to read information from the kernel driver's sysfs interface.

refer [Basics of sysfs in device driver basics document](device_driver_basics.md#Basics-of-sysfs)

