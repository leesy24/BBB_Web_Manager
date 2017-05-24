/*
 *    Filename: cfag12864b-example.c
 *     Version: 0.1.0
 * Description: cfag12864b LCD userspace example program
 *     License: GPLv2
 *
 *      Author: Copyright (C) Miguel Ojeda Sandonis <maxextreme@gmail.com>
 *        Date: 2006-10-31
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * ------------------------
 * start of cfag12864b code
 * ------------------------
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <stdarg.h>
#include "lcd_image/eddy_glcd.h"
#include "lcd_image/linux_glcd.h"
#include "lcd_image/sysbas_glcd.h"

#include "lcd_font/font_8x8.h"


#define CFAG12864B_WIDTH		(128)
#define CFAG12864B_HEIGHT		(64)
#define CFAG12864B_SIZE			(128 * 64 / 8)
#define CFAG12864B_BPB			(8)
#define CFAG12864B_ADDRESS(x, y)	((y) * CFAG12864B_WIDTH / \
		CFAG12864B_BPB + (x) / CFAG12864B_BPB)
#define CFAG12864B_BIT(n)		(((unsigned char) 1) << (n))

#undef CFAG12864B_DOCHECK
#ifdef CFAG12864B_DOCHECK
#define CFAG12864B_CHECK(x, y)		((x) < CFAG12864B_WIDTH && \
		(y) < CFAG12864B_HEIGHT)
#else
#define CFAG12864B_CHECK(x, y)		(1)
#endif

unsigned char * cfag12864b_mem;
unsigned char cfag12864b_buffer[CFAG12864B_SIZE];

int cfag12864b_init(char *path);
void cfag12864b_exit(int fd);
void cfag12864b_set(unsigned char x, unsigned char y);
void cfag12864b_unset(unsigned char x, unsigned char y);
unsigned char cfag12864b_isset(unsigned char x, unsigned char y);
void cfag12864b_not(unsigned char x, unsigned char y);
void cfag12864b_fill(void);
void SB_LCDclear(void);
void cfag12864b_format(unsigned char * matrix);
int SB_LCDprintimage(unsigned char * tmp_buffer);
int SB_LCDputchar(int loc_x, int loc_y, char ascii );
int SB_LCDprintf(int x, int y, const char *fmt, ...);
void SB_LCDwrite(void);
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
int lcdfd;
char ch;


	lcdfd = cfag12864b_init("/dev/fb0");
	if (lcdfd < 0) 
		{
		system ("insmod /etc/drivers/pca9539.ko");			// DIO and LCD 
		system ("insmod /etc/drivers/cfbcopyarea.ko");
		system ("insmod /etc/drivers/cfbfillrect.ko");
		system ("insmod /etc/drivers/cfbimgblt.ko");
		system ("insmod /etc/drivers/eddy_lcd.ko");
		system ("insmod /etc/drivers/eddy_lcdfb.ko");
		sleep (3);
		lcdfd = cfag12864b_init("/dev/fb0");
		if (lcdfd < 0) 
			{
			printf("Can't init /dev/fb0 fbdev ret = %d \n", lcdfd);
			return -2;
			}
		}

	SB_LCDclear ();

	SB_LCDprintimage((unsigned char *)sysbas_glcd_bmp);
	ch = getchar();
	while (ch != '\n');

	SB_LCDprintimage((unsigned char *)eddy_glcd_bmp);
	ch = getchar();
	while (ch != '\n');

	SB_LCDprintimage((unsigned char *)linux_glcd_bmp);
	ch = getchar();
	while (ch != '\n');

	SB_LCDprintf(0, 6, "Welcome to Eddy\n");
	SB_LCDprintf(0 ,7, "%s", "Development Kit");
	SB_LCDwrite ();
	
	cfag12864b_exit(lcdfd);
	return 0;
}



/*
 * init a cfag12864b framebuffer device
 *
 * No error:       return = 0
 * Unable to open: return = -1
 * Unable to mmap: return = -2
 */
int cfag12864b_init(char *path)
{
int cfag12864b_fd;
	
	cfag12864b_fd = open(path, O_RDWR);
	if (cfag12864b_fd == -1)
		return -1;

	cfag12864b_mem = mmap(0, CFAG12864B_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, cfag12864b_fd, 0);
	if (cfag12864b_mem == MAP_FAILED) 
		{
		close(cfag12864b_fd);
		return -2;
		}

	return cfag12864b_fd;
}

/*
 * exit a cfag12864b framebuffer device
 */
void cfag12864b_exit(int fd)
{
	munmap(cfag12864b_mem, CFAG12864B_SIZE);
	close(fd);
}

/*
 * set (x, y) pixel
 */
void cfag12864b_set(unsigned char x, unsigned char y)
{
	if (CFAG12864B_CHECK(x, y))
		cfag12864b_buffer[CFAG12864B_ADDRESS(x, y)] |=
			CFAG12864B_BIT(x % CFAG12864B_BPB);
}

/*
 * unset (x, y) pixel
 */
