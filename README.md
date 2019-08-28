Student Names:

Adam Nguyen

Instructions:

1. FTP the file "fsaccess.c" into your UNIX environment.
2. run command gcc -lm fsaccess.c in order to compile the program
3. run command ./a.out to run the program
4. mount a data file to be used as v6 file system
5. initialize a file system initfs using valid arguments
5. enter any valid shell command when prompted
6. enter command "q" to exit

Important Notes:

*program must be run in a UNIX environment*\
*fsaccess.c and data file must be in the same folder*\
*must mount then initfs before performing actions on file system*\
*invalid commands or arguments will terminate the program*\
*command "q" must be used when wanting to exit the program*

Sample Run:

Enter command: mount file.data\
File successfully mounted.\
Enter command: initfs 8000 300\
File system initiated.\
Enter command: cpin extFile.txt /in.txt\
File copy successful.\
Enter command: cpout /in.txt out.txt\
cpout successful.\
Enter command: q\
Program terminated.
