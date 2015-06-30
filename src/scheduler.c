#include <linux/printk.h>

#include "mypcb.h"

extern tPCB task[MAX_TASK_NUM];
extern tPCB *my_current_task;
extern volatile int my_need_sched;
volatile int time_count = 0;

/*
 * Called by timer interrupt.
 * it runs in the name of current running process,
 * so it use kernel stack of current running process
 */
void my_timer_handler(void)
{
#if 1
	if (time_count % 1000 == 0 && my_need_sched != 1) {
		printk(KERN_NOTICE ">>> %s <<<\n", __func__);
		my_need_sched = 1;
	} 
	time_count++;
#endif
	return;  	
}

void my_schedule(void)
{
	tPCB *next;
	tPCB *prev;

	if (my_current_task == NULL || my_current_task->next == NULL)
		return;

	printk(KERN_NOTICE ">>> %s <<<\n", __func__);
	/* schedule */
	next = my_current_task->next;
	prev = my_current_task;
	if (next->state == 0) { /* -1 unrunnable, 0 runnable, >0 stopped */
		/* switch to next process */
		asm volatile(	
                        "pushfl\n\t"
			"movl %%esp,%0\n\t"	/* save esp */
			"movl %2,%%esp\n\t"	/* restore  esp */
			"movl $1f,%1\n\t"	/* save eip */	
			"pushl %3\n\t" 
			"ret\n\t"		/* restore  eip */
			"1:\t"			/* next process start here */
                        "popfl"
			: "=m" (prev->thread.sp), "=m" (prev->thread.ip)
			: "m" (next->thread.sp), "m" (next->thread.ip)
		); 
		my_current_task = next; 
		printk(KERN_NOTICE ">>>switch from %d to %d<<<\n",
		       prev->pid, next->pid);   	
	} else {
		next->state = 0;
		my_current_task = next;
		printk(KERN_NOTICE ">>>switch from %d to %d<<<\n",
		       prev->pid, next->pid);
		/* switch to new process */
		asm volatile(	
			"movl %%esp,%0\n\t"	/* save esp */
			"movl %2,%%esp\n\t"	/* restore esp */
			"movl $1f,%1\n\t"	/* save eip */	
			"pushl %3\n\t" 
			"ret\n\t"		/* restore  eip */

			: "=m" (prev->thread.sp),
			  "=m" (prev->thread.ip)
			: "m" (next->thread.sp),
			  "m" (next->thread.ip)
		);          
	}
}