void cfag12864b_unset(unsigned char x, unsigned char y)
{
	if (CFAG12864B_CHECK(x, y))
		cfag12864b_buffer[CFAG12864B_ADDRESS(x, y)] &=
			~CFAG12864B_BIT(x % CFAG12864B_BPB);
}

/*
 * is set (x, y) pixel?
 *
 * Pixel off: return = 0
 * Pixel on:  return = 1
 */
unsigned char cfag12864b_isset(unsigned char x, unsigned char y)
{
	if (CFAG12864B_CHECK(x, y))
		if (cfag12864b_buffer[CFAG12864B_ADDRESS(x, y)] &
				CFAG12864B_BIT(x % CFAG12864B_BPB))
			return 1;

	return 0;
}

/*
 * not (x, y) pixel
 */
void cfag12864b_not(unsigned char x, unsigned char y)
{
	if (cfag12864b_isset(x, y))
		cfag12864b_unset(x, y);
	else
		cfag12864b_set(x, y);
}

/*
 * fill (set all pixels)
 */
void cfag12864b_fill(void)
{
	unsigned short i;

	for (i = 0; i < CFAG12864B_SIZE; i++)
		cfag12864b_buffer[i] = 0xFF;
}

/*
 * clear (unset all pixels)
 */
void SB_LCDclear(void)
{
	unsigned short i;

	for (i = 0; i < CFAG12864B_SIZE; i++)
		cfag12864b_buffer[i] = 0;
	SB_LCDwrite ();	
}

/*
 * format a [128*64] matrix
 *
 * Pixel off: src[i] = 0
 * Pixel on:  src[i] > 0
 */
void cfag12864b_format(unsigned char * matrix)
{
	unsigned char i, j, n;

	for (i = 0; i < CFAG12864B_HEIGHT; i++)
	{
		for (j = 0; j < CFAG12864B_WIDTH / CFAG12864B_BPB; j++) 
		{
			cfag12864b_buffer[i * CFAG12864B_WIDTH / CFAG12864B_BPB + j] = 0;
			for (n = 0; n < CFAG12864B_BPB; n++)
			{
				if (matrix[i * CFAG12864B_WIDTH + j * CFAG12864B_BPB + n])
					cfag12864b_buffer[i * CFAG12864B_WIDTH / CFAG12864B_BPB + j] |= CFAG12864B_BIT(n);
			}
		}
	}
}
/*
 * deformat
 */ 

int SB_LCDprintimage(unsigned char * tmp_buffer)
{
	unsigned char n;
	int i,j,k;
	unsigned char tmp_matrix[CFAG12864B_WIDTH * CFAG12864B_HEIGHT];
	unsigned char new_matrix[CFAG12864B_WIDTH * CFAG12864B_HEIGHT];

	for (i = 0; i < 1024; i++)
		{
			for (n = 0; n < CFAG12864B_BPB; n++)
			{
				if(tmp_buffer[i] & CFAG12864B_BIT(n))
				{
					tmp_matrix[i * CFAG12864B_BPB + n] = 1;
				}
				else
				{
					tmp_matrix[i * CFAG12864B_BPB + n] = 0;
				}
			}
		}

	for (i = 0; i < 1024; i++)
		{
			for (n = 0; n < CFAG12864B_BPB; n++)
			{
				j = i*8 + n;
				k = 128*n + i + ((int)(i/128) * 1024) - 128 * ((int)(i/128));
				new_matrix [k] = tmp_matrix[j];
			}
		}
	cfag12864b_format(new_matrix);
	SB_LCDwrite();
	return 0;
}
int SB_LCDputchar(int loc_x, int loc_y, char ascii )
{
	int font_x, font_y;
	int i;
	char tmp_buffer[8][8][16];
	
	memcpy(tmp_buffer,cfag12864b_buffer,CFAG12864B_SIZE);	
	
	font_x = ((int)ascii) % 16;
	font_y = (int) (ascii / 16);
		
//	printf("(%d,%d \n",loc_x,loc_y);
		
	for(i = 0; i < 8; i++)
	{
		tmp_buffer[loc_y][i][loc_x] = fontdata_8x8[font_y][i][font_x];
//		printf("(%d,%d,(%02x)(%02x)\n",font_x,font_y,fontdata_8x8[font_y][i][font_x],(int)ascii);
	}
	
	memcpy(cfag12864b_buffer,tmp_buffer,CFAG12864B_SIZE);	
		
	return 0;
}

//==========================================================================
int SB_LCDprintf(int x, int y, const char *fmt, ...)  // x=0~15, y=0~7
{	
int i;
va_list args;
char buf[300];
	
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

	if (x > 15 || y > 7) return -1;
	for(i=0; i<strlen(buf); i++)
		{
		if(x > 15)
			{
			y++;
			x = 0;
			}
		if(y > 7) break;
		if (buf[i] == 0x0a)
			{
			x = 0;  
			y ++;
			}
		else		
			{
			SB_LCDputchar(x++, y, buf[i] );
			}
		}

	return (i);
}


/*
 * blit buffer to lcd
 */

void SB_LCDwrite(void)
{
	memcpy(cfag12864b_mem, cfag12864b_buffer, CFAG12864B_SIZE);
}


