Application Folder
==================

Contents
--------

  General
  Directory Location
  Built-In Applications
  NuttShell (NSH) Built-In Commands
  Synchronous Built-In Commands
  Application Configuration File
  Example Built-In Application
  Building NuttX with Board-Specific Pieces Outside the Source Tree

General
-------
This folder provides various applications found in sub-directories.  These
applications are not inherently a part of NuttX but are provided to help
you develop your own applications.  The apps/ directory is a "break away"
part of the configuration that you may choose to use or not.

Directory Location
------------------
The default application directory used by the NuttX build should be named
apps/ (or apps-x.y/ where x.y is the NuttX version number).  This apps/
directory should appear in the directory tree at the same level as the
NuttX directory.  Like:

 .
 |- nuttx
 |
 `- apps

If all of the above conditions are TRUE, then NuttX will be able to
find the application directory.  If your application directory has a
different name or is location at a different position, then you will
have to inform the NuttX build system of that location.  There are several
ways to do that:

1) You can define CONFIG_APPS_DIR to be the full path to your application
   directory in the NuttX configuration file.
2) You can provide the path to the application directory on the command line
   like:  make APPDIR=<path> or make CONFIG_APPS_DIR=<path>
3) When you configure NuttX using tools/configure.sh, you can provide that
   path to the application directory on the configuration command line
   like: ./configure.sh -a <app-dir> <board-name>/<config-name>

Built-In Applications
---------------------
NuttX also supports applications that can be started using a name string.
In this case, application entry points with their requirements are gathered
together in two files:

  - builtin/builtin_proto.h  Entry points, prototype function
  - builtin/builtin_list.h   Application specific information and requirements

The build occurs in several phases as different build targets are executed:
(1) context, (2) depend, and (3) default (all). Application information is
collected during the make context build phase.

To execute an application function:

  exec_builtin() is defined in the nuttx/include/apps/builtin/builtin.h

NuttShell (NSH) Built-In Commands
---------------------------------
One use of builtin applications is to provide a way of invoking your custom
application through the NuttShell (NSH) command line.  NSH will support
a seamless method invoking the applications, when the following option is
enabled in the NuttX configuration file:

  CONFIG_NSH_BUILTIN_APPS=y

Applications registered in the apps/builtin/builtin_list.h file will then
be accessible from the NSH command line.  If you type 'help' at the NSH
prompt, you will see a list of the registered commands.

Synchronous Built-In Commands
-----------------------------
By default, built-in commands started from the NSH command line will run
asynchronously with NSH.  If you want to force NSH to execute commands
then wait for the command to execute, you can enable that feature by
adding the following to the NuttX configuration file:

  CONFIG_SCHED_WAITPID=y

The configuration option enables support for the waitpid() RTOS interface.
When that interface is enabled, NSH will use it to wait, sleeping until
the built-in command executes to completion.

Of course, even with CONFIG_SCHED_WAITPID=y defined, specific commands
can still be forced to run asynchronously by adding the ampersand (&)
after the NSH command.

Application Configuration File
------------------------------
The NuttX configuration uses kconfig-frontends tools and the NuttX
configuration file (.config) file.  For example, the NuttX .config
may have:

  CONFIG_EXAMPLES_HELLO=y

This will select the apps/examples/hello in the following way:

- The top-level make will include examples/Make.defs
- examples/Make.defs will set CONFIGURED_APPS += examples/hello
  like this:

  ifeq ($(CONFIG_EXAMPLES_HELLO),y)
  CONFIGURED_APPS += examples/hello
  endif

Example Built-In Application
----------------------------
An example application skeleton can be found under the examples/hello
sub-directory.  This example shows how a builtin application can be added
to the project. One must:

 1. Create sub-directory as: appname

 2. In this directory there should be:

    - A Make.defs file that would be included by the apps/Makefile
    - A Kconfig file that would be used by the configuration tool (see the
      file kconfig-language.txt in the NuttX tools repository).  This
      Kconfig file should be included by the apps/Kconfig file
    - A Makefile, and
    - The application source code.

 3. The application source code should provide the entry point:
    appname_main()

 4. Set the requirements in the file: Makefile, specially the lines:

    APPNAME    = appname
    PRIORITY   = SCHED_PRIORITY_DEFAULT
    STACKSIZE  = 768
    ASRCS      = asm source file list as a.asm b.asm ...
    CSRCS      = C source file list as foo1.c foo2.c ..

 4b. The Make.defs file should include a line like:

    ifeq ($(CONFIG_APPNAME),y)
    CONFIGURED_APPS += appname
    endif

Building NuttX with Board-Specific Pieces Outside the Source Tree
-----------------------------------------------------------------

Q: Has anyone come up with a tidy way to build NuttX with board-
   specific pieces outside the source tree?
A: Here are three:

   1) There is a make target called 'make export'. It will build
      NuttX, then bundle all of the header files, libraries, startup
      objects, and other build components into a .zip file. You
      can can move that .zip file into any build environment you
      want. You even build NuttX under a DOS CMD window.

      This make target is documented in the top level nuttx/README.txt.

   2) You can replace the entire apps/ directory. If there is
      nothing in the apps/ directory that you need, you can define
      CONFIG_APPS_DIR in your .config file so that it points to a
      different, custom application directory.

      You can copy any pieces that you like from the old apps/directory
      to your custom apps directory as necessary.

      This is documented in NuttX/configs/README.txt and
      nuttx/Documentation/NuttxPortingGuide.html (Online at
      https://bitbucket.org/nuttx/documentation/src/master/NuttxPortingGuide.html#apndxconfigs
      under Build options). And in the apps/README.txt file.

   3) If you like the random collection of stuff in the apps/ directory
      but just want to expand the existing components with your own,
      external sub-directory then there is an easy way to that too:
      You just create a symbolic link in the apps/ directory that
      redirects to your application sub-directory.

      In order to be incorporated into the build, the directory that
      you link under the apps/ directory should contain (1) a Makefile
      that supports the clean and distclean targets (see other Makefiles
      for examples), and (2) a tiny Make.defs file that simply adds the
      custom build directories to the variable CONFIGURED_APPS like:

        CONFIGURED_APPS += my_directory1 my_directory2

      The apps/Makefile will always automatically check for the
      existence of subdirectories containing a Makefile and a Make.defs
      file.  The Makefile will be used only to support cleaning operations.
      The Make.defs file provides the set of directories to be built; these
      directories must also contain a Makefile.  That Makefile must be able
      to build the sources and add the objects to the apps/libapps.a archive.
      (see other Makefiles for examples).  It should support the all,
      install, context, and depend targets.

      apps/Makefile does not depend on any hardcoded lists of directories.
      Instead, it does a wildcard search to find all appropriate
      directories.  This means that to install a new application, you
      simply have to copy the directory (or link it) into the apps/
      directory.  If the new directory includes a Makefile and Make.defs
      file, then it will automatically be included in the build.

      If the directory that you add also includes a Kconfig file, then it
      will automatically be included in the NuttX configuration system as
      well.  apps/Makefile uses a tool at apps/tools/mkkconfig.sh that
      dynamically builds the apps/Kconfig file at pre-configuration time.

      You could, for example, create a script called install.sh that
      installs a custom application, configuration, and board specific
      directory:

        a) Copy 'MyBoard' directory to configs/MyBoard.
        b) Add a symbolic link to MyApplication at apps/external
        c) Configure NuttX (usually by:

           tools/configure.sh MyBoard/MyConfiguration

      Use of the name ''apps/external'' is suggested because that name
      is included in the .gitignore file and will save you some nuisance
      when working with GIT.
