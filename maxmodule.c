#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim V");
MODULE_DESCRIPTION("Just starting kernel modules development process");
MODULE_VERSION("1.00");


static int __init

lkm_example_init(void) {
    pr_info("Hello, World!\n");
    return 0;
}


static void __exit

lkm_example_exit(void) {
    pr_info("Goodbye, World!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);

 