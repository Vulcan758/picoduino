#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

/*
(all pin nums given in accords to gpio number)
-8 configurable DIO (2, 3, 6, 7, 8, 9, 10, 11)
-2 analog out (12, 13)
-2 analog in (26, 27)
-UART
--- primary UART (0, 1)
--- secondary UART (4, 5)
-error handling
*/

const bool OUTPUT = true;
const bool INPUT = false;

const bool HIGH = true;
const bool LOW = false;

const int DIO_N = 8;
const int AO_N = 2;
const int AI_N = 2;
const int UART_PRIM_N = 2;
const int UART_SEC_N = 2;

const int DIO[8] = {2, 3, 6, 7, 8, 9, 10, 11};
const int AO[2] = {12, 13};
const int AI[2][2] = {{26, 0}, {27, 1}};
const int UART_PRIM[2] = {0, 1};
const int UART_SEC[2] = {4, 5};
const int BAUD[2] = {9600, 115200}; 


int pinMode(uint gpio, bool in_out){
    for (int i = 0; i < DIO_N; i++){
        if (gpio == DIO[i]){
            gpio_init(gpio);
            gpio_set_dir(gpio, in_out);
            return 0;
        }
    }
    return 1;
}

int digitalWrite(uint gpio, bool high_low){
    for (int i = 0; i < DIO_N; i++){
        if (gpio == DIO[i]){
            gpio_put(gpio, high_low);  
            return 0;
        }
    }
    return 1;
}

int togglePin(uint gpio){
    for (int i = 0; i < DIO_N; i++){
        if (gpio == DIO[i]){
            if (gpio_get_out_level(gpio) == true){
                gpio_put(gpio, false);
            }
            else{
                gpio_put(gpio, true);
            }
        return 0;
        }
    }
    return 1;
}

float analogRead(uint gpio){
    for (int i = 0; i < AI_N; i++){
        if (gpio == AI[i][0]){
            adc_init();
            adc_gpio_init(gpio);
            adc_select_input(AI[i][1]);
            uint16_t value = adc_read();
            float val = value * 1.0;
            return val;
        }
    }
    return 1.0;

}

int io_pwm_init(uint gpio){
    for (int i = 0; i < AO_N; i++){
        if (gpio == AO[i]){
            gpio_set_function(gpio, GPIO_FUNC_PWM);
            uint slice_num = pwm_gpio_to_slice_num(gpio);
            pwm_config config = pwm_get_default_config();
            pwm_config_set_clkdiv(&config, 4.f);
            pwm_init(slice_num, &config, true);

            return 0;
        }
    }
    return 1;

}

int analogWrite(uint gpio, float value){
    if (value > 1023 | value < 0){
        return 1;
    }
    for (int i = 0; i < AO_N; i++){
        if (gpio == AO[i]){
            pwm_set_gpio_level(gpio, value);
            return 0;
        }
    }
    return 1;
}

// do make sure uart is enabled on the cmakelist file for both
int primary_uart_init(uint baudrate){
    for (int i = 0; i < 2; i++){
        if (baudrate == BAUD[i]){
            gpio_set_function(UART_PRIM[0], UART_FUNCSEL_NUM(uart0, 0));
            gpio_set_function(UART_PRIM[1], UART_FUNCSEL_NUM(uart0, 1));
            uart_init(uart0, baudrate);
            return 0;
        }
    }
    return 2;
}

int secondary_uart_init(uint baudrate){
    for (int i = 0; i < 2; i++){
        if (baudrate == BAUD[i]){
            gpio_set_function(UART_SEC[0], UART_FUNCSEL_NUM(uart1, 0));
            gpio_set_function(UART_SEC[1], UART_FUNCSEL_NUM(uart1, 1));
            uart_init(uart1, baudrate);
            return 0;
        }
    }
    return 2;
}

int primary_uart_write(const char *s){
    // somehow preprocess the string to check if there is anything wrong and return another value if something is wrong
    if (uart_is_enabled(uart0)){
        uart_puts(uart0, s);
        return 0;
    }
    return 2;
}

int secondary_uart_write(const char *s){ // this is the debug uart btw
    // somehow preprocess the string to check if there is anything wrong and return another value if something is wrong
    if (uart_is_enabled(uart1)){
        uart_puts(uart1, s);
        return 0;
    }
    return 2;
}
/*
    here is how we will define errors:
    0 ---> success
    1 ---> pin error, i.e. wrong pin, or pin not usable for specified purpose
    2 --> uart related possible errors.
*/
void error_check(int func){
    stdio_init_all();
    secondary_uart_init(115200);
    char *message;
    if (func == 0){
        message = "Success";
    }
    else if (func == 1){
        message = "Wrong pin for specified purpose or specified values may not be in range";
    }
    else if (func == 2){
        message = "Wrong baudrate may be selected or uart port may not have been initialized";
    }

    printf(message);
    secondary_uart_write(message);
}

int main(){

    const uint test_io = 2;
    pinMode(test_io, OUTPUT);
    stdio_init_all();

    while(true){
        printf("hello, beginning \n");
        sleep_ms(500);
        digitalWrite(test_io, HIGH);
        printf("going HIGH \n");
        sleep_ms(1500);
        digitalWrite(test_io, LOW);
        printf("going LOW \n");
        sleep_ms(1500);
        
    }
    
    return 0;
}