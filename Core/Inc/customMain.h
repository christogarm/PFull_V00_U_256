
 #define lowByte(w)  ((uint8_t)((w) & 0xff))
 #define highByte(w) ((uint8_t)((w) >> 8))

 #define BitClear(reg,bit) 			(reg &= ~((1 << bit)))
 #define BitSet(reg,bit) 			(reg |= (1 << bit))
 #define BitComplement(reg,bit) 	(reg ^= (1 << bit))

#define GetRegFlagState(reg, bit)     ((reg & (1 << bit))  ==  (1 << bit) )



/* Size of Transmission buffer */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


//----------------------------------- Nuevas Definiciones --------------------------------------------------------------------

#define size_Header		    (uint8_t) 8       // Tamano del buffer de datos Bloque Header
#define size_handshake      (uint8_t) 21      // Tamano del buffer de datos Bloque Hand shake
#define size_TiempoReal     (uint8_t) 12      // Tamano del buffer de datos Bloque Tiempo Real



#define I2C_ADDRESS        (0x2A)<<1		  // Direccion de esclavo 0x2A

//----------------------------------------------------------------------------------------------------------------------------------------


#define DATA_TO_TRANSFER_USART4      (uint8_t)  30      // buffer de TX salida
#define DATA_TO_RECEIVE_USART4       (uint8_t) 30      // buffer de RX entrada Modbus

#define DATA_TO_TRANSFER_USART2      (uint8_t)  144      // buffer de TX salida
#define DATA_TO_RECEIVE_USART2       (uint8_t)  144      // buffer de RX entrada

//extern ADC_HandleTypeDef hadc;
extern ADC_HandleTypeDef hadc1;

extern I2C_HandleTypeDef hi2c1;
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c1_tx;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart4_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart4_tx;
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;

extern uint8_t    STM8_A;					// Simul A 8 bit Register
extern uint16_t   STM8_16_X;				//


extern _Bool  tick_1ms;					// flag base de tiempo 1ms
extern _Bool  tick_10ms;				// flag base de tiempo 10ms
extern _Bool  tick_100ms;				// flag base de tiempo 100ms
extern _Bool  tick_1s;					// flag base de tiempo 1s

extern _Bool  device_conected;			// bandera dispositivo conectado

extern uint8_t BluetoothState;			// Maquina de estados Bluetooth 1:Configuracion 2:Obtencion parametros 3:Tx/RX


extern uint16_t variable_corriente;         //Variable que despliega el resultado


#define 	num_ver		 5				//;04;	Versión del programa 0.4
#define 	time_auto	 9				//;	Segundos de autoprueba
#define 	time_ini	 7				//;	Segundos de indicacion inicial
#define 	Fam_ID		 128			//; //100;   Familia del equipo (para comunicación con la llave)		>> IJG EXPANSION DE PARAMETROS
#define 	time_ht		 120			//	time_ht:		equ		 120//;	Minutos para indicar alta temperatura en evaporador

#define 	tms_retcz_compresor    164		//	tms_retcz_compresor:     equ   164
#define 	tms_retcz_ventilador   197		// tms_retcz_ventilador:    equ   197
#define 	tms_retcz_deshielo   	4 		// tms_retcz_deshielo:      equ   4    // Se activa a la BAJADA
#define 	tms_retcz_lampara       15		// tms_retcz_lampara:       equ   15   // Se activa a la BAJADA (3.2ms a 3.7 ms de activación de relevador KSHOI S1 + 3.75 ms PROGRAMA = 7.25 + 1ms DEFASE)


//LN_97:	#pragma space extern [] @near

extern uint8_t TxBuffer_Ble[DATA_TO_TRANSFER_USART2];      // Manuel, han de ser como 80 caracteres
extern uint8_t RxBuffer_Ble[DATA_TO_RECEIVE_USART2];

extern uint8_t	SerialAnswBLE[DATA_TO_RECEIVE_USART2];


extern uint8_t TxBuffer_RS485[DATA_TO_TRANSFER_USART4];      // Manuel, han de ser como 80 caracteres
extern uint8_t RxBuffer_RS485[DATA_TO_RECEIVE_USART4];
extern uint8_t SerialAnsModBus[DATA_TO_RECEIVE_USART4];

extern uint16_t Eventos[14];			// Aray para Eventos
extern uint16_t Mediciones[4];			// Aray para Mediciones, Manuel 06-dic-2021
extern uint16_t Alarmas[9];				// Aray para Alarmas, Manuel 06-dic-2021
extern uint16_t DatosMAC[3];			// Aray para Datos MAC, Manuel 06-dic-2021
extern uint16_t DatosFirmware[1];		// DatosFirmware, Manuel 06-dic-2021: BCD = 0.1.0.0

extern			uint16_t Count_Test;				// 17-dic-2021, contador de prueba

extern			uint16_t PNU_0x3000;						// 14-Ene-2022:  ID del cliente del refrigerador
extern			uint16_t PNU_0x3002;						// 14-Ene-2022:  Unidad de temperatura  C=0 y F=1
extern			uint16_t PNU_0x3004;						// 14-Ene-2022:  Temperatura Setpoint MODO NORMAL
extern			uint16_t PNU_0x3006;						// 14-Ene-2022:  Temperatura Diferencial  MODO NORMAL
extern			uint16_t PNU_0x3008;						// 14-Ene-2022:  MODO NOCTURNO: duración en mins
extern			uint16_t PNU_0x300A;						// 14-Ene-2022:  Consumo por variable
extern			uint16_t PNU_0x300C;						// 14-Ene-2022:  Compresor: tiempo minimo de apagado en mins
extern			uint16_t PNU_0x300E;						// 14-Ene-2022:  Compresor: tiempo minimo de encendido en mins
extern			uint16_t PNU_0x3010;						// 14-Ene-2022:  Deshielo Programa 1: habilitación de 3 o 5 fases
extern			uint16_t PNU_0x3012;						// 14-Ene-2022:  Deshielo Programa 1: duración fase 1 en mins
extern			uint16_t PNU_0x3014;						// 14-Ene-2022:	 Deshielo Programa 1: duración fase 2 en mins
extern			uint16_t PNU_0x3016;						// 14-Ene-2022:	 Deshielo Programa 1: duración fase 3 en mins
extern			uint16_t PNU_0x3018;						// 14-Ene-2022:  Deshielo Programa 1: periodo en mins para iniciar deshielo
extern			uint16_t PNU_0x301A;						// 14-Ene-2022:	 Deshielo Programa 1: temperatura para inicio de periodo de deshieloDeshielo Programa 1: temperatura para inicio de periodo de deshielo
extern			uint16_t PNU_0x301C;						// 14-Ene-2022:	 Deshielo Programa 1: temperatura para salir de deshielo  y verifica si entra a deshielo
extern			uint16_t PNU_0x301E;						// 14-Ene-2022:	 Modbus: dirección de la CIR
//----------------------------------------------------------
extern			uint16_t PNU_0x3104;						// 14-Ene-2022:		Alarma puerta abierta: tiempo de activación en mins
extern			uint16_t PNU_0x3106;						// 14-Ene-2022:		Alarma Congelamiento Programa 1: temperatura para entrar en alarma
extern			uint16_t PNU_0x3108;						// 14-Ene-2022:		Alarma U 110V: voltaje para entrar en alarma
extern			uint16_t PNU_0x310A;						// 14-Ene-2022:		Alarma U 110V: voltaje para salir de alarma
extern			uint16_t PNU_0x310C;						// 14-Ene-2022:		Alarma -U 110V: voltaje para entrar en alarma
extern			uint16_t PNU_0x310E;						// 14-Ene-2022:		Alarma -U 110V: voltaje para salir de alarma
//----------------Solo Escritura--------------------------
extern			uint16_t PNU_0x3200;						// 14-Ene-2022:		Comando activar Modo Nocturno
extern			uint16_t PNU_0x3202;						// 14-Ene-2022:		Comando activar MODO DE AHORRO ES2
extern			uint16_t PNU_0x3204;						// 14-Ene-2022:		Comando activar MODO VAULT
extern			uint16_t PNU_0x3206;						// 14-Ene-2022:		Comando desactivar MODO VAULT
extern			uint16_t PNU_0x3208;						// 14-Ene-2022:		Comando activar MODO Servicio
extern			uint16_t PNU_0x320A;						// 14-Ene-2022:		Comando Ventilador siempre ON con compresor OFF
extern			uint16_t PNU_0x320C;						// 14-Ene-2022:		Comando Ventilador activar ciclos ON-OFF con compresor OFF

//extern	uint8_t s_reloj;        //Frecuencia de la línea
extern 	_Bool 	s_reloj[3];


extern	uint8_t fact_mul; 	//;-Factor multiplicativo aplicado a muestras
extern	uint8_t fact_div; 	//;-Factor división aplicado a muestras

extern	uint8_t cnt_mues;           //Contador de muestras
extern	uint8_t cnt_mues_ant; 		//;-Respalo de cont muestras cuando es nuevo ciclo
extern	uint8_t cnt_mues_ciclos; 	//;-Contador muestras de ciclo promedio (8 ciclos)



extern	_Bool ban_muestreo  [8]; //;Banderas para indicar el estado del muestreo

extern  uint32_t acu8m_voltaje; //;-Acumulado para 8 muestras de voltaje

extern	uint32_t acu8m_corriente; //Acumulado para 8 muestras de corriente

extern	uint16_t voltaje_trms; //;Voltaje trms medido en V
extern	uint16_t corriente_trms; //;;Corriente trms medida en mA


extern	uint32_t sigma_cuad_sampl; //valor de sampling sumado
extern	uint32_t c_sigma_cuad_sampl; //valor de sampling sumado
extern	uint32_t sigma_cuad_sampl_1; //valor de sampling sumado
extern	uint32_t sigma_cuad_sampl_2; //valor de sampling sumado
extern	uint32_t c_sigma_cuad_sampl_1; 	  //valor de sampling sumado
extern	uint32_t c_sigma_cuad_sampl_2; 	  //valor de sampling sumado



extern	uint32_t level_4st_mult;

extern	uint8_t  vl_ram [32];

//uint8_t ram_h                  = 0; //;Registro de almacenamiento temporal de ADC
//uint8_t ram_l                  = 0; //;Registro de almacenamiento temporal de ADC
extern	uint16_t	 ram_h;

extern	uint8_t  cnt_veces_muestreo;



extern 	uint8_t  cnt_2ms_corrIMB;
extern  uint8_t cnt_muestras_corrIMB;
extern	uint16_t muestra_corrIMB;
extern	uint16_t mcorr_ponderada_2;
extern	uint16_t mcorr_ponderada_1;
extern	uint16_t corriente_IMB;


extern	uint8_t  volt_trms;
extern	uint8_t  edo_muestreo;


// LN_196:	#pragma section  {asmpt44_0}

extern uint8_t		resulh;					//08/FEB/2022		DS.B 1      	;	/ch man
extern uint8_t		resull;					//08/FEB/2022		DS.B 1      	;	/ch man Resultado de operaciones varias
extern uint8_t		cntproc;					//01/MAR/2022		DS.B 1      	;	/ Contador de procesos

extern uint8_t		cntcent;				//08/FEB/2022		DS.B 1      	;	/ Contador de tiempo "Centésimas"
extern uint8_t		cntseg;					//08/FEB/2022		DS.B 1      	;	/ Contador de tiempo "Segundos"


extern uint16_t		cntblkh;				//08/FEB/2022		DS.B 1      	; ch_man   / Contador para parpadeos
extern uint8_t		cntblkl;				//08/FEB/2022		DS.B 1      	; ch_man   / Contador para parpadeos

extern uint8_t		tempo2;					//08/FEB/2022		DS.B 1      	;	/ch man Registro 2 temporal para operaciones varias
extern uint8_t		tempo1;					//08/FEB/2022		DS.B 1     	;	/ch man Registro 1 temporal para operaciones varias

extern uint8_t		datdig2;				//08/FEB/2022		DS.B 1      	;	/ Registro de datos para el dígito 2
extern uint8_t		datdig1;				//08/FEB/2022		DS.B 1      	;	/ Registro de datos para el dígito 1
extern uint8_t		modo;					//08/FEB/2022		DS.B 1      	;	/ Modo de operación para el enfriador
//;Bits para el modo de cambio, (Registro MODO)
#define		tvig			 0//;		1 = Terminó tiempo del estado vigente; 0 = El tiempo esta corriendo
#define		limach			 1//;		1 = tdev es igual o mayor que el límite dado; 0 = tdev es menor que el límite dado
//;IJG codigo innecesario			bdh0					equ		 2;		\ Estado de la tecla Desh; 00=inactiva;01=Función primaria;10=N/A;11=Función secundaria
//;IJG codigo innecesario			bdh1					equ		 3;		/
#define		limech			 4//;		1 = teva es igual o mayor que el límite dado; 0 = teva es menor que el límite dado
//;IJG codigo innecesario			bno0					equ		 5;		\ Estado de la tecla Noct; 00=inactiva;01=Función primaria;10=N/A;11=Función secundaria
//;IJG codigo innecesario			bno1					equ		 6;		/
#define		tinic			 7//;		1 = Período de inicio (8 s); 0 = Control en operación



extern uint8_t		holdseg;				//08/FEB/2022		DS.B 1      	;	/ Registro de conteo de segundos para la retención de lectura

extern uint8_t		waux;						//08/FEB/2022		DS.B 1      	;	/ch_man Registro auxiliar de trabajo
extern uint8_t		wreg;						//08/FEB/2022		DS.B 1      	;	/ch_man Registro de trabajo
extern _Bool  		portX [8];
enum NumportX  {
	rel_co,				// 	rel_co:			equ		 5//;		//RM_20220622		Salida	Relevador Compresor
	dp_sw,				//	TRASN: puertob  Flag para el pin dp_sw
	rel_lz,				//portFlag_rel_lz = 0;		//TRASN: puertoe
	rel_fn,				//TRASN: puertoa
	rel_dh,
	led_pot				//	led_pot:		equ		 3//;		Salida de reloj para la programación de parámetros
};


