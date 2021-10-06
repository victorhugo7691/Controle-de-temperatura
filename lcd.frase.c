// PIC18F4550 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config FOSC = HS        

#pragma config WDT = OFF        

#pragma config MCLRE = ON  

#define _XTAL_FREQ 4000000

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.intermed.h";

char temperature[8];
float temperatura;
float temperaturaMaxima = 50;
float temperaturaMinima = 25;
int encerrar = 0;

void alerta(){
    if(temperatura > temperaturaMaxima || temperatura < temperaturaMinima){
            limpa_lcd( );
            __delay_ms(1000);
            comando_lcd(128);
            imprime_lcd("ERRO Temperatura");
            __delay_ms(1000);
            limpa_lcd( );
        }
}

void verificaOValor(){
    if(temperatura>=50){
                temperatura--; 
            } else if(temperatura <= 25){
                temperatura++;
            }
}

void __interrupt() isr(void){
    
    if(INTCON3bits.INT1IF==1) {
        
        INTCON3bits.INT1IF = 0;
       //Interrupção externa    
            if(temperatura>=50){
                temperatura--;
                
            } else if(temperatura <= 25){
                temperatura++;
            } else  {
                temperatura--;
            }
       // PORTD = 0x00;
       // __delay_ms(1000);
           }
    
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        //Interrupção de timer
        alerta();
        verificaOValor();
        
        temperatura = temperatura - 0.01;
        
		TMR0L = 5; //'recarga do timer0
        
        INTCONbits.TMR0IF = 0; // clear this interrupt condition
    }
    
   /* if(INTCONbits.INT0IF == 1){
        encerrar =1;
    }
    if(INTCON3bits.INT2IF==0) {
        encerrar = 1;
    }*/
            
}

int main(){
    PORTAbits.RA4 = 0; //'pino contador T0CKI configurado 

    INTCONbits.GIE =1;
    INTCONbits.TMR0IE = 1;
    
	T0CON = 0B11000111;
    TMR0L = 5;
    
    TRISD = 0;
    TRISE = 0;
    ADCON1 = 15;
        
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON2bits.INTEDG1 = 1;
    
    comando_lcd(0b00111100);
    comando_lcd(0b00001100);

    limpa_lcd( );
    
    comando_lcd(128);
    imprime_lcd("CTRL Temperatura");
    temperatura = 40.0;
     
    while (encerrar == 0) {
        limpa_lcd( );
        __delay_ms(1000);
        comando_lcd(128);
        imprime_lcd("Temperatura");

        comando_lcd(192);
        sprintf(temperature, "%3.2f", temperatura);
        imprime_lcd(temperature);
        __delay_ms(1000);

    }
}
