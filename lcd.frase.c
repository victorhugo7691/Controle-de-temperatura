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
float temperatura = 40;
float temperaturaMaxima = 85;
float temperaturaMinima = 15;
int tensao;

void geraTensao(){
    switch(tensao){
        case 0:
            temperatura = 0;
            break;
        case 1: 
            temperatura = 20;
            break;
        case 2:
            temperatura = 40;
            break;
        case 3:
            temperatura = 60;
            break;
        case 4:
            temperatura = 80;
            break;
        case 5:
            temperatura = 100;
            break;
        default:
            temperatura = 10;
    }
}

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
    if(temperatura >= temperaturaMaxima || temperatura <= temperaturaMinima){
        tensao = 2;
    } 
}

void __interrupt() isr(void){
    //INTCON registrador
    if(INTCON3bits.INT1IF == 1) {
        
        INTCON3bits.INT1IF = 0;
       //Interrupção externa    
       tensao--;
    }
    
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        /*Interrupção de timer 
         bit 5 do registrador INTCON
         * TMR0IF informa a ocorrencia do overflow, bit 2 do registrador INTCON
         */
        alerta();
        verificaOValor();
        geraTensao();
        
		TMR0L = 5; //'recarga do timer0
        
        INTCONbits.TMR0IF = 0; // clear this interrupt condition
    }
            
}

int main(){
    PORTAbits.RA4 = 0; //'pino contador T0CKI configurado 

    INTCONbits.GIE =1; //Habilita interrupção global, bit 7 do registrador INTCON
    INTCONbits.TMR0IE = 1; //Habilita a interrupção do TMR0
    
	T0CON = 0B11000111; //Configura o prescalar, o prescaler define o número de vezes que um evento deve ocorrer
    TMR0L = 5; //16,38s
    
    TRISD = 0; // Configura a porta D como sáida
    TRISE = 0; // Configura a porta E como sáida
    ADCON1 = 15; // Os dados são todos digitais 
        
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON2bits.INTEDG1 = 1;
    
    comando_lcd(0b00111100);
    comando_lcd(0b00001100);

    limpa_lcd( );
    
    comando_lcd(128);
    imprime_lcd("CTRL Temperatura");
    tensao = 2;
     
    while (1) {
        limpa_lcd( );
        __delay_ms(1000);
        comando_lcd(128);
        imprime_lcd("Temperatura");

        comando_lcd(192);
        sprintf(temperature, "%3.2f", temperatura);
        imprime_lcd(temperature);
        __delay_ms(1000);
        tensao++;
        if(tensao >= 5){
            tensao = 0;
        }
    }
}