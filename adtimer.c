#include <stdio.h>
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
#include "string.h"
#include "hw_ints.h"


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


void 		Delay(uint32_t value);
char ASCII2Disp(char *buff); 
void 		S800_GPIO_Init(void);
uint8_t 	I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData);
uint8_t 	I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr);
void		S800_I2C0_Init(void);
volatile uint8_t result;
uint32_t ui32SysClock;
uint8_t seg7[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x58,0x5e,0x079,0x71,0x5c};
uint16_t kouXuan=0x01;
uint16_t hour=0,minute=0,second=0;
uint16_t ge_cnt=10,cnt=0;
uint16_t flag1=0;
char disp_buff[9];
char	const disp_tab[]			={'0','1','2','3','4','5',     //the could display char and its segment code   
															 '6','7','8','9','A','b',
															 'C','d','E','F',
															 'H','L','P','o',
															 '.','-','_',' '}; 
char const disp_tab_7seg[]	={0x3F,0x06,0x5B,0x4F,0x66,0x6D,  
															0x7D,0x07,0x7F,0x6F,0x77,0x7C,
															0x39,0x5E,0x79,0x71, 
															0x76,0x38,0x73,0x5c,
															0x80,0x40, 0x08,0x00}; 








int main(void)
{
//use internal 16M oscillator, HSI


   ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_16MHZ |SYSCTL_OSC_INT |SYSCTL_USE_OSC), 16000000);


	 SysTickPeriodSet(ui32SysClock);	//?????100mS
	 SysTickEnable();	          //??SysTick
	 SysTickIntEnable();       //SysTick????,?1mS??????
	 IntMasterEnable();      //?????
	 S800_GPIO_Init();
	 S800_I2C0_Init();
	 sprintf(disp_buff,"%02u%s%02u%s%02u",hour,"-",minute,"-",second);
	while(1){
		
		          sprintf(disp_buff,"%02u%s%02u%s%02u",hour,"-",minute,"-",second);
		          result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0x00);					//write port 2						
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,ASCII2Disp(disp_buff+cnt));						//write port 1 		
							result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,kouXuan);					//write port 2
							Delay(10000);
		 
						  cnt++;
							kouXuan = kouXuan<<1;
							if(kouXuan==0x100)
							{
							  kouXuan = 0x01;	
                cnt=0;								
							}
							 result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~kouXuan);					//write led
					 	
		}
					 
					
}

	 

void SysTick_Handler(void)
{
    second++;
//???????
		if (second==60) {minute++; second=0;}
		if (minute==60) {hour++; minute=0;}
		if (hour==24) {hour=0;}	
}

char ASCII2Disp(char *buff)
{
	char * pcDisp;
	pcDisp 				=(char*)strchr(disp_tab,*buff);	
	if (pcDisp		== NULL)
		return 0x0;
	else
		return (disp_tab_7seg[pcDisp-disp_tab]);

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



