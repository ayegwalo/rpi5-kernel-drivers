// 1. INCLUDE KERNEL TOOLS
#include <linux/init.h>    // Lets us mark startup and shutdown functions
#include <linux/module.h>  // Required for every kernel module
#include <linux/kernel.h>  // Gives us printk() to print messages

// 2. MODULE ID CARD
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Beginner");
MODULE_DESCRIPTION("My first kernel module");

// 3. STARTUP FUNCTION (Runs when you type 'sudo insmod hello.ko')
static int __init start_module(void) {
    printk(KERN_INFO "HELLO! My code is now inside the Linux kernel!\n");
    return 0; 
}

// 4. SHUTDOWN FUNCTION (Runs when you type 'sudo rmmod hello')
static void __exit stop_module(void) {
    printk(KERN_INFO "GOODBYE! My code left the Linux kernel!\n");
}

// 5. REGISTER YOUR FUNCTIONS
module_init(start_module);
module_exit(stop_module);
