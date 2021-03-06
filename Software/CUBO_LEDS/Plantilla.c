#include "EMm47J53_StackConfig.h"
#INCLUDE <STDLIB.H>
#include <math.h>

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define LED4 PIN_C7
#define LED3 PIN_C6
#define LED2 PIN_C2
#define LED1 PIN_C1
#define LED8 PIN_C0
#define LED7 PIN_D7
#define LED6 PIN_D6
#define LED5 PIN_D5
#define LED9 PIN_D4
#define LED10 PIN_D3
#define LED11 PIN_D2
#define LED12 PIN_E0
#define LED13 PIN_E1
#define LED14 PIN_E2
#define LED15 PIN_B0
#define LED16 PIN_B1
#define PISO1 PIN_B7
#define PISO2 PIN_B6
#define PISO3 PIN_B5
#define PISO4 PIN_B4 
#define time 200 //el tiempo de multiplexacion

/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
unsigned INT16 LEDS[16]={LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8,LED9,LED10,LED11,LED12,LED13,LED14,LED15,LED16};
unsigned INT16 pisos[4]={PISO1,PISO2,PISO3,PISO4};
const UNSIGNED int8 num[4]={0b0001,0b0011,0b0111,0b1111};
unsigned INT16 i=0,j=0,k=0,h=0,m=0,sound,speed;
unsigned INT16  mic=0;
unsigned INT16 timeRefresh=0,animation=0;
volatile UNSIGNED int16 sample,peakToPeak;
int1 reverse=0,change=0,enable=0,positionBits=1;
  UNSIGNED int signalMax = 0;
  UNSIGNED int signalMin = 256;
unsigned INT8 vector[4]={0b0001,0b0010,0b0100,0b1000};
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(LONG)((x)+0.5):(long)((x)-0.5))
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

void off_leds(VOID)
{
   //Esta funci�n apaga los leds
   FOR (i = 0; i < 16; i++)
   {
      output_low (LEDS[i]);
   }

   output_low (pisos[0]);
   output_low (pisos[1]);
   output_low (pisos[2]);
   output_low (pisos[3]);
}

int1 reordernar()
{
   positionBits = ! positionBits;
   STATIC int16 aux[16];
   STATIC int8 factor;
   FOR (i = 0; i < 16; i++)
   {
      //Hacer una copia del vector
      aux[i] = LEDS[i];
   }

   h = 0;
   FOR (i = 3; i < 16; i = i + 4)
   {
      //Reodernar
      LEDS[h] = aux[i];
      h++;
   }

   FOR (i = 2; i < 15; i = i + 4)
   {
      //Reodernar
      LEDS[h] = aux[i];
      h++;
   }

   FOR (i = 1; i < 14; i = i + 4)
   {
      //Reodernar
      LEDS[h] = aux[i];
      h++;
   }

   FOR (i = 0; i < 13; i = i + 4)
   {
      //Reodernar
      LEDS[h] = aux[i];
      h++;
   }

   RETURN positionBits;
}

void piso(j)
{
   // esta funcion escoge el piso que se desea encender
   SWITCH (j)
   {
      CASE 0:
      output_high (pisos[0]);
      output_low (pisos[1]);
      output_low (pisos[2]);
      output_low (pisos[3]);
      BREAK;

      CASE 1:
      output_high (pisos[1]);
      output_low (pisos[0]);
      output_low (pisos[2]);
      output_low (pisos[3]);
      BREAK;

      CASE 2:
      output_high (pisos[2]);
      output_low (pisos[0]);
      output_low (pisos[1]);
      output_low (pisos[3]);
      BREAK;

      CASE 3:
      output_high (pisos[3]);
      output_low (pisos[0]);
      output_low (pisos[1]);
      output_low (pisos[2]);
      BREAK;
   }
}

void output_LEDs(INT8 nibble, int8 num_nibble, int8 level)
{
   //enciede los leds por nibbles
   //EJEMPLO:
   //0b0001 - 0001 - 0001 - 0001
   //piso4 piso3 piso2 piso1
   STATIC int8 start, end;
   piso (level); //Enciende el piso que se escogio
   SWITCH (num_nibble)
   {
      CASE 1:
      start = 4; end = 8;
      BREAK;

      CASE 2:
      start = 8; end = 12;
      BREAK;

      CASE 3:
      start = 12; end = 16;
      BREAK;

      DEFAULT:
      start = 0; end = 4;
      BREAK;
   }

   h = 0;
   FOR (i = start; i < end; i++)
   {
      IF (bitRead (nibble, h) )
      {
         output_high (LEDS[i]);
      }

      h++;
   }
}

