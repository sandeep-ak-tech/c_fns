####  Hello World vs Character Device driver – The Key Difference

| Aspect | Hello World Kernel Module | Character Device Driver with Major/Minor Numbers |
| --- | --- | --- |
| Purpose | Demonstrate basic module loading/unloading + printk | Provide an actual interface to communicate with hardware or emulate a device |
| What it does | Prints "Hello world" on insmod, "Goodbye" on rmmod | Registers itself as a device → user programs can open/read/write/ioctl it via /dev/mydev |
| Creates a device node? | No — no /dev/ entry at all | Yes — creates /dev/mydev (or many) with major + minor |
| User space interaction | None (only visible in dmesg) | Full — open(), read(), write(), ioctl(), close() via file operations |
| Major & Minor numbers | Not used / not registered | Explicitly registered (static or dynamic) → identifies the driver and instance |
| Registers with kernel | Only module_init() / module_exit() | Also registers char device (register_chrdev(), cdev_add(), etc.) |
| file_operations struct | Not present | Required — defines .open, .read, .write, .ioctl, etc. |
| Typical visibility | lsmod shows module, dmesg shows messages | /dev/ node appears, ls -l /dev/mydev shows c major,minor |
| Example use case | Learning module basics, debug prints | Serial port, I2C sensor, GPIO, custom hardware, virtual device |
| Code complexity | Very simple (~10 lines) | Much more (~100+ lines minimum) |

|Location                  |What gets created                                           |Created by whom?        |Exact kernel moment / function call                                                 |Visible to user almost immediately after?  |Typical path example                  |
|--------------------------|------------------------------------------------------------|------------------------|------------------------------------------------------------------------------------|-------------------------------------------|--------------------------------------|
|**/sys/class/xxx          |The class directory itself                                  |Kernel                  |class_create(THIS_MODULE, "myclass")                                                |Yes — right after the call succeeds        |/sys/class/myclass/                   |
|/sys/class/xxx/yourdevice/|Device subdirectory + attributes (dev, uevent, power/, etc.)|Kernel                  |device_create(cls, NULL, devt, NULL, "mydevice")                                    |Yes — right after the call succeeds        |/sys/class/myclass/mydevice/          |
|/sys/devices/...          |The "real" device location (bus topology)                   |Kernel (usually earlier)|During device registration (device_register(), platform_device_add(), probe(), etc.)|Yes                                        |/sys/devices/platform/.../mydevice/   |
|/dev/mydevice             |The actual character/block device node                      |udev (userspace daemon) |When udev receives the uevent sent by device_create() → then udev calls mknod()     |Usually < 100 ms later (but not guaranteed)|/dev/mydevice (major:minor from dev_t)|

			

####  Quick Summary – The Key Difference

* Hello World module = just a loadable piece of kernel code that prints messages (no user-space device interface)
* Device driver with major/minor = registers as a character device → gets a device file in /dev/ → user-space can actually open/read/write to it → kernel calls your callback functions (.open, .read, etc.) when that happens

The major number basically says "this is handled by my driver", and the minor number says "which specific instance/sub-device" (if the driver supports multiple devices).

### MAJOR and MINOR Number
* Its applicable only for char/ block devices
* MAJOR Number = Identifies the driver for the device - i.. identifies which code handles open/read/write/ioctl etc.. for this device. 
* MINOR Number - 
    * Inside the driver code Minor Number helps to distinguish different physical devices controlled by the same driver eg: ttyso,ttys1,ttys2 all share same MAJOR number but different MINOR number.
    * Inside the driver code Minor Number helps to distinguish different instances or sub-devices eg:- multiple serial ports or multiple GPIOs
    * Inside the driver code Minor Number helps to make different configurations for different devices.
* Parameter Count - Number of consecutive minors (devices) to reserve in one go. 

```
#define MY_MAJOR  240
#define MY_FIRST_MINOR 0
#define NUM_DEVICES 4

dev_t dev_base;

dev_base = MKDEV(MY_MAJOR, MY_FIRST_MINOR);

if (register_chrdev_region(dev_base, NUM_DEVICES, "my_multi_dev") < 0) {
    pr_err("Failed to register device region\n");
    return -EBUSY;
}

```
Explanation : It reserves MAJOR NUmber 240 and minors=0,1,2,3.
Commands: `cat /proc/devices ` will lists all devices and the Major number reserved with the devices. 
Still there is no entry in `/dev`. For that you need to 
    1. create class using 
    `my_class = class_create(THIS_MODULE, "my_class"); `
    2. Add device using 
    `my_dev = device_create(my_class, NULL, dev_base, NULL, "mydevice");`
    3. Sends a Uevent to user space. The udev sees the uevent and automatically creates /dev/mydevice with correct major/minor number

* uevent - a notification message sent from the Linux kernel to userspace whenever something important happens with a device or kernel object. Eg: creating or removing a device node in /dev
* udev - udev is the userspace daemon (background proram) that listens to kernel uevents and automatically creates, configures, and removes device files in /dev

