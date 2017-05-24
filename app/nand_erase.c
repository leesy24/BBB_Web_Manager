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
//===============================================================================	
int main ()
{
int mtd_fd;
int nand_size, ret = 0;
char buff[255];

	mtd_fd = open ("/dev/mtd6", O_RDWR | O_SYNC);
	if (mtd_fd < 0)
		{
		printf("/dev/mtd6 Open Error! fd = %d \n", mtd_fd);
		return 0;
		}	
	
	lseek (mtd_fd, 0, 0);	

	nand_size = (256*1024*1024);
	
	ret = erase_flash (mtd_fd, nand_size);
	
	close (mtd_fd);
	
	return 0;
	
}

//==============================================================================
int erase_flash (int fd, unsigned int bytes)
{
	int ret, sp=0;
	struct erase_info_user erase;
	struct mtd_info_user info;
	int page_num =0;

	ioctl (fd, MEMGETINFO, &info);
	
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
