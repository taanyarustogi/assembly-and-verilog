/* This files provides address values that exist in the system */
//for best results copy the code on https://cpulator.01xz.net/?sys=arm-de1soc (make sure to click C) and run to demo.

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000
#define ADDR_7SEG1 ((volatile long *) 0xFF200020)

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
#define NAVY 0x000F
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BLACK 0x0000
	
#define DARKGREY 0x7BEF
#define DARKGREEN 0x03E0
	
#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void draw_car(int x, int y, short int color);
void write_char(int x, int y, char c);
void user_car(int x, int y, short int color);
void draw_line(int x0, int y0, int x1, int y1, short int color);
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_background();
void start_screen();
void game_over();
void clear_char();
void write_char(int x, int y, char c);

volatile int pixel_buffer_start; // global variable
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

int main(void)
{
	int PS2_data, RVALID;
	int car_number = 4;
	int counter = 0;
	int counter1 = 0;
	int counter2 = 0;
	
	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
//    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	
	unsigned char byte1 = 0;
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;
	
	int bits[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,0xFF, 0x6F};
	
    // declare arrays for cars
	int xcoordinates[4] = {160, 80, 160, 240};
	int ycoordinates[4] = {RESOLUTION_Y - 40, 5, 5, 5};
	int dy[4];
	int dx = -2;
	short int colors[9] = {YELLOW, RED, GREEN, BLUE, MAGENTA, NAVY, CYAN, PINK, ORANGE};
	int yc[4] = {0};
	int xc = 0;
    // initialize color and speed of cars
	for(int i=0; i<car_number; i++) {
		dy[i] = (rand()%3+1)*-1;
		colors[i] = colors[rand()%9];
	}	
	//set user car
	colors[0] = WHITE;
	dy[0] = 0;

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	draw_background();
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	
	draw_background();
	clear_char();
	
	while (1) {
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			/* always save the last three bytes received */
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}
		if ( (byte2 == 0xAA) && (byte3 == 0x00) )
		{
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
		}
		if(byte3 == 0x76) {
			clear_char();
			break;
		}
		start_screen();
	}

    while (1)
    {
		
        /* Erase any cars */
		user_car(xc, ycoordinates[0], DARKGREY);
		for (int i = 1; i < 4; i++) {
			if(yc[i] != 0) 
				draw_car(xcoordinates[i], yc[i], DARKGREY);
		}
		//erase bottom of screen
		for(int i = 0; i < 28; i++){
        	for(int j = 0; j < 28; j++){
            	plot_pixel(66+i,RESOLUTION_Y-28+j, DARKGREY);
        	}
    	}
		for(int i = 0; i < 28; i++){
        	for(int j = 0; j < 28; j++){
            	plot_pixel(160-14+i,RESOLUTION_Y-28+j, DARKGREY);
        	}
    	}
		for(int i = 0; i < 28; i++){
        	for(int j = 0; j < 28; j++){
            	plot_pixel(240-14+i,RESOLUTION_Y-28+j, DARKGREY);
        	}
    	}
		
		user_car(0, RESOLUTION_Y-40, DARKGREEN);
		
		draw_line(120,RESOLUTION_Y-40, 120, RESOLUTION_Y, WHITE);
		draw_line(200,RESOLUTION_Y-40, 200, RESOLUTION_Y, WHITE);
		
		for(int i=79; i<81; i++){
			for(int j=0; j<RESOLUTION_Y; j=j+20){
				for(int z=0; z<8; z++){
					plot_pixel(i,j+z, WHITE);
				}
			}
		}
		for(int i=159; i<161; i++){
			for(int j=0; j<RESOLUTION_Y; j=j+20){
				for(int z=0; z<8; z++){
					plot_pixel(i,j+z, WHITE);
				}
			}
		}
		
		for(int i=239; i<241; i++){
			for(int j=0; j<RESOLUTION_Y; j=j+20){
				for(int z=0; z<8; z++){
					plot_pixel(i,j+z, WHITE);
				}
			}
		}
		
		//draw user car
		user_car(xcoordinates[0], ycoordinates[0], colors[0]);
		xc = xcoordinates[0] + dx;	
		
		//draw other cars
		for (int i = 1; i < car_number; i++) {
			draw_car(xcoordinates[i], ycoordinates[i], colors[i]);
			yc[i] = ycoordinates[i] + dy[i];
		}
		
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			/* always save the last three bytes received */
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}
		if ( (byte2 == 0xAA) && (byte3 == 0x00) )
		{
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
		}
		
		// using arrows keys to control user car
		if (byte3==0x74) {
			dx = -2;
		} else if (byte3==0x6B) {
			dx = 2;
		}
		xcoordinates[0] = xcoordinates[0] - dx;

		//pauses with space and plays with enter keys
		if (byte3 == 0x29) {
			dx = 0;
			for (int i=0; i < car_number; i++) 
				dy[i] = 0;
		} else if (byte3==0x5A) {
			dx=-2;
			for (int i=0; i<car_number; i++) 
				dy[i] = (rand()%3+1)*-1;
		}
		
		if (byte3 == 0x16) {
			for (int i=0; i<car_number; i++) 
				dy[i] = (1+rand()%(3+1-1))*-1;
		} else if (byte3 == 0x1E) {
			for (int i=0; i<car_number; i++) 
				dy[i] = (4+rand()%(6+1-4))*-1;
		} else if (byte3 == 0x26) {
			for (int i=0; i<car_number; i++) 
				dy[i] = (7+rand()%(9+1-7))*-1;
		}

		for (int i = 1; i < car_number; i++) {
			//reset cars when they reach bottom of screen
			if (ycoordinates[i] > (RESOLUTION_Y-20)) {
				if (byte3 == 0x16) {
					for (int i=0; i<car_number; i++) 
						dy[i] = (1+rand()%(3+1-1))*-1;
				} else if (byte3 == 0x1E) {
					for (int i=0; i<car_number; i++) 
						dy[i] = (4+rand()%(6+1-4))*-1;
				} else if (byte3 == 0x26) {
					for (int i=0; i<car_number; i++) 
						dy[i] = (7+rand()%(9+1-7))*-1;
				}
				colors[i] = colors[rand()%9];
				ycoordinates[i] = 5;
				counter++;
			}
			ycoordinates[i] = ycoordinates[i] - dy[i];
		}
		
		//show counter on hex display
		counter1 = counter%10;
		counter2 = (counter - counter1)/10;
		*ADDR_7SEG1 = (bits[counter2]<<8) + bits[counter1];
		
		//check collision with other cars
		if ((xcoordinates[0] == 40) || (xcoordinates[0] == 272)) {
			game_over();
			return;
		}
		for (int i=1; i< car_number; i++) {
			if((ycoordinates[0] <= (ycoordinates[i]+28))&&(ycoordinates[0] <= ycoordinates[i])){
				if((xcoordinates[0] >= (xcoordinates[i]-10))&& (xcoordinates[0] <= (xcoordinates[i]+10))){
					game_over();
					return;
				}
				if(((xcoordinates[0]+20) >= (xcoordinates[i]-10))&& ((xcoordinates[0]+20) <= (xcoordinates[i]+10)))
					{game_over();
					return;
					}
			}
			if((xcoordinates[0] <= (xcoordinates[i]+10))&&(xcoordinates[0] >= (xcoordinates[i]-10))) {
				if((ycoordinates[0] >= ycoordinates[i])&& (ycoordinates[0] <= (ycoordinates[i]+28))){
					game_over();
					return;
				}
				if(((ycoordinates[0]+28) >= ycoordinates[i])&& ((ycoordinates[0]+28) <= (ycoordinates[i]+28)))
					{game_over();
					return;
			}
		}
			if(((xcoordinates[0]+20) >= (xcoordinates[i]-10))&&((xcoordinates[0]+20)<= (xcoordinates[i]+10))) {
				if((ycoordinates[0] >= ycoordinates[i])&& (ycoordinates[0] <= (ycoordinates[i]+28)))
					{game_over();
					return;
					}
				if(((ycoordinates[0]+28) >= ycoordinates[i])&& ((ycoordinates[0]+28) <= (ycoordinates[i]+28)))
					{game_over();
						return;
					}
			}
		}
			
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }	
}