Now `ls -l /dev` -> lists all devices with major and minor number

#### Linux Device Driver Structure - Character Driver
1. alloc_chrdev_region() - To allocate a Major and Minor number for the device
2. cdev_init() and cdev_add() - Register the character device structure with the kernel
    * connects the file operations to the major and minor number
3. class_create() - To create a class in sysfs i.e.`/sys/class/my_class` for the device
    * Groups similar device
4. device_create() - To create the device instance under the class + trigger /dev node creation
    * sends uevent to userspace
    * causes udev to automatically ceate the actual /dev/node with correct major/ minor/ permissions
    * It adds device instance to sysfs (/sys/class/my_class/mydeviceX)
#### Filesystem Hierarchy Standard (FHS)
The directory structure you're referring to in Linux (with `/`, `/bin`, `/etc`, `/home`, `/proc`, `/sys`, `/dev`, `/usr`, etc.) is officially called the Filesystem Hierarchy Standard — most commonly abbreviated as FHS.

| Feature              | /proc                                 | /sys (sysfs)                              | /dev                                      |
|----------------------|---------------------------------------|-------------------------------------------|-------------------------------------------|
| Original purpose     | Process info                          | Clean structured device & driver info     | Access to hardware & special devices      |
| Organization         | Flat + messy (everything dumped here) | Very hierarchical (bus → device → driver) | Mostly flat (udev can create subdirs)     |
| Main usage today     | Processes + legacy kernel parameters  | Modern hardware/driver control            | Talking to devices                        |
| Write support        | Limited (mostly /proc/sys/)           | Very common (tunables & control)          | Almost never                              |


Ex:- `cat /proc/cpuinfo`          - CPU model, cores, flags
      `tree -L 2 /sys/bus/i2c/devices/` - See I2C devices tree

```
/dev → "I want to read/write hardware or special device"
/proc → "I want to know what the kernel / processes are doing right now" (classic info)
/sys  → "I want to control or get detailed structured info about devices/drivers" (modern way)
```

#### Basics of sysfs
/sys is a virtual filesystem (sysfs) mounted in user space.

sysfs is one of the most important virtual filesystems in the Linux kernel. It is mounted at /sys and acts as a clean, structured interface that lets user-space programs (and you via shell commands) view and sometimes change information about hardware devices, kernel subsystems, drivers, power management, and more — all without needing special tools or recompiling the kernel.

Virtual File System means it exists only in Ram and no data is stored on the disk i.e. power goes => the file system goes.

Every time kernel discovers a device (like I2C device - PSOC4 device here), it creates directories and files in /sys. These files usually contain one ASCII value and is easy to read with `cat` and write with `echo`.

* `/sys/module/<module_name>/` = this entry always created on load i.e. on insmod — it's about the module itself (metadata, parameters (mod parameters and callbacks), dependencies) - happens automatically
* `/dev/something node (character or block device)` = only created if the module is a device driver that registers a device using the driver model (and udev picks it up) - module code explicitly registers a device during initialization - device driver that exposes I/O via a device node

#### Main purpose of sysfs
1. To export kernel or device information (like firmware version, driver name etc..) to user space
2. Allow configuration/ control from user space like change brighness , reset device etc..
3. Provide uniform way for tools like udev, systemd, libudev, hwinfo etc..

#### Directory Structure in /sys

![sysfs structure](sysfs_structure.png)

#### How sysfs Works Internally (High-Level for Device Drivers)
1. When a driver registers a device using `probe()` function, 
    * it make a `device_register()` function that creates `/sys/devices/...` or `/sys/bus/...`
    * then adds attributes eg: 
    `static DEVICE_ATTR_RO(fw_ver);   // read-only attribute`
    * Each attributes gets a `.show` call back for `cat` system call i.e. for read and `store` call back for `echo` system call i.e. write.
2. So when ` cat /sys/bus/i2c/devices/i2c-1/psoc4-capsense/fw_ver ` is called, 
    * VFS i.e. sysfs finds the attribute and calls the .show() function
    * Driver reads the values via I2C from PSOC4  Hardware device
    * Formats it as a string and kernel copies it to user space.

