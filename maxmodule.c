#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim V");
MODULE_DESCRIPTION("Just starting kernel modules development process");
MODULE_VERSION("1.00");


static int kthread_func(void *arg)
{
 
    return 0;
}


static int __init lkm_example_init(void) {


    struct task_struct *ts1; 
    int err;

    printk(KERN_INFO "Starting 2 threads\n");

/*struct task_struct *kthread_create(int (*threadfn)(void *data), void *data, \
 *                         const char *namefmt, ...);
 * This function creates a kernel thread and starts the thread.
 */
    ts1 = kthread_run(kthread_func, NULL, "thread-1");
    if (IS_ERR(ts1)) {
        printk(KERN_INFO "ERROR: Cannot create thread ts1\n");
        err = PTR_ERR(ts1);
        ts1 = NULL;
        return err;
    }

  

    printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);
    return 0;
}


static void __exit lkm_example_exit(void) {
 
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);

 