void draw_background() {
	draw_line(40,0,40,RESOLUTION_Y,WHITE);
	draw_line(120,0,120,RESOLUTION_Y,WHITE);
	draw_line(200,0,200,RESOLUTION_Y,WHITE);
	draw_line(280,0,280,RESOLUTION_Y,WHITE);
	
	for(int i=79; i<81; i++){
			for(int j=0; j<RESOLUTION_Y; j=j+20){
				for(int z=0; z<8; z++){
					plot_pixel(i,j+z, WHITE);
				}
			}
		}
		for(int i=159; i<161; i++){
			for(int j=0; j<RESOLUTION_Y; j=j+20){
				for(int z=0; z<8; z++){
					plot_pixel(i,j+z, WHITE);
				}
			}
		}
		
		for(int i=239; i<241; i++){
			for(int j=0; j<RESOLUTION_Y; j=j+20){
				for(int z=0; z<8; z++){
					plot_pixel(i,j+z, WHITE);
				}
			}
		}
		
		for(int i=0; i<40; i++){
			draw_line(i,0,i,RESOLUTION_Y, DARKGREEN);
		}
		
		for(int i=281; i<320; i++){
			draw_line(i,0,i,RESOLUTION_Y, DARKGREEN);
		}
}

// drawing user car
void user_car(int x, int y, short int color) {
	
	short int tire = BLACK;
	short int shield = DARKGREY;
	
	for(int i = 0; i < 20; i++){
        for(int j = 0; j < 28; j++){
            plot_pixel(x+i,y+j, color);
        }
    }
	
	if (color == DARKGREY) 
		tire = DARKGREY;
	if (color == DARKGREEN) {
		tire = DARKGREEN;
		shield = DARKGREEN;
	}

//TO DRAW TIRES L UP
		for(int i = -1; i < 4; i++){
        for(int j =-2; j < 6; j++){
            plot_pixel(x+i,y+j, tire);
        }
    }
// L DOWN
	for(int i = -1; i < 4; i++){
        for(int j = 22; j < 30; j++){
            plot_pixel(x+i,y+j, tire);
        }
    }
// R UP
	for(int i = 16; i < 21; i++){
        for(int j = -2; j < 6; j++){
            plot_pixel(x+i,y+j, tire);
        }
    }
	
// R DOWN
	for(int i = 16; i < 21; i++){
        for(int j = 22; j < 30; j++){
            plot_pixel(x+i,y+j, tire);
        }
    }
//for windshield	
	for(int i=0; i<10; i++){
		for(int j=0; j<5; j++){
			plot_pixel(x+i+5,y+j+7, shield);
		}
	}
}