```markdown  
// Global variables for the I2C client and kobject
static struct i2c_client *psoc4_client;
static struct kobject *my_kobj;

static int psoc4_probe(struct i2c_client *client)
{
    printk(KERN_INFO "i2c-psoc4-driver: Probing device\n");

    // Check if the device address is valid
    if(client->addr != 0x08)
    {
        printk(KERN_ERR "i2c-psoc4-driver: Invalid address: 0x%02x\n", client->addr);
        return -1;
    }

    psoc4_client = client;

    // Create a kobject for sysfs entries
    my_kobj = kobject_create_and_add("i2c-psoc4", kernel_kobj);
    if (!my_kobj)
    {
        printk(KERN_ERR "i2c-psoc4-driver: Failed to create /sys/kernel/i2c-psoc4\n");
        return -ENOMEM;
    }

    // Create sysfs file for firmware version
    if(sysfs_create_file(my_kobj, &version_attr.attr))
    {
        printk(KERN_ERR "i2c-psoc4-driver: Failed to create /sys/kernel/i2c-psoc4/version\n");
        kobject_put(my_kobj); // Cleanup kobject
        return -ENOMEM; // Return error if sysfs creation fails
    }

    // Create sysfs file for LED blinking control
    if(sysfs_create_file(my_kobj, &led_blink_attr.attr))
    {
        printk(KERN_ERR "i2c-psoc4-driver: Failed to create /sys/kernel/i2c-psoc4/led_blink\n");
        sysfs_remove_file(my_kobj, &version_attr.attr); // Cleanup version file
        kobject_put(my_kobj); // Cleanup kobject
        return -ENOMEM; // Return error if sysfs creation fails
    }

    return 0;
}
```  

#### Bus Device Driver vs Character Device Driver
* Character driver = "I give user space a file-like interface to talk to my device using read()/write()/ioctl()"
* Bus driver = "I discover devices, match them with the right drivers, manage resources, handle hotplug, etc."

|Aspect                     |Character Driver                                                                              |Bus Driver                                                                                         |
|---------------------------|----------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------|
|Category                   |Device access type (how data is transferred)                                                  |Device discovery & management type                                                                 |
|Main purpose               |Provides file-like interface (/dev/xxx) to user space for byte-stream devices                 |Manages a whole family of devices (discovery, probing, resource allocation, power management, etc.)|
|Data transfer style        |Stream of bytes (no fixed block size), usually unbuffered by kernel                           |Not directly about data transfer - it enables other drivers                                      |
|File operations            |Implements file_operations (open, read, write, ioctl, etc.)                                   |Usually does not implement file_operations                                                         |
|Examples                   |/dev/ttyS0 (serial), /dev/input/mouse0, /dev/mem, GPIO driver, I2C client driver, most sensors|PCI bus driver, USB bus driver, I2C bus driver (adapter), SPI bus driver, platform bus             |
|Major number               |Yes (major/minor number pair)                                                                 |No (bus drivers don't create /dev/ entries themselves)                                             |
|Typical implementation file|file_operations struct                                                                        |struct bus_type, probe(), remove(), match() functions                                              |
|Layer in kernel            |Lower-level (end-device driver)                                                               |Higher-level (bus/subsystem infrastructure)                                                        |
|Can it be both?            |A driver can be a character driver and part of a bus (very common)                            |--                                                                                               |

#### Linux Device Driver Structure - Character Driver
__bus_register() -> driver_register() -> device_register()__
1. Define struct bus_type and provide functions for the bus
    * .match function - at least this function is needed. optional functions = .probe, .remove, .uevent, .shutdown etc...
    * The structure represents bus itself like I2C bus.
2. bus_register() - register bus type with kernel 
    * creates /sys/bus/my_bus/ and also creates /sys/bus/my_bus/devices/ and /sys/bus/my_bus/drivers/
    * From now kernel knows this bus exists and can hold devices and drivers
3. Define struct type device_driver (the bus driver) - parameters are as below
    * .name = .my_sensor_driver, .bus = links driver to bus
    * .probe = called when binding is success, .remove=called on unbind/ rmmod
4. driver_register() - registers driver to the bus and add it to /sys/bus/my_bus/drivers/my_sensor_driver/
    * Kernel tries to match it against all already registered devices on this bus
        * Calls bus_type.match() for each device
        * if match succeeds calls driver.probe() 
5. Define struct device - a device on the bus - parameters
    * .init_name = "my_sensor_0", .bus = to link device to bus,.release
6. device_register() or device_add() - registers device on to the bus
    * add it to /sys/bus/my_bus/devices/my_sensor_0/
    * Kernel tries to match it against all already registered drivers on this bus
        * Calls bus_type.match() for each driver
        * if match succeeds -> call driver.probe(device)
        * sends uevent to userspace
7. class_create() + device_create()
    * creates class /sys/class/my_bus_class/  
    * device_create() - creates device node /dev/my_node_0/ and also creates symlink in /sys/class/my_bus_class/my_node_0

The goal: The kernel wants to automatically bind (probe) a driver to a device as soon as both exist on the same bus, without caring which one was registered first.

* When you register a driver (point 4): The kernel says, "Hey, a new driver just showed up. Let me check if any existing devices on this bus match it." It loops over all devices already on the bus, calls `.match(device, driver)` for each, and if yes, binds by calling driver`.probe(device)`.
* When you register a device (point 6): The kernel says, "Hey, a new device just showed up. Let me check if any existing drivers on this bus match it." It loops over all drivers already on the bus, calls `.match(device, driver)` for each, and if yes, binds by calling driver`.probe(device)`.