extern uint16_t		adcramh;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
extern uint16_t		tminstoph;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;
extern uint16_t		retnocth;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;


extern uint16_t		retproth;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;


extern uint8_t		retvent;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo para el encendido del ventilor
extern uint8_t		minfunc;				//08/FEB/2022		DS.B 1         ;0x00;	Tiempo mínimo de función activa
extern uint8_t		durautop;				//08/FEB/2022		DS.B 1         ;0x00;	Duración de autoprueba
extern uint8_t		cntdpyev;				//08/FEB/2022		DS.B 1         ;0x00;	Tiempo desplegado temperatura de evaporador
extern uint8_t		cntobscu;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de obscuridad en la fotocelda
extern uint8_t		cntfail;				//08/FEB/2022		DS.B 1         ;0x00;	Contador para indicar fallas
extern uint8_t		valflvl;				//08/FEB/2022		DS.B 1         ;0x00;																	>> IJG AGOSTO 2012
extern uint8_t		valfhvl;				//08/FEB/2022		DS.B 1         ;0x00;																	>> IJG AGOSTO 2012
extern uint8_t		cnt_prog;				//08/FEB/2022		DS.B 1
extern uint8_t		cnt_btn_hld;		//08/FEB/2022		DS.B 1;
extern uint8_t		ret_fan_co;			//08/FEB/2022		DS.B 1         ;0x00;	Retardo para el encendido del ventilor por compresor recien encendido
extern uint8_t		End_sec;				//08/FEB/2022		DS.B 1         ;0x00;


// Flags:  uint8_t		flagsa = 0;					//08/FEB/2022		DS.B 1      	;	/ Registro A de banderas para el programa
extern	_Bool  	flagsa [8];			//08/FEB/2022		DS.B 1      	;	/ Registro A de banderas para el programa
//extern	uint8_t		flagsa;					//08/FEB/2022		DS.B 1      	;	/ Registro A de banderas para el programa

//;Definición de banderas, (Registro FLAGSA)
#define		arran				 0//;		Bandera de indicación de arranque
#define		noctman				 1//;		Bandera de activación de modo nocturno manual
//;VOID					equ		 2//;		datotemp			equ		 2;		1=Indica que se desplegará dato de temperatura; 0=Otro tipo de dato
#define		nocturno			 3//;		Bandera de indicación de modo nocturno
//;IJG codigo innecesario			fprim					equ		 4;		Bandera de función primaria, se modifica en rutina watch
//;IJG codigo innecesario			fsecu					equ		 5;		Bandera de función secundaria, se modifica en rutina watch
#define		tursant				 6//;		1: Turbo, 0: Nnormal
#define		inicial				 7//;		Bandera de inicialización


// Flags:  uint8_t		trefst2 = 0;				//08/FEB/2022		DS.B 1
//extern _Bool  	trefst2 [8];				//08/FEB/2022		DS.B 1
extern	_Bool  		trefst2 [8];				//08/FEB/2022		DS.B 1
//;Definición de bits de estado, (Registro TREFST2)
#define		f_tvolt			 0//;		Bandera de indicación de Falla en la tarjeta de medición de voltaje
#define		f_co_ex			 1//;		Bandera de indicación de Falla en la tarjeta de medición de voltaje
#define		f_ambHi			 2
#define		f_ambLo			 3
#define		f_s3short		 4
#define		f_s3open		 5
#define		f_defi			 6
#define		f_s4open		 7
#define		f_retCo			 7


// Flags:  uint8_t		trefst = 0;					//08/FEB/2022		DS.B 1      	;	/ Registro de indicación de estado del equipo TREF
extern _Bool  	trefst [8];							//08/FEB/2022		DS.B 1      	;	/ Registro de indicación de estado del equipo TREF
//extern 		uint8_t  	trefst;					//08/FEB/2022		DS.B 1      	;	/ Registro de indicación de estado del equipo TREF
//;Definición de bits de estado, (Registro TREFST)
#define		f_sac		 0//;		Bandera de indicación de sensor de ambiente en corto
#define		f_saa		 1//;		Bandera de indicación de sensor de ambiente abierto
#define		f_sdc		 2//;		Bandera de indicación de sensor de evaporador en corto
#define		f_sda		 3//;		Bandera de indicación de sensor de evaporador abierto
#define		f_pa		 4//;		Bandera de indicación de alarma por puerta abierta
//f_ta:					equ		 5//; 		Bandera de indicación de falla por temperatura alta
#define		f_s4short	 5
#define		f_iny		 5
#define		f_lv		 6//; 		Bandera de indicación de voltaje bajo
#define		f_hv		 7//; 		Bandera de indicación de voltaje alto


extern _Bool  	datled  [8];					//08/FEB/2022		DS.B 1      	;	/ Registro de datos para los leds
//extern 		uint8_t		datled;					//08/FEB/2022		DS.B 1      	;	/ Registro de datos para los leds
//;Bits de control de los leds y signos, (Registro DATLED)
#define		dp				 0//	dp:						equ		 0//;		Bit de control de encendido del punto decimal
#define		sign			 1//	sign:					equ		 1//;		Bit de control de encendido del signo menos
#define		luzC			 2//	luzC:					equ		 2//;		Bit de control de encendido del Led de Compresor
#define		luzD			 3//	luzD:					equ		 3//;		Bit de control de encendido del Led de Deshielo
#define		luzN			 4//	luzN:					equ		 4//;		Bit de control de encendido del Led de Nocturno
#define		luzA			 5//	luzA:					equ		 5//;		Bit de control de encendido del Led de Alarma
#define		dl6				 6//	dl6:					equ		 6//;		Bandera de modo PULL DOWN (nocturno inhibido)	 >> IJG AGOSTO 2012
//;seldig		equ		 7//;		Bit de selección de dígito
#define		luzF			 7//	luzF:					equ		 7//;		Bit de control de encendido del Led de ventilador
extern uint8_t		edorefri;				//08/FEB/2022		DS.B 1      	;	/ Estado del refrigerador

extern _Bool  		botonst [8]; // uint8_t		botonst;				//08/FEB/2022		DS.B 1      	;	/ Estado actual de las entradas digitales
//;Bits de control de las banderas (Registro botonst)
#define		b1_f1		0//;		Función primaria botón 1
#define		b1_f2		1//;		Función secundaria boton 1
#define		b2_f1		2//;		Función primaria botón 2
#define		b2_f2		3//;		Función secundaria botón 2
#define		b3_f1		4//;		Función primaria botón 3
#define		b3_f2		5//;		Función secundaria boton 3
#define		b4_f1		6//;		Función primaria botón 4
#define		b4_f2		7//;		Función secundaria botón 4
//------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Flags:  uint8_t		GPIOR0 = 0;				//08/FEB/2022		DS.B 1      	; / Bits de control de las banderas (Registro GPIOR0)
extern _Bool  	GPIOR0 [8];	// egistro de proposito general
//;Bits de control de las banderas (Registro GPIOR0)
#define		f_comp				 0//;		Bandera de compresor
#define		f_dh				 1//;		Bandera de deshielo
#define		f_lamp				 2//;		Bandera de lampara
#define		f_ahorra			 3//;		Bandera de equipo ahorrador
#define		f_disable			 4//;		Desabilitado el deshielo al arranque
//;IJG codigo innecesario			f_ul					equ		 5;		Intercambio entre salidas de lámpara y deshielo
#define		f_volt				 6//;		Bandera de protección de voltaje
//----------------------------------------------------------------------------------
extern 	uint16_t	liminf_w;				//08/FEB/2022		DS.W 1         ;ch_man 0x00;	Valor del límite inferior de control parte entera

extern 	uint16_t	limsup_w;				//08/FEB/2022		DS.W 1         ;ch_man 0x00;	Valor del límite superior de control parte entera


extern uint8_t		tdevl;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del ambiente interno en °C, parte entera
extern uint8_t		tdevf;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del ambiente interno en °C, parte fraccionaria
extern uint8_t		teval;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del evaporador en 10 bits, entero
extern uint8_t		tevaf;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del evaporador en 10 bits, fracción
extern uint16_t		tdevdpy_w;			//08/FEB/2022		DS.W 1         ;ch_man 0x00;


extern uint16_t		interdhh;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//extern uint8_t		durdhl;						//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Tiempo de deshielo
extern uint16_t		durdhh;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;

extern uint16_t		latchtimeh;			//08/FEB/2022		DS.B 1         ;0x00;


extern uint16_t		coontimeh;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;

extern uint16_t		cntpah;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;



extern uint8_t		cntbase;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de desbordes del timer 0 (cada 125 us)
extern uint8_t		cnttemp;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de retardo para tomar lecturas de temperatura
extern uint8_t		cntmues;				//08/FEB/2022		DS.B 1         ;0x00;;	Contador de muestra de temperatura para filtro digital
extern uint8_t		debdoor;				//08/FEB/2022		DS.B 1         ;0x00;	Debounce para validar puerta abierta
extern uint8_t		cntdpysp;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de desplegado del set point


extern uint8_t		cntdpysp;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de desplegado del set point
extern uint8_t		ret_up;					//08/FEB/2022		DS.B 1         ;0x00;	Contador de retardo para mostrar la lectura cuando va hacia arriba
extern uint8_t		retcncfa;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo de cancelación de falla de sensor de ambiente
extern uint8_t		retcncfe;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo de cancelación de falla de sensor de evaporador
extern uint8_t		cntsame;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de promedios iguales hacia arriba
extern uint8_t		cnthold;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de retención de lectura
extern uint8_t		retnoct;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo para la activación de modo nocturno en automático



extern uint8_t		retcapfc;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo para captura de señal de fotocelda

extern uint8_t		luminos;				//08/FEB/2022		DS.B 1         ;0x00;	Dato válido de luminosidad
extern uint8_t		cnthitemp;			//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de temperatura alta
extern uint8_t		cntvolt;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo para tomar muestra de voltaje
extern uint8_t		cntmuesv;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de muestras de voltaje


extern uint8_t		volth;					//08/FEB/2022		DS.B 1         ;0x00;	Resultado de la conversión ADC en medición de voltaje
extern uint8_t		voltl;					//08/FEB/2022		DS.B 1         ;0x00;

//;Memoria ISAAC
extern _Bool 		fbtn_deb[8];		// uint8_t		fbtn_deb;				//08/FEB/2022		DS.B 1


extern _Bool  		fauto [8];					//08/FEB/2022		DS.B 1


// Flags:  uint8_t		GPIOR1 = 0;		//08/FEB/2022		DS.B 1      	; / Bits de control de las banderas (Registro GPIOR1)
extern _Bool  	GPIOR1 [8];
//;Bits de control de las banderas (Registro GPIOR1)
#define		f_fan			 0//;		Bandera de ventilador
#define		f_led			 1//;		Bandera de led indicador potencia

//;Bits de control de las banderas (Registro Botones_T)
#define		btn_1			 0//;		Botón 1 oprimido
#define		btn_2			 1//;		Botón 2 oprimido
#define		btn_3			 2//;		Botón 3 oprimido
#define		btn_4			 3//;		Botón 4 oprimido


extern _Bool  	flagsb  [8];
//extern	uint8_t		flagsb;					//08/FEB/2022		DS.B 1      	; / Registro B de banderas para el programa
//;Definición de banderas, (Registro FLAGSB)
#define		f_luzb			0	//f_luzb:			equ		 0//;		Bandera control de lampara por boton
#define		f_prog			1	//f_prog:			equ		 1//;		Bandera de programacion de parametros
//;f_sgpo				equ		 2;		Bandera de seleccion de grupo de parámetros
#define		f_menu2		 	2	//f_menu2:			equ		 2//;
#define		f_sprm			3	//f_sprm:			equ		 3//;		Bandera de seleccion de parámetros
//;f_lmt					equ		 4;		Bandera que indica paso por el limite de programacion
#define		f_nv2Menu2		4	//f_nv2Menu2:		equ		 4
//;		Bandera de seleccion de grupo de parámetros
#define		f_ulck			5	//f_ulck:			equ		 5//;		Bandera de programación de parametros liberada
#define		f_nd_temp		6	//f_nd_temp:		equ		 6//;		Bandera que indica el dato a modificar no es temperatura  ;//man math_change  Bandera que indica tipo de para metro offset
#define		f_excom			7	//f_excom:			equ		 7//;   Bandera de error de comunicación

extern _Bool  		btn_pr  [8];// uint8_t		btn_pr;					//08/FEB/2022		DS.B 1				; / Copia del resgistro botonst para utilizar en programación de parámetros
extern uint8_t		ton_fan;				//08/FEB/2022		DS.B 1      	; / Tiempo de encendido ventilador
extern uint8_t		toff_fan;				//08/FEB/2022		DS.B 1      	; / Tiempo de apagado ventilador


extern uint16_t		drp_comph;			//08/FEB/2022		DS.B 1

extern uint16_t		drp_fanh;				//08/FEB/2022		DS.B 1

extern uint8_t		param2;					//08/FEB/2022		DS.B 1				; / indice de parametros
extern uint8_t		param;					//08/FEB/2022		DS.B 1
extern uint8_t		key;				//08/FEB/2022		DS.B 1

extern uint16_t		lmt_up_w;		//08/FEB/2022		DS.W 1
extern uint16_t		lmt_dw_w;		//08/FEB/2022		DS.W 1

//extern uint8_t		Botones_COPY;		//08/FEB/2022		DS.B 1;
extern _Bool 		Botones_COPY[8];
extern uint8_t		deb2_btn1;			//08/FEB/2022		DS.B 1;
extern uint8_t		deb2_btn2;			//08/FEB/2022		DS.B 1;
extern uint8_t		deb2_btn3;			//08/FEB/2022		DS.B 1;
extern uint8_t		deb2_btn4;			//08/FEB/2022		DS.B 1;
extern uint8_t		deb1_btn2;			//08/FEB/2022		DS.B 1;
extern uint8_t		deb1_btn3;			//08/FEB/2022		DS.B 1;
extern uint8_t		deb_2btn;			//08/FEB/2022		DS.B 1;
//extern uint8_t		botonst2;			//08/FEB/2022		DS.B 1;
extern _Bool botonst2[8];