int16 map(INT16 x, int16 in_min, int16 in_max, int16 out_min, int16 out_max)
{
   RETURN (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void secuencia_init()
{
   IF (animation == 20){ reordernar (); animation = 0; }
   FOR (timeRefresh = 0; timeRefresh < 60; timeRefresh++)
   {
      IF (k == 3)
      {
         reverse = 1;
         }else IF (k == 0){
         off_leds ();
         k = 0;
         reverse = 0;
      }

      FOR (m = 0; m < 4; m++)
      {
         
         FOR (j = 0; j < 4; j++)
         {
            IF (k == - 1)
            {
               off_leds ();
               }ELSE{
               output_LEDS (vector[k], j, m) ;
            }
         }

         delay_us (time);
         off_leds ();
      }
   }

   animation++;

   IF (reverse)
   {
      k--;
      }ELSE{
      k++;
   }
}

void LED_NUM(INT8 number,int8 m)
{
   SWITCH (number)
   {
      CASE 0:
      off_leds ();
      BREAK;

      CASE 1:
      output_LEDS (num[0], 0, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 2:
      output_LEDS (num[1], 0, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 3:
      output_LEDS (num[2], 0, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 4:
      output_LEDS (num[3], 0, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 5:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 6:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      output_LEDS (0b1000, 2, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 7:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      output_LEDS (0b1000, 2, m);
      output_LEDS (0b1000, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 8:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      output_LEDS (0b1000, 2, m);
      output_LEDS (0b1100, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 9:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      output_LEDS (0b1000, 2, m);
      output_LEDS (0b1110, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 10:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      output_LEDS (0b1000, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 11:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1000, 1, m);
      output_LEDS (0b1001, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 12:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1001, 1, m);
      output_LEDS (0b1001, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 13:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1011, 1, m);
      output_LEDS (0b1001, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 14:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1111, 1, m);
      output_LEDS (0b1001, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      CASE 15:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1111, 1, m);
      output_LEDS (0b1101, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;

      DEFAULT:
      output_LEDS (num[3], 0, m);
      output_LEDS (0b1111, 1, m);
      output_LEDS (0b1111, 2, m);
      output_LEDS (0b1111, 3, m);
      delay_us (time);
      off_leds ();
      BREAK;
   }
}

#INT_TIMER0

void  TIMER0_isr(VOID)//Funci�n de interrupci�n por desbordamiento TMR1  
{
   output_toggle (PIN_D1);

   
   set_timer0 (65000);
}

/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram

void main()
{
   // Inicializa microcontrolador
   mcu_init ();
   setup_adc_ports (sAN9);
   setup_adc (ADC_CLOCK_INTERNAL);
   setup_timer_0 (RTCC_INTERNAL|RTCC_DIV_64);
   enable_interrupts (GLOBAL); //Habilito interrupciones globales
   FOR (;;)
   {
      IF ( ! input (PIN_D0))
      {
         enable_interrupts (INT_TIMER0);
         m = j = i = 0;
         enable = 1;

         DO
         {
            reordernar ();
         }WHILE ( ! positionBits) ;
      }

      IF ( ! enable)
      {
         set_adc_channel (9); // Inicia lectura por ADC canal 3 PinA3
         speed = map (read_adc (), 0, 255, 1, 100) ; // Guarda en variable el resultado del ADC y hace regla de tres
         secuencia_init (); //Secuencia de inicio
         }ELSE{

       set_adc_channel (9); 
         signalMax = 0;
         signalMin = 255;
         peaktoPeak=0;
          mic=0;
    WHILE (mic < 20)
         {
           sample = read_adc();
            IF (sample < 255) // toss out spurious readings
            {
               IF (sample > signalMax)
               {
                  signalMax = sample; // save just the max levels
               }

               else IF (sample < signalMin)
               {
                  signalMin = sample; // save just the min levels
               }
            }

            mic++;
         }
         peakToPeak = signalMax - signalMin;
         sound = map (peaktoPeak, 0, 255,0,80) ;
         sound = constrain(sound,0,80);
 
         off_leds();
         IF (sound <= 16)
         {
            LED_NUM (sound, 0) ;
            }
            IF (sound <= 32){
            LED_NUM (16, 0) ;
            LED_NUM (floor (sound / 2), 1) ;
            }
            IF (sound <= 48){
            LED_NUM (16, 0) ;
            LED_NUM (16, 1) ;
            LED_NUM (floor (sound / 3), 2) ;
            }
            IF (sound <= 64 && sound >=64 ){
            LED_NUM (16, 0) ;
            LED_NUM (16, 1) ;
            LED_NUM (16, 2) ;
            LED_NUM (floor (sound / 4), 3) ;
         }
      }

   }
}

