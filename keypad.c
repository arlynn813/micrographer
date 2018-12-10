#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <string.h>
#include <stdio.h>

char line[17] = "                 ";
int line_idx = 0;
int mode_selected = 0;
int enter = 0;
static int row = 0;

void display1(const char *);
void nano_wait(int);

void init_keypad(void){

  // enable port a
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // set up pa[1-7] (clear then set)... [1-3] are alternate functions, [4-7] are outputs
  GPIOA->MODER &= ~(3<<(2*1)); //pa1
  GPIOA->MODER |= 2<<(2*1);
  GPIOA->MODER &= ~(3<<(2*2)); //pa2
  GPIOA->MODER |= 2<<(2*2);
  GPIOA->MODER &= ~(3<<(2*3)); //pa3
  GPIOA->MODER |= 2<<(2*3);
  GPIOA->MODER &= ~(3<<(2*4)); //pa4
  GPIOA->MODER |= 1<<(2*4);
  GPIOA->MODER &= ~(3<<(2*5)); //pa5
  GPIOA->MODER |= 1<<(2*5);
  GPIOA->MODER &= ~(3<<(2*6)); //pa6
  GPIOA->MODER |= 1<<(2*6);
  GPIOA->MODER &= ~(3<<(2*7)); //pa7
  GPIOA->MODER |= 1<<(2*7);

  // set up afr (clear then set)
  GPIOA->AFR[0] &= ~(0xf<<(4*1)); //pa1
  GPIOA->AFR[0] |= 0x2<<(4*1);
  GPIOA->AFR[0] &= ~(0xf<<(4*2)); //pa2
  GPIOA->AFR[0] |= 0x2<<(4*2);
  GPIOA->AFR[0] &= ~(0xf<<(4*3)); //pa3
  GPIOA->AFR[0] |= 0x2<<(4*3);

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // enable sys clock for timer 2
  TIM2->PSC = 0; // set prescaler
  TIM2->ARR = 0xffffffff; //set auto reload

  // pull down pa1, pa2, pa3
  GPIOA->PUPDR |= 2<<(2*1); //pa1
  GPIOA->PUPDR |= 2<<(2*2); //pa2
  GPIOA->PUPDR |= 2<<(2*3); //pa3

  // channel 2
  TIM2->CR1 &= ~TIM_CR1_CKD;
  TIM2->CR1 |= TIM_CR1_CKD_1;
  TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
  TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
  TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
  TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;

  // channel 3
  TIM2->CR1 &= ~TIM_CR1_CKD;
  TIM2->CR1 |= TIM_CR1_CKD_1;
  TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
  TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
  TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
  TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;

  // channel 4
  TIM2->CR1 &= ~TIM_CR1_CKD;
  TIM2->CR1 |= TIM_CR1_CKD_1;
  TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
  TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
  TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
  TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;

  TIM2->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP|TIM_CCER_CC3P|TIM_CCER_CC3NP|TIM_CCER_CC4P|TIM_CCER_CC4NP); //rising edge
  TIM2->CCER |= TIM_CCER_CC2E|TIM_CCER_CC3E|TIM_CCER_CC4E; //enable capture
  TIM2->DIER |= TIM_DIER_CC2IE|TIM_DIER_CC3IE|TIM_DIER_CC4IE; //allow channels to generate interrupts
  TIM2->CR1 |= TIM_CR1_CEN; //enable timer counter
  NVIC->ISER[0] = 1 << TIM2_IRQn; //enable timer 2 interrupt

}

