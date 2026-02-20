### I2C Touchpad Project
#### User space to kernel driver to hardware (PSOC4) Flow
From userspace we make a call:

` cat /sys/bus/i2c/devices/i2c-1/psoc4-capsense/fw_ver `

is a valid user-space command — it's a standard way to read information from the kernel driver's sysfs interface.

refer [Basics of sysfs in device driver basics document](device_driver_basics.md#Basics-of-sysfs)

Below sections explain what really happens in the user space, kernel and inside the driver.

### 1. USer Space : Execution of cat command
1. `cat` program is executed in the shell
2. `cat` performs three major system call
    * `open()` system call - attempts to open the file path `/sys/bus/i2c/devices/i2c-1/psoc4-capsense/fw_ver`
    * `read()` system call - after `open()` is success, `cat()` calls `read()` in a loop to fetch the file content until EOF
    * `write()` system call - `cat` writes this read data to stdout using `write()` system call

### 2. Transition to Kernel Space : systemcall (syscall) Entry
Inside the kernel space, the below process runs in process context and hence can sleep.
1. `open()/ read()` system call happens -> context switch from user mode to kernel mode -> the system call handler routes to appropriate function.
    * `Open()` - Kernel calls `do_sys_open()`
    * `read()` - Kernel calls `ksys_read()`
2. `do_sys_open()` -> resolves the path to open via VFS i.e `/sys/bus/i2c/devices/i2c-1/psoc4-capsense/` is the path of the device registered during probe. Then `fw_ver` is a sysfs attribute file. If the file exists, and if the open succeeds, the `open()` returns fd (file descriptor) to the user space.
3.   `read()` -> in kernel mode `ksys_read()` -> `vfs_read()` ->sysfs specific read handler.

### 3. Kernel Space : sysfs attribute handling
At Kernel space, we have the device driver taking care of the call backs. Each attributes of VFS gets a `.show` call back for `cat` system call i.e. for read and `store` call back for `echo` system call i.e. write.

```
static struct kobj_attribute version_attr = {
    .attr = { .name = "version", .mode = 0666 }, // Directly set mode without VERIFY_OCTAL_PERMISSIONS
    .show = version_read,
    .store = version_write,
};

// Sysfs read handler for firmware version
static ssize_t version_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u8 version;
    version = i2c_smbus_read_byte_data(psoc4_client, I2C_REG_VER); // Read firmware version
    return sprintf(buf, "PSOC4 FW ver is 0x%02x\n", version); // Return version as a string
}

// Sysfs write handler for firmware version (logs the input)
static ssize_t version_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    printk("i2c-psoc4-driver: Write to /sys/kernel/%s%s: %s\n", kobj->name, attr->attr.name, buf);
    return count;
}

```
Now when `read()` system call reaches sysfs, sysfs calls the attribute's `show()` function. The `show()` function is where the driver specific action happens i.e.it fetches the firmware version by reading the fw version from PSOC4 device via I2C.The version returned from PSOC4 is number and is converted to string by `sprintf`. The `sprintf` returns the length of the string using return. If `read()` is successful, sysfs performs the `.store()` operation and the buf content is written to the `/sys/kernel/i2c-psoc4/version/` i.e. the buf content is sent to user space.

### 4. Kernel Space to Hardware : I2C Communication to PSOC4 Device
Inside the .show() function i.e. version read function of the device driver, we make I2C subsystem calls.The address of the I2C  

``` 
i2c_smbus_read_byte_data(psoc4_client, I2C_REG_VER); // Read firmware version`
```
where
```
#define I2C_REG_VER 0x00      // Firmware version register
```
`I2C_REG_VER` is firmware version register i.e. address of the register where the version is stored i.e in the PSOC4 code (device code), the version information is stored inside an array remap. at the 0th index of this array, we have the firmware version. So the address of the register where this firmware version is stored is nothing but the index of the array.
```
// Register map array
uint8_t registerMap[REGISTER_COUNT] = {0xBB, 0x00, 0x00, 0x00, 0x00};

registerMap[I2C_REG_VER] = registerMap[0] = firmware version = 0xBB

```
Now as per the I2C protocol, lets say a different register i.e. index = 3, Master wants to write 0xA5 to register 3:
* Master (Raspberry pi) sends data in the sequence => START + SLA + W + 0X03 + 0XA5 + STOP
* I2C write to slave buffer => i2cWriteBuffer[] = {0x03,0xA5}
* code first checks reg=3<5 and reg!=0 (REG_DEVICE_STATUS) (regs = i2cWriteBuffer[0]=3) -> registerMap[3] = 0xA5
* Then registerMap[3] (=0xA5) is copied to i2cReadBuffer[0] (prepares for possible next read)

```
    if (0UL != (CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT & event))
    {
        // Master completed a write; update the corresponding register
        if (i2cWriteBuffer[0] < REGISTER_COUNT) // uint8_t i2cWriteBuffer[2]; // Buffer for master write (register index + value) i.e i2cWriteBuffer[0] = register index 
        {
            if (i2cWriteBuffer[0] != REG_DEVICE_STATUS)// command is not to read fw version, but to write to register (registerMap) with index i2cWriteBuffer[0] = 3
            {
                registerMap[i2cWriteBuffer[0]] = i2cWriteBuffer[1];//i2cWriteBuffer[0]=3 index,registerMap[3]=value=0xA5,i.e. new value is available at i2cWriteBuffer[1]
            }
            i2cReadBuffer[0] = registerMap[i2cWriteBuffer[0]];  //i2cReadBuffer[0] = registerMap[3] = updated value of the register is read buffer for next I2C read operation
        }
        Cy_SCB_I2C_SlaveConfigWriteBuf(CYBSP_I2C_HW, i2cWriteBuffer, sizeof(i2cWriteBuffer), &i2cContext);
    }
```

Master wants to read register 2:

* Master → START + SLA+W + 0x02 + repeated START + SLA+R
* Slave receives 1 byte → WR_CMPLT → sees reg2 i.e. registerMap[2], does not write (because only 1 byte), but copies current registerMap[2] → i2cReadBuffer[0]
* Master reads 1 byte → gets correct value

```
    if (0UL != (CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT & event))
    {
        // Master completed a read; prepare the next read value
        if (i2cWriteBuffer[0] < REGISTER_COUNT)
        {
            i2cReadBuffer[0] = registerMap[i2cWriteBuffer[0]];
        }
        else
        {
            i2cReadBuffer[0] = INVALID_REGISTER; // Invalid register
        }
        Cy_SCB_I2C_SlaveConfigReadBuf(CYBSP_I2C_HW, i2cReadBuffer, sizeof(i2cReadBuffer), &i2cContext);
    }
```

### 5. Return Path: Hardware to Kernel to User Space
* PSOC4 (I2C Slave) sends data bytes over I2C -> The I2C master (raspberry pi) receives the data-> stores the data into the buffer in `show()`
* The device driver formats the string -> sysfs copies this data to VFS read buffer
* VFS vfs_read() completes -> syscall returns the data length to user space
* context switch back to user mode
* `cat` receives data in its buffer -> prints it on stdout
* `cat` calls `close()` -> kernel `vfs_close()`