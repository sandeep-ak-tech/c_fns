#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/i2c.h>

// Define I2C register addresses
#define I2C_REG_VER 0x00      // Firmware version register
#define I2C_REG_CUSTOM 0x01   // Custom register (not used in this code)
#define I2C_REG_LED 0x04      // LED control register

// Global variables for the I2C client and kobject
static struct i2c_client *psoc4_client;
static struct kobject *my_kobj;

// Function prototypes for probe and remove
static int psoc4_probe(struct i2c_client *client);
static void psoc4_remove(struct i2c_client *client);

// Function prototypes for sysfs attribute handlers
static ssize_t version_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t version_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
static ssize_t led_blink_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t led_blink_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);


// static struct kobj_attribute version_attr = __ATTR(version, 0666, version_read, version_write);
// static struct kobj_attribute led_blink_attr = __ATTR(led_blink, 0666, led_blink_read, led_blink_write);

static struct kobj_attribute version_attr = {
    .attr = { .name = "version", .mode = 0666 }, // Directly set mode without VERIFY_OCTAL_PERMISSIONS
    .show = version_read,
    .store = version_write,
};

static struct kobj_attribute led_blink_attr = {
    .attr = { .name = "led_blink", .mode = 0666 }, // Directly set mode without VERIFY_OCTAL_PERMISSIONS
    .show = led_blink_read,
    .store = led_blink_write,
};

// Define compatible device IDs for device tree matching
static struct of_device_id my_driver_ids[] = {
    { .compatible = "i2c-psoc4,my_device" },
    {},
};
MODULE_DEVICE_TABLE(of, my_driver_ids);

// Define I2C device IDs for driver matching
static struct i2c_device_id my_i2c_ids[] = {
    { "i2c-psoc4", 0 },
    {},
};
MODULE_DEVICE_TABLE(i2c, my_i2c_ids);

// Define the I2C driver structure
static struct i2c_driver my_driver = {
    .probe = psoc4_probe, // Probe function
    .remove = psoc4_remove, // Remove function
    .id_table = my_i2c_ids, // I2C device ID table
    .driver = {
        .name = "i2c-psoc4-driver", // Driver name
        .of_match_table = my_driver_ids, // Device tree match table
    },
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

// Sysfs read handler for LED blinking state
static ssize_t led_blink_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u8 is_led_blinking;
    is_led_blinking = i2c_smbus_read_byte_data(psoc4_client, I2C_REG_LED); // Read LED state
    return sprintf(buf, "LED blinking state is 0x%02x\n", is_led_blinking); // Return state as a string
}

// Sysfs write handler for LED blinking state
static ssize_t led_blink_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int ret;
    u8 led_state = 0;

    // Parse the input buffer for LED state
    if (sscanf(buf, "%hhu", &led_state) != 1)
    {
        printk(KERN_ERR "i2c-psoc4-driver: Invalid input for LED state\n");
        return -EINVAL;
    }

    // Write the LED state to the I2C register
    ret = i2c_smbus_write_byte_data(psoc4_client, I2C_REG_LED, led_state);
    if (ret < 0)
    {
        printk(KERN_ERR "i2c-psoc4-driver: Failed to write to LED register\n");
        return ret;
    }

    printk(KERN_INFO "i2c-psoc4-driver: LED blinking state set to 0x%02x\n", led_state);

    return count;
}

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

static void psoc4_remove(struct i2c_client *client)
{
    printk(KERN_INFO "i2c-psoc4-driver: Removing device\n");

    // Remove sysfs files and cleanup kobject
    sysfs_remove_file(my_kobj, &version_attr.attr);
    sysfs_remove_file(my_kobj, &led_blink_attr.attr);
    kobject_put(my_kobj);
}

// Register the I2C driver
module_i2c_driver(my_driver);

/* Meta information */
MODULE_LICENSE("GPL"); // License type
MODULE_AUTHOR("Vadym Kotsiuba <Vadym.Kotsiuba-EE@infineon.com>"); // Author information
MODULE_DESCRIPTION("I2C driver for PSoC4"); // Description of the module

