//***************************************************************************
// examples/cctype/cctype_main.cxx
//
//   Copyright (C) 2016 Gregory Nutt. All rights reserved.
//   Author: Gregory Nutt <gnutt@nuttx.org>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
// 3. Neither the name NuttX nor the names of its contributors may be
//    used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//***************************************************************************

//***************************************************************************
// Included Files
//***************************************************************************


/***************************************************************************/
#include "pencil_main.h"



#if 0
static int pencil_daemon(int argc, char *argv[])
{

	printf("Hello pencil_daemon \r\n");
	while(1)
	{

	}

}
#endif

Pencil::Pencil() :
	_task_should_exit(false),
	_main_task(-1)
{

}

Pencil::~Pencil()
{
	if ( _main_task != -1 ) {
		_task_should_exit = true;

	}
}


int
Pencil::start()
{
	/* start the task */
	  
  _main_task = task_create("pencil_daemon", CONFIG_EXAMPLES_PENCIL_PRIORITY,
                    CONFIG_EXAMPLES_PENCIL_STACKSIZE,task_main_trampoline,
                    NULL);
  if (_main_task < 0)
    {
     
      printf("ERROR: Failed to start pencil_daemon");
      return EXIT_FAILURE;
    }
	return 0;
}

void 
Pencil::status()
{
	printf("status test!\r\n");
}

int
Pencil::task_main_trampoline(int argc,char *argv[])
{
	pencil::g_pencil->task_main();
	return 0;
}

void
Pencil::task_main()
{
	printf("pencil task_main \r\n");

//	while(1)
//	{
//
//	}
}


static void usage(void)
{
	printf("usage: pencil {start|stop|status}\r\n");
}

#ifdef BUILD_MODULE
int main(int argc, FAR char *argv[])
#else
int pencil_main(int argc, FAR char *argv[])
#endif
//int pencil_main(int argc, char *argv[])
{

#if 0
	int ret;
  printf("add  pencil!\r\n");
    ret = task_create("pencil_daemon", CONFIG_EXAMPLES_PENCIL_PRIORITY,
                    CONFIG_EXAMPLES_PENCIL_STACKSIZE, pencil_daemon,
                    NULL);
  if (ret < 0)
    {
      
      printf("pencil error\r\n");
      return EXIT_FAILURE;
    }
#endif

	if (argc < 2) {
		usage();
	}

	if ( !strcmp(argv[1],"start") ) {

		if ( pencil::g_pencil != NULL ) {
			printf("already running \r\n");

		}
		pencil::g_pencil = new Pencil;

		if ( pencil::g_pencil == NULL ) {
			printf("alloc failed! \r\n");
		}

		if ( OK != pencil::g_pencil -> start() ) {
			delete pencil::g_pencil;
			pencil::g_pencil = NULL;
			printf("start fsiled! \r\n");
		}
		return 0;
	}

	if ( !strcmp(argv[1],"stop") ) {
		delete pencil::g_pencil;
		pencil::g_pencil = NULL;
	} else if ( !strcmp(argv[1],"status") ) {
		pencil::g_pencil->status();
	} else {
		usage();
	}

	

 	return 0;
}

