// =========================================================================
// 1. INCLUDE KERNEL TOOLS
// =========================================================================
#include <linux/init.h>        // Needed for __init and __exit startup/shutdown tags
#include <linux/module.h>      // Core header required for every kernel module
#include <linux/kernel.h>      // Gives us printk() for kernel logging
#include <linux/fs.h>          // Defines 'file_operations' (open, read, write)
#include <linux/uaccess.h>     // Gives us copy_to_user() & copy_from_user() for safe memory transfer
#include <linux/miscdevice.h>  // Easy helper that automatically creates a /dev/ file for us

// =========================================================================
// 2. MODULE ID CARD
// =========================================================================
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ayegwalo");
MODULE_DESCRIPTION("Level 3: Beginner Character Device Driver (/dev/mybox)");

// =========================================================================
// 3. KERNEL STORAGE BOX (PRIVATE RAM INSIDE THE KERNEL)
// =========================================================================
#define BUFFER_SIZE 256
static char kernel_buffer[BUFFER_SIZE]; // Holds up to 255 characters of text inside kernel RAM
static size_t data_size = 0;            // Tracks how many characters are currently stored in the box

// =========================================================================
// 4. WRITE FUNCTION: Runs when a user sends text into /dev/mybox
//    Example terminal command: echo "hello" > /dev/mybox
// =========================================================================
static ssize_t mybox_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos) {
    // Safety Check 1: Prevent a crash (buffer overflow) if the user sends too much text
    if (count >= BUFFER_SIZE) {
        count = BUFFER_SIZE - 1; // Leave 1 byte at the end for the string terminator '\0'
    }

    // Safety Check 2: Move data safely from User Space (your terminal) to Kernel Space (kernel_buffer).
    // RULE: Never use normal C memcpy() here! User pointers can be invalid or malicious.
    // copy_from_user() safely verifies the memory address before copying.
    if (copy_from_user(kernel_buffer, user_buf, count) != 0) {
        return -EFAULT; // Return "Bad Address" error if copying failed
    }

    // Add a null-terminator '\0' so C knows where the text ends
    kernel_buffer[count] = '\0';
    data_size = count;

    // Log what happened to dmesg so we can see it working
    printk(KERN_INFO "mybox: Received %zu bytes from user: %s\n", count, kernel_buffer);

    // Tell the terminal "I successfully wrote all 'count' bytes"
    return count;
}

// =========================================================================
// 5. READ FUNCTION: Runs when a user reads text out of /dev/mybox
//    Example terminal command: cat /dev/mybox
// =========================================================================
static ssize_t mybox_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos) {
    // Check if the user has already read the message (*ppos > 0) or if the box is empty.
    // Returning 0 tells commands like 'cat' "End of File—stop reading!"
    // Without this check, 'cat' would read the same text in an infinite loop forever.
    if (*ppos > 0 || data_size == 0) {
        return 0;
    }

    // Don't send more bytes than we actually have stored in our box
    if (count > data_size) {
        count = data_size;
    }

    // Safely copy the text from Kernel Space back out to User Space (your terminal)
    if (copy_to_user(user_buf, kernel_buffer, count) != 0) {
        return -EFAULT; // Return "Bad Address" error if copying failed
    }

    // Move the file reading cursor forward so the next read call knows we are done
    *ppos += count;

    printk(KERN_INFO "mybox: Sent %zu bytes back to user\n", count);

    // Tell the terminal how many bytes we sent
    return count;
}

// =========================================================================
// 6. FILE OPERATIONS TABLE (THE WIRING DIAGRAM)
//    Connects standard Linux file actions (read/write) to our custom C functions
// =========================================================================
static const struct file_operations mybox_fops = {
    .owner = THIS_MODULE,  // Prevents the module from being unloaded while a program is using it
    .write = mybox_write,  // Connect file writes to our mybox_write() function
    .read  = mybox_read,   // Connect file reads to our mybox_read() function
};

// =========================================================================
// 7. DEVICE REGISTRATION SETTINGS
//    Tells Linux to automatically create the file /dev/mybox when loaded
// =========================================================================
static struct miscdevice mybox_device = {
    .minor = MISC_DYNAMIC_MINOR, // Let Linux automatically assign an unused device ID number
    .name  = "mybox",            // This sets the exact filename: /dev/mybox
    .fops  = &mybox_fops,        // Attach our wiring diagram from Step 6
    .mode  = 0666,               // File permissions: allow everyone (not just root) to read & write
};

// =========================================================================
// 8. STARTUP FUNCTION (Runs on 'sudo insmod mybox.ko')
// =========================================================================
static int __init mybox_init(void) {
    // Register /dev/mybox with the Linux kernel
    int ret = misc_register(&mybox_device);
    if (ret) {
        printk(KERN_ERR "mybox: Failed to create /dev/mybox\n");
        return ret; // Abort loading if registration failed
    }

    printk(KERN_INFO "mybox: /dev/mybox created and ready!\n");
    return 0; // Success!
}

// =========================================================================
// 9. SHUTDOWN FUNCTION (Runs on 'sudo rmmod mybox')
// =========================================================================
static void __exit mybox_exit(void) {
    // Delete /dev/mybox cleanly from the system when unloading
    misc_deregister(&mybox_device);
    printk(KERN_INFO "mybox: /dev/mybox removed.\n");
}

// Register our startup and shutdown functions with the kernel
module_init(mybox_init);
module_exit(mybox_exit);
