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
#include <linux/ioctl.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim V");
MODULE_DESCRIPTION("Just starting kernel modules development process");
MODULE_VERSION("1.00");

dev_t awesomeDevice = 0;
static struct class *awesomeDeviceClass = NULL;
static struct cdev awesomeCDev;
uint8_t *kernel_buffer;
uint32_t  current_buffer_size;

static struct proc_dir_entry *parent = NULL;
int32_t value = 0;
char etx_array[20]="try_proc_array\n";
static int len = 1;

#define MEM_SIZE        1024

static int      __init lkm_example_init(void);
static void     __exit lkm_example_exit(void);

static int      max_dev_open(struct inode *inode, struct file *file);
static int      max_dev_release(struct inode *inode, struct file *file);
static ssize_t  max_dev_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  max_dev_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static void cleanUp(void);



static int      open_proc(struct inode *inode, struct file *file);
static int      release_proc(struct inode *inode, struct file *file);
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);





static int readBytes = 0;

// That's a real implementation of the supported operations.
// Some kind of interface implementation
static struct file_operations supportedFileOperations =
        {
                .owner      = THIS_MODULE,
                .read       = max_dev_read,
                .write      = max_dev_write,
                .open       = max_dev_open,
                .release    = max_dev_release,
        };


static int max_dev_open(struct inode *inode, struct file *file) {
    pr_debug("Max awesome device's been opened.\n");
    readBytes = 0;
    return 0;
}

static int max_dev_release(struct inode *inode, struct file *file) {
    pr_debug("Max awesome device's been released.\n");
    readBytes = 0;
    return 0;
}


static ssize_t max_dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    pr_debug("Read operation.\n");
    if (readBytes >= current_buffer_size)
        return 0;

    if (copy_to_user(buf, kernel_buffer, current_buffer_size)) {
        pr_err("Data reading : Err!\n");
    }

    readBytes += current_buffer_size;
    return current_buffer_size;
}

static ssize_t max_dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
    pr_info("Write operation\n");
    if( copy_from_user(kernel_buffer, buf, len) ) {
        pr_err("Data Write : Err!\n");
    }
    pr_info("Data Write : Done!\n");
    return len;
}



// for proc_fs entry
static struct proc_ops max_proc_ops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};



static int open_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}
static int release_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}

static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_INFO "proc file write.....\t");
    copy_from_user(etx_array,buff,len);
    return len;
}

static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t* offset) {
    printk(KERN_INFO "proc file read.....\n");
    if(len) {
        len = 0;
    }
    else{
        len = 1;
        return 0;
    }
    copy_to_user(buffer, etx_array, 20);
    return length;;
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

    if (parent != NULL) {
        proc_remove(parent);
        pr_info("The proc_fs parent entry has been removed");
    }
}


static int __init

lkm_example_init(void) {
    pr_info("Max module has been loaded!\n");

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

    if((kernel_buffer = kmalloc(MEM_SIZE , GFP_KERNEL)) == 0){
        pr_info("Cannot allocate memory in kernel\n");
        cleanUp();
        return -1;
    }

    parent = proc_mkdir("maxawesomedevice",NULL);
    if( parent == NULL )
    {
        pr_info("Error creating proc entry");
        cleanUp();
    }


    proc_create("maxawesomedevice-proc-entry", 0666, parent, &max_proc_ops);

    char* initialData = "Hello_World\n";
    strcpy(kernel_buffer, initialData);
    current_buffer_size = strlen(initialData);
    pr_info("current buffer size is %d \n", current_buffer_size);

    return 0;
}


static void __exit

lkm_example_exit(void) {
    cleanUp();
    pr_info("Max module has been loaded!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);

 