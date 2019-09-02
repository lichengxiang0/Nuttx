#ifndef __PENCIL_MAIN_H__
#define __PENCIL_MAIN_H__

#include <nuttx/config.h>

#include <cstdio>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include <string.h>


extern "C"  int pencil_main(int argc, char *argv[]);

class Pencil
{
public:
	Pencil();
	~Pencil();

	/* start the task */
	int start();

	void status();

private:
	bool _task_should_exit;
	int _main_task;

	void task_main();
	static int task_main_trampoline(int argc,char *argv[]);

};

namespace pencil
{
	Pencil *g_pencil;
}





#endif






