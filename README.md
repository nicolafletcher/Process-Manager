# Process-Manager
## Description

Program: PMan
Author: Nicola Fletcher - 09/20/20

The executable is called "PMan". PMan starts specified processes in the background,
and continues to prompt the user. The user can use a selection of commands (described further down)
to send signals to the processes and display information.

## Usage


    * Compilation: There is a Makefile provided to take care of building the code, which also requires
                   pid_list.c and pid_list.h to run. Simply run "make" to compile and create the executable PMan.
                   To fully recompile if changes are made, run "make clean" first.

    * To run: $ ./PMan

    * Once running, PMan will deliver the following prompt to the user:

            PMan:   >

    * The correct usage of this prompt is:  PMan:   >  <command> [args]


## Commands


The following commands are recognized by PMan:

    * exit: discontinues the user input prompt and ends the PMan session

            PMan:   >  exit

    * bg: starts the specified background process in the background

            PMan:   >  bg ./[file executable] [args]

    * bglist: displays a list of all the processes currently running in the background

            PMan:   >  bglist

    * bgkill: terminates the specified background process

            PMan:   >  bgkill <pid>

    * bgstop: temporarily stops the specified background process

            PMan:   >  bgstop <pid>

    * bgstart: restarts the specified background process (that has been previously stopped)

            PMan:   >  bgstart <pid>

    * pstat: displays a list of the following information for a specified process: comm, state,
         utime, stime, rss, voluntary_ctxt_switches, and nonvoluntary_ctxt_switches

            PMan:   >  pstat <pid>
