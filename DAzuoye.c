#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "debug.h"
#include "gpio.h"
#include "hw_i2c.h"
#include "hw_types.h"
#include "i2c.h"
#include "pin_map.h"
#include "sysctl.h"
#include "systick.h"
#include "interrupt.h"
#include "uart.h"
#include "hw_ints.h"
#include <string.h>

#define TCA6424_I2CADDR 					0x22
#define PCA9557_I2CADDR						0x18

#define PCA9557_INPUT							0x00
#define	PCA9557_OUTPUT						0x01
#define PCA9557_POLINVERT					0x02
#define PCA9557_CONFIG						0x03

#define TCA6424_CONFIG_PORT0			0x0c
#define TCA6424_CONFIG_PORT1			0x0d
#define TCA6424_CONFIG_PORT2			0x0e

#define TCA6424_INPUT_PORT0				0x00
#define TCA6424_INPUT_PORT1				0x01
#define TCA6424_INPUT_PORT2				0x02

#define TCA6424_OUTPUT_PORT0			0x04
#define TCA6424_OUTPUT_PORT1			0x05
#define TCA6424_OUTPUT_PORT2			0x06

void    addbai(void);
void    addge(void);
void    task1(void);
void    task2(void);
void    task3(void);

void 		Delay(uint32_t value);
void 		S800_GPIO_Init(void);
uint8_t 	I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData);
uint8_t 	I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr);
void		S800_I2C0_Init(void);
void 		S800_UART_Init(void);
void UARTStringPut(char *cMessage);
void UARTStringPutNonBlocking(const char *cMessage);
volatile uint8_t result;
uint32_t ui32SysClock;
uint8_t seg7[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x58,0x5e,0x079,0x71,0x5c};
uint16_t kouXuan=0x01;
char getchar[20];
uint8_t i=0,j=0;
uint16_t ge_cnt=10;
uint16_t  flag1=0, flag2=0, flag3=0,startflag1=0,startflag2=0,startflag3=0;
uint16_t ge=0,shi=0,bai=0,qian=0;
uint16_t state1=0,state2=0,flashcnt1=5,flashcnt2=5,flashflag1=0,flashflag2=0;
uint16_t reversetime1=5, reversetime2=5;
uint16_t task1_priority=5,task2_priority=5,task3_priority=5;

int main(void)
{
//use internal 16M oscillator, HSI


   ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_16MHZ |SYSCTL_OSC_INT |SYSCTL_USE_OSC), 16000000);


	 SysTickPeriodSet(ui32SysClock/10);	//?????100mS
	 SysTickEnable();	          //??SysTick
	 SysTickIntEnable();       //SysTick????,?1mS??????
	 IntMasterEnable();      //?????
	 S800_GPIO_Init();
	 S800_I2C0_Init();
	 S800_UART_Init();
	 IntEnable(INT_UART0);
   UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);	//Enable UART0 RX,TX interrupt
	while(1){
		
		
		if((strncasecmp(getchar,"TASK1+PRIORITY",14)==0)) 
		{
		    task1_priority=getchar[15]-'0';
		}
		if((strncasecmp(getchar,"TASK2+PRIORITY",14)==0)) 
		{
		    task2_priority=getchar[15]-'0';
		}
		if((strncasecmp(getchar,"TASK3+PRIORITY",14)==0)) 
		{
		    task3_priority=getchar[15]-'0';
		}
		
    if((strncasecmp(getchar,"TASK1+START",11)==0)) 
		{
		    startflag1=1;
		}
		if((strncasecmp(getchar,"TASK1+STOP",10)==0)) 
		{
		    startflag1=0;
		}
		 if((strncasecmp(getchar,"TASK2+START",11)==0)) 
		{
		    startflag2=1;
		}
		if((strncasecmp(getchar,"TASK2+STOP",10)==0)) 
		{
		    startflag2=0;
		}
		 if((strncasecmp(getchar,"TASK3+START",11)==0)) 
		{
		    startflag3=1;
		}
		if((strncasecmp(getchar,"TASK3+STOP",10)==0)) 
		{
		    startflag3=0;
		}
		if((strncasecmp(getchar,"TASK1+FLASH200MS",16)==0)) 
		{
		    reversetime1=2;
		}
		if((strncasecmp(getchar,"TASK1+FLASH500MS",16)==0)) 
		{
		    reversetime1=5;
		}
		if((strncasecmp(getchar,"TASK1+FLASH1000MS",17)==0)) 
		{
		    reversetime1=10;
		}
		if((strncasecmp(getchar,"TASK1+FLASH2000MS",17)==0)) 
		{
		    reversetime1=20;
		}
			if((strncasecmp(getchar,"TASK2+FLASH200MS",16)==0)) 
		{
		    reversetime2=2;
		}
		if((strncasecmp(getchar,"TASK2+FLASH500MS",16)==0)) 
		{
		     reversetime2=5;
		}
		if((strncasecmp(getchar,"TASK2+FLASH1000MS",17)==0)) 
		{
		     reversetime2=10;
		}
		if((strncasecmp(getchar,"TASK2+FLASH2000MS",17)==0)) 
		{
		     reversetime2=20;
		}  
		while(GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)==0)
		{
			task1();
			if((task2_priority<task1_priority))
		{
				 task2();
		}
		if((task3_priority<task1_priority))
		{
				 task3();
		}
		}
		
		while(GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1)==0)
		{
			task2();
			if((task1_priority<task2_priority))
		{
				 task1();
		}
		if((task3_priority<task2_priority))
		{
				 task3();
		}
		}
		
		task1();
		task2();
		task3();
	}
}


