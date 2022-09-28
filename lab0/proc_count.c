#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

// global variables
static int numLive;
static struct proc_dir_entry *count;

// helper functions implementation
static int proc_count_show(struct seq_file *m, void *v){
	seq_printf(m, "Hello, world!")
	return 0;
}

// init and exit functions
static int __init proc_count_init(void)
{
	pr_info("proc_count: init\n");
	numLive = 0;
	count = proc_create_single("count", 0, NULL, proc_count_show);
	return 0;
}

static void __exit proc_count_exit(void)
{
	proc_remove(count);
	pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Alyssa Tadeo");
MODULE_DESCRIPTION("Counts total number of live programs");
MODULE_LICENSE("GPL");
