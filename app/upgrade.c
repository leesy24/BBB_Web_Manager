/*****************************************************************************
  Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

  Flash Updater (BootLoader or Kernel or FileSystem)		09, 31, 2007. yhlee
 ******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_ioctl.h"
#include <fcntl.h>

struct erase_info_user {
	uint32_t start;
	uint32_t length;
};

struct mtd_info_user {
	uint8_t type;
	uint32_t flags;
	uint32_t size;   // Total size of the MTD
	uint32_t erasesize;
	uint32_t writesize;
	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	uint32_t ecctype;
	uint32_t eccsize;
};

int web_flag = 0;
#define MEMERASE		_IOW('M', 2, struct erase_info_user)
#define MEMGETINFO      _IOR('M', 1, struct mtd_info_user)

int erase_flash (int fd, unsigned int bytes);
unsigned int copy_flash (int fw_fd, int mtd_fd, int file_size);
unsigned int verify_flash (int mtd_fd, int file_size);
unsigned int get_CRC16(unsigned char data, unsigned int crc16);
int upgrade (char *filename);
void chk_old_version ();
//===============================================================================	
int main (int argc, char *argv[])
{
char name[100], listname[100], filename[100], cmd[90];
FILE *fd;
int i, flag=0, len, ret;

	
	if (argc != 2)
		{
		printf("Usage: upgrade <image filename>\n");
		return 0;
		}

//	chk_old_version ();	
	strcpy (filename, argv[1]);
	len = strlen(filename);

	if (strncmp (filename, "/tmp/image_file", 9) == 0)		// WEB upgrade
		{
		web_flag = 1;
		}	

	if (strncmp (&filename[len-3], "bin", 3) == 0)		// xx.bin  upgrade
		{
		ret = upgrade (filename);
		if (ret < 0) upgrade (filename);
//		printf("Update Complete\n\n");
		return 0;
		}
		
	sprintf (cmd, "/bin/tar xvf %s -C /tmp > /tmp/list", filename);
	system (cmd);
	if ((fd = fopen("/tmp/list", "r")) == NULL) return 0;
	while (1) 
		{
		if (fgets(name, 80, fd) == NULL) 
			{
			if (flag==0) 
				{
				ret = upgrade (filename);
				if (ret < 0) upgrade (filename);
				}
			break;
			}
		flag = 1;
		sprintf (listname, "/tmp/%s", name);
		for (i=0; i<100; i++)
			{
			if (listname[i] == 0x0d || listname[i] == 0x0a) 
				{
				listname[i] = 0;
				break;
				}
			}
		printf ("upgrade file is %s \n", listname);	
		ret = upgrade (listname);
		if (ret < 0) upgrade (listname);
		unlink  (listname);
		sleep (1);
		}
	fclose(fd);
	unlink  ("/tmp/list");
	
	if (web_flag)	printf("<BR><BR>Rebooting.....<br>\n");
	return 0;
}
//---------------------------------------------------------------------------
int upgrade (char *filename)
{
unsigned int crc16A=0xffff;
unsigned int crc16B=0xffff;
int fw_fd, mtd_fd, i;
int mtd_no, fw_size, ret = 0;
char buff[255];

	
	fw_fd = open(filename, O_RDWR);
	if (fw_fd < 0)
		{
		printf("Update (%s) Image file not found!\n", filename);
		return 0;
		}

	lseek(fw_fd, 0x20, 0);
	read(fw_fd, buff, 32);
	close(fw_fd);

	if ((strncmp (filename, "/tmp/eddy-bs-", 13) == 0) || (strncmp (filename, "eddy-bs-", 8) == 0))
		{
		printf("Sorry !, USB_Recovery is only updated bootsrap \n");
		return 0;
		}

	for (i=0; i<32; i++)
		{
		if (buff[i] == '-' || buff[i] == '_') break;
		}	
		
	if (i==32)
		{
		printf("Update (%s) Image file Mismatch !\n", filename);
		return 0;
		}

	buff[i+1] = tolower( buff[i+1]);	
	buff[i+2] = tolower( buff[i+2]);	

	if (strncmp (&buff[i+1], "bs", 2) == 0)								// bootstrap
		{
		printf("Sorry !, USB_Recovery is only updated bootsrap \n");
		return 0;
		}
	else
		if (strncmp (&buff[i+1], "bl", 2) == 0) 						// blotloader
			mtd_no = 2;
		else		
			if (strncmp (&buff[i+1], "kr", 2) == 0) 					// os
				mtd_no = 3;
			else
				if (strncmp (&buff[i+1], "os", 2) == 0) 				// os
					mtd_no = 3;
				else
					if (strncmp (&buff[i+1], "fs", 2) == 0) 			// filesystem
						mtd_no = 4;
					else	
						mtd_no = 99;
up:
	if (mtd_no == 99) 
		{
		printf("/dev/mtd%d Not Found!\n", mtd_no);
		return 0;
		}

	sprintf (buff, "/dev/mtd%d", mtd_no);
	mtd_fd = open (buff, O_RDWR | O_SYNC);
	if (mtd_fd < 0)
		{
		printf("/dev/mtd%d Open Error! ret = %d \n", mtd_no, mtd_fd);
		return 0;
		}	
	lseek (mtd_fd, 0, 0);	

	fw_fd = open(filename, O_RDWR);
	if (fw_fd < 0)
		{
		printf("Update (%s) Image file not found!\n", filename);
		return 0;
		}

	fw_size = lseek(fw_fd, 0, SEEK_END);
	lseek (fw_fd, 0, 0);

	switch (mtd_no)
		{
		case 0 : 
			if (fw_size >= (8*1056)) 
				{
				printf ("The size of the Bootstrap too Big !\n");
				goto endd;
				}
			printf ("Bootstrap Erase ... %d Bytes, ", fw_size); 
			break;
		case 2: 
			if (fw_size >= (236*1056)) 
				{
				printf ("The size of the BootLoader too Big !\n");
				goto endd;
				}
			printf ("BootLoader Erase ... %d Bytes, ", fw_size); 
			break;
		case 3: 
			if (fw_size >= (2100*1024)) 
				{
				printf ("The size of the Kernel too Big !\n");
				goto endd;
				}			
			printf ("Kernel     Erase ... %d Bytes, ", fw_size); 
			break;
		case 4 : 
			if (fw_size >= (4261*1024)) 
				{
				printf ("The size of the FileSystem too Big !\n");
				goto endd;
				}			
			printf ("FileSystem Erase ... %d Bytes, ", fw_size); 
			break;

		}
	if (erase_flash (mtd_fd, fw_size) < 0)
		{
		printf ("Erase Error!\n");
		goto end;
		}	

	switch (mtd_no)
		{
		case 0 : 
			printf ("Bootstrap  Write ... %d Bytes, %s\n", fw_size, filename); 
			break;
		case 2 : 
			printf ("BootLoader Write ... %d Bytes, %s\n", fw_size, filename); 
			break;
		case 3 : 
			printf ("Kernel     Write ... %d Bytes, %s\n", fw_size, filename); 
			break;
		case 4 : 
			printf ("FileSystem Write ... %d Bytes, %s\n", fw_size, filename); 
			break;
		}

	crc16A = copy_flash (fw_fd, mtd_fd, fw_size);
	if (crc16A == 0) goto end;
	printf ("Flash Write OK\n");
	if (web_flag)	printf("<BR>");
	crc16B = verify_flash(mtd_fd, fw_size);
	if (crc16B == 0) goto end;

	if (crc16A != crc16B)
		{
		printf ("Flash Verify Failed!  crc16A = 0x%x, crc16B = 0x%x\n",crc16A,crc16B);
		goto end;
		}

	printf("Flash Verify OK\n");
	if (web_flag)	printf("<BR>");
	ret = 0;
	printf("Update Complete\n\n"); 
	goto endd;
end:
	printf ("Update Failed !!\n");
	ret = -1;
endd:
	close (mtd_fd);
	close (fw_fd);
	return (ret);
}
//==============================================================================
int erase_flash (int fd, unsigned int bytes)
{
	int ret, sp=0;
	struct erase_info_user erase;
	struct mtd_info_user info;
	int page_num =0;

	ioctl (fd, MEMGETINFO, &info);

	if (info.erasesize == 0x2000) 
		{
		printf ("\nEON Flash \n");
		return 0;
		}
		
	printf("info.erasesize = %d\n",info.erasesize);
	page_num = (bytes / info.erasesize) + 1;
	bytes = page_num * info.erasesize;

	erase.start = sp;
	erase.length = bytes ;
	ret = (ioctl (fd, MEMERASE, &erase));
	if (ret < 0)
	{
		perror ("MEMERASE");
		return -1;
	}
	return (0);
}
//==============================================================================
unsigned int copy_flash (int fw_fd, int mtd_fd, int file_size)
{
#define  READ_SIZE	128*1056
	char buffer [READ_SIZE];
	int read_size, r_len, w_len, i, total_len, write_len=0;
	unsigned int crc16A=0xffff;
	
	total_len = file_size;
	printf ("\n");
	
	while (1)
	{
		if (file_size < READ_SIZE) 
			read_size = file_size;
		else
			read_size = READ_SIZE;

		r_len = read (fw_fd, buffer, read_size);
		if (r_len < 0) 
		{
			printf ("Firmware read Error!\n");
			return 0;
		}

		for (i=0;i<r_len;i++) 
		{
			crc16A = get_CRC16 (buffer[i], crc16A);
		}
		w_len = write (mtd_fd, buffer, r_len);
		if (w_len != r_len) 
		{
			printf ("MTD write Error! write=%d read=%d  point=%d\n", w_len, r_len ,  file_size);
			return 0;
		}

		file_size -= w_len;
		write_len += w_len;

		if (web_flag==0)
			printf ("[1A%7d  (%7d bytes)\n", write_len, total_len);
		else
			printf ("%7d  (%7d bytes)\n<BR>", write_len, total_len);
			
		if (file_size == 0) return crc16A;
	}
}
//==============================================================================
unsigned int verify_flash (int mtd_fd, int file_size)
{
// shlee change the VREAD_SIZE : data flash can read page size
#define  VREAD_SIZE	1056
	char buffer [VREAD_SIZE];
	int read_size, r_len, i;
	unsigned int crc16A=0xffff;

	printf ("\nVerifying ......\n");
	if (web_flag)	printf("<BR>");
		
	lseek (mtd_fd, 0, 0);
	while (1)
	{

		if (file_size < VREAD_SIZE) 
			read_size = file_size;
		else
			read_size = VREAD_SIZE;

		r_len = read (mtd_fd, buffer, read_size);
		if (r_len < 0) 
		{
			printf ("Flash read Error!\n");
			return 0;
		}

		for (i=0;i<r_len;i++) 
		{
			crc16A = get_CRC16 (buffer[i], crc16A);
		}
		file_size -= r_len;
//		putchar ('.');
		if (file_size == 0) return crc16A;
	}
}
/*=============================================================================*/
unsigned int get_CRC16(unsigned char data, unsigned int crc16) 
{ 
	int i; 
	unsigned int temp_crc16; 
	unsigned char temp_data; 

	temp_crc16 = crc16; 
	temp_data = (unsigned char) data; 

	temp_data <<=1; 
	for ( i = 8; i > 0; i--){ 

		temp_data >>= 1; 
		if ((temp_data ^ temp_crc16) & 0x0001) 
			temp_crc16 = (temp_crc16 >> 1) ^ 0x8408; 
		else 
			temp_crc16 >>= 1; 
	} 
	return(temp_crc16); 
} 	
/*=============================================================================*/
void chk_old_version () 
{
unsigned char buff[60];
int fd, i;

	
	fd = open ("/etc/k_name", O_RDWR);
	if (fd <= 0) return;
	lseek (fd, 0l, 0);
	if (read (fd, buff, 0x20) <= 0) return;
	close (fd);
	for (i=0; i<0x20; i++)
		{
		if (buff[i] >= 0x30 && buff[i] <= 0x39) break;
		}
	if (i==0x20) return;
	
	if (buff[i]=='2' && buff[i+2] == '1')		
		return;									// check 2.0 version

	printf ("Sorry refused update!\n");
	printf ("DK board should be upgraded through the boot loader.\n");
	exit (0);
}
