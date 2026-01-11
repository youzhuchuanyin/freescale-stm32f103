#include "uart.h"				//声明串口1
#include "uart2.h"
#include "string.h"			//声明字符串操作库函数
#include "init.h"
#include "cmd.h"
#include "interrupt.h"
#include "dma.h"
#include "esp8266.h"
#include "stdio.h"
#include "msg.h"
#include "led.h"
#include "ds18b20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "beep.h"
#include "motor.h"
#include "oled.h"
#define BUF_LEN			100
//分配内存暂存从上位机接收到的字符串
static u8 buf[BUF_LEN];
//暂存匹配成功的命令结构体首地址
static cmd_t *pcmd;
//RS485MODBUS
void vTaskFor485(void * arg){
	int moisture;
	int temperature;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(6000);
	while(1){	
		u8 display[40]={0};
		unsigned short CRC16 = 0;
		UART2_TXDATA[0] = 0x01;
		UART2_TXDATA[1] = 0x03;
		UART2_TXDATA[2] = 0x00;
		UART2_TXDATA[3] = 0x00;
		UART2_TXDATA[4] = 0x00;
		UART2_TXDATA[5] = 0x02;
		CRC16 = Get_CRC16(UART2_TXDATA, 6);
		UART2_TXDATA[6] = CRC16 & 0xFF;
		unsigned char temp= (CRC16 >> 8) & 0xFF;
		//printf("temp = 0x%02X\n", temp);     // 打印调试
		UART2_TXDATA[7] =temp;
		UART2_TXFUNC(UART2_TXDATA, 8);
		
		while(1) {
			if(UART2_RxCounter != 0) {//有接受到数据
				if(UART2_RXDATA[0] == 0x01) {
						modbus_req_t modbus_req;
						modbus_req.msg.msgid = O_STM32_X6818_MODBUS_REQ;
						modbus_req.temp = ((UART2_RXDATA[3] << 8)|UART2_RXDATA[4])/10.0;
						modbus_req.moisture = ((UART2_RXDATA[5] << 8)|UART2_RXDATA[6])/10.0;
						UART3_TxData((unsigned char *)&modbus_req, sizeof(modbus_req));
						printf("**** temp = %.2f mois = %.2f ******\n", modbus_req.temp, modbus_req.moisture);
						temperature = modbus_req.temp;
						moisture= modbus_req.moisture;
						sprintf((char *)display, "Temp:%d Mois:%d", temperature, moisture);
						OLED_Draw_String(4,0,display);
						UART2_RxCounter = 0;
						break;
					}
			}
		}
	vTaskDelay(xTicksToWait);
	}
}
//发送温度
void vTaskForTemp(void *arg) {
	while(1) {
		const TickType_t xTicksToWait = pdMS_TO_TICKS(5000);
		
		temp_req_t temp_req;
		temp_req.msgh.msgid = O_STM32_X6818_TEMP_REQ;
		temp_req.temp = DS18B20_GetTemperture();
		UART3_TxData((unsigned char *)&temp_req, sizeof(temp_req));
		printf("**** temp = %.2f ******\n", temp_req.temp);
		
		vTaskDelay(xTicksToWait);
	}
}
//接收led控制命令并返回状态
void vTaskForLed(void *arg) {
	int ccr=(int)arg;
	while(1) {
		const TickType_t xTicksToWait = pdMS_TO_TICKS(50);
		
		if(WIFI_RxCounter != 0) {
			msghead_t msgh;
			memcpy(&msgh, WIFI_RxBuff, sizeof(msgh));
			printf("***** msgid = %d ********\n", msgh.msgid);
			switch(msgh.msgid) {
				case O_X6818_STM32_LED_REQ:
				{
					led_req_t led_req;
					memcpy(&led_req, WIFI_RxBuff, sizeof(led_req));
					if(led_req.index == 1 && led_req.cmd == 1)
						LED1 = 0;
					else if(led_req.index == 1 && led_req.cmd == 0)
						LED1 = 1;
					else if(led_req.index == 2 && led_req.cmd == 1)
						LED2 = 0;
					else if(led_req.index == 2 && led_req.cmd == 0)
						LED2 = 1;
					
					led_rsp_t led_rsp;
					led_rsp.msgh.msgid = O_STM32_X6818_LED_RSP;
					led_rsp.success = 1;
					UART3_TxData((unsigned char *)&led_rsp, sizeof(led_rsp));
					break;
				}
				case O_X6818_STM32_BEEP_REQ:
				{
					beep_req_t beep_req;
					memcpy(&beep_req,WIFI_RxBuff,sizeof(beep_req));
					if(beep_req.index == 1 && beep_req.cmd == 1)
						BEEP = 1;
					else if(beep_req.index == 1 && beep_req.cmd == 0)
						BEEP = 0;
					
					beep_rsp_t beep_rsp;
					beep_rsp.msgh.msgid = O_STM32_X6818_BEEP_RSP;
					beep_rsp.success = 1;
					UART3_TxData((unsigned char *)&beep_rsp, sizeof(beep_rsp));
				}
				break;
				case O_X6818_STM32_MOTOR_REQ:
				{
					motor_req_t motor_req;
					memcpy(&motor_req,WIFI_RxBuff,sizeof(motor_req));
					switch(motor_req.cmd) {
						case MOTOR_FORWARD:
							GPIO_SetBits(GPIOB, GPIO_Pin_7);
							GPIO_ResetBits(GPIOB, GPIO_Pin_4);
							TIM_SetCompare2(TIM2,ccr);
							GPIO_SetBits(GPIOB, GPIO_Pin_0);
							GPIO_ResetBits(GPIOB, GPIO_Pin_2);
							TIM_SetCompare4(TIM3,ccr);
							printf("*** FRONT%d ***\n",ccr);
							break;
						case MOTOR_BACK:
							GPIO_SetBits(GPIOB, GPIO_Pin_4);
							GPIO_ResetBits(GPIOB, GPIO_Pin_7);
							TIM_SetCompare2(TIM2,ccr);
							GPIO_SetBits(GPIOB, GPIO_Pin_2);
							GPIO_ResetBits(GPIOB, GPIO_Pin_0);
							TIM_SetCompare4(TIM3,ccr);
							printf("*** BACK ***\n");
							break;
						case MOTOR_STOP:
							GPIO_ResetBits(GPIOB, GPIO_Pin_4);
							GPIO_ResetBits(GPIOB, GPIO_Pin_7);
							TIM_SetCompare2(TIM2,0);
							GPIO_ResetBits(GPIOB, GPIO_Pin_2);
							GPIO_ResetBits(GPIOB, GPIO_Pin_0);
							TIM_SetCompare4(TIM3,0);
							printf("*** STOP ***\n");
							break;
						case MOTOR_ACCELERATE:
							ccr += 2;
							TIM_SetCompare2(TIM2,ccr);
							TIM_SetCompare4(TIM3,ccr);
							printf("*** UP ***\n");
							break;
						case MOTOR_SLOWdOWN:
							ccr -= 2;
							TIM_SetCompare2(TIM2,ccr);
							TIM_SetCompare4(TIM3,ccr);
							printf("*** DOWN ***\n");
							break;
						case MOTOR_LEFT:
							GPIO_SetBits(GPIOB, GPIO_Pin_7);
							GPIO_ResetBits(GPIOB, GPIO_Pin_4);
							TIM_SetCompare2(TIM2,ccr);
							GPIO_SetBits(GPIOB, GPIO_Pin_2);
							GPIO_ResetBits(GPIOB, GPIO_Pin_0);
							TIM_SetCompare4(TIM3,ccr);
							printf("*** LEFT ***\n");
							break;
						case MOTOR_RIGHT:
							GPIO_SetBits(GPIOB, GPIO_Pin_4);
							GPIO_ResetBits(GPIOB, GPIO_Pin_7);
							TIM_SetCompare2(TIM2,ccr);
							GPIO_SetBits(GPIOB, GPIO_Pin_0);
							GPIO_ResetBits(GPIOB, GPIO_Pin_2);
							TIM_SetCompare4(TIM3,ccr);
							printf("*** RIGHT ***\n");
							break;
					}
					motor_rsp_t motor_rsp;
					motor_rsp.msgh.msgid=O_STM32_X6818_MOTOR_RSP;
					motor_rsp.msgh.msglen=sizeof(motor_rsp);
					motor_rsp.success=1;
					UART3_TxData((unsigned char *)&motor_rsp, sizeof(motor_rsp));
				}
			}			
			WIFI_RxCounter = 0;	//等待接收下一个数据
			memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE); //情况接收缓冲区准备存储下一个数据
		}
		vTaskDelay(xTicksToWait);
	}
}

int main(void) {

	//硬件初始化
	DEVICE_Init();
	
	//指定中断优先级分组，分2组,表示抢占式优先级和子优先级的级别都是2，值分别是0~3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*测试区*/
	//SERVO_Test();
	//printf("%s\n","Modbus_03_Master");
	//Modbus_03_Master();
	
	
	//读取ROM值
	DS18B20_ReadRom();
	
	//连接X6818服务器
	WIFI_Connect();
	//下位机循环给上位机发送字符串	
	//创建任务1
	xTaskCreate(vTaskForTemp,	"Temp", 100, NULL, 1, NULL);
	xTaskCreate(vTaskForLed,	"Led", 100, (void *)60, 3, NULL);
	xTaskCreate(vTaskFor485,	"RS485",100,NULL,2,NULL);
	//启动调度器，任务开始执行
	vTaskStartScheduler();

}

