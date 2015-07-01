#include <linux/types.h>

#define MAX_TASK_NUM		4
#define KERNEL_STACK_SIZE	(1024 * 8)

/* CPU-specific state of this task */
struct myThread {
	uintptr_t ip;
	uintptr_t sp;
};

enum myState { S_unrunnable = -1, S_runnable = 0, S_stopped = 1 };
typedef struct _myPCB {
	int pid;
	volatile enum myState state;
	char stack[KERNEL_STACK_SIZE];
	/* CPU-specific state of this task */
	struct myThread thread;
	uintptr_t task_entry;
	struct _myPCB *next;
} myPCB;

void my_schedule(void);
