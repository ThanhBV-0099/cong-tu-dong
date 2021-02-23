#include "wifip.h"
#ifdef USE_WIFI 
#include "wifi.h"
#include "mcu_api.h" 
#else
#include "zigbee.h"
#include "mcu_api.h"
#endif
#include "protocol.h"
#ifndef USE_CT1
	#define TOUCH1_PAD1 HAL_GPIO_ReadPin(TOUCH_PAD1_GPIO_Port,TOUCH_PAD1_Pin)
	#define LED1_RED SW1_LED_RED_GPIO_Port,SW1_LED_RED_Pin
	#define LED1_BLUE SW1_LED_BLUE_GPIO_Port,SW1_LED_BLUE_Pin
#else
	#define TOUCH1_PAD1 HAL_GPIO_ReadPin(TOUCH_PAD2_GPIO_Port,TOUCH_PAD2_Pin)
	#define LED1_RED SW2_LED_RED_GPIO_Port,SW2_LED_RED_Pin
	#define LED1_BLUE SW2_LED_BLUE_GPIO_Port,SW2_LED_BLUE_Pin
#endif

#ifndef USE_CT2
	#define TOUCH1_PAD2 HAL_GPIO_ReadPin(TOUCH_PAD2_GPIO_Port,TOUCH_PAD2_Pin)
	#define LED2_RED SW2_LED_RED_GPIO_Port,SW2_LED_RED_Pin
	#define LED2_BLUE SW2_LED_BLUE_GPIO_Port,SW2_LED_BLUE_Pin
#else
	#ifdef PCB4_NUT
		#define TOUCH1_PAD2 HAL_GPIO_ReadPin(TOUCH_PAD4_GPIO_Port,TOUCH_PAD4_Pin)
		#define LED2_RED SW4_LED_RED_GPIO_Port,SW4_LED_RED_Pin
		#define LED2_BLUE SW4_LED_BLUE_GPIO_Port,SW4_LED_BLUE_Pin
	#else
		#define TOUCH1_PAD2 HAL_GPIO_ReadPin(TOUCH_PAD3_GPIO_Port,TOUCH_PAD3_Pin)
		#define LED2_RED SW3_LED_RED_GPIO_Port,SW3_LED_RED_Pin
		#define LED2_BLUE SW3_LED_BLUE_GPIO_Port,SW3_LED_BLUE_Pin
	#endif
#endif

#define TOUCH1_PAD3 HAL_GPIO_ReadPin(TOUCH_PAD3_GPIO_Port,TOUCH_PAD3_Pin)
#define TOUCH1_PAD4 HAL_GPIO_ReadPin(TOUCH_PAD4_GPIO_Port,TOUCH_PAD4_Pin)


#define LED3_RED SW3_LED_RED_GPIO_Port,SW3_LED_RED_Pin
#define LED3_BLUE SW3_LED_BLUE_GPIO_Port,SW3_LED_BLUE_Pin
#define LED4_RED SW4_LED_RED_GPIO_Port,SW4_LED_RED_Pin
#define LED4_BLUE SW4_LED_BLUE_GPIO_Port,SW4_LED_BLUE_Pin



////////////////struct rf
typedef struct
{
	uint8_t SOF;
	uint8_t STATE_SW1;
	uint8_t STATE_SW2;
	uint8_t STATE_SW3;
	uint8_t STATE_SW4;
	uint64_t MAC;
	uint8_t EOF1;
	uint8_t EOF2;
}FRAME_RECEIVE_RF;
volatile FRAME_RECEIVE_RF Frame_Receive_Rf;
extern UART_HandleTypeDef huart1;
volatile uint8_t Nhanbuff,state_receive = 0;
volatile uint8_t Nhanbuff_rf[200],Nhan_rf,state_receive_rf = 0,count_rf = 0;
void Uart_PutChar(unsigned char value)
{
	HAL_UART_AbortReceive_IT(&huart1);
	HAL_UART_Transmit(&huart1,&value,1,100);
	HAL_UART_Receive_IT(&huart1,&Nhanbuff,1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==huart1.Instance)//nhan du lieu uart1
	{
		uart_receive_input(Nhanbuff);
		//tiep tuc nhan du lieu
		HAL_UART_Receive_IT(&huart1,&Nhanbuff,1);
	}
	}
