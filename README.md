# Simple Shell

## General Instructions

### Hardware Requirements:
- You will require a machine having any Operating System that supports Unix APIs. You should not use MacOS for OS assignments. You can either have a dual boot system having any Linux OS (e.g., Ubuntu), or install a WSL.

### Software Requirements:
- C compiler and GNU make.
- You must do version controlling of all your code using GitHub. You should only use a PRIVATE repository. If you are found to be using a PUBLIC access repository, then it will be considered plagiarism. NOTE that TAs will check your GitHub repository during the demo.

## Assignment Details

### Summary
You have to implement a SimpleShell that waits for user input, executes commands provided in the user input, and then repeats until terminated using ctrl-c. The pseudocode for the Shell was discussed in the Lecture 06 and Lecture 07 slides. This assignment will teach you how to use different system calls. You don’t have to implement the individual Unix commands that will execute on your SimpleShell.

### SimpleShell Implementation ("simple-shell.c")
We are not providing any starter code for this assignment, but a detailed description is provided herewith for your implementation:

- The main job of any shell (including SimpleShell) is to read the user command from the standard input, parse the command string (command and its arguments if any), and execute the command along with the command line arguments provided by the user. All these three steps should be carried out in an infinite do-while loop as shown in the Lecture 06 slides.
- The shell must display a command prompt (of your choice) where user can provide the command as mentioned above.
- The user command has certain restrictions to simplify your implementation. The user command is not supposed to include backslash or quotes. The command and its argument will simply be separated by a whitespace.
- The command provided by the user will be executed by calling the `launch` method that would create a child process to execute the command provided by the user. Feel free to use any of the seven exec functions for executing the user command.
- SimpleShell should also support history command that should only show the commands entered on the SimpleShell command prompt (along with their command line arguments).
- Terminating the SimpleShell should display additional details on the execution of each command, e.g., process pid, time at which the command was executed, total duration the command took for execution, etc. You don’t need to display details of the commands executed in the past invocations of the SimpleShell. Basically, display all the mentioned details only for the entries in the history.

### Bonus
- Support "&" for background processes in your SimpleShell [+1 marks]
- Your SimpleShell can execute the commands from inside a Shell Script (by reading that file) [+1 marks]

## Usage
- Compile the `simple-shell.c` file using a C compiler and GNU make.
- Run the compiled executable to start the SimpleShell.
- Enter commands at the prompt and press Enter to execute them.
- Use ctrl-c to terminate the SimpleShell.

## Contributors
- [Your Name](link_to_github_profile) - Role

