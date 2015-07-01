#include <linux/printk.h>

#include "mypcb.h"

extern myPCB task[MAX_TASK_NUM], *my_current_task;
extern volatile int my_need_sched;
volatile int time_count = 0;

/*
 * Called by timer interrupt.
 * it runs in the name of current running process,
 * so it use kernel stack of current running process
 */
void my_timer_handler(void)
{
	if (time_count % 1000 == 0 && my_need_sched != 1)
		my_need_sched = 1;
	time_count++;
}

void my_schedule(void)
{
	myPCB *next, *prev;

	if (my_current_task == NULL || my_current_task->next == NULL)
		return;

	printk(KERN_NOTICE ">>> %s <<<\n", __func__);
	/* schedule */
	next = my_current_task->next;
	prev = my_current_task;
	if (next->state == S_runnable) {
		my_current_task = next;
		printk(KERN_NOTICE ">>>switch from %d to %d<<<\n",
			prev->pid, next->pid);
                /* switch to next process */
		asm volatile(	
			"movl	%%esp, %0\n\t"	/* save esp */
			"movl	%2, %%esp\n\t"	/* restore  esp */
			"movl	$1f, %1\n\t"	/* save eip */
			"jmp	*%3\n"
			"1:\t"			/* next process start here */
			: "=m" (prev->thread.sp), "=m" (prev->thread.ip)
			: "m" (next->thread.sp), "m" (next->thread.ip)
		);
	} else {
		next->state = S_runnable;
		my_current_task = next;
		printk(KERN_NOTICE ">>>switch to new process %d<<<\n",
			next->pid);
	}
}
