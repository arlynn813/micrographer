/*NOTES
  ******************************************************************************
  * The 32x32 (WxH) is actually two 32x16 grids. (R/G/B)1 controls top half
  * while (R/G/B)2 controls bottom half. Both of these grids run on the same
  * CLK, OE, and LAT. When selecting a row (r) the respective row is selected
  * for both grids. So on a 32x32, select row (r) selects both row r and r+16.
  * (R/G/B)1 values correspond to row r, while (R/G/B)2 corresponds to row r+16.
  * Only one row select can be controlled at a given time. Flash through rows
  * quickly enough for a perceivable full grid image.
  *
  * Pinouts (respective to order):
  * -[PC3, PC2, PC1, PC0] ... [D, C, B, A] (row select pins - white) odd order
  * -[PB10, PB9, PB8] ... [R1, G1, B1] (3 bits for top half - rgb)
  * -[PB13, PB12, PB11] ... [R2, G2, B2] (3 bits for bottom half - rgb)
  * -PB3 ... OE (yellow)
  * -PB4 ... CLK (orange)
  * -PB5 ... LAT (latch - grey)
  *
  * Steps to Output Light (loop for every row r WITH NO DELAY ... MUST BE FAST)
  * -OE, CLK and LAT low before starting
  * -set desired color to rgb
  * -set CLK high then low to shift in color bits (1 column in selected row)
  * -repeat first 3 steps for each column
  * -set OE high to disable leds
  * -set LAT high then low (loads shift register contents into led outputs)
  * -set D/C/B/A (row select) to current row r
  * -set OE low to re-enable leds
  ******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "intensity.h"

void init_ports();
void blackout();
void graph(int);
void calc_z();
void init_lcd(void);
void display1(const char*);
void display2(const char*);
void init_keypad(void);
void init_keypad2(void);
void scan_keypad(void);
void init_reset();
int* string_to_rpn(char*);
float rpn_eval(int, int, int*);
int mode_selected;
char line[17];
int shunted[30];

void init_reset()
{

}

void blackout()
{
  for (int row=0; row<16; row++)
    {
      for (int column=0; column<32; column++)
        {
          GPIOB->ODR &= RGB_MASK;
          GPIOB->ODR |= CLK_EN; //clk high
          GPIOB->ODR &= CLK_MASK; //clk low
        }
      GPIOB->ODR |= OE_EN; //oe high
      GPIOB->ODR |= LAT_EN; //lat high
      GPIOB->ODR &= LAT_MASK; //lat low
      GPIOC->ODR &= ROW_MASK; //clear row
      GPIOC->ODR |= row; //set row
      GPIOB->ODR &= OE_MASK; //oe low
    }
}

void graph(int frame)
{
  for (int row=0; row<16; row++) //y values
    {
      for (int column=0; column<32; column++) //x values
        {
          GPIOB->ODR &= RGB_MASK;
          int idx_1 = z[row][column];
          int idx_2 = z[row+16][column];
          GPIOB->ODR |= (intensity[idx_1][frame]<<8) | (intensity[idx_2][frame]<<11);

          GPIOB->ODR |= CLK_EN; //clk high
          GPIOB->ODR &= CLK_MASK; //clk low
        }
      GPIOB->ODR |= OE_EN; //oe high
      GPIOB->ODR |= LAT_EN; //lat high
      GPIOB->ODR &= LAT_MASK; //lat low
      GPIOC->ODR &= ROW_MASK; //clear row
      GPIOC->ODR |= row; //set row
      GPIOB->ODR &= OE_MASK; //oe low
    }
}

void calc_y() {
        float scale = 1.0;

        int intx;
        int inty;
        int xcoef = 2;

        for (inty = 0; inty < 32; inty++) {
            for (intx=0; intx<32; intx++){
                z[intx][inty] = 0;
                z[16][inty] = 1;
                z[intx][15] = 1;
            }
        }
        float y_arr[32];
        float x_arr[32 * 2];
        for (int i = 0; i < 32; i++) {
                y_arr[i] = (16 - i) * scale;
        }
        float xstep = scale / xcoef;
        for (int i = 0; i < (31 * xcoef + 1); i++) {
                x_arr[i] = ((i*xstep) - 15);
        }
        for (int i = 0; i < (32 * xcoef); i++) {
                float curr_y = rpn_eval(x_arr[i],0,shunted);                          //function string nonsense goes here
                if ((curr_y <= y_arr[0]) && (curr_y >= y_arr[31])) {
                        int j = 0;
                        while (curr_y < y_arr[j]) {
                                j++;
                        }
                        z[j][i/xcoef] = 2;
                }
        }
        return;
}

void graph_y()
{
  for (int row=0; row<16; row++)
    {
      for (int column=0; column<32; column++)
        {
          GPIOB->ODR &= RGB_MASK;
          GPIOB->ODR |= 0x4<<8 | 0x4<<11; //set background red
          if (column==15) //draw white y axis
            {
              GPIOB->ODR &= RGB_MASK;
              GPIOB->ODR |= 0x7<<8 | 0x7<<11;
            }

          if (row==1) //draw white x axis
            {
              GPIOB->ODR &= RGB_MASK;
              GPIOB->ODR |= 0x4<<8 | 0x7<<11; //keeping top row red
            }

          if (z[row][column] == 2.0f) //draw green points on top half
            {
              GPIOB->ODR &= RGB_MASK;
              GPIOB->ODR |= 2<<8;
              if(z[row+15][column] == 2.0f)
                  GPIOB->ODR |= 2<<11;
              else if(z[row+15][column] == 1.0f)
                  GPIOB->ODR |= 7<<11;
              else
                  GPIOB->ODR |= 4<<11;
            }
          if (z[row+15][column] == 2.0f) //draw green points on bottom half
            {
              GPIOB->ODR &= RGB_MASK;
              GPIOB->ODR |= 2<<11;
              if(z[row][column] == 2.0f)
                  GPIOB->ODR |= 2<<8;
              else if(z[row][column] == 1.0f)
                  GPIOB->ODR |= 7<<8;
              else
                  GPIOB->ODR |= 4<<8;
            }

          GPIOB->ODR |= CLK_EN; //clk high
          GPIOB->ODR &= CLK_MASK; //clk low
        }
      GPIOB->ODR |= OE_EN; //oe high
      GPIOB->ODR |= LAT_EN; //lat high
      GPIOB->ODR &= LAT_MASK; //lat low
      GPIOC->ODR &= ROW_MASK; //clear row
      GPIOC->ODR |= row; //set row
      GPIOB->ODR &= OE_MASK; //oe low
     }
}

void calc_z()
{
  int x;
  int y;

  // store array of z values and find min/max.
  // using these bounds, we define the base domain as [-15,16]
  // and store the top left value first then work right through the row
  // before moving to the next row. This is very important as this is how
  // values are clocked into the led matrix display
  for (y=16; y>=-15; y--)
    {
      for (x=-15; x<=16; x++)
        {
          z[16-y][x+15] = rpn_eval(x,y,shunted);
          if (x==-15 && y==16) //if first run, store max/min as initial value
            {
              z_max = z[0][0];
              z_min = z[0][0];
            }
          if (z[16-y][x+15] > z_max)
            z_max = z[16-y][x+15];
          if (z[16-y][x+15] < z_min)
            z_min = z[16-y][x+15];
        }
    }

  // shift all z so min z is 0
  // scale z so it ranges from 0 to 31
  float shift = 0-z_min;
  z_max += shift;
  float factor = z_max/31;
  for (y=0; y<32; y++)
    {
      for (x=0; x<32; x++)
        {
          z[y][x] += shift;
          z[y][x] /= factor; //z values are now intensity values
        }
    }
}

void init_ports()
{
  //enable pb and pc
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

  //enable control outputs
  GPIOB->MODER &= 0xfffff03f;
  GPIOB->MODER |= 1<<6 | 1<<8 | 1<<10;

  //enable rgb1 and rgb2 outputs
  GPIOB->MODER &= 0xf000ffff;
  GPIOB->MODER |= 1<<16 | 1<<18 | 1<<20 | 1<<22 | 1<<24 | 1<<26;

  //enable row select output
  GPIOC->MODER &= 0xffffff00;
  GPIOC->MODER |= 1 | 1<<2 | 1<<4 | 1<<6;

  //initialize OE, CLK and LAT to low
  GPIOB->ODR &= 0xfff8;
}

int main(void)
{
  // INITIALIZATION
  init_ports();
  blackout(); //make sure func display is cleared from previous run
  init_lcd();
  char line2[17];
  sprintf(line2, "  SELECT MODE   ");
  display1(line2);
  sprintf(line2, "    2D / 3D     ");
  display2(line2);

  // ACCEPT FUNCTION INPUT
  init_keypad();
  init_keypad2();
  scan_keypad(); //expression to evaluate is now stored in line
  sprintf(line2, "D:[-15,16]      ");
  display2(line2);
  string_to_rpn(line);

  // CALCULATE, LINEARIZE and DISPLAY FUNCTION (2D=1 and 3D=2)
  if(mode_selected == 1) {
      calc_y();
      for(;;)
        {
          graph_y();
        }
    }
  else if(mode_selected == 2) {
      calc_z();
      for(;;)
        {
          for (int frame=0; frame<FRAMES; frame++)
            {
              graph(frame);
            }
        }
    }
  }

