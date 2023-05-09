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
#define BLACK 0x0000

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


// Begin part3.c code for Lab 7

void draw_line(int x0, int x1, int y0, int y1, short int color);
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_rect(int x, int y, short int color);

volatile int pixel_buffer_start; // global variable

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
	int xcoordinates[8];
	int ycoordinates[8];
	int dx[8];
	int dy[8];
	short int colors[10] = {0xFFFF, 0xFFE0, 0xF800, 0x07E0, 0x001F, 0x07FF, 0xF81F, 0xC618, 0xFC18, 0xFC00};
	short int linecolors[10] = {0xFFFF, 0xFFE0, 0xF800, 0x07E0, 0x001F, 0x07FF, 0xF81F, 0xC618, 0xFC18, 0xFC00}; 
	int xc[8] = {0};
	int yc[8] = {0};
    // initialize location and direction of rectangles(not shown)
	
	for(int i=0; i<8; i++) {
		xcoordinates[i] = rand()%(RESOLUTION_X);
		ycoordinates[i] = rand()%(RESOLUTION_Y);
		dx[i] = rand()%2*2-1;
		dy[i] = rand()%2*2-1;
		colors[i] = colors[rand()%10];
		linecolors[i] = linecolors[rand()%10];
	}

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    while (1)
    {
        /* Erase any boxes and lines that were drawn in the last iteration */
        //...
		for (int i = 0; i < 8; i++) {
			draw_rect(xc[i], yc[i], BLACK);
			draw_line(xc[i], yc[i], xc[(i+1)%8], yc[(i+1)%8], BLACK);
		}
        // code for drawing the boxes and lines (not shown)
		for (int i = 0; i < 8; i++) {
			draw_rect(xcoordinates[i], ycoordinates[i], colors[i]);
			draw_line(xcoordinates[i], ycoordinates[i], xcoordinates[(i+1)%8], ycoordinates[(i+1)%8], linecolors[i]);
			xc[i] = xcoordinates[i]- dx[i];
			yc[i] = ycoordinates[i] - dy[i];
		}
		// code for updating the locations of boxes (not shown)
		for (int i = 0; i < 8; i++) {
			if (xcoordinates[i] == 320 && dx[i] == 1) {
				dx[i] = -1;
			}
			if (xcoordinates[i] == 0 && dx[i] == -1) {
				dx[i] = 1;
			}
			if (ycoordinates[i] == 240 && dy[i] == 1) {
				dy[i] = -1;
			}
			if (ycoordinates[i] == 0 && dy[i] == -1) {
				dy[i] = 1;
			}
			
			xcoordinates[i] = xcoordinates[i] + dx[i];
			ycoordinates[i] = ycoordinates[i] + dy[i];
		}
			

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

// code for subroutines (not shown)
void draw_rect(int x, int y, short int color){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            plot_pixel(x+i,y+j, color);
        }
    }
}

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
			plot_pixel(i, j, BLACK);
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}