extern uint8_t		cnt_ecom;				//08/FEB/2022		DS.B 1;
extern uint8_t		cnt_dpyrst;			//08/FEB/2022		DS.B 1;
extern uint8_t		cnt_btn;				//08/FEB/2022		DS.B 1;
extern uint8_t		volt_max;				//08/FEB/2022		DS.B 1;
extern uint8_t		volt_min;				//08/FEB/2022		DS.B 1;
extern uint8_t		volt_hmax;			//08/FEB/2022		DS.B 1;
extern uint8_t		volt_hmin;			//08/FEB/2022		DS.B 1;


extern uint8_t		cnt_pta_fan;		//08/FEB/2022		DS.B 1;
extern uint8_t		durautop2;			//08/FEB/2022		DS.B 1;
extern uint8_t		cm_lo50;				//08/FEB/2022		DS.B 1;
extern uint8_t		cm_h255;				//08/FEB/2022		DS.B 1;

extern uint16_t		cntworkh;				//08/FEB/2022		DS.B 1;
extern uint16_t		cntexhah;				//08/FEB/2022		DS.B 1;

extern uint16_t		t_ahorro1_H;		//08/FEB/2022		DS.B 1
extern uint16_t		t_ahorro2_H;		//08/FEB/2022		DS.B 1


extern uint8_t		cnt_blk_btn;		//08/FEB/2022		DS.B 1;

extern uint16_t		cntDefi_H;
extern uint16_t		tempDefi1;
extern uint16_t		cntDefi1_H;

extern _Bool  	flagsC [8];			//08/FEB/2022		DS.B 1      	; / Registro C de banderas para el programa
//extern uint8_t		flagsC;					//08/FEB/2022		DS.B 1      	; / Registro C de banderas para el programa

//;Bits del puerto D
#define 	swdoor			 3//;		Manuel 28-feb-2022:	Entrada Swich puerta



//;Definición de banderas, (Registro FLAGSC)
#define		f_doorOpen		 0//;		Bandera de puerta abierta
#define		f_ahorro1		 1//;		Bandera de modo de ahorro 1
#define		f_ahorro2		 2//;		Bandera de modo de ahorro 2
#define		f_tempAhorro	 3//;		Bandera que indica que se cumplio la temperatura de ahorro al arranque

//;Definición de banderas, (Registro FCOM)
#define 	f_pprog			0	//;	Bandera se requiere porgramación de parámetros
#define 	f_ecom			1	//;	Bandera comunicacion lista para programación de parámetros
#define 	f_progr			2	//;	Bandera programacion realizada
#define 	f_doprog		3	//;	Bandera programacion en proceso
#define 	f_2btn			4	//;	Bandera error 2 botones oprimidos




//
#define		f_lampDoor		 6//;		Bandera de estado de lampara con puerta cerrada
#define		f_spReached		 7//; Bandera que indica que el set point se alcanzó al menos una vez

//;Bits de control de las banderas (Registro fbtn_deb)
#define 	b1_ow			 0//;
#define 	b1_iw			 1//;
#define 	b2_ow			 2//;
#define 	b2_iw			 3//;
#define 	b3_ow			 4//;
#define 	b3_iw			 5//;
#define 	b4_ow			 6//;
#define 	b4_iw			 7//;

//;Bits de control de las banderas (Registro logicos)
#define 	f_lampAH		5			//f_lampAH:				equ		5//;

extern uint16_t		cntNoct_H;			//08/FEB/2022		DS.B 1

extern _Bool  		flags_menu [8]; // uint8_t		flags_menu;			//08/FEB/2022		DS.B 1
extern uint8_t		flags_accMenu;		//08/FEB/2022		DS.B 1
extern uint8_t		timerOffManto;
extern uint8_t		retcncfr;
extern uint8_t		retcncfc;


extern	uint16_t		tret_w;				// +-SIGN //




extern uint8_t		retzc_ms_compresor;		//RM_20220913 Contador de retardo al cruce por cero para COMPRESOR
extern uint8_t		retzc_ms_deshielo;		//RM_20220913 Contador de retardo al cruce por cero para DESHIELO
extern uint8_t		retzc_ms_ventilador;	//RM_20220913 Contador de retardo al cruce por cero para VENTILADOR
extern uint8_t		retzc_ms_lampara;		//RM_20220913 Contador de retardo al cruce por cero para LÁMPARA
extern _Bool 		cruze_por_cero[8];// uint8_t		cruze_por_cero;  				//RM_20220913 Banderas para cruce por cero


extern uint8_t		cntCiclosCmp;			//RM_20230421

extern uint8_t		muestras_cal_volt;      //RM_20230908 Para mejorar la calibración de voltaje
extern uint8_t		voltaje_ant_cal;        //RM_20230908 Para mejorar la calibración de voltaje



extern	uint8_t		ctlmemo;				//08/FEB/2022		DS.B 1         ;0x00;	Registro de control de manejo de memoria EEPROM 0xAA = Grabar; cualquier otra cosa = leer
extern	uint8_t		cntmemo;				//08/FEB/2022		DS.B 1         ;0x00;	Puntero de datos de memoria EEPROM




//LN_460:	#pragma section @near {asmpt44_1}


extern uint8_t	Plantilla [128];
enum parametrosPlantilla  {
		dato_seg1,															//uint8_t		dato_seg1 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0100	;	256 d	;Dato de seguridad 1

		//;-------------------  GRUPO DE PARÁMETROS T (Temperatura)  -----------------------------------
		//;-------------------  Parámetros de temperatura  -------------------------------
		spdiur_H,     			spdiur_L, 				//uint16_t		spdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0101	;	257 d	;t0 - Set point compresor diurno
		difdiur_H,				difdiur_L,				//uint16_t		difdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0103	;	259 d	;t1 - Diferencial para el modo diurno
		osdiur_H,     			osdiur_L, 				//uint16_t		osdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0105	;	261 d	;t2 - Offset para el modo diurno
		spnoct_H,     			spnoct_L, 				//uint16_t		spnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0107	;	263 d	;t3 - Set point compresor nocturno
		difnoct_H,     			difnoct_L, 				//uint16_t		difnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0109	;	265 d	;t4 - Diferencial para el modo nocturno
		osnoct_H,     			osnoct_L, 				//uint16_t		osnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010B	;	267 d	;t5 - Offset para el modo nocturno
		rngmin_H,     			rngmin_L, 				//uint16_t		rngmin_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010D	;	269 d	;t6 - Rango mínimo para ajuste del set point
		rngmax_H,     			rngmax_L, 				//uint16_t		rngmax_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010F	;	271 d	;t7 - Rango máximo para ajuste del set point
		alarmaRetCo_H,     		alarmaRetCo_L, 					//uint16_t		t8_w = 0;					//
		defrResetTemp_H,     	defrResetTemp_L, 		//uint16_t		defrResetTemp = 0;//
		defrStartTemp_H,     	defrStartTemp_L, 		//uint16_t		defrStartTemp = 0;					//08/FEB/2022		DS.W 1	;	equ	$0115	;	277 d	;tA_W
		tempRetCo_H,     		tempRetCo_L, 	//uint16_t		defrStartTempAmb = 0;					//08/FEB/2022		DS.W 1	;	equ	$0117	;	279 d	;tB_W
		tC_H,     				tC_L, 					//uint16_t		tC_w = 0;					//08/FEB/2022		DS.W 1	;	equ	$0119	;	281 d	;tC_W
		difAhorro1_H,     		difAhorro1_L, 			//uint16_t		difAhorro1 = 0;		//08/FEB/2022		DS.W	1	;	tD
		difAhorro2_H,     		difAhorro2_L, 			//uint16_t		difAhorro2 = 0;		//08/FEB/2022		DS.W	1	;	tE
		alarmaIny_H,     		alarmaIny_L,					//uint16_t		tF_w = 0;					//08/FEB/2022		DS.W 1	;	equ	$011F	;	287 d	;tF_W

		//;-------------------  GRUPO DE PARÁMETROS A (Alarmas)  -----------------------------------
		//;-------------------  Parámetros de Alarma  ------------------------------------
		limevach_H,				limevach_L,				//uint16_t		limevach_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0121	;	289 d	;A0 - Temperatura sensor evaporador para cancelar deshielo
		limambch_H,				limambch_L,				////uint16_t		limambch_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0123	;	291 d	;A1 - Temperatura sensor ambiente para cancelar deshielo
		horaAhOn_H,				horaAhOn_L,				////uint16_t		anticong_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0127	;	295 d	;A3 - Temperatura de anticongelamiento del producto
		horaAhOff_H,			horaAhOff_L,			////uint16_t		tsumidero_w = 0;	//08/FEB/2022		DS.W 1	;	equ	$0125	;	293 d	;A2 - Temperatura de sumidero
		pulldown_H,				pulldown_L,				////uint16_t		pulldown_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0129	;	297 d	;A4 - Temperatura mínima para que se active el modo pulldown
		limineav_H,				limineav_L,				////uint16_t		limineav_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012B	;	299 d	;A5 - Limite inferior de alarma de temperatura evaporador
		limsual_H,				limsual_L,				////uint16_t		limsual_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012D	;	301 d	;A6 - Limite superior alarma de temperatura
		liminal_H,				liminal_L,				////uint16_t		liminal_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012F	;	303 d	;A7 - Limite inferior alarma de temperatura
		difTempAl_H,			difTempAl_L,			////uint16_t		difTempAl = 0;		//08/FEB/2022		DS.W 1	;	equ	$0131	;	305 d	;A8 -
		difDefi_H,				difDefi_L,				////uint16_t		difDefi = 0;					//08/FEB/2022		DS.W 1	;	equ	$0133	;	307 d	;A9 -
		AA_b_H,					AA_b_L,				////uint16_t		tempTC1 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0135	;	309 d	;AA -
		AB_b_H,					AB_b_L,				////uint16_t		tempTC2 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0137	;	311 d	;AB -
		spdiur_BK_H,			spdiur_BK_L,			////uint16_t		spdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$0139	;	313 d	;AC -
		difdiur_BK_H,			difdiur_BK_L,			////uint16_t		difdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013B	;	315 d	;AD -
		limsual_BK_H,			limsual_BK_L,			////uint16_t		limsual_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013D	;	317 d	;AE -
		liminal_BK_H,			liminal_BK_L,			//uint16_t		liminal_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013F	;	319 d	;AF -

		dato_seg2,										////uint8_t		dato_seg2 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0141	;	321 d	;Dato de seguridad 2

		//;-------------------  GRUPO DE PARÁMETROS L (Límites)  -----------------------------------
		//;-------------------  Parámetros de limites  -----------------------------------
		volt_000,					//uint8_t		volt_000 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0142	;	322 d	;L0 - Voltaje mínimo de protección para 120v
		volt_100,					////uint8_t		volt_100 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0143	;	323 d	;L1 - Voltaje máx de protección para 120v y mín para 220v
		volt_200,					////uint8_t		volt_200 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0144	;	324 d	;L2 - Voltaje máximo de protección para 220v
		protval,					////uint8_t		protval = 0;				//08/FEB/2022		DS.B 1	;	equ	$0145	;	325 d	;L3 - Tiempo de validación para protección del compresor
		interdh,					////uint8_t		interdh = 0;				//08/FEB/2022		DS.B 1	;	equ	$0146	;	326 d	;L4 - Tiempo de inter-deshielo
		timedh,						////uint8_t		timedh = 0;					//08/FEB/2022		DS.B 1	;	equ	$0147	;	327 d	;L5 - Tiempo de deshielo
		timepaf,					////uint8_t		timepaf = 0;				//08/FEB/2022		DS.B 1	;	equ	$0148	;	328 d	;L6 - Tiempo de puerta abierta para cancelar control ventilador
		timepa,						//uint8_t		timepa = 0;					//08/FEB/2022		DS.B 1	;	equ	$0149	;	329 d	;L7 - Tiempo de validación para alarma de puerta abierta
		alarval,					////uint8_t		alarval = 0;				//08/FEB/2022		DS.B 1	;	equ	$014A	;	330 d	;L8 - Tiempo de validación de alarma de temperatura
		timeNoct,					////uint8_t		timeNoct = 0;				//08/FEB/2022		DS.B 1	;	equ	$014B	;	331 d	;L9 - Tiempo de modo nocturno
		timeHold,					////uint8_t		timeHold = 0;						//08/FEB/2022		DS.B 1	;	equ	$014C	;	332 d	;LA -
		timeDefi,					////uint8_t		timeDefi = 0;						//08/FEB/2022		DS.B 1	;	equ	$014D	;	333 d	;LB -
		alarmDelay,					////uint8_t		alarmDelay = 0;						//08/FEB/2022		DS.B 1	;	equ	$014E	;	334 d	;LC -
		timeRetCo,						////uint8_t		LD_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$014F	;	335 d	;LD -
		tmDoorEvent,				////uint8_t		tmDoorEvent = 0;						//08/FEB/2022		DS.B 1	;	equ	$0150	;	336 d	;LE -
		loggerTime,					//uint8_t		loggerTime = 0;						//08/FEB/2022		DS.B 1	;	equ	$0151	;	337 d	;LF -

