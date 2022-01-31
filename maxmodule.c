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


static struct timer_list my_timer;

void my_timer_callback( struct timer_list *args)
{
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
    printk(KERN_INFO"Hello from timer!\n");
}

static int __init lkm_example_init(void) {
    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
    printk(KERN_INFO
    "Hello, World!\n");
    return 0;
}


static void __exit lkm_example_exit(void) {
    del_timer(&my_timer);
    printk(KERN_INFO
    "Goodbye, World!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);

