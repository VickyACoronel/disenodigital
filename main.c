#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "uart.h"    
/*
#frecuencias de las notas 2
////OCTAVA 3
	DO		130,813
	RE		146,832
	MI		164,814
	FA		174,614
	SOL		195,998
	LA		220.000
	SI		246.942
	*/    
/*
#frecuencias de las notas 2
////OCTAVA 4
	DO		261.626
	RE		293.665
	MI		329.628
	FA		349.228
	SOL		391.995
	LA		440.000
	SI		493.883
*/      
//valores de tiempo de espera para cada tarea
#define t1 1 //
#define begin {
#define end   }   
//nombres de estados
#define NoPush 1 
#define MaybePush 2
#define Pushed 3
#define MaybeNoPush 4
#define eeprom_true 0
#define eeprom_data 1 
volatile uint8_t dato_adc;// para el potenciometro
volatile uint8_t dato_adc_anterior;//para el potenciometro como aux
volatile int16_t distorsion =0;  //para el potenciometro
#define		factor 50; // valor para distorcion con el potenciometro
//TAREAS
	void initialize(void); 
	void MaquinaEstado(void);	// permite cambiar a los diferentes estados
	void configuracionNull(void); //permite cancelar el sonido
	//OCTAVA 3  sonidos o frecunecias mas graves 
	void nota1(void);
	void nota2(void);
	void nota3(void);
	void nota4(void);
	void nota5(void);
	void nota6(void);
	void nota7(void);
	// Octava 4 sonidos o frecuencias mas agudas
	void nota8(void);
	void nota9(void);
	void nota10(void);
	void nota11(void);
	void nota12(void);
	void nota13(void);
	void nota14(void);
//variables
    //sirve para cambiar a los diferentes estados de las notas
	//el estado empieza cero e ira incrementado cada que se pulse el boton sonando notas musicales
	unsigned int estado;
	//arreglo de 14 frecuecias o sonidos distintos  
	//                  -------OCTAVA 3                         |   |------OCTAVA 4
	//                   Do     Re   Mi    Fa    Sol   La    Si     do     re   mi     fa    sol   la   si
	//POSICIONES         0      1     2     3    4      5     6      7    8      9     10    11    12    13
	unsigned arreglo[]={61155,53484,48538,45814,40815,36362,32395, 30577,27241,24269,22907,20407,18181,16197};
	//arreglo de numeros aleatorios enteros para que los sonidos suenen aleatoriamente	
	int  numero[80];   
	volatile unsigned char   time3;	//contador tienpo de espera
	unsigned char PushFlag;		//message indicating a button push 
	unsigned char PushState;	//maquina estados  
         
//**********************************************************
//timer 0 compare match ISR rutina de interrupt 
	ISR (TIMER0_COMPA_vect){ 
	if (time3>0)	--time3;
	}
	FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
//Inicializacion de variables
void initialize(void){
		OCR0A = 249;  		//set the compare re to 250 time ticks
		TIMSK0= (1<<OCIE0A);	//turn on timer 0 cmp match ISR
		//set prescalar to divide by 64
		TCCR0B= 3; //0b00000011;
		// turn on clear-on-match
		TCCR0A= (1<<WGM01) ;
		DDRB|= (1<<PINB5);	 //SE ACTIVA EL PINB5 COMO SALIDA B11
		DDRF = 0x00;	// registro de entrada para el boton
		PORTF = 255;
		//configuracion del adc para el potenciometro de la distorsion
		//REFSO PETMITE ESTABLECER UN VOLTAJE DE REFERNCIA
		//ADLAR AJUSTA LA SEÑAL  HACIA LA IZUIERDA
		ADMUX = (1<<REFS0)|(1<<ADLAR)|(7<<MUX0);
		//ADEN COVIERTE A SEÑAL ANALOGIA
		//ADSC INICIA LA CONVERION ANALOGICA
		ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS0);
		time3=t1;
		PushFlag = 0;
		PushState = NoPush;
		estado=0;
		uart_init();
		stdout = stdin = stderr = &uart_str;
        fprintf(stdout,"Rand Game Sound \n");
		//crank up the ISRs
		sei() ; //La instrucción que sigue a la SEI se ejecutará antes de cualquier interrupción pendiente

}
//**********************************************************       
int main(void){
	  initialize();
	  //ciclo para generar numeros aleatorio enteros
	  for(int i=0; i<80 ;i++){
		  numero[i]= rand() % 15;
		  eeprom_write_word((uint16_t*)eeprom_data,numero[i]);
		  fprintf(stdout,"nummeros aleatorios: ");	
		  fprintf(stdout,"%d \n\r", eeprom_read_word((uint16_t*)eeprom_data)) ;
	  }
	  //ciclo que ejecuta todas las tareas mientras su valor sea 1
	  while(1){ 
		if (time3==0){
				MaquinaEstado();
		}
		distorsion = (int16_t) (dato_adc) * factor;

	  }
	
}
ISR (ADC_vect){
	// guarda el valor actual del adc en una variable
	dato_adc = ADCH;
}
void nota1(void){
//posicion 5 del arreglo	
// OCR1A=(FUC/2N*Fcco)-1
OCR1A = arreglo[numero[4]]+distorsion ;//carga la frecuencia que se almaceno en arreglo
// configura al timer como salida para que emita la frecuencia
TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);// ESTABLECE EL MODO CTC 
//( modo  comparacion con el  registro OCR1A) Y DETERMINA UN PRESCALER DE 1
}