unsigned char wifi_state;
volatile unsigned char State_switch_1;
volatile unsigned char State_switch_2;
volatile unsigned char State_switch_3;
volatile unsigned char State_switch_4;
unsigned long State_countdown_1;
unsigned long State_countdown_2;
unsigned long State_countdown_3;
unsigned long State_countdown_4;
unsigned long State_thong_so1; // thong so add vao cong suat toi thieu 10 wh
unsigned long State_thong_so2; // thong so dong dien don vi mA
unsigned long State_thong_so3; // Thong so cong suat don vi chia 10 ra W
unsigned long State_thong_so4; // thong so dien ap chia 10 ra vol
unsigned long State_thong_so1_count; // thong so add vao cong suat toi thieu 10 wh
unsigned long State_thong_so2_count; // thong so dong dien don vi mA
unsigned long State_thong_so3_count; // Thong so cong suat don vi chia 10 ra W
unsigned long State_thong_so4_count; // thong so dien ap chia 10 ra vol
float diennang = 0;
unsigned char dodienap = 0,nead_update_dienanng = 0;
uint16_t count_nhay = 0,count_update = 0,count_setup = 0,time_count_setup = 0,count_reset_heart = 0;
extern unsigned char switch_1;
extern unsigned char switch_2;
extern unsigned char switch_3;
extern unsigned char up_switch_1;
extern unsigned char up_switch_2;
extern unsigned char up_switch_3;
//thong so 4 1605Hz tuong ung 220V
//thong so 3 20 tuong ung 25W
//thong so 2 12 tuong ung 0.11A


void coundown_process(void)
{
	static uint16_t count_1s = 0;
	
	if(count_1s >= 1000)
	{
		count_1s = 0;
		//cu 1 s do dien nang 1 lan
		
		//process cho count down 1
		if(State_countdown_1 > 0)
		{
			if(State_countdown_1 >1)
			{
				State_countdown_1 --;
			}
			else //neu dung bang 1 thi togle thiet bi
			{
				State_countdown_1 = 0;
				count_update = TIME_NEED_UPDATE;
				if(State_switch_1 == 0)
				{
					State_switch_1 = 1;
				}
				else
				{
					State_switch_1 = 0;
				}
			}
		}
		else
		{
			State_countdown_1 = 0;
		}
		
		//process cho count down 2
		if(State_countdown_2 > 0)
		{
			if(State_countdown_2 >1)
			{
				State_countdown_2 --;
			}
			else //neu dung bang 1 thi togle thiet bi
			{
				State_countdown_2 = 0;
				count_update = TIME_NEED_UPDATE;
				if(State_switch_2 == 0)
				{
					State_switch_2 = 1;
				}
				else
				{
					State_switch_2 = 0;
				}
			}
		}
		else
		{
			State_countdown_2 = 0;
		}
		
		//process cho count down 3
		if(State_countdown_3 > 0)
		{
			if(State_countdown_3 >1)
			{
				State_countdown_3 --;
			}
			else //neu dung bang 1 thi togle thiet bi
			{
				State_countdown_3 = 0;
				count_update = TIME_NEED_UPDATE;
				if(State_switch_3 == 0)
				{
					State_switch_3 = 1;
				}
				else
				{
					State_switch_3 = 0;
				}
			}
		}
		else
		{
			State_countdown_3 = 0;
		}
		
		//process cho count down 4
		if(State_countdown_4 > 0)
		{
			if(State_countdown_4 >1)
			{
				State_countdown_4 --;
			}
			else //neu dung bang 1 thi togle thiet bi
			{
				State_countdown_4 = 0;
				count_update = TIME_NEED_UPDATE;
				if(State_switch_4 == 0)
				{
					State_switch_4 = 1;
				}
				else
				{
					State_switch_4 = 0;
				}
			}
		}
		else
		{
			State_countdown_4 = 0;
		}
	}
	else
	{
		count_1s ++;
	}
	
}

