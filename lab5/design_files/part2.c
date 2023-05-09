/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Begin part1.s for Lab 7

volatile int pixel_buffer_start; // global variable

void draw_line(int x0, int x1, int y0, int y1, short int color);
void clear_screen();
void plot_pixel(int x, int y, short int line_color);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
	clear_screen();
	int y=0;
	int y_step = 1;
	draw_line(0, y, 319, y, 0xF800); 

	while(TRUE) {
	draw_line(0, y, 319, y, WHITE);
	y=y+y_step;
    draw_line(0, y, 319, y, 0xF800); 
	wait_for_vsync();
	
	if (y == RESOLUTION_Y)
		y_step = -1;
	if (y==0) 
		y_step = 1;
	}
}

// code not shown for clear_screen() and draw_line() subroutines

void wait_for_vsync()
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	register int status;
	*pixel_ctrl_ptr = 1;
	status = *(pixel_ctrl_ptr+3);
	while((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr+3);
	}
}
void draw_line(int x0, int y0, int x1, int y1, short int color)
{
	bool is_steep = abs(y1-y0) > abs(x1 - x0);
	if (is_steep) 
	{
		int temp = x0;
		x0 = y0;
		y0 = temp;
		
		int temp2 = x1;
		x1 = y1;
		y1 = temp2;
	}
	if (x0 > x1) 
	{
		int temp = x0;
		x0 = x1;
		x1 = temp;
		
		int temp2 = y0;
		y0 = y1;
		y1 = temp2;
	}
	int deltax = x1-x0;
	int deltay = abs(y1-y0);
	int error = -(deltax / 2);
	int y = y0;
	int y_step;
	if (y0 < y1) {
		y_step = 1;
	} else {
		y_step = -1;
	}
	for (int x = x0; x<= x1; x++) 
	{
		if (is_steep) {
			plot_pixel(y, x, color);
		} else {
			plot_pixel(x, y, color);
		}
		error = error + deltay;
		if (error > 0) 
		{
			y = y + y_step;
			error = error - deltax;
		}
	}
}	

void clear_screen() 
{
	for (int i = 0; i < RESOLUTION_X; i++) {
		for (int j = 0; j < RESOLUTION_Y; j++) {
			plot_pixel(i, j, WHITE);
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}
