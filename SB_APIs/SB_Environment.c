#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//==========================================================================
void SB_GetVersion (int type, char *ver)
{
char fname[20];
unsigned char buff[60];
int fd, i;

	strcpy ((char *)ver, "2.1.0.1");		// default

	switch (type)
		{
		case 'b' :
		case 'B' :  strcpy (fname, "/etc/b_name");  break;
		case 'k' :	
		case 'K' :  strcpy (fname, "/etc/k_name");  break;
		case 'f' :	
		case 'F' :  strcpy (fname, "/etc/f_name");  break;						
		default  :  return;
		}
	
	fd = open (fname, O_RDWR);
	if (fd <= 0) return;
	lseek (fd, 0l, 0);
	if (read (fd, buff, 0x20) <= 0) return;
	close (fd);
	for (i=0; i<0x20; i++)
		{
		if (buff[i] >= 0x30 && buff[i] <= 0x39) break;
		}
	if (i==0x20) return;
	memset (ver, 0x00, strlen(ver)+1);
	if (buff[i+7] == '.')
		memcpy (ver, &buff[i], 7);
	else
		memcpy (ver, &buff[i], 8);
} 
//===============================================================================
int SB_ReadConfig (char *FileName, char *Dest, int Get_Size)
{
	int fd;
	fd = open((char *)FileName, O_RDONLY);
	if (fd <= 0) return -1;
	lseek(fd, 0, 0);
	read (fd, Dest, Get_Size);
	close (fd);
	return 1;
}
//===============================================================================
int SB_WriteConfig (char *FileName, char *Source, int Put_Size)
{
	int fd;
	fd = open((char *)FileName, O_RDWR|O_CREAT);
	if (fd <= 0) return -1;
	lseek(fd, 0, 0);
	write (fd, Source, Put_Size);
	close (fd);
	chmod ((char *)FileName, S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	return 1;
}
//===============================================================================
void *SB_GetSharedMemory (int Key_ID, int Shared_Size)
{
void *shared_memory = (void *)0;	
int	ID = -1;
	
	ID = shmget((key_t)Key_ID, Shared_Size, 0);
	if (ID == -1)	return (void *)-1;	

	shared_memory = shmat(ID, (void *)0, 0);
	if (shared_memory != (void *)-1)
		return (void *)(shared_memory);
	else
		return (void *)-1;	
}
//===============================================================================
void *SB_SetSharedMemory (int Key_ID, int Shared_Size)
{
void *shared_memory = (void *)0;	
int	ID = -1;

	ID = shmget((key_t)Key_ID, Shared_Size, 0666|IPC_CREAT|IPC_EXCL);
	if (ID == -1)
		{
		ID = shmget((key_t)Key_ID, Shared_Size, 0);
		if (ID == -1)	return (void *)-1;	
		}

	shared_memory = shmat(ID, (void *)0, 0);
	if (shared_memory != (void *)-1)
		return (void *)(shared_memory);
	else
		return (void *)-1;	
}	

