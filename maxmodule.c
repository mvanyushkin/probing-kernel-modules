#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim V");
MODULE_DESCRIPTION("Just starting kernel modules development process");
MODULE_VERSION("1.00");

dev_t awesomeDevice = 0;
static struct class *awesomeDeviceClass = NULL;
static struct cdev awesomeCDev;

static int      __init lkm_example_init(void);
static void     __exit lkm_example_exit(void);

static int      max_dev_open(struct inode *inode, struct file *file);
static int      max_dev_release(struct inode *inode, struct file *file);
static ssize_t  max_dev_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  max_dev_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static void cleanUp(void);

static struct file_operations supportedFileOperations =
        {
                .owner      = THIS_MODULE,
                .read       = max_dev_read,
                .write      = max_dev_write,
                .open       = max_dev_open,
                .release    = max_dev_release,
        };

static int max_dev_open(struct inode *inode, struct file *file) {
    pr_info("Driver Open Function Called...!!!\n");
    return 0;
}

static int max_dev_release(struct inode *inode, struct file *file) {
    pr_info("Driver Release Function Called...!!!\n");
    return 0;
}


static ssize_t max_dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    pr_info("Driver Read Function Called...!!!\n");
    return 0;
}

static ssize_t max_dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("Driver Write Function Called...!!!\n");
    return len;
}

static void cleanUp() {
    if (awesomeDeviceClass != NULL && awesomeDevice != 0) {
        device_destroy(awesomeDeviceClass,awesomeDevice);
        pr_info("The class and device have been unbound");
    }

    if (awesomeDeviceClass != NULL) {

        class_destroy(awesomeDeviceClass);
        pr_info("The awesome class has been destroyed");
    }

    if (awesomeCDev.count > 0) {
        cdev_del(&awesomeCDev);
        pr_info("The char device has been removed");
    }

    if (awesomeDevice != 0) {
        unregister_chrdev_region(awesomeDevice, 1);
        pr_info("The major & minor pair has been revoked");
    }
}


static int __init

lkm_example_init(void) {

    // allocating numbers
    if((alloc_chrdev_region(&awesomeDevice, 0, 1, "maxawesomedevice")) <0){
        pr_err("Cannot allocate major number for device\n");
        return -1;
    }

    pr_info("Major = %d Minor = %d \n",MAJOR(awesomeDevice), MINOR(awesomeDevice));

    cdev_init(&awesomeCDev, &supportedFileOperations);

    if ( (cdev_add(&awesomeCDev, awesomeDevice, 1)) < 0) {
        pr_err("we've fucked up");
        cleanUp();
        return -1;
    }

    // create a completely new class for my devices
    if((awesomeDeviceClass = class_create(THIS_MODULE,"maxawesomeclass")) == NULL){
        pr_err("Cannot create the struct class for device\n");
        cleanUp();
        return -1;
    }

    // create a device file, that's a kind of mknod call
    if((device_create(awesomeDeviceClass,NULL,awesomeDevice,NULL,"maxawesomedevice")) == NULL){
        pr_err("Cannot create the Device\n");
        cleanUp();
        return -1;
    }

    return 0;
}


static void __exit

lkm_example_exit(void) {
    cleanUp();
    pr_info("Goodbye, World!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);

 