void    task1(void)
{
		if(flashflag1&&startflag1)
		{
			 flashflag1=0;
			if(flag1)
			{
			 flag1=0;	
			 UARTStringPut("task1 is running\n");
			}
			 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, state1);	
		    
		}

}
void    task2(void)
{
		if(flashflag2&&startflag2)
		{
			 flashflag2=0;
			if(flag2)
			{
		   UARTStringPut("task2 is running\n");
			 flag2=0;
			}	
		   GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0,  state2);
		}
		
}
void    task3(void)
{
	if(flag3&&startflag3)  //0_65535
					{
							flag3=0;
							ge++;
							if(ge>=10)
							{
								ge=0;
								shi++;
								if(shi>=6)
								{
									shi=0;
									bai++;
									if(bai>=10)
									{
										bai=bai%10;
										qian++;
										if(qian>=6)
										{
													qian=0;
													shi=0;
													bai=0;
													ge=0;
										 }
										}
									}
								}
							kouXuan = kouXuan<<1;
							if(kouXuan==0x100)
							{
							  kouXuan = 0x01;							
							}
							 result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~kouXuan);					//write led
						}
					 
		switch(j)
					{
						case 0:
						{
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0x00);					//write port 2						
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[qian]);						//write port 1 		
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(1));					//write port 2
							Delay(20000);
							j=1;
						}
						case 1:
						{
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0x00);					//write port 2				
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[bai]);						//write port 1				
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(2));					//write port 2
							Delay(20000);
							j=2;
						}
						case 2:
						{
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0x00);					//write port 2			
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[shi]);						//write port 1 
											
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(4));					//write port 2
							Delay(20000);
							j=3;
						}
						case 3:
						{
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0x00);					//write port 2	
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[ge]);						//write port 1						
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(8));					//write port 2
							Delay(20000);
							j=0;
						}
				}
}

	void UART0_Handler(void)
{

	int32_t uart0_int_status;
  uart0_int_status 		= UARTIntStatus(UART0_BASE, true);		// Get the interrrupt status.

  UARTIntClear(UART0_BASE, uart0_int_status);								//Clear the asserted interrupts
	i=0;
  while(UARTCharsAvail(UART0_BASE))    											// Loop while there are characters in the receive FIFO.
  {
		///Read the next character from the UART and write it back to the UART.
       getchar[i]=UARTCharGetNonBlocking(UART0_BASE);
		   i++;
		   Delay(1000);     //????FIFO???
	}

}