void nota2(void){
	///posicion 2 del arrelo
	OCR1A = arreglo[numero[13]]+distorsion  ;   
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);

	
}

void nota3(void){
	//posicion 3 del arreglo
	OCR1A = arreglo[numero[20]]+distorsion;  
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
	}
	
void nota4(void){
	//posicion 0 del arreglo
	OCR1A = arreglo[numero[21]]+distorsion;
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota5(void){
	//posicion 1 del arreglo
	OCR1A = arreglo[numero[23]]+distorsion;
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota6(void){
	//posicion 6 del arreglo
	OCR1A = arreglo[numero[29]]+distorsion;
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
	}
	
void nota7(void){
	//posicion 4 del arreglo
	OCR1A = arreglo[numero[36]]+distorsion;   
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota8(void){
	OCR1A = arreglo[numero[4]] ;  
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota9(void){
	OCR1A = arreglo[numero[13]+distorsion];  
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota10(void){
	OCR1A = arreglo[numero[20]];
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota11(void){
	OCR1A = arreglo[numero[21]]; 
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void nota12(void){
	OCR1A = arreglo[numero[23]];
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}


void nota13(void){
	OCR1A = arreglo[numero[29]];
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);// ESTABLECE EL MODO CTC ( modo  comparacion con el  registro OCR1A) Y DETERMINA UN PRESCALER DE 1
}

void nota14(void){
	OCR1A = arreglo[numero[36]];
	TCCR1A = (0<<COM1A1)|(1<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS10);
}

void configuracionNull(void){
	TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(0<<CS10);
}

void MaquinaEstado(void){
	  time3=t1;     //reset the task timer
	  if(estado==1){
		 nota1();    
	   }
	   if (estado==2){
		 nota2(); 
       }
	   if (estado==3){
		 nota3();  
	   }
	   if (estado==4){
		 nota4();
       }
	   if (estado==5){
	     nota5();
	   }
	   if (estado==6){
		 nota6();  
	   }
	   if (estado==7){
		 nota7();
	   }
	   if (estado==8){
		  nota1();
		  _delay_ms(20);
		  nota2();
		  _delay_ms(20);
			nota3();
		  _delay_ms(20);
		  nota4();
		  _delay_ms(20);
		  nota5();
		  _delay_ms(20);
		  nota6();
		  _delay_ms(20);
		  nota7();
		  _delay_ms(20);
		   estado=9;
	   }
	   if (estado==9){
		 fprintf(stdout,"las notas que toco fueron:  La, Mi, Fa, Do, Re, Si, Sol \n");  
		 configuracionNull();
		 estado=0;
		   
	   }
//Maquina de Estado
	  switch (PushState){
		 case NoPush: 
			if (!(PINF&(1<<PINF0))){
				PushState=MaybePush;
			}else{ 
				PushState=NoPush;
			}
			break;
		 case MaybePush:
			if (!(PINF&(1<<PINF0))) {
			   PushState=Pushed;   
			   estado++;   
			}
			else{
				 PushState=NoPush;
			}
			break;
		 case Pushed:  
			if (!(PINF&(1<<PINF0))){ 
				PushState=Pushed; 
			}
			else{
				 PushState=MaybeNoPush;    
			}
			break;
		 case MaybeNoPush:
			if (!(PINF&(1<<PINF0))){
				 PushState=Pushed; 
			}
			else{
			   PushState=NoPush;
			}
			break;
	}
}
  
//********************************************************** 
//Set it all up
 

   


