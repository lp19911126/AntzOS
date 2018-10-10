#include "todo.h"
#include <stdio.h>
#include <string.h>
extern struct FIFO8 keyfifo, mousefifo;
void enable_mouse(void);
void init_keyboard(void);
void new_pe(struct BOOTINFO *binfo);

int write_x = 55 ;
int write_y = 57 ;

char* replace_char(char s[40]){
	char *chr = "&";
	if(strcmp(s,"1E")==0){
			chr = "A" ;
	}else if(strcmp(s,"30")==0){
			chr = "B" ;
	}else if(strcmp(s,"2E")==0){
			chr = "C" ;
	}else if(strcmp(s,"20")==0){
			chr = "D" ;
	}else if(strcmp(s,"12")==0){
			chr = "E" ;
	}else if(strcmp(s,"21")==0){
			chr = "F" ;
	}else if(strcmp(s,"22")==0){
			chr = "G" ;
	}else if(strcmp(s,"23")==0){
			chr = "H" ;
	}else if(strcmp(s,"17")==0){
		chr = "I" ;
	}else if(strcmp(s,"24")==0){
		chr = "J" ;
	}else if(strcmp(s,"25")==0){
		chr = "K" ;
	}else if(strcmp(s,"26")==0){
		chr = "L" ;
	}else if(strcmp(s,"32")==0){
		chr = "M" ;
	}else if(strcmp(s,"31")==0){
		chr = "N" ;
	}else if(strcmp(s,"18")==0){
		chr = "O" ;
	}else if(strcmp(s,"19")==0){
		chr = "P" ;
	}else if(strcmp(s,"10")==0){
		chr = "Q" ;
	}else if(strcmp(s,"13")==0){
		chr = "R" ;
	}else if(strcmp(s,"1F")==0){
		chr = "S" ;
	}else if(strcmp(s,"14")==0){
		chr = "T" ;
	}else if(strcmp(s,"16")==0){
		chr = "U" ;
	}else if(strcmp(s,"2F")==0){
		chr = "V" ;
	}else if(strcmp(s,"11")==0){
		chr = "W" ;
	}else if(strcmp(s,"2D")==0){
		chr = "X" ;
	}else if(strcmp(s,"15")==0){
		chr = "Y" ;
	}else if(strcmp(s,"2C")==0){
		chr = "Z" ;
	}
	return chr ;
}

int flag = 1 ;

void key(struct BOOTINFO *binfo,char s[40]){
	if(strcmp(s,"1C")==0){
			write_x = 58 ;
			write_y += 19 ;
			putfonts8_asc(binfo->vram, binfo->scrnx, 4, write_y, COL8_FFFFFF, "AntzOS>");
	}else if((strcmp(s,"3B")==0)||(strcmp(s,"BB")==0)){
			flag = 0 ;
			new_pe(binfo);
	}else if(strcmp(s,"39")==0){
			putfonts8_asc(binfo->vram, binfo->scrnx,  write_x,  write_y, COL8_FFFFFF, " ");
			write_x += 8 ;
	}else if(strcmp(s,"0E")==0){
			// 回退
			write_x -= 8 ;
			boxfill8(binfo->vram, binfo->scrnx , COL8_000000,  write_x,     write_y,     write_x+19, write_y+19);
			if(write_x<=58){ 
				write_x = 146 ;
				write_y -= 19 ;
			}
	}else {
			//putfonts8_asc(binfo->vram, binfo->scrnx,  write_x,  write_y, COL8_FFFFFF, s);
			putfonts8_asc(binfo->vram, binfo->scrnx,  write_x,  write_y, COL8_FFFFFF, replace_char(s));
			write_x += 8 ;
	}
	if(write_x>148){
		write_x = 58 ;
		write_y += 19 ;
		//putfonts8_asc(binfo->vram, binfo->scrnx, 4, 57, COL8_FFFFFF, "AntzOS>");
	}
	if(write_y>180){
 		new_pe(binfo);
	}

}

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], mcursor[256], keybuf[32], mousebuf[128];
	int mx, my, i;

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC的初始化已经完成，于是开放CPU的中断 */

	fifo8_init(&keyfifo, 32, keybuf);
	fifo8_init(&mousefifo, 128, mousebuf);
	io_out8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */

	init_keyboard();

	init_palette();
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

	putfonts8_asc(binfo->vram, binfo->scrnx,  0,  0, COL8_FFFFFF, "Terminal-Antz");
	putfonts8_asc(binfo->vram, binfo->scrnx,  0,  0, COL8_000000, "Terminal-Antz");
	putfonts8_asc(binfo->vram, binfo->scrnx,  107,  0, COL8_000000, "|-|o|x|");
	putfonts8_asc(binfo->vram, binfo->scrnx, 4, 19, COL8_FFFFFF, "AntzOS> SayHello()");
	putfonts8_asc(binfo->vram, binfo->scrnx, 4, 38, COL8_FFFFFF, "Hello My AntzOs.");
	putfonts8_asc(binfo->vram, binfo->scrnx, 4, 57, COL8_FFFFFF, "AntzOS>_");


	enable_mouse();
	for (;;) {
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
			io_stihlt();
		} else {
			if (fifo8_status(&keyfifo) != 0) {

				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				//boxfill8(binfo->vram, binfo->scrnx, COL8_008484,  0, 16, 15, 31);
				//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				if (flag){
					key(binfo,s);
				}
				if(flag==1){
					flag = 0 ;
				}else {
					flag = 1 ;
				}
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
			}
		}
	}
}

void new_pe(struct BOOTINFO *binfo){
	write_x = 58 ;
	write_y = 19 ;
	// 右边并没有保存
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
	putfonts8_asc(binfo->vram, binfo->scrnx,  0,  0, COL8_FFFFFF, "Terminal-Antz");
	putfonts8_asc(binfo->vram, binfo->scrnx,  0,  0, COL8_000000, "Terminal-Antz");
	putfonts8_asc(binfo->vram, binfo->scrnx,  107,  0, COL8_000000, "|-|o|x|");
	putfonts8_asc(binfo->vram, binfo->scrnx, 4, write_y, COL8_FFFFFF, "AntzOS>");
}

#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* 等待键盘控制电路准备完毕 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void)
{
	/* 初始化键盘控制电路 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(void)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; /* 顺利的话，键盘控制器会返回ACK(0xfa) */
}
