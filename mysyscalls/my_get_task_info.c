#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<asm/current.h>
#include<linux/sched.h>
#include <linux/ktime.h> // for time
#include <linux/rtc.h>   // for time
#include<linux/list.h>

// form fs/proc/array.c
static const char * const task_state_array[] = {

	"R (running)",		/* 0x00 */
	"S (sleeping)",		/* 0x01 */
	"D (disk sleep)",	/* 0x02 */
	"T (stopped)",		/* 0x04 */
	"t (tracing stop)",	/* 0x08 */
	"X (dead)",		/* 0x10 */
	"Z (zombie)",		/* 0x20 */
	"P (parked)",		/* 0x40 */

	"I (idle)",		/* 0x80 */
};


/* my_get_task_info */
SYSCALL_DEFINE0(my_get_task_info)
{
	struct task_struct *curr = current;
	struct task_struct *parent = curr->parent;

	// state is stored in curr->state as a bit mask
	// using the task_state_index to convert it into a inde
	char *state = task_state_array[task_state_index(curr)];

	// ktime_get_real gets the unix time
	struct rtc_time t = rtc_ktime_to_tm(ktime_get_real());
	printk(KERN_INFO "Current time: %ptRs", &t);

	printk("current task - name: %s, pid: %d, state: %s\n",
			curr->comm, curr->pid, state);

	if(parent){
		printk("parrent -      name: %s, pid: %d, state: %s\n",
				parent->comm, parent->pid, task_state_array[task_state_index(parent)]);
	}
	else{
		printk("no parent\n");
	}

	// print child tasks
	printk("\nChild processes:\n");
	printk("name\tpid\tstate\t\n");
	struct task_struct *tsk;
	struct list_head *task_list_head;
	list_for_each(task_list_head, &(curr->children)){
		tsk = list_entry(task_list_head, struct task_struct, children);
		printk("%s\t%d\t%s\n",
			tsk->comm, tsk->pid, task_state_array[task_state_index(tsk)]);
	}

	return curr->pid;
}
