#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomsk State University");
MODULE_DESCRIPTION("A simple Linux kernel module");
MODULE_VERSION("1.0");

// Функция загрузки модуля
static int __init tsu_module_init(void) {
    printk(KERN_INFO "Welcome to the Tomsk State University\n");
    return 0;
}

// Функция выгрузки модуля
static void __exit tsu_module_exit(void) {
    printk(KERN_INFO "Tomsk State University forever!\n");
}

module_init(tsu_module_init); 
module_exit(tsu_module_exit); 
