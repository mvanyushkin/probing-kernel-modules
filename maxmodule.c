#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim V");
MODULE_DESCRIPTION("Just starting kernel modules development process");
MODULE_VERSION("1.00");


char *myCustomParameter;

// creates a param at
// /sys/module/maxmodule/parameters/myCustomParameter
module_param(myCustomParameter, charp, S_IRUSR|S_IWUSR);

static int __init

lkm_example_init(void) {
    printk("myCustomParameter = %s \n", myCustomParameter);
    pr_info("Hello, World!\n");
    return 0;
}


static void __exit

lkm_example_exit(void) {
    printk("myCustomParameter = %s \n", myCustomParameter);
    pr_info("Goodbye, World!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);
