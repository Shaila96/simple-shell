# Unix Based Shell

The Shell is an implementation of command language interpreter with processes and input/output redirection. This program simulates the working of command line interface in Unix-like environment.
 

## Specifications
- Prompt for input from the user by its standard input
- A single command consists of a sequence of arguments separated by either an arbitrary number of spaces or one of the two I/O redirection characters (“<” and “>”)
- Able to handle background processes and input/output redirection
- **processes**: Displays a list of the background processes (process id and original command line) that have been initiated by the shell and are not yet terminated
- **exit**: kills all background processes the shell has forked has forked and terminates it
- An error message is returned for an inaccessible program file specified in the input line


## Compile and Execute the program

#### Compile command

`$ gcc -o myshell MyShell.c`


#### Execute command

`$ ./myshell`



## Test Cases

`$ ls -alg`

`$ /bin/ls -l`

`$ cd`

`$ mkdir test`

`$ cd test`

`$ pwd`

`$ cd /usr`

`$ cd usr` --> invalid command

`$ pwd>file.txt`

`$ echo hello`

`$ echo hello > file.txt`

`$ bg sleep 2`

`$ bg sleep 200`

`$ processes`

`$ rm -r test`

`$ rmdir test`

`$ ping`

`$ clear`

`$ touch 1.txt`

`$ uname`

`$ hostname`

`$ who`

`$ ls –l>file.txt`

`$ ls –l>` --> invalid command

`$ wc -w < file.txt`

`$ wc -w <` --> invalid command

`$ cat new-file.txt` --> if it is a new file, put some input and close by Ctrl+d

`$ cat new-file.txt` --> if not a new file, will show the contents of the file

`$ cat < file.txt > new-file.txt`

`$ cat>new-file.txt<file.txt`

`$ sort < file.txt > new-file.txt`

`$ sort>new-file.txt<file.txt`

`$ exit`









