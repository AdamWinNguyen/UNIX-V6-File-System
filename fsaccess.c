//Adam Nguyen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

struct superBlock {
	unsigned short isize;
	unsigned short fsize;
	unsigned short nfree;
	unsigned short free[100];
	unsigned short ninode;
	unsigned short inode[100];
	char flock;
	char ilock;
	char fmod;
	unsigned short time[2];
};
struct inode {
	unsigned short flag;	//Stores inode flag.  Later converted into type
	char nlinks;
	char uid;
	char gid;
	char fsize1;	//Size of size0
	unsigned short fsize2;	//Size of size1
	unsigned short addr[8];
	unsigned short actime[2];
	unsigned short modtime[2];
};

struct directory {
	unsigned short inodeNumber;
	char name[14];
};

int fd; //File descriptor of v6 file system
struct superBlock sb; //Superblock
unsigned int tempArray[512];
unsigned int buffer[512];
struct directory dir1;

void freeBlock(unsigned int n);
unsigned int allocFreeBlock();
unsigned short allocINode();
int createEntry(struct inode cNode, struct directory cDir);

int main(){

int i, k;
int j;
int index = 1024;
struct inode s2;
int isMounted = 0;
int count1;
char buffer1[512];
struct directory dir;
struct inode tempNode;
struct inode tempNode1;
struct inode cpinNode;
struct inode rootNode;
struct directory rootDir;
struct directory cpinDir;
struct inode cpoutNode;
struct directory cpoutDir;
struct directory cpoutDir1;
char emptyDName[16];
char emptyNode[32];
char emptyBlock[512];
struct inode rmNode;
struct inode rmNode1;
struct directory rmDir;
char* f1;
char* f2;	
unsigned int cpinInum;
unsigned short cpinBlock;
int f1fd, f2fd, t;
char buff[512];
unsigned short counter;
int dirFound;
while (1)
{
for(i = 0; i < 16; i++)
	emptyDName[i] = 0;
for(i = 0; i < 32; i++)
	emptyNode[i] = 0;
for(i = 0; i < 512; i++)
	emptyBlock[i] = 0;
char command[64];
char *args[64];
int count = 0;
printf("Enter command: ");
fgets(command, sizeof(command), stdin);
char *temp;
//Removes the '\n' character from the input and allocates memory to args (to avoid core dump error)
command[strlen(command) - 1] = 0;
for (i = 0; i < 10; i++)
	args[i] = (char*)malloc(20 * sizeof(char));

temp = strtok(command, " ");
strcpy(args[count++], temp);
while (temp != NULL)
{
	temp = strtok(NULL, " ");
	if (temp == NULL)
		break;
	strcpy(args[count++], temp);
}
args[count] = NULL;
//Quit the program.
if(strcmp("q", command) == 0)
{
	lseek(fd, 512, SEEK_SET);
	write(fd, &sb, 512);
	printf("Program terminated by user.\n");
	return 1;
}
if (strcmp(args[0], "mount") == 0)
{
	//make sure test.data exists. If not, use touch command to create an empty file called test.data
	fd = open(args[1], 2);
	printf("File successfully mounted.\n");
	isMounted = 1;
}
else if (strcmp(args[0], "initfs") == 0)
{
	if(isMounted == 0)
	{
		printf("Please mount a data file before you initiate a file system.\n");
	}
	else
	{
		j = 0;
		int numBlocks = atoi(args[1]);
		int numNodes = atoi(args[2]);
		sb.fsize = numBlocks;
		sb.isize = numNodes;
		sb.ninode = numNodes;
		sb.nfree = 0;
		int dbSize, dbNext;		//dbSize = number of available data blocks
		for(i = 0; i < 100; i++)
		{
			sb.free[i] = 0;		//Clear free[]
		}
		for(i = 0; i < numNodes; i++)
		{
			sb.inode[i] = i;
		}
		
		//Placeholder values
		sb.flock = 'p';
		sb.ilock = 'p';
		sb.fmod = 'p';
		sb.time[0] = 0;
		sb.time[1] = 0;
		
		//Write the superblock to the fs
		lseek(fd, 512, SEEK_SET);
		write(fd, &sb, 512);
		lseek(fd, 1024, SEEK_SET);
		
		
		for(i = 0; i < numBlocks; i++)
		{
			write(fd, buffer, 512);
		}
		dbSize = numBlocks - sb.isize - 2;
		while(dbSize >= 100)
		{
			if(j == 0)
			{
				freeBlock(2 + sb.isize);
			}
			else
			{
				freeBlock(2 + sb.isize + (100 * j - 1));
			}
			j++;
			dbSize -= 100;
		}
		dbNext = 2 + sb.isize + (100 * j) - 1;
		lseek(fd, 512 * dbNext, SEEK_CUR);
		while(dbSize > 0 && dbSize < 100)
		{
			sb.free[sb.nfree] = dbNext;
			dbSize--;
			dbNext++;
			sb.nfree++;
		}
		
		
		//Create root directory
		unsigned short rootBlock;
		sb.nfree--;
		rootBlock = sb.free[sb.nfree];
		if(sb.nfree == 0)
		{
			lseek(fd, rootBlock * 512, SEEK_SET);
			read(fd, buffer, 512);
			sb.nfree = buffer[0];
			for(i = 0; i < 100; i++)
			{
				sb.free[i] = buffer[1 + i];
			}
		}
		for (i=0;i<14;i++)
			rootDir.name[i] = 0;
		rootDir.name[0] = '.';
		rootDir.name[1] = '\0';
		rootDir.inodeNumber = 1;
		rootNode.flag = 0140000;
		rootNode.nlinks = 2;
		rootNode.uid = '0';
		rootNode.gid = '0';
		rootNode.fsize1 = '0';
		rootNode.fsize2 = 32;
		for (i=1;i<8;i++)
			rootNode.addr[i] = 0;
		rootNode.addr[0] = rootBlock;
		rootNode.actime[0] = 0;
		rootNode.modtime[0] = 0;
		rootNode.modtime[1] = 0;
		lseek(fd, 2 * 512 + 0 * 32, SEEK_SET);
		write(fd, &rootNode, 32);
		lseek(fd, rootBlock * 512, SEEK_SET);
		write(fd, &rootDir, 16);
		rootDir.name[1] = '.';
		rootDir.name[2] = '\0';
		
		printf("File system initiated.\n");
	}
}
else if (strcmp(args[0], "mkdir") == 0)
{
	char* dname = args[1];
	unsigned int bNum = sb.free[sb.nfree];
	unsigned int nNum = allocINode();
	unsigned int pNum = 1;
	char mkdirBuf[512];
	strncpy(dir.name, dname, 14);
	dir.inodeNumber = nNum;
	lseek(fd, 2 * 512, SEEK_SET);
	
	//Create directory inode
	tempNode1.flag = 0140000;
	tempNode1.nlinks = 2;
	tempNode1.uid = '0';
	tempNode1.gid = '0';
	tempNode1.fsize1 = '0';
	tempNode1.fsize2 = 32;
	memset(tempNode1.addr, 0, 8 * sizeof(tempNode.addr[0]));
	tempNode1.addr[0] = bNum;
	tempNode1.actime[0] = 0;
	tempNode1.modtime[0] = 0;
	tempNode1.modtime[1] = 0;
	
	lseek(fd, 1024 + (nNum * sb.isize), SEEK_SET);
	write(fd, &tempNode1, sb.isize);
	lseek(fd, 1024, SEEK_SET);
	int blck = read(fd, &tempNode1, sizeof(tempNode1));
	tempNode1.nlinks++;
	
	createEntry(tempNode1, dir);
	for(i = 0; i < 512; i++)
	{
		mkdirBuf[i] = 0;
	}
	
	memcpy(mkdirBuf, &nNum, sizeof(nNum));
	mkdirBuf[2] = '.';
	mkdirBuf[3] = '\0';
	
	memcpy(mkdirBuf, &pNum, sizeof(pNum));
	mkdirBuf[18] = '.';
	mkdirBuf[19] = '.';
	mkdirBuf[20] = '\0';
	lseek(fd, bNum * 512, 0);
	write(fd, mkdirBuf, 512);
	printf("Directory created.\n");
}
else if (strcmp(args[0], "cpin") == 0)
{
	f1 = args[1];
	f2 = args[2];
	off_t cpinSize;
	i = 0;
	if((f1fd = open(f1, O_RDONLY)) == -1)
	{
		printf("File opened unsuccessfully.\n");
		break;
	}
	f2fd = open(f2, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	cpinSize = lseek(f1fd, 0, SEEK_END);
	
	//Create inode
	if(sb.ninode > 0)
	{
		cpinInum = allocINode();
		cpinNode.flag = 0100000;
		cpinNode.nlinks = 1;
		cpinNode.uid = '0';
		cpinNode.gid = '0';
		cpinNode.fsize1 = '0';
		cpinNode.fsize2 = cpinSize;
		cpinNode.actime[0] = 0;
		cpinNode.modtime[0] = 0;
		cpinNode.modtime[1] = 0;
	}

	if(cpinInum < 0)
	{
		printf("There are no more available inodes.\n");
		break;
	}
	
	cpinDir.inodeNumber = cpinInum;
	memcpy(cpinDir.name, f2, strlen(f2));
	lseek(f1fd, 0, SEEK_SET);
	while(cpinSize > 0)
	{
		for(j = 0; j < 512; j++)
		{
			buff[j] = '\0';
		}
		cpinBlock = allocFreeBlock();
		t = read(f1fd, buff, 512);
		if(t < 0)
		{
			printf("cpin error.\n");
			printf("Program terminated.\n");
			exit(0);
		}
		if(t == 0 && i < 8)
		{
			printf("File copy successful.\n");
			break;
		}
		
		cpinNode.addr[i] = cpinBlock;
		lseek(fd, cpinBlock * 512, SEEK_SET);
		write(fd, buff, 512);
		cpinSize -= 512;
		i++;
		if(i > 7)
		{
			printf("Large files are not supported.\n");
			printf("Program terminated.\n");
			exit(0);
		}
		
	}
	
	lseek(fd, 1024 + (cpinInum * 32), SEEK_SET);
	write(fd, &cpinNode, 32);
	
	lseek(fd, 1024, SEEK_SET);
	read(fd, &cpinNode, 32);
	createEntry(cpinNode, cpinDir);
	close(f1fd);
	close(f2fd);
}
else if (strcmp(args[0], "cpout") == 0)
{
	char* f3 = args[1];
	char* f4 = args[2];
	char bufff[512];
	int outfd;
	dirFound = 0;
	int outNode = 0;
	int blocksUsed = 0;
	int extraBytes = 0;
	if((outfd = open(f4, O_RDWR | O_CREAT, 0600)) == -1)
	{
		printf("File opened unsuccessfully.\n");
		break;
	}
	
	//Find source file
	lseek(fd, 1024, SEEK_SET);
	read(fd, &cpoutNode, 32);
	for(counter = 0; counter <= 7; counter++)
	{
		if(dirFound != 1)
		{
			lseek(fd, (cpoutNode.addr[counter] * 512), SEEK_SET);
			for(i = 0; i < 32; i++)
			{
				read(fd, &cpoutDir, 16);
				if(strcmp(cpoutDir.name, f3) == 0)
				{
					outNode = cpoutDir.inodeNumber;
					dirFound = 1;
				}
			}
		}
	}
	
	if(outNode == 0)
	{
		printf("Source file not found.\n");
		break;
	}
	
	//Write to external file
	lseek(fd, 2 * 512 + 32 * outNode, SEEK_SET);
	read(fd, &cpoutNode, 32);
	
	blocksUsed = (int)ceil(cpoutNode.fsize2 / 512.0);
	extraBytes = cpoutNode.fsize2 % 512;
	for(i = 0; i < blocksUsed; i++)
	{
		lseek(fd, (cpoutNode.addr[i]) * 512, SEEK_SET);
		read(fd, bufff, 512);
		if(i == (blocksUsed - 1))
		{
			write(outfd, bufff, extraBytes);
			printf("cpout successful.\n");
			break;
		}
		else
		{
			write(outfd, bufff, 512);
		}
	}
	close(outfd);
}
else if (strcmp(args[0], "rm") == 0)
{
	char* f5 = args[1];
	lseek(fd, 1024, SEEK_SET);
	read(fd, &rmNode, 32);
	dirFound = 0;
	int rmNum = 0;
	
	//Find file to be removed
	lseek(fd, 1024, SEEK_SET);
	read(fd, &rmNode, 32);
	for(counter = 0; counter <= 7; counter++)
	{
		if(dirFound != 1)
		{
			lseek(fd, (rmNode.addr[counter] * 512), SEEK_SET);
			for(i = 0; i < 32; i++)
			{
				read(fd, &rmDir, 16);
				if(strcmp(rmDir.name, f5) == 0)
				{
					rmNum = rmDir.inodeNumber;
					dirFound = 1;
				}
			}
		}
	}
	
	if(rmNum == 0)
	{
		printf("Source file not found.\n");
		break;
	}
	
	//Free inode for file that was removed
	lseek(fd, 1024 + 32 * rmNum, SEEK_SET);
	write(fd, emptyNode, 32);
	sb.nfree++;
	sb.free[sb.nfree] = rmNode.addr[0];
	sb.ninode++;
	sb.inode[sb.ninode] = rmDir.inodeNumber;
	rmNode.addr[0] = 0;
	rmDir.inodeNumber = 0;
	rmDir.name[0] = '\0';
	
	printf("File removed successfully.\n");	
}
}
printf("Program terminated.\n");
}

//Add block to free list
void freeBlock(unsigned int n)
{
	int i;
	if(n == sb.isize + 2)
	{
		lseek(fd, 50688, SEEK_CUR);
	}
	else
	{
		lseek(fd, 51200, SEEK_CUR);
	}
	while(sb.nfree != 100)
	{
		sb.free[sb.nfree] = n;
		sb.nfree++;
		n++;
	}
	write(fd, &sb.nfree, sizeof(sb.nfree));
	write(fd, sb.free, sizeof(sb.free));
	sb.nfree = 0;
	for(i = 0; i < 100; i++)
	{
		sb.free[i] = 0;
	}
}

//Allocate a free block
unsigned int allocFreeBlock()
{
	sb.nfree--;
	unsigned int block = sb.free[sb.nfree];
	unsigned short buffer[512];
	sb.free[sb.nfree] = 0;
	
	if(sb.nfree == 0)
	{
		int i;
		lseek(fd, block * 512, SEEK_SET);
		read(fd, buffer, 512);
		sb.nfree = buffer[0];
		for(i = 0; i < 100; i++)
		{
			sb.free[i] = buffer[i + 1];
		}
	}
	return block;
}

//Allocate an inode
unsigned short allocINode()
{
	unsigned short iNum;
	sb.ninode--;
	iNum = sb.inode[sb.ninode];
	return iNum;
}

int createEntry(struct inode cNode, struct directory cDir)
{

	unsigned short index = 0;
	char entryBuffer[512];	//Buffer that stores inodeNumber and name
	int i = 0;
	
	//Find an unallocated inode
	for(index = 0; index <= 7; index++)
	{
		lseek(fd, cNode.addr[index] * 512, SEEK_SET);
		read(fd, entryBuffer, 512);
		for(i = 0;i < 32; i++)
		{
			if(entryBuffer[16 * i] == 0)
			{
				memcpy(entryBuffer + 16 * i, &cDir.inodeNumber, sizeof(cDir.inodeNumber));
				memcpy(entryBuffer + 16 * i + sizeof(cDir.inodeNumber), &cDir.name,sizeof(cDir.name));
				lseek(fd, cNode.addr[index] * 512, SEEK_SET);
				write(fd, entryBuffer, 512);
			}
		}
	}
}