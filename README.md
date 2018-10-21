# C-Development-on-Linux

<h1>  Low level programming in language c for the course Operating System in National Technical University of Athens. </h1>


<h2> Assigment 1: Merging_two_files_into_a_third. </h2> 
 Using input-output system calls and handling files in C.<br>
 
 <div>
 
 <h2>Assigment 2: Implementation of a scheduler with Round-robin algorithm.</h2>
 Pass from the command line N programms that you want to schedule
 and using fork and execve N proccesses will be born(one for each programm). Also, in shell.c there is an implementation of a shell ,  where the user types in terminal and using pipes communicate with the scheduler. The shell has the following commands:
<br><br>  
 • Command ’p’: The scheduler prints in the output the list of all the proccesses,
     including the id, the PID, and the name of the exetuable.
 
 • Command ’k’: Takes as argument the id of a proccesses (careful: not the PID) 
    and asks the scheduler to terminate that proccess
  
 • Command ’e’: Takes as argument the name of an executable in the current directory and asks the scheduler,
      to create a new proccess that will execute that executable.
  
 • Command ’q’: The shell terminates his activity.
</div>

Kernel Drivers Development
