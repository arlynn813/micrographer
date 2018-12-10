#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

#define SCL             1
#define SDI             2
#define SS              4
#define SPI_DELAY       4000

void nano_wait(int t);

static void sendbit(int b){
        GPIOB->BRR = SCL;
        GPIOB->BSRR = b ? SDI : (SDI << 16);
        nano_wait(SPI_DELAY);
        GPIOB->BSRR = SCL;
        nano_wait(SPI_DELAY);
}

static void sendbyte(char b){
        int x;
        for (x = 8; x > 0; x--){
                sendbit(b & 0x80);
                b <<= 1;
        }
}

static void cmd(char b){
        GPIOB->BRR = SS;
        nano_wait(SPI_DELAY);
        sendbit(0);
        sendbit(0);
        sendbyte(b);
        nano_wait(SPI_DELAY);
        GPIOB->BSRR = SS;
        nano_wait(SPI_DELAY);
}

static void data(char b){
        GPIOB->BRR = SS;
        nano_wait(SPI_DELAY);
        sendbit(1);
        sendbit(0);
        sendbyte(b);
        nano_wait(SPI_DELAY);
        GPIOB->BSRR = SS;
        nano_wait(SPI_DELAY);
}

void init_lcd(void){
        RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
        GPIOB->ODR |= 7;
        GPIOB->MODER &= ~0x3f;
        GPIOB->MODER |= 0x15;

        nano_wait(100000000);
        cmd(0x38);
        cmd(0x0c);
        cmd(0x01);
        nano_wait(6200000);
        cmd(0x02);
        cmd(0x06);
}

void display1(const char *s){
        cmd(0x02);
        int len;
        for (len = 0; len < 16; len++){
                if (s[len] == '\0')
                        break;
                data(s[len]);
        }
}

void display2(const char *s){
        cmd(0xc0);
        int len;
        for (len = 0; len < 16; len++){
                if (s[len] == '\0')
                        break;
                data(s[len]);
        }
}
