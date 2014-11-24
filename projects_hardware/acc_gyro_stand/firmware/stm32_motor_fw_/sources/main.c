#include "general_types.h"

#include "hal.h"

#include "tm_stm32f4_delay.h"
#include "sd_processing.h"
#include "main_config_processor.h"

#include "motor.h"
#include "wifi.h"
   
uint8_t tries = 10;
mcu_state_t program_state = INITIALIZING_MCU;

int main(){
    error_t reset_cause = NO_ERROR;

    //while(1){}
    
    while (1){//Switch between programm states in loop
        switch (program_state){
            
            case (INITIALIZING_MCU):
                init();
                TM_DELAY_Init();
            
                //If we reset MCU 10 times, smth goes wrong
                tries--;
                if (tries == 0)
                    error_loop(STM_ERROR);
                
                program_state = MOUNT_SD_CARD;
                led_on(STM_STATUS_LED, 0);//Turn on stm status led
                //chg_wifi_led_state(WF_ON_LED, 1);
                break;
                
            case (MOUNT_SD_CARD):
                if (mount_sd_card() != 0 ){
                    reset_cause = SD_ERROR;
                    program_state = INITIALIZING_MCU;
                }else{
                    program_state = CHECK_SD_CARD;
                    led_off(STM_STATUS_LED);
                }
                break;
                
            case (CHECK_SD_CARD):
                if (check_sd_card_size() != 0 ){
                    reset_cause = SD_ERROR;
                    program_state = INITIALIZING_MCU;
                }else{
                    program_state = READ_MODULE_CONFIG;
                    led_on(STM_STATUS_LED, 0);//Turn on stm status led
                    led_on(SD_STATUS_LED, 0);//Turn on sd status led
                }
            
                program_state = READ_MODULE_CONFIG;
                break;
                
            case (READ_MODULE_CONFIG):
                read_main_cofig();
                //TODO: read_motor_script(). Depends from config
                //TODO: switch sd led state
                led_off(STM_STATUS_LED);
            
                program_state = WIFI_UP;
                break;
            
            case (WIFI_UP):
                if (ex_motor_script() != 0 ){
                    reset_cause = WIFI_ERROR;
                    program_state = INITIALIZING_MCU;
                }
                while(1){}
            
                break;
                
//            case (WIFI_UP):
//                if (ex_wifi_script() != 0 ){
//                    reset_cause = WIFI_ERROR;
//                    program_state = INITIALIZING_MCU;
//                }else{
//                    program_state = WIFI_D_EXCHAGE;
//                    led_on(STM_OK_LED, 0);//Turn on stm ok led
//                    delay_us(600000);
//                    chg_wifi_led_state(WF_ON_LED, 1);
//                    chg_wifi_led_state(WF_UP_LED, 1);
//                    
//                    while(is_adc_connected() != 1){
//                        chg_wifi_led_state(WF_UP_LED, 1);
//                        delay_us(100000);
//                        chg_wifi_led_state(WF_UP_LED, 0);
//                        delay_us(100000);
//                    }
//                }
//            
//                break;
            
           case (WIFI_D_EXCHAGE):
                //TODO: write wifi data exchange processing
            
                program_state = UNMOUNT_SD_CARD;
                break;
  
           case (UNMOUNT_SD_CARD):
                unmount_sd_card();  
                while(1){
                    led_on(SD_STATUS_LED, 300000);
                    delay_us(100000);
                }
                    
                break;
           
           default:
               program_state = INITIALIZING_MCU;
               break;  
        } 
    }        
    
}   