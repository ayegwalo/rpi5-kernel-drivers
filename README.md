# Linux Kernel & Device Driver Labs (Raspberry Pi 5)

Hands-on Linux kernel modules and device drivers built and tested on Raspberry Pi OS (Kernel 6.18 / BCM2712).

## Projects
- **Level 1:** Basic loadable kernel module (`insmod` / `rmmod` lifecycle & `printk` logging)
- **Level 2:** Kernel module parameters (`module_param` & `sysfs` exposure)

## Build & Run
```bash
make
sudo insmod hello.ko user_name="RaspberryPi" repeat_count=3
sudo dmesg | tail -n 5
sudo rmmod hello
```