		//;-------------------  GRUPO DE PARÁMETROS C (Configuración)  -----------------------------------
		//;-------------------  Parámetros de configuración  -----------------------------
		escala,						//uint8_t		escala = 0;					//08/FEB/2022		DS.B 1	;	equ	$0152	;	338 d	;C0 - Escala de temperatura
		logicos,					//uint8_t		logicos = 0;				//08/FEB/2022		DS.B 1	;	equ	$0153	;	339 d	;C1 - Banderas de configuración
		filtro,						//uint8_t		filtro = 0;					//08/FEB/2022		DS.B 1	;	equ	$0154	;	340 d	;C2 - Intensidad filtro del sensor ambiente a la subida
		dhmode,						//uint8_t		dhmode = 0;					//08/FEB/2022		DS.B 1	;	equ	$0155	;	341 d	;C3 - Modo de deshielo
		timeadap,					//uint8_t		timeadap = 0;				//08/FEB/2022		DS.B 1	;	equ	$0156	;	342 d	;C4 - Tiempo adaptivo de deshielo
		protype,					//uint8_t		protype = 0;				//08/FEB/2022		DS.B 1	;	equ	$0157	;	343 d	;C5 - Tipo de protección del compresor
		sensit,						//uint8_t		sensit = 0;					//08/FEB/2022		DS.B 1	;	equ	$0158	;	344 d	;C6 - Sensibilidad de la fotocelda
		numSens,					//uint8_t		numSens = 0;						//08/FEB/2022		DS.B 1	;	equ	$0159	;	345 d	;C7 -
		nivDpyFail,					//uint8_t		nivDpyFail = 0;						//08/FEB/2022		DS.B 1	;	equ	$015A	;	346 d	;C8 -
		logicos2,					//uint8_t		logicos2 = 0;						//08/FEB/2022		DS.B 1	;	equ	$015B	;	347 d	;C9 -
		flagsBuzz,						//uint8_t		CA_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015C	;	348 d	;CA -
		flagsBuzz2,						//uint8_t		CB_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015D	;	349 d	;CB -
		CC_b,						//uint8_t		CC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015E	;	350 d	;CC -
		CD_b,					//uint8_t		CD_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015F	;	351 d	;CD -
		CE_b,					//uint8_t		CE_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0160	;	352 d	;CE -
		FlagBLE,						//uint8_t 	FlagBLE						= 0	;//

		//;-------------------  GRUPO DE PARÁMETROS F (Función)  -----------------------------------
		//;-------------------  Parámetros de función  -----------------------------------
		histvol,					//uint8_t		histvol = 0;				//08/FEB/2022		DS.B 1	;	equ	$0162	;	354 d	;F0 - Histéresis para la protección de voltaje
		rangein,					//uint8_t		rangein = 0;				//08/FEB/2022		DS.B 1	;	equ	$0163	;	355 d	;F1 - Tiempo de validación para salir de protección de voltaje
		dripposd,					//uint8_t		dripposd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0164	;	356 d	;F2 - Tiempo de goteo
		minbrake,					//uint8_t		minbrake = 0;				//08/FEB/2022		DS.B 1	;	equ	$0165	;	357 d	;F3 - Tiempo mínimo de descanso del compresor
		maxwork,					//uint8_t		maxwork = 0;				//08/FEB/2022		DS.B 1	;	equ	$0166	;	358 d	;F4 - Tiempo máximo de compresor encendido
		exhausted,					//uint8_t		exhausted = 0;			//08/FEB/2022		DS.B 1	;	equ	$0167	;	359 d	;F5 - Tiempo compresor OFF si cumplió máx de compresor ON
		cicloFd,					//uint8_t		cicloFd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0168	;	360 d	;F6 - Cicleo para ventilador en modo diurno
		cicloFn,					//uint8_t		cicloFn = 0;				//08/FEB/2022		DS.B 1	;	equ	$0169	;	361 d	;F7 - Cicleo para ventilador en modo nocturno
		timedoor,					//uint8_t		timedoor = 0;				//08/FEB/2022		DS.B 1	;	equ	$016A	;	362 d	;F8 - Tiempo mínimo de puerta cerrada para entrar a nocturno
		tOnVh,					//uint8_t		paramSr = 0;				//08/FEB/2022		DS.B 1	;	equ	$016B	;	363 d	;F9 - Pre-salida del modo Nocturno
		tOffVh,					//uint8_t		margdes = 0;				//08/FEB/2022		DS.B 1	;	equ	$016C	;	364 d	;FA - Margen de descarte
		timeluzoff,					//uint8_t		timeluzoff = 0;			//08/FEB/2022		DS.B 1	;	equ	$016D	;	365 d	;FB - Retardo apagado de pancarta al entrar a nocturno
		FC_b,						//uint8_t		FC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$016E	;	366 d	;FC -
		tiempoAhorro1,				//uint8_t		tiempoAhorro1 = 0;	//08/FEB/2022		DS.B	1	;	FD
		tiempoAhorro2,				//uint8_t		tiempoAhorro2 = 0;	//08/FEB/2022		DS.B	1	;	FE
		FF_b,						//uint8_t		FF_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0171	;	369 d	;FF -

		//;------------------  GRUPO DISPONIBLE  -----------------------
		//;------------------  Grupo Disponible  -----------------------
		AddModBus,				//uint8_t		D0_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0172	;	370 d	:D0 -
		password,				//uint8_t		password = 0;						//08/FEB/2022		DS.B 1	;	equ	$0173	;	371 d	:D1 -
		D2_b,					//uint8_t		D2_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0174	;	372 d	;D2 -
		//@near	uint8_t		D3_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0175	;	373 d	;D3 -
		//@near	uint8_t		D4_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0176	;	374 d	;D4 -
		//@near	uint8_t		D5_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0177	;	375 d	;D5 -
		//@near	uint8_t		D6_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0178	;	376 d	;D6 -
		//@near uint8_t modelo  		      = 0;
		cliente,				//uint8_t		cliente 			= 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -
		clienteELTEC,			//uint8_t		clienteELTEC 	= 0;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -
		hardware,				//uint8_t		hardware			= 0;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -
		modelo0,				//uint8_t		modelo0				= 0;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -
		modelo,					//uint8_t	 	modelo	      =	0;

		//;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
		//;------------------  Identificador y bytes disponibles  -----------------------
		param_ID,				//uint8_t		param_ID = 0;				//08/FEB/2022		DS.B 1	;	equ	$017A	;	378 d	;B0 – Identificador de parámetros programados
		version1,				//uint8_t version1					= 0;
		version2,				//uint8_t version2	      	= 0;
		plantilla1,				//uint8_t plantilla1				= 0;
		plantilla2,				//uint8_t plantilla2   		  = 0;

		dato_seg3				//uint8_t		dato_seg3 = 0;			//08/FEB/2022		DS.B 1	;	equ	$017F	;	383 d	;Dato de seguridad 3
};





////;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
////;------------------  Identificador y bytes disponibles  -----------------------
//extern uint8_t	param_ID;				//08/FEB/2022		DS.B 1	;	equ	$017A	;	378 d	;B0 – Identificador de parámetros programados
//extern uint8_t version1;
//extern uint8_t version2;
//extern uint8_t plantilla1;
//extern uint8_t plantilla2;
//
//extern uint8_t		dato_seg3;			//08/FEB/2022		DS.B 1	;	equ	$017F	;	383 d	;Dato de seguridad 3
//-------------------------- Limite de 128 Bytes ------------------------------------------------



extern uint8_t		voltram [16];				//08/FEB/2022		DS.B	16; RAM para Voltaje

extern uint8_t	copiaPlantilla [128];
enum parametrosCPlantilla  {
		cdato_seg1,															//uint8_t		dato_seg1 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0100	;	256 d	;Dato de seguridad 1

		//;-------------------  GRUPO DE PARÁMETROS T (Temperatura)  -----------------------------------
		//;-------------------  Parámetros de temperatura  -------------------------------
		cspdiur_H,     			cspdiur_L, 				//uint16_t		cspdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0101	;	257 d	;t0 - Set point compresor diurno
		cdifdiur_H,				cdifdiur_L,				//uint16_t		cdifdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0103	;	259 d	;t1 - Diferencial para el modo diurno
		cosdiur_H,     			cosdiur_L, 				//uint16_t		cosdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0105	;	261 d	;t2 - Offset para el modo diurno
		cspnoct_H,     			cspnoct_L, 				//uint16_t		cspnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0107	;	263 d	;t3 - Set point compresor nocturno
		cdifnoct_H,     		cdifnoct_L, 			//uint16_t		cdifnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0109	;	265 d	;t4 - Diferencial para el modo nocturno
		cosnoct_H,     			cosnoct_L, 				//uint16_t		cosnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010B	;	267 d	;t5 - Offset para el modo nocturno
		crngmin_H,     			crngmin_L, 				//uint16_t		crngmin_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010D	;	269 d	;t6 - Rango mínimo para ajuste del set point
		crngmax_H,     			crngmax_L, 				//uint16_t		crngmax_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010F	;	271 d	;t7 - Rango máximo para ajuste del set point
		calarmaRetCo_H,     	calarmaRetCo_L, 					//uint16_t		ct8_w = 0;				//
		cdefrResetTemp_H,     	cdefrResetTemp_L, 		//uint16_t		cdefrResetTemp = 0;//
		cdefrStartTemp_H,     	cdefrStartTemp_L, 		//uint16_t		cdefrStartTemp = 0;		//08/FEB/2022		DS.W 1	;	equ	$0115	;	277 d	;tA_W
		ctempRetCo_H,    		ctempRetCo_L, 	//uint16_t		cdefrStartTempAmb = 0;	//08/FEB/2022		DS.W 1	;	equ	$0117	;	279 d	;tB_W
		ctC_H,     				ctC_L, 					//uint16_t		ctC_w = 0;				//08/FEB/2022		DS.W 1	;	equ	$0119	;	281 d	;tC_W
		cdifAhorro1_H,     		cdifAhorro1_L, 			//uint16_t		cdifAhorro1 = 0;		//08/FEB/2022		DS.W	1	;	tD
		cdifAhorro2_H,     		cdifAhorro2_L, 			//uint16_t		cdifAhorro2 = 0;		//08/FEB/2022		DS.W	1	;	tE
		calarmaIny_H,     		calarmaIny_L,					//uint16_t		ctF_w = 0;				//08/FEB/2022		DS.W 1	;	equ	$011F	;	287 d	;tF_W

		//;-------------------  GRUPO DE PARÁMETROS A (Alarmas)  -----------------------------------
		//;-------------------  Parámetros de Alarma  ------------------------------------
		climevach_H,			climevach_L,			//uint16_t		climevach_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0121	;	289 d	;A0 - Temperatura sensor evaporador para cancelar deshielo
		climambch_H,			climambch_L,			////uint16_t	climambch_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0123	;	291 d	;A1 - Temperatura sensor ambiente para cancelar deshielo
		choraAhOn_H,			choraAhOn_L,			////uint16_t	canticong_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0127	;	295 d	;A3 - Temperatura de anticongelamiento del producto
		choraAhOff_H,			choraAhOff_L,			////uint16_t	ctsumidero_w = 0;	//08/FEB/2022		DS.W 1	;	equ	$0125	;	293 d	;A2 - Temperatura de sumidero
		cpulldown_H,			cpulldown_L,			////uint16_t	cpulldown_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0129	;	297 d	;A4 - Temperatura mínima para que se active el modo pulldown
		climineav_H,			climineav_L,			////uint16_t	climineav_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012B	;	299 d	;A5 - Limite inferior de alarma de temperatura evaporador
		climsual_H,				climsual_L,				////uint16_t	climsual_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012D	;	301 d	;A6 - Limite superior alarma de temperatura
		climinal_H,				climinal_L,				////uint16_t	climinal_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012F	;	303 d	;A7 - Limite inferior alarma de temperatura
		cdifTempAl_H,			cdifTempAl_L,			////uint16_t	cdifTempAl = 0;		//08/FEB/2022		DS.W 1	;	equ	$0131	;	305 d	;A8 -
		cdifDefi_H,				cdifDefi_L,				////uint16_t	cdifDefi = 0;					//08/FEB/2022		DS.W 1	;	equ	$0133	;	307 d	;A9 -
		cAA_b_H,				cAA_b_L,				////uint16_t	ctempTC1 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0135	;	309 d	;AA -
		cAB_b_H,				cAB_b_L,				////uint16_t	ctempTC2 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0137	;	311 d	;AB -
		cspdiur_BK_H,			cspdiur_BK_L,			////uint16_t	cspdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$0139	;	313 d	;AC -
		cdifdiur_BK_H,			cdifdiur_BK_L,			////uint16_t	cdifdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013B	;	315 d	;AD -
		climsual_BK_H,			climsual_BK_L,			////uint16_t	climsual_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013D	;	317 d	;AE -
		climinal_BK_H,			climinal_BK_L,			////uint16_t	climinal_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013F	;	319 d	;AF -

		cdato_seg2,										////uint8_t		dato_seg2 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0141	;	321 d	;Dato de seguridad 2

		//;-------------------  GRUPO DE PARÁMETROS L (Límites)  -----------------------------------
		//;-------------------  Parámetros de limites  -----------------------------------
		cvolt_000,					////uint8_t		cvolt_000 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0142	;	322 d	;L0 - Voltaje mínimo de protección para 120v
		cvolt_100,					////uint8_t		cvolt_100 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0143	;	323 d	;L1 - Voltaje máx de protección para 120v y mín para 220v
		cvolt_200,					////uint8_t		cvolt_200 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0144	;	324 d	;L2 - Voltaje máximo de protección para 220v
		cprotval,					////uint8_t		cprotval = 0;				//08/FEB/2022		DS.B 1	;	equ	$0145	;	325 d	;L3 - Tiempo de validación para protección del compresor
		cinterdh,					////uint8_t		cinterdh = 0;				//08/FEB/2022		DS.B 1	;	equ	$0146	;	326 d	;L4 - Tiempo de inter-deshielo
		ctimedh,					////uint8_t		ctimedh = 0;				//08/FEB/2022		DS.B 1	;	equ	$0147	;	327 d	;L5 - Tiempo de deshielo
		ctimepaf,					////uint8_t		ctimepaf = 0;				//08/FEB/2022		DS.B 1	;	equ	$0148	;	328 d	;L6 - Tiempo de puerta abierta para cancelar control ventilador
		ctimepa,					////uint8_t		ctimepa = 0;				//08/FEB/2022		DS.B 1	;	equ	$0149	;	329 d	;L7 - Tiempo de validación para alarma de puerta abierta
		calarval,					////uint8_t		calarval = 0;				//08/FEB/2022		DS.B 1	;	equ	$014A	;	330 d	;L8 - Tiempo de validación de alarma de temperatura
		ctimeNoct,					////uint8_t		ctimeNoct = 0;				//08/FEB/2022		DS.B 1	;	equ	$014B	;	331 d	;L9 - Tiempo de modo nocturno
		ctimeHold,					////uint8_t		ctimeHold = 0;				//08/FEB/2022		DS.B 1	;	equ	$014C	;	332 d	;LA -
		ctimeDefi,					////uint8_t		ctimeDefi = 0;				//08/FEB/2022		DS.B 1	;	equ	$014D	;	333 d	;LB -
		calarmDelay,				////uint8_t		calarmDelay = 0;			//08/FEB/2022		DS.B 1	;	equ	$014E	;	334 d	;LC -
		ctimeRetCo,					////uint8_t		cLD_b = 0;					//08/FEB/2022		DS.B 1	;	equ	$014F	;	335 d	;LD -
		ctmDoorEvent,				////uint8_t		ctmDoorEvent = 0;			//08/FEB/2022		DS.B 1	;	equ	$0150	;	336 d	;LE -
		cloggerTime,				////uint8_t		cloggerTime = 0;			//08/FEB/2022		DS.B 1	;	equ	$0151	;	337 d	;LF -