void init_keypad2(void){

  // enable ports a and c
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;

  // set up pc[7-9], pa[10-13] (clear then set)... pc[7-9] are alternate functions, pa[10-13] are outputs
  GPIOC->MODER &= ~(3<<(2*7)); //pc7
  GPIOC->MODER |= 2<<(2*7);
  GPIOC->MODER &= ~(3<<(2*8)); //pc8
  GPIOC->MODER |= 2<<(2*8);
  GPIOC->MODER &= ~(3<<(2*9)); //pc9
  GPIOC->MODER |= 2<<(2*9);
  GPIOA->MODER &= ~(3<<(2*10)); //pa10
  GPIOA->MODER |= 1<<(2*10);
  GPIOA->MODER &= ~(3<<(2*11)); //pa11
  GPIOA->MODER |= 1<<(2*11);
  GPIOA->MODER &= ~(3<<(2*12)); //pa12
  GPIOA->MODER |= 1<<(2*12);
  GPIOA->MODER &= ~(3<<(2*13)); //pa13
  GPIOA->MODER |= 1<<(2*13);

  // set up afr (clear then set)
  GPIOA->AFR[0] &= ~(0xf<<(4*7)); //pc7
  //GPIOA->AFR[0] |= 0x2<<(4*7);
  GPIOA->AFR[1] &= ~(0xf<<(4*0)); //pc8
  //GPIOA->AFR[1] |= 0x2<<(4*0);
  GPIOA->AFR[1] &= ~(0xf<<(4*1)); //pc9
  //GPIOA->AFR[1] |= 0x2<<(4*1);

  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // enable sys clock for timer 2
  TIM3->PSC = 0; // set prescaler
  TIM3->ARR = 0xffffffff; //set auto reload

  // pull down pc7, pc8, pc9
  GPIOC->PUPDR |= 2<<(2*7); //pc7
  GPIOC->PUPDR |= 2<<(2*8); //pc8
  GPIOC->PUPDR |= 2<<(2*9); //pc9

  // channel 2
  TIM3->CR1 &= ~TIM_CR1_CKD;
  TIM3->CR1 |= TIM_CR1_CKD_1;
  TIM3->CCMR1 &= ~TIM_CCMR1_CC2S;
  TIM3->CCMR1 |= TIM_CCMR1_CC2S_0;
  TIM3->CCMR1 &= ~TIM_CCMR1_IC2F;
  TIM3->CCMR1 &= ~TIM_CCMR1_IC2PSC;

  // channel 3
  TIM3->CR1 &= ~TIM_CR1_CKD;
  TIM3->CR1 |= TIM_CR1_CKD_1;
  TIM3->CCMR2 &= ~TIM_CCMR2_CC3S;
  TIM3->CCMR2 |= TIM_CCMR2_CC3S_0;
  TIM3->CCMR2 &= ~TIM_CCMR2_IC3F;
  TIM3->CCMR2 &= ~TIM_CCMR2_IC3PSC;

  // channel 4
  TIM3->CR1 &= ~TIM_CR1_CKD;
  TIM3->CR1 |= TIM_CR1_CKD_1;
  TIM3->CCMR2 &= ~TIM_CCMR2_CC4S;
  TIM3->CCMR2 |= TIM_CCMR2_CC4S_0;
  TIM3->CCMR2 &= ~TIM_CCMR2_IC4F;
  TIM3->CCMR2 &= ~TIM_CCMR2_IC4PSC;

  TIM3->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP|TIM_CCER_CC3P|TIM_CCER_CC3NP|TIM_CCER_CC4P|TIM_CCER_CC4NP); //rising edge
  TIM3->CCER |= TIM_CCER_CC2E|TIM_CCER_CC3E|TIM_CCER_CC4E; //enable capture
  TIM3->DIER |= TIM_DIER_CC2IE|TIM_DIER_CC3IE|TIM_DIER_CC4IE; //allow channels to generate interrupts
  TIM3->CR1 |= TIM_CR1_CEN; //enable timer counter
  NVIC->ISER[0] = 1 << TIM3_IRQn; //enable timer 3 interrupt

}

void scan_keypad(void){
  for(;;){
    for(row=1; row<5; row++){
        GPIOA->BSRR = 1 << (row+3) | 1 << (row+9); //pa4, pa5, pa6, pa7
        nano_wait(1000000);
        GPIOA->BRR = 0xf0 | 0x3c00; //reset bits 4,5,6,7 for pa4, pa5, pa6, pa7
        nano_wait(1000000);
    }
    if (enter == 1)
      {
        // REMOVE y= or z= BEFORE PASSING EXPRESSION INTO RPN
        for(int j = 0; j < line_idx-2; j++) {
            line[j] = line[j+2];
        }
        line[line_idx - 2] = 0; //null terminate string for rpn function
        break;
      }
  }
}

