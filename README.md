### STEPS TO FOLLOW:

1. **Compile the Code**: 
gcc scheduler.c -o scheduler -lpthread

2. **Run the Program**: 
After successfully compiling the code, you can run the executable. Since this program requires command-line arguments (number of CPUs and time slice), you need to provide them when running the program. For example, to run the program with 4 CPUs and a time slice of 5:

./scheduler 4 5


3. **Interacting with the Program**: 
Once the program is running, you can interact with it by typing commands at the prompt (`Aahan@Aahan:~$`). Follow the instructions provided by the program to submit tasks and perform other actions.

4. **Terminating the Program**: 
You can exit the program by sending a SIGINT signal (Ctrl+C) from the terminal where it's running. This will trigger the cleanup process and exit the program gracefully.

**Note**: 
- Make sure you have the necessary permissions to compile and execute the code. 
- Additionally, ensure that you have a C compiler installed on your system (such as gcc) and that you're familiar with using the terminal or command prompt.