		//;-------------------  GRUPO DE PARÁMETROS C (Configuración)  -----------------------------------
		//;-------------------  Parámetros de configuración  -----------------------------
		cescala,					//uint8_t		cescala = 0;				//08/FEB/2022		DS.B 1	;	equ	$0152	;	338 d	;C0 - Escala de temperatura
		clogicos,					//uint8_t		clogicos = 0;				//08/FEB/2022		DS.B 1	;	equ	$0153	;	339 d	;C1 - Banderas de configuración
		cfiltro,					//uint8_t		cfiltro = 0;				//08/FEB/2022		DS.B 1	;	equ	$0154	;	340 d	;C2 - Intensidad filtro del sensor ambiente a la subida
		cdhmode,					//uint8_t		cdhmode = 0;				//08/FEB/2022		DS.B 1	;	equ	$0155	;	341 d	;C3 - Modo de deshielo
		ctimeadap,					//uint8_t		ctimeadap = 0;				//08/FEB/2022		DS.B 1	;	equ	$0156	;	342 d	;C4 - Tiempo adaptivo de deshielo
		cprotype,					//uint8_t		cprotype = 0;				//08/FEB/2022		DS.B 1	;	equ	$0157	;	343 d	;C5 - Tipo de protección del compresor
		csensit,					//uint8_t		csensit = 0;				//08/FEB/2022		DS.B 1	;	equ	$0158	;	344 d	;C6 - Sensibilidad de la fotocelda
		cnumSens,					//uint8_t		cnumSens = 0;				//08/FEB/2022		DS.B 1	;	equ	$0159	;	345 d	;C7 -
		cnivDpyFail,				//uint8_t		cnivDpyFail = 0;			//08/FEB/2022		DS.B 1	;	equ	$015A	;	346 d	;C8 -
		clogicos2,					//uint8_t		clogicos2 = 0;				//08/FEB/2022		DS.B 1	;	equ	$015B	;	347 d	;C9 -
		cflagsBuzz,						//uint8_t		cCA_b = 0;					//08/FEB/2022		DS.B 1	;	equ	$015C	;	348 d	;CA -
		cflagsBuzz2,						//uint8_t		cCB_b = 0;					//08/FEB/2022		DS.B 1	;	equ	$015D	;	349 d	;CB -
		cCC_b,						//uint8_t		cCC_b = 0;					//08/FEB/2022		DS.B 1	;	equ	$015E	;	350 d	;CC -
		cCD_b,					//uint8_t		cCD_b = 0;					//08/FEB/2022		DS.B 1	;	equ	$015F	;	351 d	;CD -
		cCE_b,					//uint8_t		cCE_b = 0;					//08/FEB/2022		DS.B 1	;	equ	$0160	;	352 d	;CE -
		cFlagBLE,					//uint8_t 		cFlagBLE					= 0	;//

		//;-------------------  GRUPO DE PARÁMETROS F (Función)  -----------------------------------
		//;-------------------  Parámetros de función  -----------------------------------
		chistvol,					//uint8_t		chistvol = 0;				//08/FEB/2022		DS.B 1	;	equ	$0162	;	354 d	;F0 - Histéresis para la protección de voltaje
		crangein,					//uint8_t		crangein = 0;				//08/FEB/2022		DS.B 1	;	equ	$0163	;	355 d	;F1 - Tiempo de validación para salir de protección de voltaje
		cdripposd,					//uint8_t		cdripposd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0164	;	356 d	;F2 - Tiempo de goteo
		cminbrake,					//uint8_t		cminbrake = 0;				//08/FEB/2022		DS.B 1	;	equ	$0165	;	357 d	;F3 - Tiempo mínimo de descanso del compresor
		cmaxwork,					//uint8_t		cmaxwork = 0;				//08/FEB/2022		DS.B 1	;	equ	$0166	;	358 d	;F4 - Tiempo máximo de compresor encendido
		cexhausted,					//uint8_t		cexhausted = 0;			//08/FEB/2022		DS.B 1	;	equ	$0167	;	359 d	;F5 - Tiempo compresor OFF si cumplió máx de compresor ON
		ccicloFd,					//uint8_t		ccicloFd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0168	;	360 d	;F6 - Cicleo para ventilador en modo diurno
		ccicloFn,					//uint8_t		ccicloFn = 0;				//08/FEB/2022		DS.B 1	;	equ	$0169	;	361 d	;F7 - Cicleo para ventilador en modo nocturno
		ctimedoor,					//uint8_t		ctimedoor = 0;				//08/FEB/2022		DS.B 1	;	equ	$016A	;	362 d	;F8 - Tiempo mínimo de puerta cerrada para entrar a nocturno
		ctOnVh,					//uint8_t		cparamSr = 0;				//08/FEB/2022		DS.B 1	;	equ	$016B	;	363 d	;F9 - Pre-salida del modo Nocturno
		ctOffVh,					//uint8_t		cmargdes = 0;				//08/FEB/2022		DS.B 1	;	equ	$016C	;	364 d	;FA - Margen de descarte
		ctimeluzoff,				//uint8_t		ctimeluzoff = 0;			//08/FEB/2022		DS.B 1	;	equ	$016D	;	365 d	;FB - Retardo apagado de pancarta al entrar a nocturno
		cFC_b,						//uint8_t		cFC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$016E	;	366 d	;FC -
		ctiempoAhorro1,				//uint8_t		ctiempoAhorro1 = 0;	//08/FEB/2022		DS.B	1	;	FD
		ctiempoAhorro2,				//uint8_t		ctiempoAhorro2 = 0;	//08/FEB/2022		DS.B	1	;	FE
		cFF_b,						//uint8_t		cFF_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0171	;	369 d	;FF -

		//;------------------  GRUPO DISPONIBLE  -----------------------
		//;------------------  Grupo Disponible  -----------------------
		cAddModBus,				//uint8_t		cD0_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0172	;	370 d	:D0 -
		cpassword,				//uint8_t		cpassword = 0;						//08/FEB/2022		DS.B 1	;	equ	$0173	;	371 d	:D1 -
		cD2_b,					//uint8_t		cD2_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0174	;	372 d	;D2 -
		//@near	uint8_t		D3_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0175	;	373 d	;D3 -
		//@near	uint8_t		D4_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0176	;	374 d	;D4 -
		//@near	uint8_t		D5_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0177	;	375 d	;D5 -
		//@near	uint8_t		D6_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0178	;	376 d	;D6 -
		//@near uint8_t modelo  		      = 0;
		ccliente,				//uint8_t		ccliente 			= 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -
		cclienteELTEC,			//uint8_t		cclienteELTEC 	= 0;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -
		chardware,				//uint8_t		chardware			= 0;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -
		cmodelo0,				//uint8_t		cmodelo0				= 0;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -
		cmodelo,				//uint8_t	 	cmodelo	      =	0;

		//;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
		//;------------------  Identificador y bytes disponibles  -----------------------
		cparam_ID,				//uint8_t	cparam_ID = 0;				//08/FEB/2022		DS.B 1	;	equ	$017A	;	378 d	;B0 – Identificador de parámetros programados
		cversion1,				//uint8_t 	cversion1					= 0;
		cversion2,				//uint8_t 	cversion2	      	= 0;
		cplantilla1,			//uint8_t 	cplantilla1				= 0;
		cplantilla2,			//uint8_t 	cplantilla2   		  = 0;

		cdato_seg3				//uint8_t	cdato_seg3 = 0;			//08/FEB/2022		DS.B 1	;	equ	$017F	;	383 d	;Dato de seguridad 3
};

//;------------------------ bancomu Variables relacionadas con el proceso de COMUNICACIÓN
extern uint8_t		T_Espera;					//08/FEB/2022		DS.B 1 ;Tiempo de espera para la comunicación


//;------------------------ bancomu Variables relacionadas con el proceso de COMUNICACIÓN
extern uint8_t		Cnt_EspComu;			//08/FEB/2022		DS.B 1 ;Contador para Tx Rx un byte cada 2ms


//;Datos que son Transmitidos de TREFPT44 a TREFTSAA
//;¡¡¡¡¡¡¡¡¡¡¡¡¡NO MODIFICAR EL ORDEN!!!!!!!!!!!!!!
extern uint8_t		Display_1;				//08/FEB/2022		DS.B 1 ;1 1B - Desplegado para el display 1 en 7 segmentos y punto
extern uint8_t		Display_2;				//08/FEB/2022		DS.B 1 ;2 1B - Desplegado para el display 2 en 7 segmentos y punto
extern _Bool 		Ind_Par[8];				//08/FEB/2022		DS.B 1 ;3 1B - 2 Indicadores LED y un bit de control de parpadeo
extern _Bool 		dms_extra[8];				//08/FEB/2022		DS.B 1 ;4 1B - 2 Indicadores LED y un bit de control de parpadeo

//;Datos que son Recibidos de TREFTSAA a TREFPT44
//;¡¡¡¡¡¡¡¡¡¡¡¡¡NO MODIFICAR EL ORDEN!!!!!!!!!!!!!!
extern _Bool Botones_T[8];

// LN_833:	#pragma space extern [] @eeprom @near
extern __attribute__((section(".myBufSectionEEPROM_P"))) uint8_t	eePlantilla [128];
enum parametrosEEplantilla  {
	eedato_seg1,											//;Dato de seguridad 1
	//;-------------------  GRUPO DE PARÁMETROS T (Temperatura)  -------------------------
	//;-------------------  Parámetros de temperatura  -------------------------------
	eespdiur_H,				eespdiur_L,						//09/FEB/2022		DC.W	{-35};{-30}	;	16385 d	4001 h	;t0 - Set point compresor diurno	-3 ° C
	eedifdiur_H,			eedifdiur_L,					//09/FEB/2022		DC.W	{25};{20}	;	16387 d	4003 h	;t1 - Diferencial para el modo diurno	2 ° C
	eeosdiur_H,				eeosdiur_L,						//09/FEB/2022		DC.W	{-5};{0}	;	16389 d	4005 h	;t2 - Offset para el modo diurno	0 ° C
	eespnoct_H,				eespnoct_L,						//09/FEB/2022		DC.W	{10};{0}	;	16391 d	4007 h	;t3 - Set point compresor nocturno	0 ° C
	eedifnoct_H,			eedifnoct_L,					//09/FEB/2022		DC.W	{25};{20}	;	16393 d	4009 h	;t4 - Diferencial para el modo nocturno	2 ° C
	eeosnoct_H,				eeosnoct_L,						//09/FEB/2022		DC.W	{-5};{0}	;	16395 d	400B h	;t5 - Offset para el modo nocturno	0 ° C
	eerngmin_H,				eerngmin_L,						//09/FEB/2022		DC.W	{-150}	;	16397 d	400D h	;t6 - Rango mínimo para ajuste del set point	-15 ° C
	eerngmax_H,				eerngmax_L,						//09/FEB/2022		DC.W	{150}	;	16399 d	400F h	;t7 - Rango máximo para ajuste del set point	15 ° C
	eealarmaRetCo_H,		eealarmaRetCo_L,							//
	eedefrResetTemp_H,		eedefrResetTemp_L,				//;t9 - t9	Defrost reset temperature 3.0°C
	eedefrStartTemp_H,		eedefrStartTemp_L,				//; tA	Defrost start temperature -10.0 ºC
	eetempRetCo_H,			eetempRetCo_L,						//09/FEB/2022		DC.W	{0}	;	16407 d	4017 h	;tB_W
	eetC_H,					eetC_L,							//09/FEB/2022		DC.W	{0}	;	16409 d	4019 h	;tC_W
	eedifAhorro1_H,			eedifAhorro1_L,					//09/FEB/2022		DC.W	{10}		;	tD
	eedifAhorro2_H,			eedifAhorro2_L,					//09/FEB/2022		DC.W	{10}		;	tE
	eealarmaIny_H,			eealarmaIny_L,							//09/FEB/2022		DC.W	{0}	;	16415 d	401F h	;tF_W