void SysTick_Handler(void)
{
  ge_cnt--;
	flashcnt1--;
	flashcnt2--;
  if(ge_cnt<=0)
	{
	  ge_cnt=10;
		flag1=1;
		flag2=1;
		flag3=1;
	}
	 if(flashcnt1<=0)
	{
	  flashcnt1=reversetime1;
		flashflag1=1;
	}
	if(flashcnt2<=0)
	{
	  flashcnt2=reversetime2;
		flashflag2=1;
	}
	if(flashflag1)
		{
			//flashflag=0;
		  state1=~state1;
		}
		if(flashflag2)
		{
			//flashflag=0;
		  state2=~state2;
		}
}

void Delay(uint32_t value)
{
	uint32_t ui32Loop;
	for(ui32Loop = 0; ui32Loop < value; ui32Loop++){};
}
void UARTStringPut(char *cMessage)
{
	while(*cMessage!='\0')
		UARTCharPut(UART0_BASE,*(cMessage++));
}
void UARTStringPutNonBlocking(const char *cMessage)
{
	while(*cMessage!='\0')
		UARTCharPutNonBlocking(UART0_BASE,*(cMessage++));
}

void S800_UART_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);						//Enable PortA
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));			//Wait for the GPIO moduleA ready

	GPIOPinConfigure(GPIO_PA0_U0RX);												// Set GPIO A0 and A1 as UART pins.
  GPIOPinConfigure(GPIO_PA1_U0TX);

  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
  UARTConfigSetExpClk(UART0_BASE, ui32SysClock,115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
	UARTStringPutNonBlocking("\r\nHello, world!\r\n");
}
void S800_GPIO_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//Enable PortF
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));			//Wait for the GPIO moduleF ready
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));			//Wait for the GPIO moduleJ ready
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);						//Enable PortN
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){};			//Wait for the GPIO moduleN ready

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);			//Set PF0 as Output pin
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);			//Set PN0 as Output pin
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);		//Set PN1 as Output pin

	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1);//Set the PJ0,PJ1 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}
/*void S800_GPIO_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//Enable PortF
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));			//Wait for the GPIO moduleF ready
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));			//Wait for the GPIO moduleJ ready

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);			//Set PF0 as Output pin
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1);//Set the PJ0,PJ1 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}
*/

void S800_I2C0_Init(void)
{
	uint8_t result;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);//???i2c??
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//??I2C??0,?????I2C0SCL--PB2?I2C0SDA--PB3
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);//??PB2?I2C0SCL
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);//??PB3?I2C0SDA
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);//I2C?GPIO_PIN_2??SCL
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);//I2C?GPIO_PIN_3??SDA

	I2CMasterInitExpClk(I2C0_BASE,ui32SysClock, true);										//config I2C0 400k
	I2CMasterEnable(I2C0_BASE);

	result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_CONFIG_PORT0,0x0ff);		//config port 0 as input
	result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_CONFIG_PORT1,0x0);			//config port 1 as output
	result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_CONFIG_PORT2,0x0);			//config port 2 as output

	result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_CONFIG,0x00);					//config port as output
	result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,0x0ff);				//turn off the LED1-8

}


uint8_t I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData)
{
	uint8_t rop;
	while(I2CMasterBusy(I2C0_BASE)){};//??I2C0???,??
		//
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false);
		//????????????????false???????,true???????

	I2CMasterDataPut(I2C0_BASE, RegAddr);//??????????
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);//????????
	while(I2CMasterBusy(I2C0_BASE)){};

	rop = (uint8_t)I2CMasterErr(I2C0_BASE);//???

	I2CMasterDataPut(I2C0_BASE, WriteData);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);//???????????
	while(I2CMasterBusy(I2C0_BASE)){};

	rop = (uint8_t)I2CMasterErr(I2C0_BASE);//???

	return rop;//??????,????0
}

uint8_t I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr)
{
	uint8_t value,rop;
	while(I2CMasterBusy(I2C0_BASE)){};
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false);
	I2CMasterDataPut(I2C0_BASE, RegAddr);
//	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_SEND);//????????
	while(I2CMasterBusBusy(I2C0_BASE));
	rop = (uint8_t)I2CMasterErr(I2C0_BASE);
	Delay(1);
	//receive data
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, true);//??????
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);//???????
	while(I2CMasterBusBusy(I2C0_BASE));
	value=I2CMasterDataGet(I2C0_BASE);//???????
		Delay(1);
	return value;
}