void wifiprocess(void)
{
	static uint16_t count_wifi_status = 0,count_blink_1s = 0,modeconfig = 0,timeout_config = 0,count_wifi_status_blink = 0,
									old_pad1 = 0,old_pad2 = 0,old_pad3 = 0,old_pad4 = 0,count_config_wifi = 0,state_config = 0,old_state1 = 0,
									old_state2 = 0,old_state3 = 0,old_state4 = 0,timeout_update_rf = 0,count_reset_touch = 0,time_count_reset_touch = 0,flag_reset_touch = 0,
									cycle_count_reset_touch = 0,time_tre1 = 0,time_tre2 = 0,time_tre3 = 0,time_tre4 = 0,bien1=0,bien2=0,bien3=0;
	static uint8_t has_change_touchpad = 0,old_button = 0;
	#ifdef USE_WIFI 
	wifi_uart_service();
		wifi_state = mcu_get_wifi_work_state();
		if(wifi_state == WIFI_LOW_POWER)
		{
//			sprintf(m,"LOWPOW");
		}
		else if(wifi_state == SMART_CONFIG_STATE)
		{
			count_wifi_status_blink = 25;
//			sprintf(m,"CONFIG");
		}
		else if(wifi_state == AP_STATE)
		{
//			sprintf(m,"AP_STA");
		}
		else if(wifi_state == WIFI_NOT_CONNECTED)
		{
			count_wifi_status_blink = 200;
//			sprintf(m,"NOT_CO");
		}
		else if(wifi_state == WIFI_CONNECTED)
		{
			count_wifi_status_blink = 0;
//			sprintf(m,"CONNEC");
		}
		else if(wifi_state == WIFI_CONN_CLOUD)
		{
//			sprintf(m,"CLOUD ");
			count_wifi_status_blink = 0;
		}
		else if(wifi_state == WIFI_SATE_UNKNOW)
		{
			count_wifi_status_blink = 1;
//			sprintf(m,"UNKNOW");
		}
		#else
		zigbee_uart_service();
		#endif
		//count cho hien thi trang thai wifi
		if(count_wifi_status_blink == 0)
		{
			HAL_GPIO_WritePin(WIFI_STATUS_GPIO_Port,WIFI_STATUS_Pin,GPIO_PIN_SET);
		}
		else if(count_wifi_status> count_wifi_status_blink)
		{
			count_wifi_status = 0;
			#ifdef USE_WIFI 
			//neu o trang thai config thi nhay 3 nut mau xanh
			if(wifi_state == SMART_CONFIG_STATE)
			{
//				HAL_GPIO_WritePin(LED1_RED,GPIO_PIN _RESET);
//				HAL_GPIO_WritePin(LED2_RED,GPIO_PIN_RESET);
//				HAL_GPIO_WritePin(LED3_RED,GPIO_PIN_RESET);
//				
//				
//				HAL_GPIO_TogglePin(LED1_BLUE);
//				HAL_GPIO_WritePin(LED2_BLUE, HAL_GPIO_ReadPin(LED1_BLUE) );
//				HAL_GPIO_WritePin(LED3_BLUE, HAL_GPIO_ReadPin(LED1_BLUE) );
			
			}
			#endif
			HAL_GPIO_TogglePin(WIFI_STATUS_GPIO_Port,WIFI_STATUS_Pin);
		}
		else
		{
			count_wifi_status++;
		}
		
		
		
		/////count cho update data;
		if(count_update> TIME_UPDATE)
		{
			if(nead_update_dienanng == 1)
			{
				count_update = 400;
			}
			else
			{
				count_update = 0;
			}

			#ifdef USE_WIFI 
			all_data_update();
			#else
			all_data_update();
			#endif
			
			State_thong_so1 = 0;
			
		}
		else
		{
			count_update++;
		}
		HAL_UART_Receive_IT(&huart1,&Nhanbuff,1);
		
		//het timeout cua count setup
		
		if(time_count_setup> 100)
		{
			//time_count_setup = 0;
			//neu het timeout ma count chua lon hon 6 thi clear di
				count_setup = 0;
			old_button = 0;
			time_count_setup = 0;
		}
		else
		{
			time_count_setup++;
		}
		
		//count cho blink cac che do
		if(count_blink_1s> 50)
		{
			count_blink_1s = 0;
			if(modeconfig == 1)
			{
				//neu o cho do config thi nhay cac led len
				if(timeout_config >= 30)
				{
					modeconfig = 0;
					timeout_config = 0;
				}
				else
				{
					timeout_config++;
				}
				HAL_GPIO_WritePin(LED1_RED,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LED2_RED,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LED3_RED,GPIO_PIN_RESET);
				HAL_GPIO_TogglePin(LED1_BLUE);
				#ifndef USE_CT1
				HAL_GPIO_TogglePin(LED2_BLUE);
				#endif
				#ifndef USE_CT2
				HAL_GPIO_TogglePin(LED3_BLUE);
				#endif
			}
			else
			{
				modeconfig = 0;
				timeout_config = 0;
			}
		}
		else
		{
			count_blink_1s++;
		}
		
		//nut 1
		if(TOUCH1_PAD1 ==GPIO_PIN_RESET && time_count_reset_touch == 0 )
		{
//			if(time_tre1 >= TIME_TRE_MAX)
//			{
				
				time_count_setup = 0;
				cycle_count_reset_touch = 0;
				if(count_config_wifi >= 200 && count_setup == NUM_OFF_COUNT_SETUP )
				{
					count_config_wifi = 200;
					#ifdef USE_WIFI 
					mcu_set_wifi_mode(0);
					#else
					mcu_network_start();
					#endif
					modeconfig = 1;//cho bang 1 de nhay led the hien dang che do config
					count_setup = 0;
				}
				
				else
				{
					count_config_wifi ++;
				}
				
				//HAL_Delay(100);
				if(old_pad1 == 0)
				{
					bien1=1;
					count_setup++;
					//reset count dem cho nhay
					count_nhay = 0;

					if(State_switch_1 == 0)
					{
						State_switch_1 = 1;
					}
					else
					{
						State_switch_1 = 0;
					}
					
					//count_update = TIME_NEED_UPDATE;
					count_update = 0;
					mcu_dp_bool_update(DPID_SWITCH_1,State_switch_1);		
					old_pad1 = 1;
//				}
//			}
//			else
//			{
//				time_tre1++;
//			}
//		}
	}
}
		else
		{
			old_pad1 = 0;
			//time_tre1=0;
		}
		
		#ifndef USE_CT1
		
		if(TOUCH1_PAD2 ==GPIO_PIN_RESET && time_count_reset_touch == 0 )
		{
//			if(time_tre1 >= TIME_TRE_MAX)
//			{
			
				time_count_setup = 0;
				cycle_count_reset_touch = 0;
				if(count_config_wifi >= 200 && count_setup == NUM_OFF_COUNT_SETUP )
				{
					count_config_wifi = 200;
					#ifdef USE_WIFI 
					mcu_set_wifi_mode(0);
					#else
					mcu_network_start();
					#endif
					modeconfig = 1;//cho bang 1 de nhay led the hien dang che do config
					count_setup = 0;
				}
				
				else
				{
					count_config_wifi ++;
				}
				
				//HAL_Delay(100);
				if(old_pad2 == 0)
				{
					bien2=1;
					count_setup++;
					//reset count dem cho nhay
					count_nhay = 0;

					if(State_switch_2 == 0)
					{
						State_switch_2 = 1;
					}
					else
					{
						State_switch_2 = 0;
					}
					
					//count_update = TIME_NEED_UPDATE;
					count_update = 0;	
					mcu_dp_bool_update(DPID_SWITCH_2,State_switch_2);	
					old_pad2 = 1;
	}
}
		else
		{
			old_pad2 = 0;
		}
		#endif
		
		#ifndef USE_CT2
		//nut 3
		if(TOUCH1_PAD3 ==GPIO_PIN_RESET && time_count_reset_touch == 0 )
		{
//			if(time_tre1 >= TIME_TRE_MAX)
//			{
			
				time_count_setup = 0;
				cycle_count_reset_touch = 0;
				if(count_config_wifi >= 200 && count_setup == NUM_OFF_COUNT_SETUP )
				{
					count_config_wifi = 200;
					#ifdef USE_WIFI 
					mcu_set_wifi_mode(0);
					#else
					mcu_network_start();
					#endif
					modeconfig = 1;//cho bang 1 de nhay led the hien dang che do config
					count_setup = 0;
				}
				
				else
				{
					count_config_wifi ++;
				}
				
				//HAL_Delay(100);
				if(old_pad3 == 0)
				{
					bien3=1;
					//reset count dem cho nhay
					count_nhay = 0;
					count_setup++;
					if(State_switch_3 == 0)
					{
						State_switch_3 = 1;
					}
					else
					{
						State_switch_3 = 0;
					}
					
					//count_update = TIME_NEED_UPDATE;
					count_update = 0;
					mcu_dp_bool_update(DPID_SWITCH_3,State_switch_3);		
					old_pad3 = 1;
	}
}
		else
		{
			old_pad3 = 0;
		}
#endif
		
		//truong hop ko co nut nao an thi reset count_config_wifi = 0;
		if( 
				(TOUCH1_PAD1 != GPIO_PIN_RESET)
				#ifndef USE_CT1
				&& (TOUCH1_PAD2 != GPIO_PIN_RESET)
				#endif		
				#ifndef USE_CT2
				&& (TOUCH1_PAD3 != GPIO_PIN_RESET)
				#endif
				#ifndef USE_CT3 
				&& (TOUCH1_PAD4 != GPIO_PIN_RESET) 
				#endif
				)
		{
			count_config_wifi = 0;
			count_reset_touch = 0;
		}
		
		if( State_switch_1 == 1 )
		{
		if(bien1==1 ||  up_switch_1==1)
		{
			HAL_GPIO_WritePin(RELAY2_GPIO_Port,RELAY2_Pin,GPIO_PIN_SET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED1_RED,GPIO_PIN_RESET);	
				HAL_GPIO_WritePin(LED1_BLUE,GPIO_PIN_SET);
			}
			HAL_Delay(200);
				HAL_GPIO_WritePin(RELAY2_GPIO_Port,RELAY2_Pin,GPIO_PIN_RESET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED1_RED,GPIO_PIN_SET);	
				HAL_GPIO_WritePin(LED1_BLUE,GPIO_PIN_RESET);
			}
		}
		bien1=0;
		up_switch_1=0;
	}
		else
		{
			if(bien1==1 || up_switch_1==1)
		{
			HAL_GPIO_WritePin(RELAY2_GPIO_Port,RELAY2_Pin,GPIO_PIN_SET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED1_RED,GPIO_PIN_RESET);	
				HAL_GPIO_WritePin(LED1_BLUE,GPIO_PIN_SET);
			}
			HAL_Delay(200);
				HAL_GPIO_WritePin(RELAY2_GPIO_Port,RELAY2_Pin,GPIO_PIN_RESET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED1_RED,GPIO_PIN_SET);	
				HAL_GPIO_WritePin(LED1_BLUE,GPIO_PIN_RESET);
			}
		}
		bien1=0;
		up_switch_1=0;
		}

		#ifndef USE_CT1
		if( State_switch_2 == 1)
		{
			if(bien2==1 || up_switch_2==1)
		{
			HAL_GPIO_WritePin(RELAY3_GPIO_Port,RELAY3_Pin,GPIO_PIN_SET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED2_RED,GPIO_PIN_RESET);	
				HAL_GPIO_WritePin(LED2_BLUE,GPIO_PIN_SET);
			}
			HAL_Delay(200);
			HAL_GPIO_WritePin(RELAY3_GPIO_Port,RELAY3_Pin,GPIO_PIN_RESET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED2_RED,GPIO_PIN_SET);	
				HAL_GPIO_WritePin(LED2_BLUE,GPIO_PIN_RESET);
			}
		bien2=0;
			up_switch_2=0;
		}
	}
		else
		{
			if(bien2==1 || up_switch_2==1)
		{
			HAL_GPIO_WritePin(RELAY3_GPIO_Port,RELAY3_Pin,GPIO_PIN_SET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED2_RED,GPIO_PIN_RESET);	
				HAL_GPIO_WritePin(LED2_BLUE,GPIO_PIN_SET);
			}
			HAL_Delay(200);
			HAL_GPIO_WritePin(RELAY3_GPIO_Port,RELAY3_Pin,GPIO_PIN_RESET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED2_RED,GPIO_PIN_SET);	
				HAL_GPIO_WritePin(LED2_BLUE,GPIO_PIN_RESET);
			}
		bien2=0;
		up_switch_2=0;
		}
	}
		#endif
		
		#ifndef USE_CT2
		if( State_switch_3 == 1)
		{
			if(bien3==1 || up_switch_3==1)
		{
			HAL_GPIO_WritePin(RELAY4_GPIO_Port,RELAY4_Pin,GPIO_PIN_SET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED3_RED,GPIO_PIN_RESET);	
				HAL_GPIO_WritePin(LED3_BLUE,GPIO_PIN_SET);
			}
			HAL_Delay(200);
				HAL_GPIO_WritePin(RELAY4_GPIO_Port,RELAY4_Pin,GPIO_PIN_RESET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED3_RED,GPIO_PIN_SET);	
				HAL_GPIO_WritePin(LED3_BLUE,GPIO_PIN_RESET);
			}
		}
		bien3=0;
		up_switch_3=0;
		}
		else
		{
			if(bien3==1 || up_switch_3==1)
		{
			HAL_GPIO_WritePin(RELAY4_GPIO_Port,RELAY4_Pin,GPIO_PIN_SET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED3_RED,GPIO_PIN_RESET);	
				HAL_GPIO_WritePin(LED3_BLUE,GPIO_PIN_SET);
			}
			HAL_Delay(200);
				HAL_GPIO_WritePin(RELAY4_GPIO_Port,RELAY4_Pin,GPIO_PIN_RESET);
			if(modeconfig == 0)
			{
				HAL_GPIO_WritePin(LED3_RED,GPIO_PIN_SET);	
				HAL_GPIO_WritePin(LED3_BLUE,GPIO_PIN_RESET);
			}
		}
		bien3=0;
		up_switch_3=0;
		}
		#endif
		
	
}
void wifi_init(void)
{
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port,ESP_RESET_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port,ESP_RESET_Pin,GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_UART_Receive_IT(&huart1,&Nhanbuff,1);
	
	
	#ifdef USE_WIFI 
	wifi_protocol_init();
	#else
	zigbee_protocol_init();
	#endif
	#ifdef USE_WIFI 
	//tiep tuc nhan du lieu
	HAL_GPIO_WritePin(RESET_ZIGBEE_GPIO_Port,RESET_ZIGBEE_Pin,GPIO_PIN_SET);
	#endif
}
