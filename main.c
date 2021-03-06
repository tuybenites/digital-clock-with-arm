#include "stm32f4xx.h"
/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2021-12-10

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/




int arr[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
int seg=-1, min=0, hora=0;
int disp=0;

void setTim10(void);
void setTim11(void);
void TIM1_TRG_COM_TIM11_IRQHandler(void);
void TIM1_UP_TIM10_IRQHandler(void);

int main(void){
	RCC->AHB1ENR = 0x87;
	GPIOA->MODER = 0x28000000;
	GPIOA->PUPDR = 0x0A;
	GPIOC->MODER = 0x1555;
	GPIOB->MODER = 0x555;

	setTim10();
	setTim11();

	int flagb0=0;
	int flagb1=0;
	int b0 = 0;
	int b1 = 0;

	while(1){

		  if((GPIOA->IDR & 0x01) != 0 && flagb0==0){
			  min++;
			  if(min > 59) min = 0;
			  flagb0=1;

		  }

		  if(flagb0){
			  if(++b0 > 40000){
				  if(!(GPIOA->IDR & 0x01)){
					  flagb0=0;
					  b0=0;

				  }
			  }
		  }

		  if((GPIOA->IDR & 0x02) != 0 && flagb1==0){
				  hora++;
				  if(hora > 23) hora = 0;
				  flagb1=1;

			  }

			  if(flagb1){
				  if(++b1 > 40000){
					  if(!(GPIOA->IDR & 0x02)){
						  flagb1=0;
						  b1 = 0;
					  }
				  }
			  }

	}
}

void setTim10(void){ // 1 segundo
	RCC->APB2ENR |= 0x60000;
	TIM10->CR1 |= 0x85;
	TIM10->ARR = 3999;
	TIM10->PSC = 3999;
	TIM10->DIER |= 0x01;
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0);
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);


}

void setTim11(void){ //timer de multiplexa????o
	RCC->APB2ENR |= 0x60000;
	TIM11->CR1 |= 0x85;
	TIM11->ARR = 127;
	TIM11->PSC = 63;
	TIM11->DIER |= 0x01;
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 1);
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

}

void TIM1_TRG_COM_TIM11_IRQHandler(void){
	switch(disp){
		case 0:
			GPIOB->ODR = 0x3E;
			GPIOC->ODR = arr[seg % 10]; //primeiro digito
		break;
		case 1:
			GPIOB->ODR = 0x3D;
			GPIOC->ODR = arr[seg/10]; //segundo digito (vai mover uma casa decimal e concatenar, pois ?? inteiro)
		break;
		case 2:
			GPIOB->ODR = 0x3B;
			GPIOC->ODR = arr[min % 10];
		break;
		case 3:
			GPIOB->ODR = 0x37;
			GPIOC->ODR = arr[min/10];
		break;
		case 4:
			GPIOB->ODR = 0x2F;
			GPIOC->ODR = arr[hora % 10];
		break;
		case 5:
			GPIOB->ODR = 0x1F;
			GPIOC->ODR = arr[hora/10];
		break;
		}
	if(++disp > 5) disp = 0;
	TIM11->SR &= ~(0x01);
}

void TIM1_UP_TIM10_IRQHandler(void){
	if(++seg > 59){
		seg = 0;
		if(++min > 59){
			min = 0;
			if(++hora > 23){
				hora = 0;
			}
		}
	}
	TIM10->SR &= ~(0x01);
}


