# Simple Shell

## General Instructions

### Hardware Requirements:
- You will require a machine having any Operating System that supports Unix APIs. You should not use MacOS for OS assignments. You can either have a dual boot system having any Linux OS (e.g., Ubuntu), or install a WSL.

### Software Requirements:
- C compiler and GNU make.
- You must do version controlling of all your code using GitHub. You should only use a PRIVATE repository. If you are found to be using a PUBLIC access repository, then it will be considered plagiarism. NOTE that TAs will check your GitHub repository during the demo.

## Assignment Details

### Summary
Implementing a SimpleShell that waits for user input, executes commands provided in the user input, and then repeats until terminated using ctrl-c. We are not implementing the individual Unix commands that will execute on your SimpleShell.

### SimpleShell Description ("simple-shell.c")

- The main job of any shell (including SimpleShell) is to read the user command from the standard input, parse the command string (command and its arguments if any), and execute the command along with the command line arguments provided by the user. All these three steps should be carried out in an infinite do-while loop.
  - The shell is displayoing a command prompt (```Aahan@Aahan```) where user can provide the command as mentioned above.
- The user command has certain restrictions. The user command is not supposed to include backslash or quotes. ```The command and its argument will simply be separated by a whitespace```.
- The command provided by the user will be executed by calling the `launch` method that would create a child process to execute the command provided by the user. 
- SimpleShell should is also supporting history command that's showing the commands entered on the SimpleShell command prompt (along with their command line arguments).
- Terminating the SimpleShell is displaying additional details on the execution of each command, e.g., process pid, time at which the command was executed, total duration the command took for execution, etc. Basically, it will diplay all the mentioned details only for the entries in the history.
- Supports "&" for background processes.
- SimpleShell can execute the commands from inside a Shell Script.

## Usage
- Compile the `simple-shell.c` file using a C compiler and GNU make.
- Run the compiled executable to start the SimpleShell.
- Enter commands at the prompt and press Enter to execute them.
- Use ctrl-c to terminate the SimpleShell.

## Contributors
- Aahan 
- Kartikeya (https://github.com/Kartikeya2022241)

