#include <linux/printk.h>

#include "mypcb.h"

myPCB task[MAX_TASK_NUM], *my_current_task = NULL;
volatile int my_need_sched = 0;

void my_process(void);

void __init my_start_kernel(void)
{
	int pid = 0;
	int i;

	/* Initialize process 0 */
	task[pid].pid = pid;
	task[pid].state = 0; /* -1 unrunnable, 0 runnable, >0 stopped */
	task[pid].task_entry = task[pid].thread.ip = (uintptr_t) my_process;
	task[pid].thread.sp =
		(uintptr_t) &task[pid].stack[KERNEL_STACK_SIZE - 1];
	task[pid].next = &task[pid];

	/* fork more process */
	for (i = 1; i < MAX_TASK_NUM; i++) {
		memcpy(&task[i], &task[0], sizeof(myPCB));
		task[i].pid = i;
		task[i].state = -1;
		task[i].thread.sp =
			(uintptr_t) &task[i].stack[KERNEL_STACK_SIZE - 1];
		task[i].next = task[i-1].next;
		task[i-1].next = &task[i];
	}

	/* start process 0 by task[0] */
	pid = 0;
	my_current_task = &task[pid];
	asm volatile(
		"movl %1,%%esp\n\t" 	/* set task[pid].thread.sp to esp */
		"pushl %0\n\t" 	        /* push task[pid].thread.ip */
		"ret\n\t"		/* pop task[pid].thread.ip to eip */		
		: 
		: "c" (task[pid].thread.ip),
		  "d" (task[pid].thread.sp)	/* input c/d mean %ecx/%edx */
	);
}

void my_process(void)
{
	int i = 0;
	while (1) {
		if (i++ % 10000000)
			continue;

		printk(KERN_NOTICE "this is process %d -\n",
		       my_current_task->pid);
		if (my_need_sched == 1) {
			my_need_sched = 0;
			my_schedule();
		}
		printk(KERN_NOTICE "this is process %d +\n",
		       my_current_task->pid);
	}
}
