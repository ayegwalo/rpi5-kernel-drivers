# Linux Kernel & Device Driver Labs (Raspberry Pi 5)

Hands-on Linux kernel modules and device drivers built and tested on Raspberry Pi OS (Kernel 6.18 / BCM2712).

## Projects
- **Level 1 & 2 (`level_01_02_hello_params/`):** Loadable kernel module lifecycle (`insmod`/`rmmod`), `printk` logging, and runtime parameters (`module_param` & `sysfs`).
- **Level 3 (`level_03_chardev/`):** Character device driver exposing `/dev/mybox` with `file_operations`, `miscdevice`, and safe user-kernel memory transfers (`copy_from_user` / `copy_to_user`).

## Build & Test (Level 3)
```bash
cd level_03_chardev
make
sudo insmod mybox.ko
echo "Hello from user space!" > /dev/mybox
cat /dev/mybox
sudo rmmod mybox
```
