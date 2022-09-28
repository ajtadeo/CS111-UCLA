#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

// global variables
static int numAlive;
static struct proc_dir_entry *count;

// count number of currently alive processes
static int count_alive_procs(void)
{
	struct task_struct *t;
	int i = 0;
	for_each_process(t) {
		i += 1;
	}
	numAlive = i;
}

// sets the contents of /proc/count when cat-ed
static int proc_count_show(struct seq_file *m, void *v)
{
	count_alive_procs();
	seq_printf(m, "%d\n", numAlive);
	pr_info("proc_count: Reading /proc/count\n");
	return 0;
}

// initalization runs on insmod
static int __init proc_count_init(void)
{
	pr_info("proc_count: init\n");
	numLive = 0;
	count = proc_create_single("count", 0, NULL, proc_count_show);
	pr_info("proc_count: Created /proc/count\n")
	return 0;
}

// exit runs on rmmod
static void __exit proc_count_exit(void)
{
	proc_remove(count);
	pr_info("proc_count: Removed /proc/count\n");
	pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Alyssa Tadeo");
MODULE_DESCRIPTION("Counts total number of live programs and prints value in /proc/count");
MODULE_LICENSE("GPL");
