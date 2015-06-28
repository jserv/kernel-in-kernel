Welcome to the kernel-in-kernel repository. Here you will find the
straightfoward environment to develop your own operating system kernel
by means of reusing Linux kernel, that implies that you do not have to
worry about low-level initilizations, platform-specific configurations,
and even device drivers.

This work was inspired by [mykernel](https://github.com/mengning/mykernel/),
the simple simulation of the linux process scheduling.
Maintained by `Jim Huang <jserv.tw@gmail.com>`.

Build Instructions
------------------
Unless otherwise noted, file and directory names refer to this repository.

1. Install [QEMU](http://www.qemu.org) and ensure its availability of x86 support.
   For Debian/Ubuntu Linux, you can simply do `sudo apt-get install qemu-system-x86`.

2. Return to the kernel-in-kernel top-level directory. Do a `make`. This will
   do several things including downloading Linux kernel 4.1 as the base of
   your own kernel, configuring the minimal development environment, and
   building everything from scratch.

3. At present, only IA32 image is generated, but it should be reasonably easy
   to enable other architectures originally supported by Linux kernel.

Modify Your Own Kernel
----------------------
Hacking kernel is interesting, and you should take a look over the files in
the `src` directory:
* `mypcb.h`: the cutomized process control block
* `scheduler.c`: timer interrupt handler and simple scheduler
* `main.c`: entry point

Running kernel-in-kernel
------------------------
Run `make run` and and you should see this:

```
this is process 0 -
this is process 0 +
this is process 1 -
this is process 1 +
this is process 2 -
this is process 2 +
this is process 3 -
this is process 3 +
```

Licensing
=========
Since kernel-in-kernel is built directly upon Linux kernel, it is certainly
licensed under GNU GPL v2.
