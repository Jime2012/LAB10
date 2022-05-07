/* 
 * File:   main.c
 * Author: Christopher Chiroy
 *
 * Comunicación serial, oscilador de 1MHz y baud rate de 9600.
 * Valor recibido en el seríal (RCREG) se muestra en el PORTD.
 * 
 * Created on 17 april 2022, 20:12
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 1000000
         // Constante para definir largo de mensaje e iteraciones al enviarlo por el serial

/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
char mensaje[11] = {'1','.','L','e','e','r',' ','p','o','t','\n'};
char enviar[16] = {'2','.','E','n','v','i','a','r',' ','A','S','C','I','I','\n',' '};
char error[6] = {'E','r','r','o','r','\n'};
char pot[3] = {' ',' ',' '};
int indice; 
int valor;// Variable para saber que posición del mensaje enviar al serial
int numero;
int anterior = 0;
char cont = 0;



/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/

void setup(void);
void cadena(char arreglo[], int size);

/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    if(PIR1bits.ADIF){              // Fue interrupci n del ADC??
        if(ADCON0bits.CHS == 0){    // Verificamos sea AN0 el canal seleccionado
            valor = ADRESH;
            for (cont = 0; cont < 3; cont++){ // CICLO FOR PARA OBTENER LOS VALORES 
             if (cont == 0) 
                pot[0] = convertir(valor, cont); //SE OBTIENE EL VALOR DE UNIDAD
             else if (cont == 1) 
                pot[1] = convertir(valor, cont);// SE OBTIENE EL VALOR DE LA DECENA
             else
               pot[2] = convertir(valor, cont);// SE OBTIENE EL VALOR DE LA UNIDAD
             
            }// Mostramos ADRESH en PORTC
        }
        PIR1bits.ADIF = 0; 
     }
    
    if(PIR1bits.RCIF){          // Hay datos recibidos?
                            // Guardamos valor recibido en el arreglo mensaje
        enviar[15] = RCREG;
    
        if (enviar[15] == 49){
          cadena(pot,3);
        }
        else if (enviar[15] == 50){
           
           enviar[16]= RCREG;
           PORTB = enviar[16];
        }
        else {cadena(error, 6);}
            // Mostramos valor recibido en el PORTD
    }
  return;   
}

/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    while(1){
                                 // Reiniciamos indice para enviar todo el mensaje
        
                                                //   sea diferente al anterior, para imprimir solo 
       __delay_ms(1000);  
           
            if (anterior != enviar[15]){
                 cadena(mensaje,11);
                 cadena(enviar,16);
                 anterior = enviar[15];
             }
        
        if(ADCON0bits.GO == 0){             // No hay proceso de conversion
            ADCON0bits.GO = 1;              // Iniciamos proceso de conversi n?
        }
        
      
        
       
        
        
                                                //   si el nuevo valor recibido es diferente al anterior. 
           
    }
    return;
}

/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){
    ANSEL = 0b00000001; // AN0 como entrada anal gica?
    ANSELH = 0;         // I/O digitales)
    
    //ANSEL = 0b00000111; // AN0, AN1 y AN2 como entrada anal gica?
    
    TRISA = 0b00000001; // AN0 como entrada
    //TRISA = 0b00000111; // AN0, AN1 y AN2 como entrada
    PORTA = 0;                 // I/O digitales
    
    TRISB = 0;
    PORTB = 0;                  // PORTD como salida
    
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    // Configuraciones de comunicacion serial
    //SYNC = 0, BRGH = 1, BRG16 = 1, SPBRG=25 <- Valores de tabla 12-5
    TXSTAbits.SYNC = 0;         // Comunicación ascincrona (full-duplex)
    TXSTAbits.BRGH = 1;         // Baud rate de alta velocidad 
    BAUDCTLbits.BRG16 = 1;      // 16-bits para generar el baud rate
    
    SPBRG = 25;
    SPBRGH = 0;                 // Baud rate ~9600, error -> 0.16%
    
    RCSTAbits.SPEN = 1;         // Habilitamos comunicación
    TXSTAbits.TX9 = 0;          // Utilizamos solo 8 bits
    TXSTAbits.TXEN = 1;         // Habilitamos transmisor
    RCSTAbits.CREN = 1;         // Habilitamos receptor
    
    ADCON0bits.ADCS = 0b01;     // Fosc/8
    
    ADCON1bits.VCFG0 = 0;       // VDD *Referencias internas
    ADCON1bits.VCFG1 = 1;       // VSS
    
    ADCON0bits.CHS = 0b0000;    // Seleccionamos AN0
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
    ADCON0bits.ADON = 1;        // Habilitamos modulo ADC
    __delay_us(40);
    
    // Configuraciones de interrupciones
    PIR1bits.ADIF = 0;          // Limpiamos bandera de int. ADC
    PIE1bits.ADIE = 1;          // Habilitamos int. de ADC
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones de perifericos
    PIE1bits.RCIE = 1;          // Habilitamos Interrupciones de recepción
}

void cadena(char arreglo[], int size)
{
    
    while(indice<size){              // Loop para imprimir el mensaje completo          // Esperamos a que esté libre el TXREG para poder enviar por el serial
                    TXREG =  arreglo[indice]; // Cargamos caracter a enviar
                    indice++; 
                    __delay_ms(2);// Incrementamos indice para enviar sigiente caracter
                }
    
    indice = 0;
}
    
    
int convertir(int a, int i){ //FUNCION PARA OBTENR VALORES
   int centenas;
   int decenas;
   int unidades;
   centenas = a/100; // OBTENENER LAS CENTENAS
   decenas = (a -(centenas*100))/10; // OBTNERE LAS DECENAS 
   unidades = ((a -(centenas*100)) -(decenas*10)); //OBTENER LAS UNIDADES
   int valores[3] = {unidades, decenas, centenas}; //FORMAR UN ARREGLO CON LOS TRES VALORES
   int j =+ valores[i]; //LLAMAR EL VALOR DESEADO
   return j;
}