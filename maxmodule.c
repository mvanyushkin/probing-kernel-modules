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

static int is_completed = 0;

static int my_thread_func(void *arg) {
    while(true && is_completed == 0) {
        usleep_range(1000000, 1000001);
        printk(KERN_INFO"I am thread: %s[PID = %d]\n", current->comm, current->pid);
    }
    return 0;
}

static int __init

lkm_example_init(void) {
    struct task_struct *ts1;
    ts1 = kthread_run(my_thread_func, NULL, "thread-1");
    return 0;
}


static void __exit

lkm_example_exit(void) {
    is_completed = 1;
    printk(KERN_INFO
    "Goodbye, World!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);

 