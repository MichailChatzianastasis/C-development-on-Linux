#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4c5efdbd, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x1a505334, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xb53bd8f5, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x4b6fd3e3, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x784213a6, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0x11229f12, __VMLINUX_SYMBOL_STR(virtqueue_kick) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xb60803a, __VMLINUX_SYMBOL_STR(nonseekable_open) },
	{ 0x661ffad1, __VMLINUX_SYMBOL_STR(virtqueue_get_buf) },
	{ 0x3d81aeb6, __VMLINUX_SYMBOL_STR(virtqueue_add_sgs) },
	{ 0x8f64aa4, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf7b46bdc, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0x42109f7b, __VMLINUX_SYMBOL_STR(unregister_virtio_driver) },
	{ 0x43261dca, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irq) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xb488e63e, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x9327f5ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0xb6244511, __VMLINUX_SYMBOL_STR(sg_init_one) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xd3719d59, __VMLINUX_SYMBOL_STR(paravirt_ticketlocks_enabled) },
	{ 0xfb39ca91, __VMLINUX_SYMBOL_STR(register_virtio_driver) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("virtio:d0000001Ev*");

MODULE_INFO(srcversion, "39CDDE53B6BE36A67E991E3");