	//;-------------------  GRUPO DE PARÁMETROS A (Alarmas)  -----------------------------------
	//;-------------------  Parámetros de Alarma  ------------------------------------
	eelimevach_H,		eelimevach_L,					//09/FEB/2022		DC.W	{150}	;	16417 d	4021 h	;A0 - Temperatura sensor evaporador para cancelar deshielo	15 ° C
	eelimambch_H,		eelimambch_L,					//09/FEB/2022		DC.W	{300}	;	16419 d	4023 h	;A1 - Temperatura sensor ambiente para cancelar deshielo	30 ° C
	eehoraAhOn_H,		eehoraAhOn_L,					//09/FEB/2022		DC.W	{-310}	;	16423 d	4027 h	;A3 - Temperatura de anticongelamiento del producto	-31 ° C
	eehoraAhOff_H,		eehoraAhOff_L,					//09/FEB/2022		DC.W	{300}	;	16421 d	4025 h	;A2 - Temperatura de sumidero	30 ° C
	eepulldown_H,		eepulldown_L,					//09/FEB/2022		DC.W	{120}	;	16425 d	4029 h	;A4 - Temperatura mínima para que se active el modo pulldown	12 ° C
	eelimineav_H,		eelimineav_L,					//09/FEB/2022		DC.W	{-300}	;	16427 d	402B h	;A5 - Limite inferior de alarma de temperatura evaporador	-30 ° C
	eelimsual_H,		eelimsual_L,					//09/FEB/2022		DC.W	{120}	;	16429 d	402D h	;A6 - Limite superior alarma de temperatura	12 ° C
	eeliminal_H,		eeliminal_L,					//09/FEB/2022		DC.W	{-120}	;	16431 d	402F h	;A7 - Limite inferior alarma de temperatura	-12 ° C
	eedifTempAl_H,		eedifTempAl_L,					//09/FEB/2022		DC.W	{0}	;	16433 d	4031 h	;A8 -
	eedifDefi_H,		eedifDefi_L,					//09/FEB/2022		DC.W	{0}	;	16435 d	4033 h	;A9 -	diferencial de deficiencia (2.0°C)
	eeAA_b_H,			eeAA_b_L,					//09/FEB/2022		DC.W	{0}	;	16437 d	4035 h	;AA -
	eeAB_b_H,			eeAB_b_L,					//09/FEB/2022		DC.W	{0}	;	16439 d	4037 h	;AB -
	eespdiur_BK_H,		eespdiur_BK_L,					//09/FEB/2022		DC.W	{0}	;	16441 d	4039 h	;AC -
	eedifdiur_BK_H,		eedifdiur_BK_L,					//09/FEB/2022		DC.W	{0}	;	16443 d	403B h	;AD -
	eelimsual_BK_H,		eelimsual_BK_L,					//09/FEB/2022		DC.W	{0}	;	16445 d	403D h	;AE -
	eeliminal_BK_H,		eeliminal_BK_L,					//09/FEB/2022		DC.W	{0}	;	16447 d	403F h	;AF -

	eedato_seg2,										//09/FEB/2022			    DC.B	$66	;	16449 d	4041 h	;Dato de seguridad 2

	//;-------------------  GRUPO DE PARÁMETROS L (Límites)  -----------------------------------
	//;-------------------  Parámetros de limites  -----------------------------------
	eevolt_000,					//09/FEB/2022		DC.B	85	;	16450 d	4042 h	;L0 - Voltaje mínimo de protección para 120v	0_85 V
	eevolt_100,					//09/FEB/2022		DC.B	40	;	16451 d	4043 h	;L1 - Voltaje máx de protección para 120v y mín para 220v	1_40 V
	eevolt_200,					//09/FEB/2022		DC.B	40	;	16452 d	4044 h	;L2 - Voltaje máximo de protección para 220v	2_40 V
	eeprotval,					//09/FEB/2022		DC.B	15	;	16453 d	4045 h	;L3 - Tiempo de validación para protección del compresor	15 segundos
	eeinterdh,					//09/FEB/2022		DC.B	04	;	16454 d	4046 h	;L4 - Tiempo de inter-deshielo	4 horas
	eetimedh,					//09/FEB/2022		DC.B	30	;	16455 d	4047 h	;L5 - Tiempo de deshielo	30 minutos
	eetimepaf,					//09/FEB/2022		DC.B	99	;	16456 d	4048 h	;L6 - Tiempo de puerta abierta para cancelar control ventilador	99 minutos
	eetimepa,					//09/FEB/2022		DC.B	03	;	16457 d	4049 h	;L7 - Tiempo de validación para alarma de puerta abierta	3 minutos
	eealarval,					//09/FEB/2022		DC.B	15	;	16458 d	404A h	;L8 - Tiempo de validación de alarma de temperatura	15 minutos
	eetimeNoct,					//09/FEB/2022		DC.B	10	;	16459 d	404B h	;L9 -	Tirmpo de modo nocturno (10 minutos)
	eetimeHold,					//;LA -	Tiempo de bloqueo de display despues del deshielo (15 minutos)
	eetimeDefi,					//09/FEB/2022		DC.B	0	;	16461 d	404D h	;LB -	 tiempo de compresor encendido para realizar mediciones de deficiencia 5 minutos (0 = funcion cancelada)
	eealarmDelay,				// LC	tiempo para silenciar alarma
	eetimeRetCo,						//09/FEB/2022		DC.B	0	;	16463 d	404F h	;LD -
	eetmDoorEvent,				//09/FEB/2022		DC.B	0	;	16464 d	4050 h	;LE -
	eeloggerTime,				//09/FEB/2022		DC.B	0	;	16465 d	4051 h	;LF -

	//;-------------------  GRUPO DE PARÁMETROS C (Configuración)  -----------------------------------
	//;-------------------  Parámetros de configuración  -----------------------------
	eeescala,				//09/FEB/2022		DC.B	00	;	16466 d	4052 h	;C0 - Escala de temperatura	00 : °C , 01: °F
	eelogicos,				//0x02;//01;//17;				//09/FEB/2022		DC.B	17	;	16467 d	4053 h	;C1 - Banderas de configuración	* Ver lista al final
	eefiltro,				//09/FEB/2022		DC.B	06	;	16468 d	4054 h	;C2 - Intensidad filtro del sensor ambiente a la subida	[ 0 a 9 ]
	eedhmode,				//09/FEB/2022		DC.B	02	;	16469 d	4055 h	;C3 - Modo de deshielo	00: flujo de aire, 01: gas, 02: resistencia
	eetimeadap,				//09/FEB/2022		DC.B	00	;	16470 d	4056 h	;C4 - Tiempo adaptivo de deshielo	[ 0 a 8 ]
	eeprotype,				//Manuel 09-MAR-2022;				//09/FEB/2022		DC.B	01	;	16471 d	4057 h	;C5 - Tipo de protección del compresor	00: sin protección , 01: 110V , 02: 220V
	eesensit,				//09/FEB/2022		DC.B	01	;	16472 d	4058 h	;C6 - Sensibilidad de la fotocelda	[ 0 a 9 ]
	eenumSens,				//09/FEB/2022		DC.B	0	;	16473 d	4059 h	;C7 - Número de sensores con los que trabajará el equipo
	eenivDpyFail,			//09/FEB/2022		DC.B	0	;	16474 d	405A h	;C8 -
	eelogicos2,				//09/FEB/2022		DC.B	0	;	16475 d	405B h	;C9 -
	eeflagsBuzz,					//09/FEB/2022		DC.B	0	;	16476 d	405C h	;CA -
	eeflagsBuzz2,					//09/FEB/2022		DC.B	0	;	16477 d	405D h	;CB -
	eeCC_b,					//09/FEB/2022		DC.B	0	;	16478 d	405E h	;CC -
	eeCD_b,				//09/FEB/2022		DC.B	0	;	16479 d	405F h	;CD -	"U"
	eeCE_b,				//09/FEB/2022		DC.B	0	;	16480 d	4060 h	;CE -	"A"
	eeFlagBLE,				//

	//;-------------------  GRUPO DE PARÁMETROS F (Función)  -----------------------------------
	//;-------------------  Parámetros de función  -----------------------------------
	eehistvol,				//09/FEB/2022				DC.B	42	;	16482 d	4062 h	;F0 - Histéresis para la protección de voltaje	 4Vmín, 2Vmáx (real 5.3)
	eerangein,				//09/FEB/2022			 	DC.B	03	;	16483 d	4063 h	;F1 - Tiempo de validación para salir de protección de voltaje	0.3 minutos
	eedripposd,				//09/FEB/2022			  DC.B	11	;	16484 d	4064 h	;F2 - Tiempo de goteo	1 minuto . 1 minuto
	eeminbrake,				//09/FEB/2022		 	  DC.B	10	;	16485 d	4065 h	;F3 - Tiempo mínimo de descanso del compresor	1.0 minutos
	eemaxwork,				//09/FEB/2022			 	DC.B	00	;	16486 d	4066 h	;F4 - Tiempo máximo de compresor encendido	Indefinido
	eeexhausted,			//09/FEB/2022		  	DC.B	250	;	16487 d	4067 h	;F5 - Tiempo compresor OFF si cumplió máx de compresor ON	25 horas
	eecicloFd,				//09/FEB/2022				DC.B	33	;	16488 d	4068 h	;F6 - Cicleo para ventilador en modo diurno	3 minutos ON. 3 minutos OFF
	eecicloFn,				//09/FEB/2022			 	DC.B	33	;	16489 d	4069 h	;F7 - Cicleo para ventilador en modo nocturno	3 minutos ON. 3 minutos OFF
	eetimedoor,				//09/FEB/2022				DC.B	20	;	16490 d	406A h	;F8 - Tiempo mínimo de puerta cerrada para entrar a nocturno	2 horas
	eetOnVh,				//09/FEB/2022			 	DC.B	20	;	16491 d	406B h	;F9 - Pre-salida del modo Nocturno	2 horas
	eetOffVh,				//09/FEB/2022			 	DC.B	35	;	16492 d	406C h	;FA - Margen de descarte 	3.5 horas
	eetimeluzoff,			//09/FEB/2022				DC.B	0	;	16493 d	406D h	;FB - Retardo apagado de pancarta al entrar a nocturno	0 horas
	eeFC_b,					//09/FEB/2022			  DC.B	0	;	16494 d	406E h	;FC -
	eetiempoAhorro1,		//09/FEB/2022		  	DC.B	20	;	FD 	2.0 horas
	eetiempoAhorro2,		//09/FEB/2022			  DC.B	10	;	FE 	1.0 horas
	eeFF_b,					//09/FEB/2022			  DC.B	0	;	16497 d	4071 h	;FF -

	//;------------------  GRUPO DISPONIBLE  -----------------------
	//;------------------  Grupo Disponible  -----------------------
	eeAddModBus,		//23/MAR/2022			      	DC.B	0	;	16498 d	4072 h	:D0 -
	eepassword,			//09/FEB/2022			      	DC.B	0	;	16499 d	4073 h	:D1 -
	eeD2_b,				//09/FEB/2022			      	DC.B	0	;	16500 d	4074 h	;D2 -	               0 = modelo de refrigerador 0
	//uint8_t			eeD3_b = 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -
	//uint8_t			eeD4_b = 0;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -
	//uint8_t			eeD5_b = 0;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -
	//uint8_t			eeD6_b = 0;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -
	//uint8_t	 		eemodelo	        =	0x50	;
	//uint8_t	 		eemodelo	        =	0x4B	; // cambia de 0x50 a 0x51 por cambio de modulo BLE a WiFi+BLE
	eecliente,			//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -	     0 = GENERICO
	eeclienteELTEC,		//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -	     2 = IMBERA
	eehardware,			//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -	    1.0
	eemodelo0,			//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -	    'E'
	eemodelo,			//                                                            '8'

	//;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
	//;------------------  Identificador y bytes disponibles  -----------------------
	eeparam_ID,				//09/FEB/2022	    DC.B	0	;	16506 d	407A h	;B0 – Identificador de parámetros programados	00: fábrica, 01: teclado, XX: llave fija
	eeversion1,				// versión 0.03
	eeversion2,
	//uint8_t	 eeversion2	      =	02		;//RM_20230908 VFW 0.002 Ajuste en calibración y envio de MAC a llave
	eeplantilla1,
	eeplantilla2,

	eedato_seg3		//09/FEB/2022	    DC.B	$CC	;	16511 d	407F h	;Dato de seguridad 3
};

extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eevolt_div;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eevolt_mul;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eef_voltaje;



extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t eeEstado1;	// inicia con refrigerador encendido y lámpara encedida y cerradura en off

extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeTimeUnix1;	//	uint8_t		eeTimeUnix1 = 0;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeTimeUnix2;	//	uint8_t		eeTimeUnix2 = 0;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeTimeUnix3;	//	uint8_t		eeTimeUnix3 = 0;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeTimeUnix4;	//	uint8_t		eeTimeUnix4 = 0;

extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat1;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat2;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat3;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat4;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong1;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong2;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong3;
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong4;

extern __attribute__((section(".myBufSectionEEPROM_V"))) uint16_t eeCntRegDATA;	//
extern __attribute__((section(".myBufSectionEEPROM_V"))) uint16_t eeCntRegEVENT;	//


/*
 * Copia de los datos de la memoria EEPROM que se almacenan en RAM
 */

extern uint8_t	reePlantilla [128];

extern uint8_t	reevolt_div; //64; 		//1;
extern uint8_t	reevolt_mul; //100;	    // 1;
extern uint8_t	reef_voltaje;
extern uint8_t reeEstado1;	//	uint8_t		eeEstado1 = 0x05;	// inicia con refrigerador encendido y lámpara encedida y cerradura en off
extern uint8_t reeTimeUnix1;	//	uint8_t		eeTimeUnix1 = 0;
extern uint8_t reeTimeUnix2;	//	uint8_t		eeTimeUnix2 = 0;
extern uint8_t reeTimeUnix3;	//	uint8_t		eeTimeUnix3 = 0;
extern uint8_t reeTimeUnix4;	//	uint8_t		eeTimeUnix4 = 0;
extern uint8_t	reeLat1;
extern uint8_t	reeLat2;
extern uint8_t	reeLat3;
extern uint8_t	reeLat4;
extern uint8_t	reeLong1;
extern uint8_t	reeLong2;
extern uint8_t	reeLong3;
extern uint8_t	reeLong4;

extern uint16_t reeCntRegDATA;	//
extern uint16_t reeCntRegEVENT;	//



// LN_1013:	#pragma section @near {data}

extern uint8_t data_buffer[128];	// RAM reservada para buffer de datos para grabación de bloques
extern uint8_t event_buffer[128];	// RAM reservada para buffer de datos para grabación de bloques


extern uint8_t ProgMemCode;
extern uint8_t *dirPointer;	// Apuntadorpara seleccionar la direccion donde seran grabados los datos de la ram
extern uint8_t *dataPointer;	// Apuntador para seleccionar los datos en ram que seran grabados


extern uint16_t  cntLogger_H;		//	contador en segundos para loggear datos
extern uint8_t	 cntBlockFlash;		//	contador de bloques de Flash grabados (con bloques de 128bytes se pueden grabar hasta 32k de memoria)
extern uint8_t   cntByteBlock;		//	contador de bytes grabados en buffer de datos
extern uint8_t	 cntByteLogg;		//	contador de bytes grabados en cada loggeo