//drawing other cars
void draw_car(int x, int y, short int color) {
	
	short int tire = BLACK;
	
	for(int i = 0; i < 20; i++){
        for(int j = 0; j < 28; j++){
            plot_pixel(x+i-10,y+j, color);
        }
    }
	
	if (color == DARKGREY)
		tire = DARKGREY;
	
//TO DRAW TIRES L UP
		for(int i = -1; i < 4; i++){
        for(int j =-2; j < 6; j++){
            plot_pixel(x+i-10,y+j, tire);
        }
    }
// L DOWN
	for(int i = -1; i < 4; i++){
        for(int j = 22; j < 30; j++){
            plot_pixel(x+i-10,y+j, tire);
        }
    }
// R UP
	for(int i = 16; i < 21; i++){
        for(int j = -2; j < 6; j++){
            plot_pixel(x+i-10,y+j, tire);
        }
    }
	
// R DOWN
	for(int i = 16; i < 21; i++){
        for(int j = 22; j < 30; j++){
            plot_pixel(x+i-10,y+j, tire);
        }
    }
//for windshield	
	for(int i=0; i<10; i++){
		for(int j=0; j<5; j++){
			plot_pixel(x+i+5-10,y+j+7+10, DARKGREY);
		}
	}
}

//drawing line
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