static void press(char c){
  if (mode_selected == 0)
    {
      if (c == 'd')
          {
            //2D mode
            //y=
            line[0] = 'y';
            line[1] = '=';
            line_idx = 2;
            mode_selected = 1; //determines calc and display mode in main
            display1(line);
          }
        if (c == 'D')
          {
            //3D mode
            //z=
            line[0] = 'z';
            line[1] = '=';
            line_idx = 2;
            mode_selected = 2; //determines calc and display mode in main
            display1(line);
          }
    }

  else if (line_idx < 16) //lcd displays 16 chars max. DO NOT OVERWRITE BOUNDS
    {
      //do not allow backspace of y= or z= after mode is chosen
      if ((c == 32) && (line_idx > 2)) line_idx--; //backspace (ascii ' '=32)

      //accept any input (vars/nums/ops)
      line[line_idx] = c;
      line_idx++;
      display1(line);
    }
  else if ((line_idx == 16) && (c == ' ')) //case for backspace when lcd is full
    {
      line_idx--;
      line[line_idx] = c;
      display1(line);
      line_idx++;
    }
}

void TIM2_IRQHandler(){
  if ((TIM2->SR & TIM_SR_UIF) != 0){
      TIM2->SR &= ~TIM_SR_UIF;
      return;
  }

  if (TIM2->SR & TIM_SR_CC2IF){
      switch(row){
      case 1: press('1'); break;
      case 2: press('4'); break;
      case 3: press('7'); break;
      case 4: press('d'); break; //d represents 2D button
      }
  }

  if (TIM2->SR & TIM_SR_CC3IF){
      switch(row){
      case 1: press('2'); break;
      case 2: press('5'); break;
      case 3: press('8'); break;
      case 4: press('0'); break;
      }
  }

  if (TIM2->SR & TIM_SR_CC4IF){
      switch(row){
      case 1: press('3'); break;
      case 2: press('6'); break;
      case 3: press('9'); break;
      case 4: press('D'); break; //D represents 3D button
      }
  }

  nano_wait(10 * 1000 * 1000);


  while((GPIOA->IDR & 0xe) != 0) ;      //yeeehaw

  nano_wait(10 * 1000 * 1000);

  int __attribute((unused)) useless;
  useless = TIM2->CCR2;
  useless = TIM2->CCR3;
  useless = TIM2->CCR4;
  return;
}

void TIM3_IRQHandler(){
  if ((TIM3->SR & TIM_SR_UIF) != 0){
      TIM3->SR &= ~TIM_SR_UIF;
      return;
  }

  if (TIM3->SR & TIM_SR_CC2IF){
      switch(row){
      case 1: press('+'); break;
      case 2: press('*'); break;
      case 3: press('x'); break;
      case 4: press(' '); line_idx--; break; //backspace
      }
  }

  if (TIM3->SR & TIM_SR_CC3IF){
      switch(row){
      case 1: press('-'); break;
      case 2: press('/'); break;
      case 3: press('^'); break;
      case 4: press('R'); break; //SOFTWARE RESET (new function) NOT YET INITIALIZED
      }
  }

  if (TIM3->SR & TIM_SR_CC4IF){
      switch(row){
      case 1: press('('); break;
      case 2: press(')'); break;
      case 3: press('y'); break;
      case 4: enter = 1; break; //used to break infinite scan keypad
      }
  }

  nano_wait(10 * 1000 * 1000);

  while((GPIOC->IDR & 0x380) != 0)
    ;

  nano_wait(10 * 1000 * 1000);

  int __attribute((unused)) useless;
  useless = TIM3->CCR2;
  useless = TIM3->CCR3;
  useless = TIM3->CCR4;
  return;
}