extern uint8_t	numBlock;	// número de bloques a grabar
extern uint8_t  numByte;	// número de bytes a cargar
extern uint8_t  *dirLogger;	//(uint16_t) dirección de inicio del logger a grabar
extern uint8_t  *dirBuffer;	//(uint16_t) dirección de inicio del buffer a cargar
extern uint8_t  *dirDataLoad;	// dirección de inicio del buffer a cargar


extern uint8_t cntByteBlockDATA;	// almacena los bytes cargados al bloque de DATOS
extern uint8_t cntBlockDATA;		// almacena los bloques de DATOS guardados
extern uint8_t cntByteBlockEVENT;	// almacena los bytes cargados al bloque de EVENTOS
extern uint8_t cntBlockEVENT;	// almacena los bloques de EVENTOS guardados
extern uint16_t timeSeconds_HW;	//
extern uint16_t timeSeconds_LW;	// variable de tiempo UNIX 1970

extern uint64_t bufferPageDATA[256];		// CGM 16/04/2025;	Buffer para el almacenamiento temporal de una Pagina de Logger Datos
extern uint64_t bufferPageEVENT[256];		// CGM 16/04/2025;	Buffer para el almacenamiento temporal de una Pagina de Logger Eventos
extern uint64_t * dirBufferPage;			// CGM 16/04/2025; 	Apuntador para los buffers bufferPageDATA y/o bufferPageEVENT


// Bloque de data logger
extern uint8_t BloqDatalooger [9];
enum parametros_Bloque_Data_Logger {
	timeLogg_4,timeLogg_3,				//@near uint16_t timeSeconds_HW	= 0;	//
	timeLogg_2,timeLogg_1,				//@near uint16_t timeSeconds_LW	= 0;	// variable de tiempo UNIX 1970
	tempAmbLogg_H,tempAmbLogg_L,	//@near uint16_t tempAmbLogg		= 0;	// Copia de temperatura ambiente para cargar al buffer de datos
	tempEvaLogg_H,tempEvaLogg_L,	//@near uint16_t tempEvaLogg		= 0;	// Copia de temperatura evaporador para cargar al buffer de datos
	voltLogg							//@near uint8_t voltLogg				= 0;	// voltaje de AC
// Fin del bloque de data logger
};


extern _Bool actuadores_TD[8];
extern uint8_t estados_TD;
extern _Bool alarmas2_TD[8]; // uint8_t alarmas2_TD;
extern uint8_t alarmas_TD;

// Bloque Header
extern uint8_t Bloque_Header [size_Header];
enum parametrosBloque_Header  {
	softVersion1,				//uint8_t softVersion1		= 0;	// versión del software
	softVersion2,				//uint8_t softVersion2		= 0;	// versión del software
	bufferSize_4,				//uint16_t bufferSize_HW	= 0;	//
	bufferSize_3,				//uint16_t bufferSize_HW	= 0;	//
	bufferSize_2,				//uint16_t bufferSize_LW	= 0;	// número de registros (bloques) en el buffer
	bufferSize_1,				//uint16_t bufferSize_LW	= 0;	// número de registros (bloques) en el buffer
	dataType,					//uint8_t dataType				= 0;	// tipo de datos en el Buffer
	dataSize					//uint8_t dataSize				= 0;	// Tamaño de cada registro (bloque) en el buffer
};
// Fin del bloque Header

// Bloque handshake
extern uint8_t Bloque_handshake [size_handshake];
enum parametrosBloque_handshake  {
	comando1,																		//uint8_t comando1			= 0;	// 0xF1
	comando2,																		//uint8_t comando2			= 0;	// 0x3F
	macAdress_11,macAdress_10,macAdress_9,macAdress_8,macAdress_7,macAdress_6,		//uint8_t macAdress[12]	= {0};
	macAdress_5,macAdress_4,macAdress_3,macAdress_2,macAdress_1,macAdress_0,
	model,																			//uint8_t model				= 0;	// Modelo del dispositivo
	versionSoft1,																	//uint8_t versionSoft1		= 0;	// Versión del software entero
	versionSoft2,																	//uint8_t versionSoft2		= 0;	// Versión del software	fracción
	plantParam1,																	//uint8_t plantParam1			= 0;	// Numero de plantilla de parámetros parte alta
	plantParam2,																	//uint8_t plantParam2			= 0;	// Numero de plantilla de parámetros parte baja
	checkList1,																		//uint8_t checkList1			= 0;
	checkList2																		//uint8_t checkList2			= 0;
};
// Fin del bloque handshake

// Bloque de estado en tiempo real
extern uint8_t Bloque_TiempoReal [size_TiempoReal];
enum parametrosBloque_TiempoReal  {
	tempAmb_RT_H,tempAmb_RT_L,			//uint16_t tempAmb_RT			= 0;	// Copia de temperatura ambiente
	tempEva_RT_H,tempEva_RT_L,			//uint16_t tempEva_RT			= 0;	// Copia de temperatura evaporador
	tempSal_RT_H,tempSal_RT_L,			//uint16_t tempSal_RT			= 0;	// Copia de temperatura de salida
	volt_RT,							//uint8_t volt_RT				= 0;	// voltaje de AC
	actuadores_RT,						//uint8_t actuadores_RT		= 0;	// Registro de alarmas en tiempo real
	alarmas2_RT,						//uint8_t alarmas2_RT			= 0;	// Registro de alarma en tiempo real
	alarmas_RT,							//uint8_t alarmas_RT			= 0;	// Registro de alarma en tiempo real
	corriente_RT_H, corriente_RT_L		//uint16_t corriente_RT			= 0;	// Copia de corriente
};
// Fin del bloque de estado en tiempo real


// Flags:  flagsLogger		[8] = {0};	// banderas para logger
extern _Bool 	flagsLogger	[8];	// banderas para logger
//		0				// se alcanzó el limite de logger ?
//		1				// / se llenó al menos una vez el DATA logger ? .. verificado
//		2				// / se llenó al menos una vez el EVENT logger ? .. verificado
// 	 	4				// permite loggeo de eventos  ..verificado
// 	 	5				// permite loggeo de datos	.. verificado
// 	 	7				// ?????
extern uint8_t 	*pointInitTx;	// apunta inicio de la Flash a transmitir
extern uint8_t 	*pointEndTx;	// apunta final de la Flash a transmitir
extern uint8_t *pointTx;	// apuntador de datos a transmitir
extern uint8_t 	cntByteTx;	// contador de bytes cargados para transmisión

extern uint8_t 	keyTx;	//

extern uint8_t cntInitTx;	//

// Flags:  uint8_t flagsTX					= 0;	// banderas para transmisión
extern  _Bool  	flagsTX [8];

extern uint8_t 	codeTX;	//
extern uint8_t 	*loggerStart;	// Dirección de inicio de una sección de logger
extern uint8_t 	*loggerEnd;	// Dirección de fin de una sección de logger
extern uint8_t	blockSizeTX;	// Tamaño del bloque de transmisión
extern uint8_t	offsetInitTX;	// Offset del puntero de inicio en caso de ser necesario
extern uint8_t	sizeTX;	// Tamaño de transmisión
extern uint32_t	chksum_32_HW_LW;	// chksum para comunicaciones
extern uint8_t	ImberaProtocolBuffer[144];


extern  _Bool  	flagsEvent [8];

extern uint16_t numBloqFirm;
extern uint16_t cntBloqFirm;	// contador de bloques de 128 bytes que componen el firmware
//extern uint16_t chksumFirm_HW;	//
//extern uint16_t chksumFirm_LW;	// chksum para comunicaciones
extern uint32_t chksumFirm_HW_LW;

// banderaas asociadas a flagsEvent
// 	flagsEvent		3			; Indica inicio de evento power-on
// 	flagsEvent		6			; indica que no se ha configurado reloj
// 	flagsEvent		7			; inidca que no se ha cerrado evento de power on


extern uint16_t temp_doorEvent;	// Registro para contar duración minima de apertura de puerta


// Flags:  uint8_t flagsRxFirm			= 0;	// banderas para recepción de Firmware
extern _Bool  	flagsRxFirm	[8];			// banderas para recepción de Firmware
extern uint8_t keyWrFirm;	//
extern uint8_t timeOutRst;	//
extern uint8_t timeOutRx;	//


extern uint8_t 	timeUnlockWIFI;

extern	uint8_t		compState;


//;Definición de bits variable numSens
#define		f_sen1		0 //;f_sen1:		equ 0
#define		f_sen2		1 //;f_sen2:		equ 1
#define		f_sen3	 	2 //;f_sen3:		equ 2

#define		f_senCo	 	7		//f_senCo:		equ 7


extern uint8_t 	timeRstBLE;	//


// Bloque de evento puerta
extern uint8_t BloqEventPuerta [18];
enum parametros_Bloque_Ev_Puerta  {
	comandoEP_2, comandoEP_1,			//uint16_t comandoEP = 0;
	softVersion1EP,						//uint8_t softVersion2EP = 0;	// versión del software
	softVersion2EP,						//uint8_t softVersion1EP = 0;	// versión del software
	EP_timeInit_4,EP_timeInit_3,		//uint16_t EP_timeInit_HW		= 0;	//
	EP_timeInit_2,EP_timeInit_1,		//uint16_t EP_timeInit_LW		= 0;	// tiempo de inicio del evento
	EP_timeEnd_4,EP_timeEnd_3,			//uint16_t EP_timeEnd_HW		= 0;	//
	EP_timeEnd_2,EP_timeEnd_1,			//uint16_t EP_timeEnd_LW		= 0;	// tiempo final del evento
	EP_eventType,						//uint8_t  EP_eventType			= 0;	// tipo de evento
	EP_tempAmbInit_H,EP_tempAmbInit_L,	//uint16_t EP_tempAmbInit		= 0;	// Copia de temperatura ambiente
	EP_tempEvaEnd_H,EP_tempEvaEnd_L,	//uint16_t EP_tempEvaEnd		= 0;	// Copia de temperatura evaporador
	EP_voltInit							//uint8_t  EP_voltInit				= 0;	// voltaje de AC
};

// Bloque de evento compresor
extern uint8_t BloqEventComp [18];
enum parametros_Bloque_Ev_Compresor  {
	comandoEC_2, comandoEC_1,			//uint16_t comandoEC = 0;
	softVersion1EC,						//softVersion1EC = 0;	// versión del software
	softVersion2EC,						// softVersion2EC = 0;	// versión del software
	EC_timeInit_4,EC_timeInit_3,		//uint16_t EC_timeInit_HW		= 0;	//
	EC_timeInit_2,EC_timeInit_1,		//uint16_t EC_timeInit_LW		= 0;	// tiempo de inicio del evento
	EC_timeEnd_4,EC_timeEnd_3,			//uint16_t EC_timeEnd_HW		= 0;	//
	EC_timeEnd_2,EC_timeEnd_1,			//uint16_t EC_timeEnd_LW		= 0;	// tiempo final del evento
	EC_eventType,						//uint8_t  EC_eventType			= 0;	// tipo de evento
	EC_tempAmbInit_H,EC_tempAmbInit_L,	//uint16_t EC_tempAmbInit		= 0;	// Copia de temperatura ambiente
	EC_tempEvaEnd_H,EC_tempEvaEnd_L,	//uint16_t EC_tempEvaEnd		= 0;	// Copia de temperatura evaporador
	EC_voltInit							//uint8_t  EC_voltInit				= 0;	// voltaje de AC
};

// Bloque de evento deshielo
extern uint8_t BloqEventDesh [18];
enum parametros_Bloque_Ev_Deshielo  {
	comandoED_2, comandoED_1,			//uint16_t comandoED = 0;
	softVersion1ED,						//uint8_t softVersion1ED = 0;	// versión del software
	softVersion2ED,						//uint8_t softVersion2ED = 0;	// versión del software
	ED_timeInit_4,ED_timeInit_3,		//uint16_t ED_timeInit_HW		= 0;	//
	ED_timeInit_2,ED_timeInit_1,		//uint16_t ED_timeInit_LW		= 0;	// tiempo de inicio del evento
	ED_timeEnd_4,ED_timeEnd_3,			//uint16_t ED_timeEnd_HW		= 0;	//
	ED_timeEnd_2,ED_timeEnd_1,			//uint16_t ED_timeEnd_LW		= 0;	// tiempo final del evento
	ED_eventType,						//uint8_t ED_eventType			= 0;	// tipo de evento
	ED_tempAmbInit_H,ED_tempAmbInit_L,	//uint16_t ED_tempAmbInit		= 0;	// Copia de temperatura ambiente
	ED_tempEvaEnd_H,ED_tempEvaEnd_L,	//uint16_t ED_tempEvaEnd		= 0;	// Copia de temperatura evaporador
	ED_voltInit							//uint8_t ED_voltInit				= 0;	// voltaje de AC
// Fin del bloque de evento deshielo
};


// Bloque de evento power-on
extern uint8_t BloqEventPwrOn [18];
enum parametros_Bloque_Ev_PowerOn  {
	comandoEPo_2,comandoEPo_1,			//uint16_t comandoEPo = 0;	//
	softVersion1EPo,					//softVersion1EPo
	softVersion2EPo,					//softVersion2EPo
	EPo_timeInit_4,EPo_timeInit_3,		//@near uint16_t EPo_timeInit_HW	= 0;	//
	EPo_timeInit_2,EPo_timeInit_1,		//@near uint16_t EPo_timeInit_LW	= 0;	// tiempo de inicio del evento
	EPo_timeEnd_4,EPo_timeEnd_3,		//@near uint16_t EPo_timeEnd_HW		= 0;	//
	EPo_timeEnd_2,EPo_timeEnd_1,		//@near uint16_t EPo_timeEnd_LW		= 0;	// tiempo final del evento
	EPo_eventType,						//@near uint8_t EPo_eventType			= 0;	// tipo de evento
	EPo_tempAmbInit_H,EPo_tempAmbInit_L,//@near uint16_t EPo_tempAmbInit	= 0;	// Copia de temperatura ambiente
	EPo_tempEvaEnd_H,EPo_tempEvaEnd_L,	//@near uint16_t EPo_tempEvaEnd		= 0;	// Copia de temperatura evaporador
	EPo_voltInit						//@near uint8_t EPo_voltInit			= 0;	// voltaje de AC
// Fin del bloque de evento power-on
};