void start_screen() {
	 char* b = "Press ESC to start!";
   int y = 35;
   while (*b) {
     write_char(y, 25, *b);
	 y++;
	 b++;
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


void clear_screen() 
{
	for (int i = 0; i < RESOLUTION_X; i++) {
		for (int j = 0; j < RESOLUTION_Y; j++) {
			plot_pixel(i, j, DARKGREY);
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void startgame(){
	for (int i = 0; i < RESOLUTION_X; i++) {
		for (int j = 0; j < RESOLUTION_Y; j++) {
			plot_pixel(i, j, BLACK);
		}
	}
	
	 char* hw = "Hello, world!";
   int x = 15;
   while (*hw) {
     write_char(x, 10, *hw);
	 x++;
	 hw++;
   }
	
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

return;
}

void game_over(){
	
	for (int i = 0; i < RESOLUTION_X; i++) {
		for (int j = 0; j < RESOLUTION_Y; j++) {
			plot_pixel(i, j, BLACK);
		}
	}
	
	 char* b = "GAME OVER!";
   int y = 35;
   while (*b) {
     write_char(y, 25, *b);
	 y++;
	 b++;
   }
	
	//G
	/*for(int i=20; i<41; i++){
		for(int j=20; j<101; j++){
		draw_line(20,20,i,j,RED);
		}
	}
	for(int i=40; i<81; i++){
		for(int j=20; j<40; j++){
		draw_line(40,20,i,j,RED);
		}
	}
	for(int i=20; i<61; i++){
		for(int j=80; j<101; j++){
		draw_line(20,80,i,j,RED);
		}
	}
	for(int i=60; i<81; i++){
		for(int j=60; j<101; j++){
		draw_line(60,60,i,j,RED);
		}
	}
	//A
	for(int i=90; i<111; i++){
		for(int j=20; j<101; j++){
		draw_line(90,20,i,j,RED);
		}
	}
	for(int i=120; i<141; i++){
		for(int j=20; j<101; j++){
		draw_line(120,20,i,j,RED);
		}
	}
	for(int i=110; i<121; i++){
		for(int j=20; j<41; j++){
		draw_line(110,20,i,j,RED);
		}
	}
	for(int i=110; i<121; i++){
		for(int j=50; j<71; j++){
		draw_line(110,50,i,j,RED);
		}
	}
	//M
	for(int i=150; i<231; i++){
		for(int j=20; j<41; j++){
		draw_line(150,20,i,j,RED);
		}
	}
	for(int i=150; i<171; i++){
		for(int j=20; j<101; j++){
		draw_line(150,20,i,j,RED);
		}
	}
	for(int i=211; i<231; i++){
		for(int j=20; j<101; j++){
		draw_line(211,20,i,j,RED);
		}
	}
	for(int i=180; i<201; i++){
		for(int j=20; j<101; j++){
		draw_line(180,20,i,j,RED);
		}
	}
	//E
	for(int i=240; i<261; i++){
		for(int j=20; j<101; j++){
		draw_line(240,20,i,j,RED);
		}
	}
	for(int i=240; i<291; i++){
		for(int j=20; j<41; j++){
			draw_line(240,20,i,j,RED);
		}
	}
	for(int i=240; i<291; i++){
		for(int j=50; j<71; j++){
			draw_line(240,50,i,j,RED);
		}
	}
	for(int i=240; i<291; i++){
		for(int j=80; j<101; j++){
			draw_line(240,80,i,j,RED);
		}
	}
	
	//O
	for(int i=20; i<41; i++){
		for(int j=140; j<221; j++){
		draw_line(20,140,i,j,RED);
		}
	}
	for(int i=40; i<81; i++){
		for(int j=140; j<161; j++){
		draw_line(40,140,i,j,RED);
		}
	}
	for(int i=20; i<61; i++){
		for(int j=200; j<221; j++){
		draw_line(20,200,i,j,RED);
		}
	}
	for(int i=60; i<81; i++){
		for(int j=140; j<221; j++){
		draw_line(60,140,i,j,RED);
		}
	}
	//V
	for(int i=0; i<50; i++){
		for(int j=0; j<80; j++){\
			int z = 2;
			draw_line(90, 140, 110+z, 170+6*z, RED);
		}
	}

	//draw_line(90,140,120,220, RED);
	//E
	for(int i=160; i<181; i++){
		for(int j=140; j<221; j++){
		draw_line(160,140,i,j,RED);
		}
	}
	for(int i=180; i<221; i++){
		for(int j=140; j<161; j++){
			draw_line(180,140,i,j,RED);
		}
	}
	for(int i=180; i<221; i++){
		for(int j=170; j<191; j++){
			draw_line(180,170,i,j,RED);
		}
	}
	for(int i=180; i<221; i++){
		for(int j=200; j<221; j++){
			draw_line(180,200,i,j,RED);
		}
	}
	//R 230- 290
	for(int i=230; i<251; i++){
		for(int j=140; j<221; j++){
		draw_line(230,140,i,j,RED);
		}
	}
	
	for(int i=250; i<291; i++){
		for(int j=170; j<186; j++){
		draw_line(230,190,i,j,RED);
		}
	}
	for(int i=250; i<291; i++){
		for(int j=140; j<156; j++){
		draw_line(230,140,i,j,RED);
		}
	}
	for(int i=275; i<291; i++){
		for(int j=140; j<181; j++){
			draw_line(270,140,i,j,RED);
		}
	}
	for(int i=0; i<20 ;i++){
		for(int j=0; j<20; j++){
			draw_line(230+i,170+j,270+i,200+j, RED);
		}
	}*/
		  
		  
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

return;
}

void clear_char() {
	for (int i = 0; i<80; i++) {
		for (int j = 0; j < 60; j++) {
			write_char(i,j,' ');
		}
	}
}

void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0xC9000000 + (y<<7) + x);
  *character_buffer = c;
}
