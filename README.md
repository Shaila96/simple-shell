# A Simple Shell

The Simple Shell is an implementation of command language interpreter with processes and input/output redirection. This is shell is developed in C programming language.
 


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