extern uint8_t timeTxTWF;	//

extern _Bool flagsTX2[8];//extern uint8_t flagsTX2;	// banderas para transmisión
extern uint16_t timeoutTWF;
extern _Bool flagsEventWF[8];		// uint8_t flagsEventWF;
extern uint16_t timeDataWF_H;		//	contador en segundos para loggear datos
extern _Bool 	flagsWIFI[8]; 	 	// banderas Wifi
extern uint8_t bufferWifiTx[30];
extern uint8_t blockLoggWifi;
extern uint8_t byteLoggWifi;
extern uint16_t cntRegTxWifi;

#define 	sizeRegTimeLogg 		9
#define		offsetTimeLogg			126
#define		numBlockDATA			96

#define		f_serverConnect			0
#define		f_timeLoggerSend		1
#define		f_eventLoggerSend		2
#define		f_timeLoggerCmd			3
#define		f_eventLoggerCmd		4
//f_timeLoggerAns:			equ		 5
//f_eventLoggerAns:			equ		 6

#define		sizeRegEventLogg		14
#define		offsetEventLogg			126
#define		numBlockEVENT			160

extern uint8_t timeoutTxWifi;
extern uint8_t delayTxLoggWifi;


// Flags:  uint8_t flagsLogger2			= 0;	// banderas para logger
extern _Bool  	flagsLogger2 [8];	// banderas para logger
//			0			/ hay fallo de energía ?
//			1			se ejecuto el grabado de emergencia ?
//			2			se detecto falla de AC?



extern uint16_t 	cnt_gen_ms;					//tiempo de buzzer

extern uint16_t t_buzzer;

extern uint8_t edo_buzzer;
extern uint16_t buzzer_ton;
extern uint16_t buzzer_toff;


extern _Bool ban_buzzer[8];// uint8_t ban_buzzer;
// banderaas asociadas a ban_buzzer    .... RGM???

extern _Bool flagsBuzzer[8];
// banderaas asociadas a flagsBuzzer    .... RGM???

extern uint16_t 	 silencioAlarmH;

extern _Bool 		flagsTC[8]; // uint8_t flagsTC;
// banderaas asociadas a flagsTC
#define		f_TC1			0
#define		f_TC2			1
#define		f_TC3			2

//;Definición de bits de estado a logicos2
#define		doorOFF			0//;		Bandera para indicar que la puerta está inhabilitada (incluidas funciones asociadas) (programa mantien puerta cerrada y cancela funciones que se activan con puerta cerrada)
#define		ventDoorOFF		1//;		Bandera que indica que el ventilador ignora señal de puerta como entrada de control.


#define		modLogic		4//		modLogic:				equ		 4//; Cambia comportamiento en ciertos aspectos de CTOF



#define		funReleDesh		3//; Cambia funcion de relevador cerradura a deshielo en equipo Health
#define		frecAC			7//;		Seleccion de frecuencia de alimentación de AC 0 = 60hz, 1 = 50 hz

extern uint8_t bufferTxControl[144];

extern uint16_t numRegTx;
extern 	uint16_t cntRegDATA	;	//
extern 	uint16_t cntRegEVENT;	//
extern 	uint16_t 	cntReg;		//
extern 	uint16_t 	*cntRegPNT;	//

extern _Bool  	estado1 [8];
extern _Bool  	estado2 [8];
extern _Bool  	estado3 [8];
extern _Bool  	estado4 [8];
//extern 	uint8_t		estado1;
//extern uint8_t		estado3;
// banderaas asociadas a estado1
#define		est1Refri		 0		//est1Refri:		equ		 0//;		estado de refrigerador
#define		est1Noct		 1		//est1Noct:			equ		 1//;		nocturno
#define		est1Lamp		 2		//est1Lamp:			equ		 2//;		lampara
#define		est1Ahorro2		 3		//est1Ahorro2:	equ		 3//;		modo ahorro 2
#define		est1Reset		 4		//est1Reset:		equ		 4//;		Reset general del proyecto
#define		est1Snooze		 5		//est1Snooze:		equ		 5//;		silencio de buzer temporal
#define		est1LockDr		 6		//est1LockDr:		equ		 6//;		ontrol de cerradura
#define     est1WifiReset    7

extern uint8_t		cntMsgCmd;
extern uint8_t		numMsg;
extern uint8_t		retLampOff;


extern  _Bool  	flagsTime [8];	// banderas para tiempo
//;Bits flagsTime
#define f_timeConfigRTC 0

extern uint8_t timeBCD_year;
extern uint8_t timeBCD_month;
extern uint8_t timeBCD_day;
extern uint8_t timeBCD_hour;
extern uint8_t timeBCD_min;
extern uint8_t timeBCD_sec;

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef hRtcTime;
extern RTC_DateTypeDef hRtcDate;

#define		f_timeConfigRTC		 0		//	f_timeConfigRTC:				equ		0//;

extern uint16_t time_year;
extern uint8_t time_month;
extern uint8_t time_day;
extern uint8_t time_hour;
extern uint8_t time_min;
extern uint8_t time_sec;

extern uint32_t timeUNIX;
extern uint32_t timeUNIX_copy;
//@near uint32_t daysToMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
extern uint32_t daysToMonth[12];
extern uint8_t leapYears;

extern uint8_t timeDpyDf;

extern uint8_t timeBuzzOn;
extern uint8_t retPowerOn;

extern uint16_t	tsac_w;
extern uint16_t	tDisplay_w;


extern uint8_t debBtn2F3;

extern uint16_t	potencia;

extern uint16_t		timeOnVaho_w;			//@near uint16_t timeOnVaho_w = 0;
extern uint16_t		timeOffVaho_w;			//@near uint16_t timeOffVaho_w = 0;
extern uint8_t 		flagsVaho[8];			//@near uint8_t flagsVaho			= 0;
extern uint16_t 	timeAlarmRetCo_w;		//@near uint16_t timeAlarmRetCo_w = 0;
extern uint8_t 		cntRetCo;				//@near uint8_t cntRetCo			= 0;


extern _Bool  	flagsBattery	[8];
//extern  uint8_t flagsBattery;
// banderas de bateria
#define		batON			 0		// batON:			equ		 0//;		El sitema está alimentado por la batería
#define		batLOW			 1		// batLOW:			equ		 1//;		batería baja
#define		batOFF			 2		// batOFF:			equ		 2//;		batería descargada

extern uint8_t cntHoldP;
extern uint8_t typeClock;

extern _Bool 	fcom[8];
extern uint16_t timeClearComst;
extern uint16_t timeClearComst1;     		//RM_20240425 Segundo timer para estado de comunicación MODBUS


extern _Bool flagsTxControl[8];

//;Definición de bits banderas de control para transmision con modulo wifi BLE
#define	f_select 		0//;			Bandera para seleccionar BLE = 0 o WiFi = 1
#define	f_statBLE		1//;			BLE status
#define	f_statWIFI		2//;
#define f_auxEventWIFI	7

extern uint8_t delayComStat;
extern uint8_t DevLock;
extern uint8_t statComFlag;
extern uint8_t statComWIFIFlag;
extern uint16_t cntSetName;
extern uint8_t difName[50];
extern uint8_t timeTxTBLE;	//
extern uint16_t timeoutTBLE;

//extern uint8_t timePreDh_h;
//extern uint8_t timePreDh_l;

// Bloque de evento WiFi EX
extern uint8_t BloqEventWiFiEx [18];
enum parametros_Bloque_Ev_Wifi_Ex  {
	comandoWF_2,comandoWF_1,			//
	softVersion1WF,						// versión del software
	softVersion2WF,						// versión del software
	WF_timeInit_4,WF_timeInit_3,		// tiempo de inicio del evento
	WF_timeInit_2,WF_timeInit_1,		//
	WF_timeEnd_4,WF_timeEnd_3,			// tiempo final del evento
	WF_timeEnd_2,WF_timeEnd_1,			//
	WF_eventType,						// tipo de evento
	WF_tempAmbInit_H,WF_tempAmbInit_L,	// Copia de temperatura ambiente
	WF_tempEvaEnd_H,WF_tempEvaEnd_L,	// Copia de temperatura evaporador
	WF_voltInit							// voltaje de AC
// Fin del bloque de evento WiFi
};

extern char directorioMQTT[17];

extern uint8_t temp_wifiEvent;		//@near uint8_t temp_wifiEvent = 0;


//extern uint16_t comandoWF;	//
//extern uint8_t softVersion1WF;	// versión del software
//extern uint8_t softVersion2WF;	// versión del software
//// Bloque de evento WiFi
//extern uint16_t WF_timeInit_HW;	//
//extern uint16_t WF_timeInit_LW;	// tiempo de inicio del evento
//extern uint16_t WF_timeEnd_HW;	//
//extern uint16_t WF_timeEnd_LW;	// tiempo final del evento
//extern uint8_t WF_eventType;	// tipo de evento
//extern uint16_t WF_tempAmbInit;	// Copia de temperatura ambiente
//extern uint16_t WF_tempEvaEnd;	// Copia de temperatura evaporador
//extern uint8_t WF_voltInit;	// voltaje de AC



extern uint8_t   t_filtro_flanco;       //RM_20240530 Para agregar el filtro de medición de voltaje

extern uint8_t timeBCD_sec_ANT;
extern uint8_t tiempoPrCargas;      //RM_20240819 Para el comando de prueba de cargas

//;Banderas para activar buzzer por alarma (Registro flagsBuzz)
#define	bz_h1		0			//bz_h1:		equ		 0//;		Activa Buzzer para alarma H1
#define	bz_h2		1			//bz_h2:		equ		 1//;		Activa Buzzer para alarma H2
#define	bz_h3		2			//bz_h3:		equ		 2//;		Activa Buzzer para alarma H3
#define	bz_t		3			//bz_t:			equ		 3//;		Activa Buzzer para alarma t
#define	bz_f		4			//bz_f:			equ		 4//;		Activa Buzzer para alarma F
#define	bz_p		5			//bz_p:			equ		 5//;		Activa Buzzer para alarma P
#define	bz_a		6			//bz_a:			equ		 6//;		Activa Buzzer para alarma A
#define	bz_c		7			//bz_c:			equ		 7//;		Activa Buzzer para alarma C

//;Banderas para activar buzzer por alarma (Registro flagsBuzz2)
#define	bz_u		0			//bz_u:			equ		 0//;		Activa Buzzer para alarma U
#define	bz_mu		1			//bz_mu:		equ		 1//;		Activa Buzzer para alarma -U
#define	bz_h4		2			//bz_h4:		equ		 2//;		Activa Buzzer para alarma H4 sensor 4

#define	bz_in		5			//bz_in:		equ		 5//;		Activa Buzzer para alarma In falla de inyeccion
#define	bz_co		6			//bz_co:		equ		 6//;		Activa Buzzer para alarma CO falla de compresor por corriente
#define	bz_ba		7			//bz_ba:		equ		 7//;		Activa Buzzer para alarma Bateria

#define f_buzzDpy	0			//;		Bandera para indicar el estado del buzzer al display


extern uint16_t timeDpyS3;			//@near uint16_t timeDpyS3		= 0;


// para reservar memoria Flash dedicada a Logger
//#pragma section @near {eventLogger}
extern __attribute__((section(".eventLogger"))) uint8_t eventLogger[20480-1];
extern __attribute__((section(".eventLogger"))) uint8_t eventLoggerFin;

//#pragma section @near {dataLogger}
extern __attribute__((section(".dataLogger"))) uint8_t dataLogger[12288-1];
extern __attribute__((section(".dataLogger"))) uint8_t dataLoggerFin;


// #pragma section @near {varFlash}
// Nota: Esta seccion debe ir en Flash
extern uint8_t versionFirm1;
extern uint8_t versionFirm2;
//@near uint8_t versionFirm2 = 02;  //RM_20230908 VFW 0.002 Ajuste en calibración y envio de MAC a llave
extern uint8_t   fm_hardware;
extern uint8_t	  fm_modelo0;  //'E'
extern uint8_t	  fm_modelo;  //'8'

extern uint8_t		countResetI2C;

#define	sizeMPx		4
#define sizePx		5

extern GPIO_TypeDef * PFULLDEF_Px_PORT[5];
extern uint16_t PFULLDEF_Px_PIN[5];

extern GPIO_TypeDef * PFULLDEF_MPx_PORT[4];
extern uint16_t PFULLDEF_MPx_PIN[4];

extern _Bool	FlagPuertaX [sizeMPx*sizePx]; // FlagPuertaX [Numero de Puertos MP][Numero de Puertos P] CGM 13/01/2025
/*					   __
 *					  |			  [P1]	  [P2]	  [P3]	  [P4]	  [P5]
 *					  |	[MP0]	| Door6 | Door12| Door18| DoorX | DoorX |
 * 	FlagPuertaX =	< |	[MP1]	| Door1 | Door2 | Door3 | Door4 | Door5 |
 * 					  |	[MP2]	| Door7 | Door8 | Door9 | Door10| Door11|
 * 					  |	[MP3]	| Door13| Door14| Door15| Door16| Door17|
 *					  |__
 *
 */
extern uint8_t	FlagPuerta10Times [sizeMPx*sizePx];
extern uint8_t countWaitPuerta;
extern uint8_t countMPx;
extern _Bool firstFlagPuerta1;


extern void SystemClock_Config(void);


//extern _Bool bandera_RTC;
extern void  reconfigura_perif(void);
extern _Bool bandera_act_fw_j;
extern void  manda_transmision (void);
extern void  datled_clear();
extern void  op_menu (uint8_t dig1, uint8_t dig2);
extern void  Bclear_Clear_trfst(uint8_t * trfst_3, uint8_t * trfst_4,uint8_t V, uint8_t W);
extern void  Bset_Clear_trfst(uint8_t * trfst_1, uint8_t * trfst_2,uint8_t X, uint8_t Y);
extern void  grabacion_exitosa_handshake (void);
extern uint16_t   timeoutRXFw;
extern void reinicio_valores_act_fw(void);
extern void clean_logger(void);



