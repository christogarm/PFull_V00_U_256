/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "board_PFULLDEF.h"
#include "bluetooth.h"
#include "customMain.h"
#include "ModbusRTU.h"
#include "stm32u0xx_hal_adc_ex.h"
#include "ELTEC_EmulatedEEPROM.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart4_rx;
DMA_HandleTypeDef hdma_usart4_tx;

/* USER CODE BEGIN PV */
uint8_t    STM8_A  = 0;					// Simul A 8 bit Register
uint16_t   STM8_16_X = 0;				// Simul X 16 bit Register

//User Private Variables
_Bool  tick_1ms = 0;					// flag base de tiempo 1ms
_Bool  tick_10ms = 0;					// flag base de tiempo 10ms
_Bool  tick_100ms = 0;					// flag base de tiempo 100ms
_Bool  tick_1s = 0;						// flag base de tiempo 1s

_Bool  device_conected = 0;				// bandera dispositivo conectado

uint8_t BluetoothState = 1;				// Maquina de estados Bluetooth 1:Configuracion 2:Obtencion parametros 3:Tx/RX

_Bool  TR485_Trasnmiting = 0;			//17-DIC-2021		El dispositivo inicialmente escucha

unsigned long	timemilis = 0;			// variable contadora de milisegundos
int presc_tick_1ms = 0;			// contador prescala 1ms
int presc_tick_10ms = 0;		// contador prescala 10ms
int presc_tick_100ms = 0;		// contador prescala 100ms
int presc_tick_1s = 0;			// contador prescala 1s

uint8_t		ProcesosC =0 ;							// Procesos C				  ;// 10-FEB-2022	 Contador de procesos para C

uint8_t		StateSleep =0 ;	 // MRG_2023-JUL-26



//LN_97:	#pragma space extern [] @near

uint8_t TxBuffer_Ble[DATA_TO_TRANSFER_USART2] = {0};      // Manuel, han de ser como 80 caracteres
uint8_t RxBuffer_Ble[DATA_TO_RECEIVE_USART2] = {0};

uint8_t	SerialAnswBLE[DATA_TO_RECEIVE_USART2] = "";


uint8_t TxBuffer_RS485[DATA_TO_TRANSFER_USART4] = {0};      // Manuel, han de ser como 80 caracteres
uint8_t RxBuffer_RS485[DATA_TO_RECEIVE_USART4] = {0};
uint8_t SerialAnsModBus[DATA_TO_RECEIVE_USART4] = {0};

uint16_t Eventos[14] = {0X1111, 0X1222, 0X1333, 0X1444, 0X1555, 0X1666, 0X1777 };				// Aray para Eventos
uint16_t Mediciones[4] = {0X2111, 0X1222, 0X1333};			// Aray para Mediciones, Manuel 06-dic-2021
uint16_t Alarmas[9] = {0X3111, 0X3222, 0X3333, 0X3444, 0X3555, 0X3666, 0X3777, 0X3888};				// Aray para Alarmas, Manuel 06-dic-2021
uint16_t DatosMAC[3] = {0X1234, 0X5678, 0X9999};				// Aray para Datos MAC, Manuel 06-dic-2021
uint16_t DatosFirmware[1] = {0X0100};	// DatosFirmware, Manuel 06-dic-2021: BCD = 0.1.0.0

uint16_t Count_Test = 0;	// 17-dic-2021, contador de prueba


uint16_t PNU_0x3000 = 1;						// 14-Ene-2022:  ID del cliente del refrigerador
uint16_t PNU_0x3002 = 0;						// 14-Ene-2022:  Unidad de temperatura  C=0 y F=1
uint16_t PNU_0x3004 = 40;						// 14-Ene-2022:  Temperatura Setpoint MODO NORMAL
uint16_t PNU_0x3006 = 10;						// 14-Ene-2022:  Temperatura Diferencial  MODO NORMAL
uint16_t PNU_0x3008 = 10;						// 14-Ene-2022:  MODO NOCTURNO: duración en mins
uint16_t PNU_0x300A = 0;						// 14-Ene-2022:  Consumo por variable
uint16_t PNU_0x300C = 3;						// 14-Ene-2022:  Compresor: tiempo minimo de apagado en mins
uint16_t PNU_0x300E = 0;						// 14-Ene-2022:  Compresor: tiempo minimo de encendido en mins
uint16_t PNU_0x3010 = 1;						// 14-Ene-2022:  Deshielo Programa 1: habilitación de 3 o 5 fases
uint16_t PNU_0x3012 = 40;						// 14-Ene-2022:  Deshielo Programa 1: duración fase 1 en mins
uint16_t PNU_0x3014 = 3;						// 14-Ene-2022:	 Deshielo Programa 1: duración fase 2 en mins
uint16_t PNU_0x3016 = 5;						// 14-Ene-2022:	 Deshielo Programa 1: duración fase 3 en mins
uint16_t PNU_0x3018 = 240;						// 14-Ene-2022:  Deshielo Programa 1: periodo en mins para iniciar deshielo
uint16_t PNU_0x301A = 50;						// 14-Ene-2022:	 Deshielo Programa 1: temperatura para inicio de periodo de deshieloDeshielo Programa 1: temperatura para inicio de periodo de deshielo
uint16_t PNU_0x301C = 90;						// 14-Ene-2022:	 Deshielo Programa 1: temperatura para salir de deshielo  y verifica si entra a deshielo
uint16_t PNU_0x301E = 222;						// 14-Ene-2022:	 Modbus: dirección de la CIR
//----------------------------------------------------------
uint16_t PNU_0x3104 = 5;						// 14-Ene-2022:		Alarma puerta abierta: tiempo de activación en mins
uint16_t PNU_0x3106 = -70;						// 14-Ene-2022:		Alarma Congelamiento Programa 1: temperatura para entrar en alarma
uint16_t PNU_0x3108 = 150;						// 14-Ene-2022:		Alarma U 110V: voltaje para entrar en alarma
uint16_t PNU_0x310A = 145;						// 14-Ene-2022:		Alarma U 110V: voltaje para salir de alarma
uint16_t PNU_0x310C = 95;						// 14-Ene-2022:		Alarma -U 110V: voltaje para entrar en alarma
uint16_t PNU_0x310E = 100;						// 14-Ene-2022:		Alarma -U 110V: voltaje para salir de alarma
//----------------Solo Escritura--------------------------
uint16_t PNU_0x3200 = 0;						// 14-Ene-2022:		Comando activar Modo Nocturno
uint16_t PNU_0x3202 = 0;						// 14-Ene-2022:		Comando activar MODO DE AHORRO ES2
uint16_t PNU_0x3204 = 0;						// 14-Ene-2022:		Comando activar MODO VAULT
uint16_t PNU_0x3206 = 0;						// 14-Ene-2022:		Comando desactivar MODO VAULT
uint16_t PNU_0x3208 = 0;						// 14-Ene-2022:		Comando activar MODO Servicio
uint16_t PNU_0x320A = 0;						// 14-Ene-2022:		Comando Ventilador siempre ON con compresor OFF
uint16_t PNU_0x320C = 0;						// 14-Ene-2022:		Comando Ventilador activar ciclos ON-OFF con compresor OFF
// ;------------------------------  Variables utilizadas en el PROCESO COMUNICACIÓN


//METERLO AQU??
//VARIABLES PARA MEDICI�N DE VOLTAJE RM_20220418
//uint8_t s_reloj = 0;        //Frecuencia de la l�nea
_Bool s_reloj[3] = {0};
uint8_t cnt_mues = 0;           //Contador de muestras

uint32_t sigma_cuad_sampl = 0; //valor de sampling sumado
uint32_t level_4st_mult = 0;

uint8_t  vl_ram [32] = {0};
uint8_t  cnt_veces_muestreo =	0;
uint8_t  volt_trms = 0;
uint8_t  edo_muestreo = 0;


//---------------------------

//LN_196:	#pragma section  {asmpt44_0}
//uint32_t ContadorDebugger=0;

uint8_t		resulh = 0;					//08/FEB/2022		DS.B 1      	;	/ch man
uint8_t		resull = 0;					//08/FEB/2022		DS.B 1      	;	/ch man Resultado de operaciones varias
uint8_t		cntproc = 0;				//01/MAR/2022		DS.B 1      	;	/ Contador de procesos

uint8_t		cntcent = 0;				//08/FEB/2022		DS.B 1      	;	/ Contador de tiempo "Centésimas"
uint8_t		cntseg = 0;					//08/FEB/2022		DS.B 1      	;	/ Contador de tiempo "Segundos"

uint16_t	cntblkh = 0;				//08/FEB/2022		DS.B 1      	; ch_man   / Contador para parpadeos
uint8_t		cntblkl = 0;				//08/FEB/2022		DS.B 1      	; ch_man   / Contador para parpadeos

uint8_t		tempo2 = 0;					//08/FEB/2022		DS.B 1      	;	/ch man Registro 2 temporal para operaciones varias
uint8_t		tempo1 = 0;					//08/FEB/2022		DS.B 1     	;	/ch man Registro 1 temporal para operaciones varias

uint8_t		datdig2 = 0;				//08/FEB/2022		DS.B 1
uint8_t		datdig1 = 0;				//08/FEB/2022		DS.B 1      	;	/ Registro de datos para el dígito 1
uint8_t		modo = 0;						//08/FEB/2022		DS.B 1      	;	/ Modo de operación para el enfriador


uint8_t		holdseg = 0;				//08/FEB/2022		DS.B 1      	;	/ Registro de conteo de segundos para la retención de lectura

uint8_t		waux = 0;						//08/FEB/2022		DS.B 1      	;	/ch_man Registro auxiliar de trabajo
uint8_t		wreg = 0;						//08/FEB/2022		DS.B 1      	;	/ch_man Registro de trabajo
//uint8_t		puertoa = 0;			//08/FEB/2022		DS.B 1      	;	/ Registro imagen para salida del puerto A
//uint8_t		puertob;				//2/MAR/2022		DS.B 1      	;	/ Registro imagen para salida del puerto B
//uint8_t		puertoc = 0;			//08/FEB/2022		DS.B 1      	;	/ Registro imagen para salida del puerto C
//uint8_t		puertod = 0;			//08/FEB/2022		DS.B 1      	;	/ Registro imagen para salida del puerto D
//uint8_t		puertoe;				//2/MAR/2022		DS.B 1      	;	/ Registro imagen para salida del puerto B
_Bool  		portX [8] = {0};

//uint8_t		adcramh = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		adcraml = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Registro de almacenamiento del dato salido del convertidor
uint16_t		adcramh = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		tminstoph = 0;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		tminstopl = 0;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Tiempo de descanso mínimo del compresor
uint16_t		tminstoph = 0;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;

uint16_t		retnocth = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		cntpah = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		cntpal = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Tiempo de puerta abierta para indicar falla
uint16_t		cntpah = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;


uint16_t		retproth = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;


uint8_t		retvent = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo para el encendido del ventilor
uint8_t		minfunc = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Tiempo mínimo de función activa
uint8_t		durautop = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Duración de autoprueba
uint8_t		cntdpyev = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Tiempo desplegado temperatura de evaporador
uint8_t		cntobscu = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de obscuridad en la fotocelda
uint8_t		cntfail = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador para indicar fallas
uint8_t		valflvl = 0;				//08/FEB/2022		DS.B 1         ;0x00;																	>> IJG AGOSTO 2012
uint8_t		valfhvl = 0;				//08/FEB/2022		DS.B 1         ;0x00;																	>> IJG AGOSTO 2012
uint8_t		cnt_prog = 0;				//08/FEB/2022		DS.B 1
uint8_t		cnt_btn_hld = 0;		//08/FEB/2022		DS.B 1;
uint8_t		ret_fan_co = 0;			//08/FEB/2022		DS.B 1         ;0x00;	Retardo para el encendido del ventilor por compresor recien encendido
uint8_t		End_sec = 0;				//08/FEB/2022		DS.B 1         ;0x00;


// Flags:  uint8_t		flagsa = 0;					//08/FEB/2022		DS.B 1      	;	/ Registro A de banderas para el programa
_Bool  		flagsa [8] = {0};			//08/FEB/2022		DS.B 1      	;	/ Registro A de banderas para el programa
//uint8_t		flagsa = 0;					//08/FEB/2022		DS.B 1      	;	/ Registro A de banderas para el programa


// Flags:  uint8_t		trefst2 = 0;		//08/FEB/2022		DS.B 1
_Bool  		trefst2 [8] = {0};			//08/FEB/2022		DS.B 1
//uint8_t		trefst2 = 0;					//08/FEB/2022		DS.B 1


// Flags:  uint8_t		trefst = 0;			//08/FEB/2022		DS.B 1      	;	/ Registro de indicaci�n de estado del equipo TREF
_Bool  		trefst		[8] = {0};		//08/FEB/2022		DS.B 1      	;	/ Registro de indicaci�n de estado del equipo TREF
//uint8_t		trefst = 0;						//08/FEB/2022		DS.B 1      	;	/ Registro de indicaci�n de estado del equipo TREF


_Bool  		datled  [8] = {0};					//08/FEB/2022		DS.B 1      	;	/ Registro de datos para los leds
//uint8_t		datled = 0;					//08/FEB/2022		DS.B 1      	;	/ Registro de datos para los leds

uint8_t		edorefri = 0;				//08/FEB/2022		DS.B 1      	;	/ Estado del refrigerador

_Bool  		botonst [8] = {0};// uint8_t		botonst = 0;				//08/FEB/2022		DS.B 1      	;	/ Estado actual de las entradas digitales

//;-------	Registro de proposito general
// Flags:  uint8_t		GPIOR0 = 0;				//08/FEB/2022		DS.B 1      	; / Bits de control de las banderas (Registro GPIOR0)
_Bool  		GPIOR0 [8] = {0};	// egistro de proposito general

uint16_t		liminf_w = 0;				//08/FEB/2022		DS.W 1         ;ch_man 0x00;	Valor del límite inferior de control parte entera

uint16_t		limsup_w = 0;				//08/FEB/2022		DS.W 1         ;ch_man 0x00;	Valor del límite superior de control parte entera



uint8_t		tdevl = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del ambiente interno en °C, parte entera
uint8_t		tdevf = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del ambiente interno en °C, parte fraccionaria
uint8_t		teval = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del evaporador en 10 bits, entero
uint8_t		tevaf = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Dato de temperatura del evaporador en 10 bits, fracción
uint16_t	tdevdpy_w = 0;			//08/FEB/2022		DS.W 1         ;ch_man 0x00;



//uint8_t		interdhh = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		interdhl = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Tiempo de interdeshielo
uint16_t		interdhh = 0;				//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		durdhh = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t			durdhl = 0;						//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Tiempo de deshielo
uint16_t		durdhh = 0;					//08/FEB/2022		DS.B 1         ;ch_man 0x00;

//uint8_t		latchtimel = 0;			//08/FEB/2022		DS.B 1         ;0x00;	Retención de tiempo de operación del estado actual
//uint8_t		latchtimeh = 0;			//08/FEB/2022		DS.B 1         ;0x00;
uint16_t		latchtimeh = 0;			//08/FEB/2022		DS.B 1         ;0x00;


//uint8_t		coontimeh = 0;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;
//uint8_t		coontimel = 0;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;	Frontera entre tiempo calendario y tiempo de compresor
uint16_t		coontimeh = 0;			//08/FEB/2022		DS.B 1         ;ch_man 0x00;




uint8_t		cntbase = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de desbordes del timer 0 (cada 125 us)
uint8_t		cnttemp = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de retardo para tomar lecturas de temperatura
uint8_t		cntmues = 0;				//08/FEB/2022		DS.B 1         ;0x00;;	Contador de muestra de temperatura para filtro digital
uint8_t		debdoor = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Debounce para validar puerta abierta
uint8_t		cntdpysp = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de desplegado del set point


uint8_t		ret_up = 0;					//08/FEB/2022		DS.B 1         ;0x00;	Contador de retardo para mostrar la lectura cuando va hacia arriba
uint8_t		retcncfa = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo de cancelación de falla de sensor de ambiente
uint8_t		retcncfe = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo de cancelación de falla de sensor de evaporador
uint8_t		cntsame = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de promedios iguales hacia arriba
uint8_t		cnthold = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de retención de lectura
uint8_t		retnoct = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo para la activación de modo nocturno en automático


uint8_t		retcapfc = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Retardo para captura de señal de fotocelda

uint8_t		luminos = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Dato válido de luminosidad
uint8_t		cnthitemp = 0;			//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo de temperatura alta
uint8_t		cntvolt = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de tiempo para tomar muestra de voltaje
uint8_t		cntmuesv = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Contador de muestras de voltaje

uint8_t		volth = 0;					//08/FEB/2022		DS.B 1         ;0x00;	Resultado de la conversión ADC en medición de voltaje
uint8_t		voltl = 0;					//08/FEB/2022		DS.B 1         ;0x00;

//;Memoria ISAAC
_Bool fbtn_deb[8] = {0};// uint8_t		fbtn_deb = 0;				//08/FEB/2022		DS.B 1


_Bool  		fauto [8] = {0};					//08/FEB/2022		DS.B 1


// Flags:  uint8_t		GPIOR1 = 0;		//08/FEB/2022		DS.B 1      	; / Bits de control de las banderas (Registro GPIOR1)
_Bool  		GPIOR1  [8] = {0};
// Flags:  uint8_t		flagsb = 0;					//08/FEB/2022		DS.B 1      	; / Registro B de banderas para el programa
_Bool  		flagsb  [8] = {0};
//uint8_t		flagsb = 0;					//08/FEB/2022		DS.B 1      	; / Registro B de banderas para el programa

_Bool  		btn_pr  [8] = {0};// uint8_t		btn_pr = 0;					//08/FEB/2022		DS.B 1				; / Copia del resgistro botonst para utilizar en programaci�n de par�metros
uint8_t		ton_fan = 0;				//08/FEB/2022		DS.B 1      	; / Tiempo de encendido ventilador
uint8_t		toff_fan = 0;				//08/FEB/2022		DS.B 1      	; / Tiempo de apagado ventilador

//uint8_t		drp_comph = 0;			//08/FEB/2022		DS.B 1
//uint8_t		drp_compl = 0;			//08/FEB/2022		DS.B 1
uint16_t		drp_comph = 0;			//08/FEB/2022		DS.B 1
//uint8_t		drp_fanh = 0;				//08/FEB/2022		DS.B 1
//uint8_t		drp_fanl = 0;				//08/FEB/2022		DS.B 1
uint16_t		drp_fanh = 0;				//08/FEB/2022		DS.B 1

uint8_t		param2 = 0;					//08/FEB/2022		DS.B 1				; / indice de parametros
uint8_t		param = 0;					//08/FEB/2022		DS.B 1
uint8_t		key = 0;						//08/FEB/2022		DS.B 1

uint16_t	lmt_up_w = 0;		//08/FEB/2022		DS.W 1
uint16_t	lmt_dw_w = 0;		//08/FEB/2022		DS.W 1

_Bool 		Botones_COPY[8] = {0}; //uint8_t		Botones_COPY = 0;		//08/FEB/2022		DS.B 1;
uint8_t		deb2_btn1 = 0;			//08/FEB/2022		DS.B 1;
uint8_t		deb2_btn2 = 0;			//08/FEB/2022		DS.B 1;
uint8_t		deb2_btn3 = 0;			//08/FEB/2022		DS.B 1;
uint8_t		deb2_btn4 = 0;			//08/FEB/2022		DS.B 1;
uint8_t		deb1_btn2 = 0;			//08/FEB/2022		DS.B 1;
uint8_t		deb1_btn3 = 0;			//08/FEB/2022		DS.B 1;
uint8_t		deb_2btn = 0;			//08/FEB/2022		DS.B 1;
_Bool botonst2[8] = {0};// uint8_t		botonst2 = 0;			//08/FEB/2022		DS.B 1;
//....
uint8_t		cnt_ecom = 0;				//08/FEB/2022		DS.B 1;
uint8_t		cnt_dpyrst = 0;			//08/FEB/2022		DS.B 1;
uint8_t		cnt_btn = 0;				//08/FEB/2022		DS.B 1;
uint8_t		volt_max = 0;				//08/FEB/2022		DS.B 1;
uint8_t		volt_min = 0;				//08/FEB/2022		DS.B 1;
uint8_t		volt_hmax = 0;			//08/FEB/2022		DS.B 1;

uint8_t		volt_hmin = 0;			//08/FEB/2022		DS.B 1;

//....
uint8_t		cnt_pta_fan = 0;		//08/FEB/2022		DS.B 1;
uint8_t		durautop2 = 0;			//08/FEB/2022		DS.B 1;

uint8_t		cm_lo50 = 0;				//08/FEB/2022		DS.B 1;
uint8_t		cm_h255 = 0;				//08/FEB/2022		DS.B 1;

//....
uint16_t	cntworkh = 0;				//08/FEB/2022		DS.B 1;
uint16_t	cntexhah = 0;				//08/FEB/2022		DS.B 1;
uint16_t	t_ahorro1_H = 0;		//08/FEB/2022		DS.B 1
uint16_t	t_ahorro2_H = 0;		//08/FEB/2022		DS.B 1


uint8_t		cnt_blk_btn = 0;		//08/FEB/2022		DS.B 1;

//uint8_t		cntDefi_H = 0;
//uint8_t		cntDefi_L = 0;
uint16_t		cntDefi_H = 0;
uint16_t		tempDefi1 = 0;
//uint8_t		cntDefi1_H = 0;
//uint8_t		cntDefi1_L = 0;
uint16_t		cntDefi1_H = 0;


// Flags:  uint8_t		flagsC = 0;					//08/FEB/2022		DS.B 1      	; / Registro C de banderas para el programa
_Bool  		flagsC [8] = {0};			//08/FEB/2022		DS.B 1      	; / Registro C de banderas para el programa
//uint8_t		flagsC = 0;					//08/FEB/2022		DS.B 1      	; / Registro C de banderas para el programa

uint16_t	cntNoct_H = 0;			//08/FEB/2022		DS.B 1

_Bool  		flags_menu [8] = {0};// uint8_t		flags_menu = 0;		//08/FEB/2022		DS.B 1
uint8_t		flags_accMenu = 0;		//08/FEB/2022		DS.B 1
uint8_t		timerOffManto = 0;
uint8_t		retcncfr = 0;
uint8_t		retcncfc = 0;


uint16_t		tret_w = 0;



uint8_t		retzc_ms_compresor;		  //RM_20220913 Contador de retardo al cruce por cero para COMPRESOR
uint8_t		retzc_ms_deshielo;			//RM_20220913 Contador de retardo al cruce por cero para DESHIELO
uint8_t		retzc_ms_ventilador;		//RM_20220913 Contador de retardo al cruce por cero para VENTILADOR
uint8_t		retzc_ms_lampara;				//RM_20220913 Contador de retardo al cruce por cero para L??MPARA
_Bool cruze_por_cero [5] = {0}; // uint8_t		cruze_por_cero;  				//RM_20220913 Banderas para cruce por cero
//  		cruze_por_cero_Bit_0 = 0;			//Detectar cruce por cero
//  		cruze_por_cero_Bit_1 = 0;			//COMPRESOR por cruece por cero
//  		cruze_por_cero_Bit_2 = 0;			//DESHILEO por cruece por cero
//  		cruze_por_cero_Bit_3 = 0;			//VENTILADOR por cruece por cero
//  		cruze_por_cero_Bit_4 = 0;			//LAMPARA por cruece por cero

uint8_t		muestras_cal_volt;      //RM_20230908 Para mejorar la calibración de voltaje
uint8_t		voltaje_ant_cal;        //RM_20230908 Para mejorar la calibración de voltaje

uint8_t		ctlmemo = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Registro de control de manejo de memoria EEPROM 0xAA = Grabar; cualquier otra cosa = leer
uint8_t		cntmemo = 0;				//08/FEB/2022		DS.B 1         ;0x00;	Puntero de datos de memoria EEPROM




//LN_460:	#pragma section @near {asmpt44_1}

uint8_t	Plantilla [128] = {
		[dato_seg1]= 0,															//uint8_t		dato_seg1 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0100	;	256 d	;Dato de seguridad 1

		//;-------------------  GRUPO DE PAR�?METROS T (Temperatura)  -----------------------------------
		//;-------------------  Parámetros de temperatura  -------------------------------
		[spdiur_H]=highByte(0),     			[spdiur_L]=lowByte(0), 				//uint16_t		spdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0101	;	257 d	;t0 - Set point compresor diurno
		[difdiur_H]=highByte(0),     			[difdiur_L]=lowByte(0), 			//uint16_t		difdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0103	;	259 d	;t1 - Diferencial para el modo diurno
		[osdiur_H]=highByte(0),      			[osdiur_L]=lowByte(0), 				//uint16_t		osdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0105	;	261 d	;t2 - Offset para el modo diurno
		[spnoct_H]=highByte(0),      			[spnoct_L]=lowByte(0), 				//uint16_t		spnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0107	;	263 d	;t3 - Set point compresor nocturno
		[difnoct_H]=highByte(0),      			[difnoct_L]=lowByte(0), 			//uint16_t		difnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0109	;	265 d	;t4 - Diferencial para el modo nocturno
		[osnoct_H]=highByte(0),     			[osnoct_L]=lowByte(0), 				//uint16_t		osnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010B	;	267 d	;t5 - Offset para el modo nocturno
		[rngmin_H]=highByte(0),  				[rngmin_L]=lowByte(0), 				//uint16_t		rngmin_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010D	;	269 d	;t6 - Rango mínimo para ajuste del set point
		[rngmax_H]=highByte(0),   				[rngmax_L]=lowByte(0), 				//uint16_t		rngmax_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010F	;	271 d	;t7 - Rango máximo para ajuste del set point
		[t8_H]=highByte(0),    					[t8_L]=lowByte(0), 					//uint16_t		t8_w = 0;					//
		[defrResetTemp_H]=highByte(0),  		[defrResetTemp_L]=lowByte(0), 		//uint16_t		defrResetTemp = 0;//
		[defrStartTemp_H]=highByte(0), 			[defrStartTemp_L]=lowByte(0),		//uint16_t		defrStartTemp = 0;					//08/FEB/2022		DS.W 1	;	equ	$0115	;	277 d	;tA_W
		[defrStartTempAmb_H]=highByte(0),   	[defrStartTempAmb_L]=lowByte(0),	//uint16_t		defrStartTempAmb = 0;					//08/FEB/2022		DS.W 1	;	equ	$0117	;	279 d	;tB_W
		[tC_H]=highByte(0),     				[tC_L]=lowByte(0), 					//uint16_t		tC_w = 0;					//08/FEB/2022		DS.W 1	;	equ	$0119	;	281 d	;tC_W
		[difAhorro1_H]=highByte(0),     		[difAhorro1_L]=lowByte(0), 			//uint16_t		difAhorro1 = 0;		//08/FEB/2022		DS.W	1	;	tD
		[difAhorro2_H]=highByte(0),     		[difAhorro2_L]=lowByte(0), 			//uint16_t		difAhorro2 = 0;		//08/FEB/2022		DS.W	1	;	tE
		[tF_H]=highByte(0),    					[tF_L]=lowByte(0), 					//uint16_t		tF_w = 0;					//08/FEB/2022		DS.W 1	;	equ	$011F	;	287 d	;tF_W

		//;-------------------  GRUPO DE PAR�?METROS A (Alarmas)  -----------------------------------
		//;-------------------  Parámetros de Alarma  ------------------------------------
		[limevach_H]=highByte(0),			[limevach_L]=lowByte(0), 		//uint16_t		limevach_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0121	;	289 d	;A0 - Temperatura sensor evaporador para cancelar deshielo
		[limambch_H]=highByte(0),   		[limambch_L]=lowByte(0), 		//uint16_t		limambch_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0123	;	291 d	;A1 - Temperatura sensor ambiente para cancelar deshielo
		[tsumidero_H]=highByte(0),   		[tsumidero_L]=lowByte(0), 		//uint16_t		tsumidero_w = 0;	//08/FEB/2022		DS.W 1	;	equ	$0125	;	293 d	;A2 - Temperatura de sumidero
		[anticong_H]=highByte(0),    		[anticong_L]=lowByte(0), 		//uint16_t		anticong_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0127	;	295 d	;A3 - Temperatura de anticongelamiento del producto
		[pulldown_H]=highByte(0),    		[pulldown_L]=lowByte(0), 		//uint16_t		pulldown_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0129	;	297 d	;A4 - Temperatura mínima para que se active el modo pulldown
		[limineav_H]=highByte(0),    		[limineav_L]=lowByte(0), 		//uint16_t		limineav_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012B	;	299 d	;A5 - Limite inferior de alarma de temperatura evaporador
		[limsual_H]=highByte(0),     		[limsual_L]=lowByte(0), 		//uint16_t		limsual_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012D	;	301 d	;A6 - Limite superior alarma de temperatura
		[liminal_H]=highByte(0),     		[liminal_L]=lowByte(0), 		//uint16_t		liminal_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012F	;	303 d	;A7 - Limite inferior alarma de temperatura
		[difTempAl_H]=highByte(0),   		[difTempAl_L]=lowByte(0), 		//uint16_t		difTempAl = 0;		//08/FEB/2022		DS.W 1	;	equ	$0131	;	305 d	;A8 -
		[difDefi_H]=highByte(0),     		[difDefi_L]=lowByte(0), 		//uint16_t		difDefi = 0;					//08/FEB/2022		DS.W 1	;	equ	$0133	;	307 d	;A9 -
		[tempTC1_H]=highByte(0),     		[tempTC1_L]=lowByte(0), 		//uint16_t		tempTC1 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0135	;	309 d	;AA -
		[tempTC2_H]=highByte(0),     		[tempTC2_L]=lowByte(0), 		//uint16_t		tempTC2 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0137	;	311 d	;AB -
		[spdiur_BK_H]=highByte(0),   		[spdiur_BK_L]=lowByte(0), 		//uint16_t		spdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$0139	;	313 d	;AC -
		[difdiur_BK_H]=highByte(0),  		[difdiur_BK_L]=lowByte(0), 		//uint16_t		difdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013B	;	315 d	;AD -
		[limsual_BK_H]=highByte(0),			[limsual_BK_L]=lowByte(0), 		//uint16_t		limsual_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013D	;	317 d	;AE -
		[liminal_BK_H]=highByte(0),  		[liminal_BK_L]=lowByte(0), 		//uint16_t		liminal_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013F	;	319 d	;AF -

		[dato_seg2]=0,			 										//uint8_t		dato_seg2 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0141	;	321 d	;Dato de seguridad 2

		//;-------------------  GRUPO DE PAR�?METROS L (Límites)  -----------------------------------
		//;-------------------  Parámetros de limites  -----------------------------------
		[volt_000]=0, 								//uint8_t		volt_000 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0142	;	322 d	;L0 - Voltaje mínimo de protección para 120v
		[volt_100]=0, 								//uint8_t		volt_100 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0143	;	323 d	;L1 - Voltaje máx de protección para 120v y mín para 220v
		[volt_200]=0, 								//uint8_t		volt_200 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0144	;	324 d	;L2 - Voltaje máximo de protección para 220v
		[protval]=0, 								//uint8_t		protval = 0;				//08/FEB/2022		DS.B 1	;	equ	$0145	;	325 d	;L3 - Tiempo de validación para protección del compresor
		[interdh]=0, 								//uint8_t		interdh = 0;				//08/FEB/2022		DS.B 1	;	equ	$0146	;	326 d	;L4 - Tiempo de inter-deshielo
		[timedh]=0, 								//uint8_t		timedh = 0;					//08/FEB/2022		DS.B 1	;	equ	$0147	;	327 d	;L5 - Tiempo de deshielo
		[timepaf]=1, 								//uint8_t		timepaf = 0;				//08/FEB/2022		DS.B 1	;	equ	$0148	;	328 d	;L6 - Tiempo de puerta abierta para cancelar control ventilador
		[timepa]=0, 								//uint8_t		timepa = 0;					//08/FEB/2022		DS.B 1	;	equ	$0149	;	329 d	;L7 - Tiempo de validación para alarma de puerta abierta
		[alarval]=0, 								//uint8_t		alarval = 0;				//08/FEB/2022		DS.B 1	;	equ	$014A	;	330 d	;L8 - Tiempo de validación de alarma de temperatura
		[timeNoct]=0, 								//uint8_t		timeNoct = 0;				//08/FEB/2022		DS.B 1	;	equ	$014B	;	331 d	;L9 - Tiempo de modo nocturno
		[timeHold]=0, 								//uint8_t		timeHold = 0;						//08/FEB/2022		DS.B 1	;	equ	$014C	;	332 d	;LA -
		[timeDefi]=0, 								//uint8_t		timeDefi = 0;						//08/FEB/2022		DS.B 1	;	equ	$014D	;	333 d	;LB -
		[alarmDelay]=0, 							//uint8_t		alarmDelay = 0;						//08/FEB/2022		DS.B 1	;	equ	$014E	;	334 d	;LC -
		[LD_b]=0, 									//uint8_t		LD_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$014F	;	335 d	;LD -
		[tmDoorEvent]=0, 							//uint8_t		tmDoorEvent = 0;						//08/FEB/2022		DS.B 1	;	equ	$0150	;	336 d	;LE -
		[loggerTime]=0,	 							//uint8_t		loggerTime = 0;						//08/FEB/2022		DS.B 1	;	equ	$0151	;	337 d	;LF -

		//;-------------------  GRUPO DE PAR�?METROS C (Configuración)  -----------------------------------
		//;-------------------  Parámetros de configuración  -----------------------------
		[escala]=0, 								//uint8_t		escala = 0;					//08/FEB/2022		DS.B 1	;	equ	$0152	;	338 d	;C0 - Escala de temperatura
		[logicos]=0, 								//uint8_t		logicos = 0;				//08/FEB/2022		DS.B 1	;	equ	$0153	;	339 d	;C1 - Banderas de configuración
		[filtro]=0, 								//uint8_t		filtro = 0;					//08/FEB/2022		DS.B 1	;	equ	$0154	;	340 d	;C2 - Intensidad filtro del sensor ambiente a la subida
		[dhmode]=0, 								//uint8_t		dhmode = 0;					//08/FEB/2022		DS.B 1	;	equ	$0155	;	341 d	;C3 - Modo de deshielo
		[timeadap]=0, 								//uint8_t		timeadap = 0;				//08/FEB/2022		DS.B 1	;	equ	$0156	;	342 d	;C4 - Tiempo adaptivo de deshielo
		[protype]=0, 								//uint8_t		protype = 0;				//08/FEB/2022		DS.B 1	;	equ	$0157	;	343 d	;C5 - Tipo de protección del compresor
		[sensit]=0, 								//uint8_t		sensit = 0;					//08/FEB/2022		DS.B 1	;	equ	$0158	;	344 d	;C6 - Sensibilidad de la fotocelda
		[numSens]=0, 								//uint8_t		numSens = 0;						//08/FEB/2022		DS.B 1	;	equ	$0159	;	345 d	;C7 -
		[nivDpyFail]=0, 							//uint8_t		nivDpyFail = 0;						//08/FEB/2022		DS.B 1	;	equ	$015A	;	346 d	;C8 -
		[logicos2]=0, 								//uint8_t		logicos2 = 0;						//08/FEB/2022		DS.B 1	;	equ	$015B	;	347 d	;C9 -
		[CA_b]=0, 									//uint8_t		CA_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015C	;	348 d	;CA -
		[CB_b]=0, 									//uint8_t		CB_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015D	;	349 d	;CB -
		[CC_b]=0, 									//uint8_t		CC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015E	;	350 d	;CC -
		[D1_Msg1]=0, 								//uint8_t		CD_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015F	;	351 d	;CD -
		[D2_Msg1]=0, 								//uint8_t		CE_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0160	;	352 d	;CE -
		[FlagBLE]=0, 								//uint8_t 	FlagBLE						= 0	;//

		//;-------------------  GRUPO DE PAR�?METROS F (Función)  -----------------------------------
		//;-------------------  Parámetros de función  -----------------------------------
		[histvol]=0, 								//uint8_t		histvol = 0;				//08/FEB/2022		DS.B 1	;	equ	$0162	;	354 d	;F0 - Histéresis para la protección de voltaje
		[rangein]=0, 								//uint8_t		rangein = 0;				//08/FEB/2022		DS.B 1	;	equ	$0163	;	355 d	;F1 - Tiempo de validación para salir de protección de voltaje
		[dripposd]=0, 								//uint8_t		dripposd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0164	;	356 d	;F2 - Tiempo de goteo
		[minbrake]=0, 								//uint8_t		minbrake = 0;				//08/FEB/2022		DS.B 1	;	equ	$0165	;	357 d	;F3 - Tiempo mínimo de descanso del compresor
		[maxwork]=0, 								//uint8_t		maxwork = 0;				//08/FEB/2022		DS.B 1	;	equ	$0166	;	358 d	;F4 - Tiempo máximo de compresor encendido
		[exhausted]=0, 								//uint8_t		exhausted = 0;			//08/FEB/2022		DS.B 1	;	equ	$0167	;	359 d	;F5 - Tiempo compresor OFF si cumplió máx de compresor ON
		[cicloFd]=0, 								//uint8_t		cicloFd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0168	;	360 d	;F6 - Cicleo para ventilador en modo diurno
		[timeBreakDh]=0, 								//uint8_t		cicloFn = 0;				//08/FEB/2022		DS.B 1	;	equ	$0169	;	361 d	;F7 - Cicleo para ventilador en modo nocturno
		[timedoor]=0, 								//uint8_t		timedoor = 0;				//08/FEB/2022		DS.B 1	;	equ	$016A	;	362 d	;F8 - Tiempo mínimo de puerta cerrada para entrar a nocturno
		[paramSr]=0, 								//uint8_t		paramSr = 0;				//08/FEB/2022		DS.B 1	;	equ	$016B	;	363 d	;F9 - Pre-salida del modo Nocturno
		[margdes]=0, 								//uint8_t		margdes = 0;				//08/FEB/2022		DS.B 1	;	equ	$016C	;	364 d	;FA - Margen de descarte
		[timeluzoff]=0, 							//uint8_t		timeluzoff = 0;			//08/FEB/2022		DS.B 1	;	equ	$016D	;	365 d	;FB - Retardo apagado de pancarta al entrar a nocturno
		[FC_b]=0, 									//uint8_t		FC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$016E	;	366 d	;FC -
		[tiempoAhorro1]=0, 							//uint8_t		tiempoAhorro1 = 0;	//08/FEB/2022		DS.B	1	;	FD
		[tiempoAhorro2]=0, 							//uint8_t		tiempoAhorro2 = 0;	//08/FEB/2022		DS.B	1	;	FE
		[FF_b]=0, 									//uint8_t		FF_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0171	;	369 d	;FF -

		//;------------------  GRUPO DISPONIBLE  -----------------------
		//;------------------  Grupo Disponible  -----------------------
		[AddModBus]=0, 								//uint8_t		D0_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0172	;	370 d	:D0 -
		[password]=0, 								//uint8_t		password = 0;						//08/FEB/2022		DS.B 1	;	equ	$0173	;	371 d	:D1 -
		[D2_b]=0, 									//uint8_t		D2_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0174	;	372 d	;D2 -
		//@near	uint8_t		D3_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0175	;	373 d	;D3 -
		//@near	uint8_t		D4_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0176	;	374 d	;D4 -
		//@near	uint8_t		D5_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0177	;	375 d	;D5 -
		//@near	uint8_t		D6_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0178	;	376 d	;D6 -
		//@near uint8_t modelo  		      = 0;
		[cliente]=0, 								//uint8_t		cliente 			= 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -
		[clienteELTEC]=0, 							//uint8_t		clienteELTEC 	= 0;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -
		[hardware]=0, 								//uint8_t		hardware			= 0;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -
		[modelo0]=0, 								//uint8_t		modelo0				= 0;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -
		[modelo]=0, 									//uint8_t	 	modelo	      =	0;

		//;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
		//;------------------  Identificador y bytes disponibles  -----------------------
		[param_ID]=0, 										//uint8_t		param_ID = 0;				//08/FEB/2022		DS.B 1	;	equ	$017A	;	378 d	;B0 – Identificador de parámetros programados
		[version1]=0, 										//uint8_t version1					= 0;
		[version2]=0, 										//uint8_t version2	      	= 0;
		[plantilla1]=0, 									//uint8_t plantilla1				= 0;
		[plantilla2]=0, 									//uint8_t plantilla2   		  = 0;

		[dato_seg3]=0, 										//uint8_t		dato_seg3 = 0;			//08/FEB/2022		DS.B 1	;	equ	$017F	;	383 d	;Dato de seguridad 3


};
////;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
////;------------------  Identificador y bytes disponibles  -----------------------
//uint8_t	param_ID = 0;				//08/FEB/2022		DS.B 1	;	equ	$017A	;	378 d	;B0 – Identificador de parámetros programados
//uint8_t version1 = 0;
//uint8_t version2 = 0;
//uint8_t plantilla1 = 0;
//uint8_t plantilla2 = 0;
//
//uint8_t		dato_seg3 = 0;			//08/FEB/2022		DS.B 1	;	equ	$017F	;	383 d	;Dato de seguridad 3
//-------------------------- Limite de 128 Bytes ------------------------------------------------


uint8_t		voltram [16] = {0};				//08/FEB/2022		DS.B	16; RAM para Voltaje


uint8_t	copiaPlantilla [128] = {
		[cdato_seg1]= 0,															//uint8_t		dato_seg1 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0100	;	256 d	;Dato de seguridad 1

		//;-------------------  GRUPO DE PAR�?METROS T (Temperatura)  -----------------------------------
		//;-------------------  Parámetros de temperatura  -------------------------------
		[cspdiur_H]=highByte(0),     			[cspdiur_L]=lowByte(0),				//uint16_t		cspdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0101	;	257 d	;t0 - Set point compresor diurno
		[cdifdiur_H]=highByte(0),     			[cdifdiur_L]=lowByte(0),			//uint16_t		cdifdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0103	;	259 d	;t1 - Diferencial para el modo diurno
		[cosdiur_H]=highByte(0),      			[cosdiur_L]=lowByte(0), 			//uint16_t		cosdiur_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0105	;	261 d	;t2 - Offset para el modo diurno
		[cspnoct_H]=highByte(0),      			[cspnoct_L]=lowByte(0), 			//uint16_t		cspnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0107	;	263 d	;t3 - Set point compresor nocturno
		[cdifnoct_H]=highByte(0),      			[cdifnoct_L]=lowByte(0),			//uint16_t		cdifnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$0109	;	265 d	;t4 - Diferencial para el modo nocturno
		[cosnoct_H]=highByte(0),     			[cosnoct_L]=lowByte(0), 			//uint16_t		cosnoct_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010B	;	267 d	;t5 - Offset para el modo nocturno
		[crngmin_H]=highByte(0),  				[crngmin_L]=lowByte(0), 			//uint16_t		crngmin_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010D	;	269 d	;t6 - Rango mínimo para ajuste del set point
		[crngmax_H]=highByte(0),   				[crngmax_L]=lowByte(0), 			//uint16_t		crngmax_w = 0;			//08/FEB/2022		DS.W 1	;	equ	$010F	;	271 d	;t7 - Rango máximo para ajuste del set point
		[ct8_H]=highByte(0),    				[ct8_L]=lowByte(0), 				//uint16_t		ct8_w = 0;					//
		[cdefrResetTemp_H]=highByte(0),  		[cdefrResetTemp_L]=lowByte(0),		//uint16_t		cdefrResetTemp = 0;//
		[cdefrStartTemp_H]=highByte(0), 		[cdefrStartTemp_L]=lowByte(0),		//uint16_t		cdefrStartTemp = 0;					//08/FEB/2022		DS.W 1	;	equ	$0115	;	277 d	;tA_W
		[cdefrStartTempAmb_H]=highByte(0),   	[cdefrStartTempAmb_L]=lowByte(0),	//uint16_t		cdefrStartTempAmb = 0;					//08/FEB/2022		DS.W 1	;	equ	$0117	;	279 d	;tB_W
		[ctC_H]=highByte(0),     				[ctC_L]=lowByte(0), 				//uint16_t		ctC_w = 0;					//08/FEB/2022		DS.W 1	;	equ	$0119	;	281 d	;tC_W
		[cdifAhorro1_H]=highByte(0),     		[cdifAhorro1_L]=lowByte(0), 		//uint16_t		cdifAhorro1 = 0;		//08/FEB/2022		DS.W	1	;	tD
		[cdifAhorro2_H]=highByte(0),     		[cdifAhorro2_L]=lowByte(0), 		//uint16_t		cdifAhorro2 = 0;		//08/FEB/2022		DS.W	1	;	tE
		[ctF_H]=highByte(0),    				[ctF_L]=lowByte(0), 				//uint16_t		ctF_w = 0;					//08/FEB/2022		DS.W 1	;	equ	$011F	;	287 d	;tF_W

		//;-------------------  GRUPO DE PAR�?METROS A (Alarmas)  -----------------------------------
		//;-------------------  Parámetros de Alarma  ------------------------------------
		[climevach_H]=highByte(0),			[climevach_L]=lowByte(0), 		//uint16_t		climevach_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0121	;	289 d	;A0 - Temperatura sensor evaporador para cancelar deshielo
		[climambch_H]=highByte(0),   		[climambch_L]=lowByte(0), 		//uint16_t		climambch_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0123	;	291 d	;A1 - Temperatura sensor ambiente para cancelar deshielo
		[ctsumidero_H]=highByte(0),   		[ctsumidero_L]=lowByte(0), 		//uint16_t		ctsumidero_w = 0;	//08/FEB/2022		DS.W 1	;	equ	$0125	;	293 d	;A2 - Temperatura de sumidero
		[canticong_H]=highByte(0),    		[canticong_L]=lowByte(0), 		//uint16_t		canticong_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0127	;	295 d	;A3 - Temperatura de anticongelamiento del producto
		[cpulldown_H]=highByte(0),    		[cpulldown_L]=lowByte(0), 		//uint16_t		cpulldown_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$0129	;	297 d	;A4 - Temperatura mínima para que se active el modo pulldown
		[climineav_H]=highByte(0),    		[climineav_L]=lowByte(0), 		//uint16_t		climineav_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012B	;	299 d	;A5 - Limite inferior de alarma de temperatura evaporador
		[climsual_H]=highByte(0),     		[climsual_L]=lowByte(0), 		//uint16_t		climsual_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012D	;	301 d	;A6 - Limite superior alarma de temperatura
		[climinal_H]=highByte(0),     		[climinal_L]=lowByte(0), 		//uint16_t		climinal_w = 0;		//08/FEB/2022		DS.W 1	;	equ	$012F	;	303 d	;A7 - Limite inferior alarma de temperatura
		[cdifTempAl_H]=highByte(0),   		[cdifTempAl_L]=lowByte(0), 		//uint16_t		cdifTempAl = 0;		//08/FEB/2022		DS.W 1	;	equ	$0131	;	305 d	;A8 -
		[cdifDefi_H]=highByte(0),     		[cdifDefi_L]=lowByte(0), 		//uint16_t		cdifDefi = 0;					//08/FEB/2022		DS.W 1	;	equ	$0133	;	307 d	;A9 -
		[ctempTC1_H]=highByte(0),     		[ctempTC1_L]=lowByte(0), 		//uint16_t		ctempTC1 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0135	;	309 d	;AA -
		[ctempTC2_H]=highByte(0),     		[ctempTC2_L]=lowByte(0), 		//uint16_t		ctempTC2 = 0;					//08/FEB/2022		DS.W 1	;	equ	$0137	;	311 d	;AB -
		[cspdiur_BK_H]=highByte(0),   		[cspdiur_BK_L]=lowByte(0), 		//uint16_t		cspdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$0139	;	313 d	;AC -
		[cdifdiur_BK_H]=highByte(0),  		[cdifdiur_BK_L]=lowByte(0), 	//uint16_t		cdifdiur_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013B	;	315 d	;AD -
		[climsual_BK_H]=highByte(0),		[climsual_BK_L]=lowByte(0), 	//uint16_t		climsual_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013D	;	317 d	;AE -
		[climinal_BK_H]=highByte(0),  		[climinal_BK_L]=lowByte(0), 	//uint16_t		climinal_w_BK = 0;					//08/FEB/2022		DS.W 1	;	equ	$013F	;	319 d	;AF -

		[cdato_seg2]=0,			 											//uint8_t		cdato_seg2 = 0;			//08/FEB/2022		DS.B 1	;	equ	$0141	;	321 d	;Dato de seguridad 2

		//;-------------------  GRUPO DE PAR�?METROS L (Límites)  -----------------------------------
		//;-------------------  Parámetros de limites  -----------------------------------
		[cvolt_000]=0, 								//uint8_t		cvolt_000 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0142	;	322 d	;L0 - Voltaje mínimo de protección para 120v
		[cvolt_100]=0, 								//uint8_t		cvolt_100 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0143	;	323 d	;L1 - Voltaje máx de protección para 120v y mín para 220v
		[cvolt_200]=0, 								//uint8_t		cvolt_200 = 0;				//08/FEB/2022		DS.B 1	;	equ	$0144	;	324 d	;L2 - Voltaje máximo de protección para 220v
		[cprotval]=0, 								//uint8_t		cprotval = 0;				//08/FEB/2022		DS.B 1	;	equ	$0145	;	325 d	;L3 - Tiempo de validación para protección del compresor
		[cinterdh]=0, 								//uint8_t		cinterdh = 0;				//08/FEB/2022		DS.B 1	;	equ	$0146	;	326 d	;L4 - Tiempo de inter-deshielo
		[ctimedh]=0, 								//uint8_t		ctimedh = 0;					//08/FEB/2022		DS.B 1	;	equ	$0147	;	327 d	;L5 - Tiempo de deshielo
		[ctimepaf]=0, 								//uint8_t		ctimepaf = 0;				//08/FEB/2022		DS.B 1	;	equ	$0148	;	328 d	;L6 - Tiempo de puerta abierta para cancelar control ventilador
		[ctimepa]=01, 								//uint8_t		ctimepa = 0;					//08/FEB/2022		DS.B 1	;	equ	$0149	;	329 d	;L7 - Tiempo de validación para alarma de puerta abierta
		[calarval]=0, 								//uint8_t		calarval = 0;				//08/FEB/2022		DS.B 1	;	equ	$014A	;	330 d	;L8 - Tiempo de validación de alarma de temperatura
		[ctimeNoct]=0, 								//uint8_t		ctimeNoct = 0;				//08/FEB/2022		DS.B 1	;	equ	$014B	;	331 d	;L9 - Tiempo de modo nocturno
		[ctimeHold]=0, 								//uint8_t		ctimeHold = 0;						//08/FEB/2022		DS.B 1	;	equ	$014C	;	332 d	;LA -
		[ctimeDefi]=0, 								//uint8_t		ctimeDefi = 0;						//08/FEB/2022		DS.B 1	;	equ	$014D	;	333 d	;LB -
		[calarmDelay]=0, 							//uint8_t		calarmDelay = 0;						//08/FEB/2022		DS.B 1	;	equ	$014E	;	334 d	;LC -
		[cLD_b]=0, 									//uint8_t		cLD_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$014F	;	335 d	;LD -
		[ctmDoorEvent]=0, 							//uint8_t		ctmDoorEvent = 0;						//08/FEB/2022		DS.B 1	;	equ	$0150	;	336 d	;LE -
		[cloggerTime]=0,	 						//uint8_t		cloggerTime = 0;						//08/FEB/2022		DS.B 1	;	equ	$0151	;	337 d	;LF -

		//;-------------------  GRUPO DE PAR�?METROS C (Configuración)  -----------------------------------
		//;-------------------  Parámetros de configuración  -----------------------------
		[cescala]=0, 								//uint8_t		cescala = 0;					//08/FEB/2022		DS.B 1	;	equ	$0152	;	338 d	;C0 - Escala de temperatura
		[clogicos]=0, 								//uint8_t		clogicos = 0;				//08/FEB/2022		DS.B 1	;	equ	$0153	;	339 d	;C1 - Banderas de configuración
		[cfiltro]=0, 								//uint8_t		cfiltro = 0;					//08/FEB/2022		DS.B 1	;	equ	$0154	;	340 d	;C2 - Intensidad filtro del sensor ambiente a la subida
		[cdhmode]=0, 								//uint8_t		cdhmode = 0;					//08/FEB/2022		DS.B 1	;	equ	$0155	;	341 d	;C3 - Modo de deshielo
		[ctimeadap]=0, 								//uint8_t		ctimeadap = 0;				//08/FEB/2022		DS.B 1	;	equ	$0156	;	342 d	;C4 - Tiempo adaptivo de deshielo
		[cprotype]=0, 								//uint8_t		cprotype = 0;				//08/FEB/2022		DS.B 1	;	equ	$0157	;	343 d	;C5 - Tipo de protección del compresor
		[csensit]=0, 								//uint8_t		csensit = 0;					//08/FEB/2022		DS.B 1	;	equ	$0158	;	344 d	;C6 - Sensibilidad de la fotocelda
		[cnumSens]=0, 								//uint8_t		cnumSens = 0;						//08/FEB/2022		DS.B 1	;	equ	$0159	;	345 d	;C7 -
		[cnivDpyFail]=0, 							//uint8_t		cnivDpyFail = 0;						//08/FEB/2022		DS.B 1	;	equ	$015A	;	346 d	;C8 -
		[clogicos2]=0, 								//uint8_t		clogicos2 = 0;						//08/FEB/2022		DS.B 1	;	equ	$015B	;	347 d	;C9 -
		[cCA_b]=0, 									//uint8_t		cCA_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015C	;	348 d	;CA -
		[cCB_b]=0, 									//uint8_t		cCB_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015D	;	349 d	;CB -
		[cCC_b]=0, 									//uint8_t		cCC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015E	;	350 d	;CC -
		[cD1_Msg1]=0, 								//uint8_t		cCD_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$015F	;	351 d	;CD -
		[cD2_Msg1]=0, 								//uint8_t		cCE_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0160	;	352 d	;CE -
		[cFlagBLE]=0, 								//uint8_t 		cFlagBLE						= 0	;//

		//;-------------------  GRUPO DE PAR�?METROS F (Función)  -----------------------------------
		//;-------------------  Parámetros de función  -----------------------------------
		[chistvol]=0, 								//uint8_t		chistvol = 0;				//08/FEB/2022		DS.B 1	;	equ	$0162	;	354 d	;F0 - Histéresis para la protección de voltaje
		[crangein]=0, 								//uint8_t		crangein = 0;				//08/FEB/2022		DS.B 1	;	equ	$0163	;	355 d	;F1 - Tiempo de validación para salir de protección de voltaje
		[cdripposd]=0, 								//uint8_t		cdripposd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0164	;	356 d	;F2 - Tiempo de goteo
		[cminbrake]=0, 								//uint8_t		cminbrake = 0;				//08/FEB/2022		DS.B 1	;	equ	$0165	;	357 d	;F3 - Tiempo mínimo de descanso del compresor
		[cmaxwork]=0, 								//uint8_t		cmaxwork = 0;				//08/FEB/2022		DS.B 1	;	equ	$0166	;	358 d	;F4 - Tiempo máximo de compresor encendido
		[cexhausted]=0, 							//uint8_t		cexhausted = 0;			//08/FEB/2022		DS.B 1	;	equ	$0167	;	359 d	;F5 - Tiempo compresor OFF si cumplió máx de compresor ON
		[ccicloFd]=0, 								//uint8_t		ccicloFd = 0;				//08/FEB/2022		DS.B 1	;	equ	$0168	;	360 d	;F6 - Cicleo para ventilador en modo diurno
		[ctimeBreakDh]=0, 								//uint8_t		ccicloFn = 0;				//08/FEB/2022		DS.B 1	;	equ	$0169	;	361 d	;F7 - Cicleo para ventilador en modo nocturno
		[ctimedoor]=0, 								//uint8_t		ctimedoor = 0;				//08/FEB/2022		DS.B 1	;	equ	$016A	;	362 d	;F8 - Tiempo mínimo de puerta cerrada para entrar a nocturno
		[cparamSr]=0, 								//uint8_t		cparamSr = 0;				//08/FEB/2022		DS.B 1	;	equ	$016B	;	363 d	;F9 - Pre-salida del modo Nocturno
		[cmargdes]=0, 								//uint8_t		cmargdes = 0;				//08/FEB/2022		DS.B 1	;	equ	$016C	;	364 d	;FA - Margen de descarte
		[ctimeluzoff]=0, 							//uint8_t		ctimeluzoff = 0;			//08/FEB/2022		DS.B 1	;	equ	$016D	;	365 d	;FB - Retardo apagado de pancarta al entrar a nocturno
		[cFC_b]=0, 									//uint8_t		cFC_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$016E	;	366 d	;FC -
		[ctiempoAhorro1]=0, 						//uint8_t		ctiempoAhorro1 = 0;	//08/FEB/2022		DS.B	1	;	FD
		[ctiempoAhorro2]=0, 						//uint8_t		ctiempoAhorro2 = 0;	//08/FEB/2022		DS.B	1	;	FE
		[cFF_b]=0, 									//uint8_t		cFF_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0171	;	369 d	;FF -

		//;------------------  GRUPO DISPONIBLE  -----------------------
		//;------------------  Grupo Disponible  -----------------------
		[cAddModBus]=0, 							//uint8_t		cD0_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0172	;	370 d	:D0 -
		[cpassword]=0, 								//uint8_t		cpassword = 0;						//08/FEB/2022		DS.B 1	;	equ	$0173	;	371 d	:D1 -
		[cD2_b]=0, 									//uint8_t		cD2_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0174	;	372 d	;D2 -
		//@near	uint8_t		D3_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0175	;	373 d	;D3 -
		//@near	uint8_t		D4_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0176	;	374 d	;D4 -
		//@near	uint8_t		D5_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0177	;	375 d	;D5 -
		//@near	uint8_t		D6_b = 0;						//08/FEB/2022		DS.B 1	;	equ	$0178	;	376 d	;D6 -
		//@near uint8_t modelo  		      = 0;
		[ccliente]=0, 								//uint8_t		ccliente 			= 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -
		[cclienteELTEC]=0, 							//uint8_t		cclienteELTEC 	= 0;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -
		[chardware]=0, 								//uint8_t		chardware			= 0;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -
		[cmodelo0]=0, 								//uint8_t		cmodelo0				= 0;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -
		[cmodelo]=0, 								//uint8_t	 	cmodelo	      =	0;

		//;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
		//;------------------  Identificador y bytes disponibles  -----------------------
		[cparam_ID]=0, 								//uint8_t 	cparam_ID = 0;				//08/FEB/2022		DS.B 1	;	equ	$017A	;	378 d	;B0 – Identificador de parámetros programados
		[cversion1]=0, 								//uint8_t 	cversion1					= 0;
		[cversion2]=0, 								//uint8_t 	cversion2	      	= 0;
		[cplantilla1]=0, 							//uint8_t 	cplantilla1				= 0;
		[cplantilla2]=0, 							//uint8_t 	cplantilla2   		  = 0;

		[cdato_seg3]=0, 							//uint8_t	cdato_seg3 = 0;			//08/FEB/2022		DS.B 1	;	equ	$017F	;	383 d	;Dato de seguridad 3

};




//;------------------------ bancomu Variables relacionadas con el proceso de COMUNICACIÓN
uint8_t		Cnt_EspComu = 0;			//08/FEB/2022		DS.B 1 ;Contador para Tx Rx un byte cada 2ms


//;Datos que son Transmitidos de TREFPT44 a TREFTSAA
//;¡¡¡¡¡¡¡¡¡¡¡¡¡NO MODIFICAR EL ORDEN!!!!!!!!!!!!!!
uint8_t		Display_1 = 0;				//08/FEB/2022		DS.B 1 ;1 1B - Desplegado para el display 1 en 7 segmentos y punto
uint8_t		Display_2 = 0;				//08/FEB/2022		DS.B 1 ;2 1B - Desplegado para el display 2 en 7 segmentos y punto
_Bool Ind_Par[8] = {0};					// uint8_t		Ind_Par = 0;					//08/FEB/2022		DS.B 1 ;3 1B - 2 Indicadores LED y un bit de control de parpadeo
uint8_t		dms_extra = 0;				//08/FEB/2022		DS.B 1 ;4 1B - 2 Indicadores LED y un bit de control de parpadeo

_Bool Botones_T[8] = {0}; // uint8_t		Botones_T = 0;				//08/FEB/2022		DS.B 1 ;7 1B - 4 botones touch e interruptor de la puerta


// LN_833:	#pragma space extern [] @eeprom @near


__attribute__((section(".myBufSectionEEPROM_P"))) uint8_t	eePlantilla [128] ={
		[eedato_seg1]= 0xAA,										//uint8_t		eedato_seg1 = 0xAA;			//09/FEB/2022		DC.B	$AA	;	16384 d	4000 h	;Dato de seguridad 1

		//;-------------------  GRUPO DE PAR�?METROS T (Temperatura)  -------------------------
		//;-------------------  Parámetros de temperatura  -------------------------------
		[eespdiur_H]=highByte(10),     			[eespdiur_L]=lowByte(10), 			//uint16_t	eespdiur_w = 10;			//09/FEB/2022		DC.W	{-35};{-30}	;	16385 d	4001 h	;t0 - Set point compresor diurno	-3 ° C
		[eedifdiur_H]=highByte(20),     		[eedifdiur_L]=lowByte(20), 			//uint16_t	eedifdiur_w = 20;			//09/FEB/2022		DC.W	{25};{20}	;	16387 d	4003 h	;t1 - Diferencial para el modo diurno	2 ° C
		[eeosdiur_H]=highByte(0),      			[eeosdiur_L]=lowByte(0), 			//uint16_t	eeosdiur_w = 0;			//09/FEB/2022		DC.W	{-5};{0}	;	16389 d	4005 h	;t2 - Offset para el modo diurno	0 ° C
		[eespnoct_H]=highByte(0),      			[eespnoct_L]=lowByte(0), 			//uint16_t	eespnoct_w = 0;			//09/FEB/2022		DC.W	{10};{0}	;	16391 d	4007 h	;t3 - Set point compresor nocturno	0 ° C
		[eedifnoct_H]=highByte(0),      		[eedifnoct_L]=lowByte(0), 			//uint16_t	eedifnoct_w = 0;			//09/FEB/2022		DC.W	{25};{20}	;	16393 d	4009 h	;t4 - Diferencial para el modo nocturno	2 ° C
		[eeosnoct_H]=highByte(0),     			[eeosnoct_L]=lowByte(0), 			//uint16_t	eeosnoct_w = 0;			//09/FEB/2022		DC.W	{-5};{0}	;	16395 d	400B h	;t5 - Offset para el modo nocturno	0 ° C
		[eerngmin_H]=highByte(-150),  			[eerngmin_L]=lowByte(-150), 		//uint16_t	eerngmin_w = -150;		//09/FEB/2022		DC.W	{-150}	;	16397 d	400D h	;t6 - Rango mínimo para ajuste del set point	-15 ° C
		[eerngmax_H]=highByte(150),   			[eerngmax_L]=lowByte(150), 		//uint16_t	eerngmax_w = 150;			//09/FEB/2022		DC.W	{150}	;	16399 d	400F h	;t7 - Rango máximo para ajuste del set point	15 ° C
		[eet8_H]=highByte(0),    				[eet8_L]=lowByte(0), 			//uint16_t	eet8_w = 0;						//
		[eedefrResetTemp_H]=highByte(300),  	[eedefrResetTemp_L]=lowByte(300), 		//uint16_t	eedefrResetTemp = 300;	//;t9 - t9	Defrost reset temperature 3.0°C
		[eedefrStartTemp_H]=highByte(-100), 	[eedefrStartTemp_L]=lowByte(-100),		//uint16_t	eedefrStartTemp = -100;	//; tA	Defrost start temperature -10.0 ºC
		[eedefrStartTempAmb_H]=highByte(100),   [eedefrStartTempAmb_L]=lowByte(100),	 	//uint16_t	eedefrStartTempAmb = 100;						//09/FEB/2022		DC.W	{0}	;	16407 d	4017 h	;tB_W
		[eetC_H]=highByte(0),     				[eetC_L]=lowByte(0), 			//uint16_t	eetC_w = 0;						//09/FEB/2022		DC.W	{0}	;	16409 d	4019 h	;tC_W
		[eedifAhorro1_H]=highByte(0),     		[eedifAhorro1_L]=lowByte(0), 			//uint16_t	eedifAhorro1 = 0;		//09/FEB/2022		DC.W	{10}		;	tD
		[eedifAhorro2_H]=highByte(0),     		[eedifAhorro2_L]=lowByte(0), 			//uint16_t	eedifAhorro2 = 0;		//09/FEB/2022		DC.W	{10}		;	tE
		[eetF_H]=highByte(0),    				[eetF_L]=lowByte(0), 			//uint16_t	eetF_w = 0;						//09/FEB/2022		DC.W	{0}	;	16415 d	401F h	;tF_W

		//;-------------------  GRUPO DE PAR�?METROS A (Alarmas)  -----------------------------------
		//;-------------------  Parámetros de Alarma  ------------------------------------
		[eelimevach_H]=highByte(300),		[eelimevach_L]=lowByte(300), 		//uint16_t	eelimevach_w = 300;		//09/FEB/2022		DC.W	{150}	;	16417 d	4021 h	;A0 - Temperatura sensor evaporador para cancelar deshielo	15 ° C
		[eelimambch_H]=highByte(300),   	[eelimambch_L]=lowByte(300), 		//uint16_t	eelimambch_w = 300;		//09/FEB/2022		DC.W	{300}	;	16419 d	4023 h	;A1 - Temperatura sensor ambiente para cancelar deshielo	30 ° C
		[eetsumidero_H]=highByte(0),   		[eetsumidero_L]=lowByte(0), 		//uint16_t	eetsumidero_w = 0;	//09/FEB/2022		DC.W	{300}	;	16421 d	4025 h	;A2 - Temperatura de sumidero	30 ° C
		[eeanticong_H]=highByte(0),    		[eeanticong_L]=lowByte(0), 		//uint16_t	eeanticong_w = 0;	//09/FEB/2022		DC.W	{-310}	;	16423 d	4027 h	;A3 - Temperatura de anticongelamiento del producto	-31 ° C
		[eepulldown_H]=highByte(120),    	[eepulldown_L]=lowByte(120), 		//uint16_t	eepulldown_w = 120;		//09/FEB/2022		DC.W	{120}	;	16425 d	4029 h	;A4 - Temperatura mínima para que se active el modo pulldown	12 ° C
		[eelimineav_H]=highByte(0),    		[eelimineav_L]=lowByte(0), 		//uint16_t	eelimineav_w = 0;	//09/FEB/2022		DC.W	{-300}	;	16427 d	402B h	;A5 - Limite inferior de alarma de temperatura evaporador	-30 ° C
		[eelimsual_H]=highByte(60),     	[eelimsual_L]=lowByte(60), 		//uint16_t	eelimsual_w = 60;		//09/FEB/2022		DC.W	{120}	;	16429 d	402D h	;A6 - Limite superior alarma de temperatura	12 ° C
		[eeliminal_H]=highByte(-120),     	[eeliminal_L]=lowByte(-120), 		//uint16_t	eeliminal_w = -120;		//09/FEB/2022		DC.W	{-120}	;	16431 d	402F h	;A7 - Limite inferior alarma de temperatura	-12 ° C
		[eedifTempAl_H]=highByte(05),   	[eedifTempAl_L]=lowByte(05), 		//uint16_t	eedifTempAl = 05;			//09/FEB/2022		DC.W	{0}	;	16433 d	4031 h	;A8 -
		[eedifDefi_H]=highByte(20),     	[eedifDefi_L]=lowByte(20), 		//uint16_t	eedifDefi = 20;						//09/FEB/2022		DC.W	{0}	;	16435 d	4033 h	;A9 -	diferencial de deficiencia (2.0°C)
		[eetempTC1_H]=highByte(40),     	[eetempTC1_L]=lowByte(40), 		//uint16_t	eetempTC1 = 40;						//09/FEB/2022		DC.W	{0}	;	16437 d	4035 h	;AA -
		[eetempTC2_H]=highByte(50),     	[eetempTC2_L]=lowByte(50), 		//uint16_t	eetempTC2 = 50;						//09/FEB/2022		DC.W	{0}	;	16439 d	4037 h	;AB -
		[eespdiur_BK_H]=highByte(10),   	[eespdiur_BK_L]=lowByte(10), 		//uint16_t	eespdiur_w_BK = 10;						//09/FEB/2022		DC.W	{0}	;	16441 d	4039 h	;AC -
		[eedifdiur_BK_H]=highByte(20),  	[eedifdiur_BK_L]=lowByte(20), 		//uint16_t	eedifdiur_w_BK = 20;						//09/FEB/2022		DC.W	{0}	;	16443 d	403B h	;AD -
		[eelimsual_BK_H]=highByte(60),		[eelimsual_BK_L]=lowByte(60), 		//uint16_t	eelimsual_w_BK = 60;						//09/FEB/2022		DC.W	{0}	;	16445 d	403D h	;AE -
		[eeliminal_BK_H]=highByte(-120),  	[eeliminal_BK_L]=lowByte(-120), 		//uint16_t	eeliminal_w_BK = -120;						//09/FEB/2022		DC.W	{0}	;	16447 d	403F h	;AF -

		[eedato_seg2]=0x66, 										//uint8_t		eedato_seg2 = 0x66;		//09/FEB/2022			    DC.B	$66	;	16449 d	4041 h	;Dato de seguridad 2

		//;-------------------  GRUPO DE PAR�?METROS L (Límites)  -----------------------------------
		//;-------------------  Parámetros de limites  -----------------------------------
		[eevolt_000]=85, 										//uint8_t		eevolt_000 = 85;			//09/FEB/2022		DC.B	85	;	16450 d	4042 h	;L0 - Voltaje mínimo de protección para 120v	0_85 V
		[eevolt_100]=30, 										//uint8_t		eevolt_100 = 30;			//09/FEB/2022		DC.B	40	;	16451 d	4043 h	;L1 - Voltaje máx de protección para 120v y mín para 220v	1_40 V
		[eevolt_200]=40, 										//uint8_t		eevolt_200 = 40;			//09/FEB/2022		DC.B	40	;	16452 d	4044 h	;L2 - Voltaje máximo de protección para 220v	2_40 V
		[eeprotval]=15, 										//uint8_t		eeprotval = 15;				//09/FEB/2022		DC.B	15	;	16453 d	4045 h	;L3 - Tiempo de validación para protección del compresor	15 segundos
		[eeinterdh]=04, 										//uint8_t		eeinterdh = 04;				//09/FEB/2022		DC.B	04	;	16454 d	4046 h	;L4 - Tiempo de inter-deshielo	4 horas
		[eetimedh]=01, 										//uint8_t		eetimedh = 1;//30;				//09/FEB/2022		DC.B	30	;	16455 d	4047 h	;L5 - Tiempo de deshielo	30 minutos
		[eetimepaf]=99, 										//uint8_t		eetimepaf = 99;				//09/FEB/2022		DC.B	99	;	16456 d	4048 h	;L6 - Tiempo de puerta abierta para cancelar control ventilador	99 minutos
		[eetimepa]=01, 										//uint8_t		eetimepa = 01;//05;				//09/FEB/2022		DC.B	03	;	16457 d	4049 h	;L7 - Tiempo de validación para alarma de puerta abierta	3 minutos
		[eealarval]=15, 										//uint8_t		eealarval = 15;				//09/FEB/2022		DC.B	15	;	16458 d	404A h	;L8 - Tiempo de validación de alarma de temperatura	15 minutos
		[eetimeNoct]=10, 										//uint8_t		eetimeNoct = 10;			//09/FEB/2022		DC.B	10	;	16459 d	404B h	;L9 -	Tirmpo de modo nocturno (10 minutos)
		[eetimeHold]=0, 										//uint8_t		eetimeHold = 0;				//;LA -	Tiempo de bloqueo de display despues del deshielo (15 minutos)
		[eetimeDefi]=0, 										//uint8_t		eetimeDefi = 0;				//09/FEB/2022		DC.B	0	;	16461 d	404D h	;LB -	 tiempo de compresor encendido para realizar mediciones de deficiencia 5 minutos (0 = funcion cancelada)
		[eealarmDelay]=20, 										//uint8_t	 	eealarmDelay = 20		;// LC	tiempo para silenciar alarma
		[eeLD_b]=0, 										//uint8_t		eeLD_b = 0;						//09/FEB/2022		DC.B	0	;	16463 d	404F h	;LD -
		[eetmDoorEvent]=05, 										//uint8_t		eetmDoorEvent = 05;						//09/FEB/2022		DC.B	0	;	16464 d	4050 h	;LE -
		[eeloggerTime]=01, 										//uint8_t		eeloggerTime = 1;//60;						//09/FEB/2022		DC.B	0	;	16465 d	4051 h	;LF -

		//;-------------------  GRUPO DE PAR�?METROS C (Configuración)  -----------------------------------
		//;-------------------  Parámetros de configuración  -----------------------------
		[eeescala]=0, 										//uint8_t		eeescala = 00;					//09/FEB/2022		DC.B	00	;	16466 d	4052 h	;C0 - Escala de temperatura	00 : °C , 01: °F
		[eelogicos]=0x02, 										//uint8_t		eelogicos = 0x02;//0x02;//01;//17;				//09/FEB/2022		DC.B	17	;	16467 d	4053 h	;C1 - Banderas de configuración	* Ver lista al final
		[eefiltro]=7, 										//uint8_t		eefiltro = 0;				//09/FEB/2022		DC.B	06	;	16468 d	4054 h	;C2 - Intensidad filtro del sensor ambiente a la subida	[ 0 a 9 ]
		[eedhmode]=0, 										//uint8_t		eedhmode = 00;				//09/FEB/2022		DC.B	02	;	16469 d	4055 h	;C3 - Modo de deshielo	00: flujo de aire, 01: gas, 02: resistencia
		[eetimeadap]=0, 										//uint8_t		eetimeadap = 0;				//09/FEB/2022		DC.B	00	;	16470 d	4056 h	;C4 - Tiempo adaptivo de deshielo	[ 0 a 8 ]
		[eeprotype]=01, 										//uint8_t		eeprotype = 01;//02	 	//Manuel 09-MAR-2022;				//09/FEB/2022		DC.B	01	;	16471 d	4057 h	;C5 - Tipo de protección del compresor	00: sin protección , 01: 110V , 02: 220V
		[eesensit]=01, 										//uint8_t		eesensit = 01;				//09/FEB/2022		DC.B	01	;	16472 d	4058 h	;C6 - Sensibilidad de la fotocelda	[ 0 a 9 ]
		[eenumSens]=04, 										//uint8_t		eenumSens = 04;				//09/FEB/2022		DC.B	0	;	16473 d	4059 h	;C7 - Número de sensores con los que trabajará el equipo
		[eenivDpyFail]=01, 										//uint8_t		eenivDpyFail = 0x01;//0x0F;						//09/FEB/2022		DC.B	0	;	16474 d	405A h	;C8 -
		[eelogicos2]=00, 										//uint8_t		eelogicos2 = 0x00;						//09/FEB/2022		DC.B	0	;	16475 d	405B h	;C9 -
		[eeCA_b]=00, 										//uint8_t		eeCA_b = 0;						//09/FEB/2022		DC.B	0	;	16476 d	405C h	;CA -
		[eeCB_b]=00, 										//uint8_t		eeCB_b = 0;						//09/FEB/2022		DC.B	0	;	16477 d	405D h	;CB -
		[eeCC_b]=00, 										//uint8_t		eeCC_b = 0;						//09/FEB/2022		DC.B	0	;	16478 d	405E h	;CC -
		[eeD1_Msg1]=0x27, 										//uint8_t		eeD1_Msg1 = 0x27;						//09/FEB/2022		DC.B	0	;	16479 d	405F h	;CD -	"U"
		[eeD2_Msg1]=0x0A, 										//uint8_t		eeD2_Msg1 = 0x0A;						//09/FEB/2022		DC.B	0	;	16480 d	4060 h	;CE -	"A"
		[eeFlagBLE]=0, 										//uint8_t	 	eeFlagBLE				= 0			;//

		//;-------------------  GRUPO DE PAR�?METROS F (Función)  -----------------------------------
		//;-------------------  Parámetros de función  -----------------------------------
		[eehistvol]=42, 										//uint8_t		eehistvol = 42;					//09/FEB/2022				DC.B	42	;	16482 d	4062 h	;F0 - Histéresis para la protección de voltaje	 4Vmín, 2Vmáx (real 5.3)
		[eerangein]=03, 										//uint8_t		eerangein = 03;					//09/FEB/2022			 	DC.B	03	;	16483 d	4063 h	;F1 - Tiempo de validación para salir de protección de voltaje	0.3 minutos
		[eedripposd]=0, 										//uint8_t		eedripposd = 00;				//09/FEB/2022			  DC.B	11	;	16484 d	4064 h	;F2 - Tiempo de goteo	1 minuto . 1 minuto
		[eeminbrake]=01, 										//uint8_t		eeminbrake = 01;				//09/FEB/2022		 	  DC.B	10	;	16485 d	4065 h	;F3 - Tiempo mínimo de descanso del compresor	1.0 minutos
		[eemaxwork]=250, 										//uint8_t		eemaxwork = 250;					//09/FEB/2022			 	DC.B	00	;	16486 d	4066 h	;F4 - Tiempo máximo de compresor encendido	Indefinido
		[eeexhausted]=250, 										//uint8_t		eeexhausted	= 250;			//09/FEB/2022		  	DC.B	250	;	16487 d	4067 h	;F5 - Tiempo compresor OFF si cumplió máx de compresor ON	25 horas
		[eecicloFd]=10, 										//uint8_t		eecicloFd = 10;					//09/FEB/2022				DC.B	33	;	16488 d	4068 h	;F6 - Cicleo para ventilador en modo diurno	3 minutos ON. 3 minutos OFF
		[eetimeBreakDh]=33, 										//uint8_t		eecicloFn = 33;					//09/FEB/2022			 	DC.B	33	;	16489 d	4069 h	;F7 - Cicleo para ventilador en modo nocturno	3 minutos ON. 3 minutos OFF
		[eetimedoor]=20, 										//uint8_t		eetimedoor = 20;				//09/FEB/2022				DC.B	20	;	16490 d	406A h	;F8 - Tiempo mínimo de puerta cerrada para entrar a nocturno	2 horas
		[eeparamSr]=20, 										//uint8_t		eeparamSr = 20;					//09/FEB/2022			 	DC.B	20	;	16491 d	406B h	;F9 - Pre-salida del modo Nocturno	2 horas
		[eemargdes]=35, 										//uint8_t		eemargdes = 35;					//09/FEB/2022			 	DC.B	35	;	16492 d	406C h	;FA - Margen de descarte 	3.5 horas
		[eetimeluzoff]=0, 										//uint8_t		eetimeluzoff = 0;				//09/FEB/2022				DC.B	0	;	16493 d	406D h	;FB - Retardo apagado de pancarta al entrar a nocturno	0 horas
		[eeFC_b]=0, 										//uint8_t		eeFC_b = 0;							//09/FEB/2022			  DC.B	0	;	16494 d	406E h	;FC -
		[eetiempoAhorro1]=20, 										//uint8_t		eetiempoAhorro1 = 20;		//09/FEB/2022		  	DC.B	20	;	FD 	2.0 horas
		[eetiempoAhorro2]=10, 										//uint8_t		eetiempoAhorro2 = 10;		//09/FEB/2022			  DC.B	10	;	FE 	1.0 horas
		[eeFF_b]=0, 										//uint8_t		eeFF_b = 0;							//09/FEB/2022			  DC.B	0	;	16497 d	4071 h	;FF -

		//;------------------  GRUPO DISPONIBLE  -----------------------
		//;------------------  Grupo Disponible  -----------------------
		[eeAddModBus]=222, 										//uint8_t		eeAddModBus = 222;		//23/MAR/2022			      	DC.B	0	;	16498 d	4072 h	:D0 -
		[eepassword]=73, 										//uint8_t		eepassword = 73;						//09/FEB/2022			      	DC.B	0	;	16499 d	4073 h	:D1 -
		[eeD2_b]=0, 										//uint8_t		eeD2_b = 0;						//09/FEB/2022			      	DC.B	0	;	16500 d	4074 h	;D2 -	               0 = modelo de refrigerador 0
		//uint8_t		eeD3_b = 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -
		//uint8_t		eeD4_b = 0;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -
		//uint8_t		eeD5_b = 0;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -
		//uint8_t		eeD6_b = 0;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -
		//uint8_t	 eemodelo	        =	0x50	;
		//uint8_t	 eemodelo	        =	0x4B	; // cambia de 0x50 a 0x51 por cambio de modulo BLE a WiFi+BLE
		[eecliente]=0, 										//uint8_t		eecliente 			= 0;						//09/FEB/2022			      	DC.B	0	;	16501 d	4075 h	;D3 -	     0 = GENERICO
		[eeclienteELTEC]=02, 										//uint8_t		eeclienteELTEC 	= 02;						//09/FEB/2022			      	DC.B	0	;	16502 d	4076 h	;D4 -	     2 = IMBERA
		[eehardware]=02, 										//uint8_t		eehardware			= 11;						//09/FEB/2022			      	DC.B	0	;	16503 d	4077 h	;D5 -	    1.0
		[eemodelo0]='F', 										//uint8_t		eemodelo0				= 69;						//09/FEB/2022			      	DC.B	0	;	16504 d	4078 h	;D6 -	    'E'
		[eemodelo]='7', 										//uint8_t	 	eemodelo	      =	56;           //

		//;------------------  IDENTIFICADOR Y BYTES DISPONIBLES  -----------------------
		//;------------------  Identificador y bytes disponibles  -----------------------
		[eeparam_ID]=0, 										//uint8_t		eeparam_ID = 0;				//09/FEB/2022	    DC.B	0	;	16506 d	407A h	;B0 – Identificador de parámetros programados	00: fábrica, 01: teclado, XX: llave fija
		[eeversion1]=00, 										//uint8_t	 eeversion1				=	00		;// versión 0.03
		[eeversion2]=01, 										//uint8_t	 eeversion2	      =	20		;
		//uint8_t	 eeversion2	      =	02		;//RM_20230908 VFW 0.002 Ajuste en calibración y envio de MAC a llave
		[eeplantilla1]=0, 										//uint8_t	 eeplantilla1     =	0			;
		[eeplantilla2]=01, 										//uint8_t	 eeplantilla2	    =	10		;

		[eedato_seg3]=0xCC, 										//uint8_t		eedato_seg3 = 0xCC;		//09/FEB/2022	    DC.B	$CC	;	16511 d	407F h	;Dato de seguridad 3
		// --------- borde 128 bytes EEPROM
};

/*
 * En esta Sección comienza las variables Sueltas de la EEPROM de 8 bits
 */

//		[128]=56, 										//uint8_t		eedato_string [512] = {0};		//RM_20220707 Espacio para que no se declaren juntas las variables de calibración
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t		eevolt_div = 70; //64; 		//1;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t		eevolt_mul = 100; //100;	    // 1;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t		eef_voltaje = 0;

__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t eeEstado1 = 0x05;	//	uint8_t		eeEstado1 = 0x05;	// inicia con refrigerador encendido y lámpara encedida y cerradura en off
//		[133]=56, 										//uint8_t		eeEstado2 = 0;
//		[134]=56, 										//uint8_t		eeEstado3 = 0;
//		[135]=56, 										//uint8_t		eeEstado4 = 0;
//
//		[136]=56, 										//uint8_t		eedato_espacio_01 [8] = {0};
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t eeTimeUnix1 = 0;	//	uint8_t		eeTimeUnix1 = 0;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t eeTimeUnix2 = 0;	//	uint8_t		eeTimeUnix2 = 0;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t eeTimeUnix3 = 0;	//	uint8_t		eeTimeUnix3 = 0;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t eeTimeUnix4 = 0;	//	uint8_t		eeTimeUnix4 = 0;
//		[141]=56, 										//uint8_t		eedato_espacio_02 [8] = {0};
//
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat1 = 1;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat2 = 2;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat3 = 3;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLat4 = 4;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong1 = 5;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong2 = 6;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong3 = 7;
__attribute__((section(".myBufSectionEEPROM_V"))) uint8_t	eeLong4 = 8;

/*
 * En esta Sección comienza las variables Sueltas de la EEPROM de 16 bits
 */

__attribute__((section(".myBufSectionEEPROM_V"))) uint16_t eeCntRegDATA = 0;	//
__attribute__((section(".myBufSectionEEPROM_V"))) uint16_t eeCntRegEVENT = 0;	//

/*
 * Copia de los datos de la memoria EEPROM que se almacenan en RAM
 */

uint8_t	reePlantilla [128];

uint8_t	reevolt_div = 1; //64; 		//1;
uint8_t	reevolt_mul = 1; //100;	    // 1;
uint8_t	reef_voltaje = 0;
uint8_t reeEstado1 = 0x05;	//	uint8_t		eeEstado1 = 0x05;	// inicia con refrigerador encendido y lámpara encedida y cerradura en off
uint8_t reeTimeUnix1 = 0;	//	uint8_t		eeTimeUnix1 = 0;
uint8_t reeTimeUnix2 = 0;	//	uint8_t		eeTimeUnix2 = 0;
uint8_t reeTimeUnix3 = 0;	//	uint8_t		eeTimeUnix3 = 0;
uint8_t reeTimeUnix4 = 0;	//	uint8_t		eeTimeUnix4 = 0;
uint8_t	reeLat1 = 1;
uint8_t	reeLat2 = 2;
uint8_t	reeLat3 = 3;
uint8_t	reeLat4 = 4;
uint8_t	reeLong1 = 5;
uint8_t	reeLong2 = 6;
uint8_t	reeLong3 = 7;
uint8_t	reeLong4 = 8;

uint16_t reeCntRegDATA = 0;	//
uint16_t reeCntRegEVENT = 0;	//






//LN_1013:	#pragma section @near {data}

uint8_t data_buffer[128] = {0};	// RAM reservada para buffer de datos para grabación de bloques
uint8_t event_buffer[128] = {0};	// RAM reservada para buffer de datos para grabación de bloques



uint8_t ProgMemCode			= 0;
uint8_t *dirPointer			= NULL;	// Apuntadorpara seleccionar la direccion donde seran grabados los datos de la ram
uint8_t *dataPointer		= NULL;	// Apuntador para seleccionar los datos en ram que seran grabados

uint16_t  cntLogger_H		= 0;	//	contador en segundos para loggear datos
uint8_t	  cntBlockFlash		= 0;	//	contador de bloques de Flash grabados (con bloques de 128bytes se pueden grabar hasta 32k de memoria)
uint8_t   cntByteBlock		= 0;	// contador de bytes grabados en buffer de datos
uint8_t	  cntByteLogg		= 0;	// contador de bytes grabados en cada loggeo

uint8_t	 numBlock		= 0;	// número de bloques a grabar
uint8_t  numByte		= 0;	// número de bytes a cargar
uint8_t  *dirLogger			= NULL;	//(uint16_t) dirección de inicio del logger a grabar
uint8_t  *dirBuffer			= NULL;	//(uint16_t) dirección de inicio del buffer a cargar
uint8_t  *dirDataLoad		= NULL;	// dirección de inicio del buffer a cargar
uint8_t  cntByteBlockDATA	= 0;	// almacena los bytes cargados al bloque de DATOS
uint8_t  cntBlockDATA		= 0;	// almacena los bloques de DATOS guardados
uint8_t  cntByteBlockEVENT	= 0;	// almacena los bytes cargados al bloque de EVENTOS
uint8_t  cntBlockEVENT			= 0;	// almacena los bloques de EVENTOS guardados


uint64_t bufferPageDATA[256] = {0};		// CGM 16/04/2025;	Buffer para el almacenamiento temporal de una Pagina de Logger Datos
uint64_t bufferPageEVENT[256] = {0};	// CGM 16/04/2025;	Buffer para el almacenamiento temporal de una Pagina de Logger Eventos
uint64_t * dirBufferPage = NULL;		// CGM 16/04/2025; 	Apuntador para los buffers bufferPageDATA y/o bufferPageEVENT

uint16_t timeSeconds_HW	= 0;	//
uint16_t timeSeconds_LW	= 0;	// variable de tiempo UNIX 1970

// Bloque de data logger
uint8_t BloqDatalooger [9] = {0};
//@near uint16_t timeSeconds_HW	= 0;	//
//@near uint16_t timeSeconds_LW	= 0;	// variable de tiempo UNIX 1970
//@near uint16_t tempAmbLogg		= 0;	// Copia de temperatura ambiente para cargar al buffer de datos
//@near uint16_t tempEvaLogg		= 0;	// Copia de temperatura evaporador para cargar al buffer de datos
//@near uint8_t voltLogg				= 0;	// voltaje de AC
// Fin del bloque de data logger

_Bool actuadores_TD[8] = {0}; 	// uint8_t actuadores_TD		= 0;	// Registro de actuadores en tiempo real
uint8_t estados_TD		= 0;	// Registro de estados en tiempo real
_Bool alarmas2_TD[8] = {0};	// uint8_t alarmas2_TD			= 0;	// Registro de alarma en tiempo real
uint8_t alarmas_TD			= 0;	// Registro de alarma en tiempo real


// Bloque Header
uint8_t Bloque_Header [size_Header] = {0};
//uint8_t softVersion1		= 0;	// versión del software
//uint8_t softVersion2		= 0;	// versión del software
//uint16_t bufferSize_HW	= 0;	//
//uint16_t bufferSize_LW	= 0;	// número de registros (bloques) en el buffer
//uint8_t dataType				= 0;	// tipo de datos en el Buffer
//uint8_t dataSize				= 0;	// Tamaño de cada registro (bloque) en el buffer
// Fin del bloque Header

// Bloque handshake
uint8_t Bloque_handshake [size_handshake] = {0};
//uint8_t comando1			= 0;	// 0xF1
//uint8_t comando2			= 0;	// 0x3F
//uint8_t macAdress[12]	= {0};
//uint8_t model				= 0;	// Modelo del dispositivo
//uint8_t versionSoft1		= 0;	// Versión del software entero
//uint8_t versionSoft2		= 0;	// Versión del software	fracción
//uint8_t plantParam1			= 0;	// Numero de plantilla de parámetros parte alta
//uint8_t plantParam2			= 0;	// Numero de plantilla de parámetros parte baja
//uint8_t checkList1			= 0;
//uint8_t checkList2			= 0;
// Fin del bloque handshake

// Bloque de estado en tiempo real
uint8_t Bloque_TiempoReal [size_TiempoReal] = {0};
//uint16_t tempAmb_RT			= 0;	// Copia de temperatura ambiente
//uint16_t tempEva_RT			= 0;	// Copia de temperatura evaporador
//uint16_t tempSal_RT			= 0;	// Copia de temperatura de salida
//uint8_t volt_RT				= 0;	// voltaje de AC
//uint8_t actuadores_RT		= 0;	// Registro de alarmas en tiempo real
//uint8_t alarmas2_RT			= 0;	// Registro de alarma en tiempo real
//uint8_t alarmas_RT			= 0;	// Registro de alarma en tiempo real
// Fin del bloque de estado en tiempo real


// Flags:  flagsLogger		[8] = {0};	// banderas para logger
_Bool 	flagsLogger		[8] = {0};	// banderas para logger

uint8_t 	*pointInitTx	= 0;	// apunta inicio de la Flash a transmitir
uint8_t 	*pointEndTx		= 0;	// apunta final de la Flash a transmitir
uint8_t 	*pointTx		= 0;	// apuntador de datos a transmitir
uint8_t 	cntByteTx				= 0;	// contador de bytes cargados para transmisión

uint8_t 	keyTx			= 0;	//

uint8_t cntInitTx				= 0;	//


// Flags:  uint8_t flagsTX					= 0;	// banderas para transmisión
_Bool  		flagsTX [8] = {0};
//_Bool  		flagsTX [0] = 0;			// toma en cuenta que la memoria ya se llenó al menos una vez
//_Bool  		flagsTX [1] = 0;			// indica que ya dió la vuelta
//_Bool  		flagsTX [2] = 0;			// Indica  transmitir Header
//_Bool  		flagsTX [3] = 0;			// evita que se mande checksum
//----------------
//_Bool  		flagsTX [7] = 0;			// bandera de dispositivo conectado BLE

uint8_t 	codeTX = 0;	//
uint8_t 	*loggerStart		= 0;	// Dirección de inicio de una sección de logger
uint8_t 	*loggerEnd			= 0;	// Dirección de fin de una sección de logger
uint8_t		blockSizeTX			= 0;	// Tamaño del bloque de transmisión
uint8_t		offsetInitTX		= 0;	// Offset del puntero de inicio en caso de ser necesario
uint8_t		sizeTX				= 0;	// Tamaño de transmisión
uint32_t 	chksum_32_HW_LW		= 0;	// chksum para comunicaciones


uint8_t		ImberaProtocolBuffer[144] = "";

// Flags:  uint8_t  flagsEvent		= 0;	// banderas para logger de eventos
_Bool  		flagsEvent [8] = {0};

uint16_t numBloqFirm	= 0;	// número de bloques de 128 bytes que componen el firmware
uint16_t cntBloqFirm	= 0;	// contador de bloques de 128 bytes que componen el firmware
//uint16_t chksumFirm_HW	= 0;	//
//uint16_t chksumFirm_LW	= 0;	// chksum para comunicaciones
uint32_t chksumFirm_HW_LW	= 0;

uint16_t temp_doorEvent	= 0;	// Registro para contar duración minima de apertura de puerta


// Flags:  uint8_t flagsRxFirm			= 0;	// banderas para recepción de Firmware
_Bool  		flagsRxFirm	[8] = {0};			// banderas para recepción de Firmware
//_Bool  		flagsRxFirm [0] = 0;		// Empezó recepción de Firmware ?
//_Bool  		flagsRxFirm [1] = 0;		// Ya se sabe cuantos bloques se van a recibir ?
//_Bool  		flagsRxFirm [2] = 0;		// indica que ya llegó un paquete




uint8_t keyWrFirm			= 0;	//
uint8_t timeOutRst			= 0;	//
uint8_t timeOutRx				= 0;	//


uint8_t timeUnlockWIFI	= 0;

uint8_t 	timeRstBLE	= 0;	//

// Bloque de estado en tiempo real
uint8_t BloqEventPuerta [18] = {0};
//uint16_t EP_timeInit_HW		= 0;	//
//uint16_t EP_timeInit_LW		= 0;	// tiempo de inicio del evento
//uint16_t EP_timeEnd_HW		= 0;	//
//uint16_t EP_timeEnd_LW		= 0;	// tiempo final del evento
//uint8_t  EP_eventType			= 0;	// tipo de evento
//uint16_t EP_tempAmbInit		= 0;	// Copia de temperatura ambiente
//uint16_t EP_tempEvaEnd		= 0;	// Copia de temperatura evaporador
//uint8_t  EP_voltInit				= 0;	// voltaje de AC
// Fin del bloque de evento puerta


// Bloque de evento compresor
uint8_t BloqEventComp [18] = {0};
//uint16_t EC_timeInit_HW		= 0;	//
//uint16_t EC_timeInit_LW		= 0;	// tiempo de inicio del evento
//uint16_t EC_timeEnd_HW		= 0;	//
//uint16_t EC_timeEnd_LW		= 0;	// tiempo final del evento
//uint8_t EC_eventType			= 0;	// tipo de evento
//uint16_t EC_tempAmbInit		= 0;	// Copia de temperatura ambiente
//uint16_t EC_tempEvaEnd		= 0;	// Copia de temperatura evaporador
//uint8_t EC_voltInit				= 0;	// voltaje de AC
// Fin del bloque de evento compresor

// Bloque de evento deshielo
uint8_t BloqEventDesh [18] = {0};
//@near uint16_t ED_timeInit_HW		= 0;	//
//@near uint16_t ED_timeInit_LW		= 0;	// tiempo de inicio del evento
//@near uint16_t ED_timeEnd_HW		= 0;	//
//@near uint16_t ED_timeEnd_LW		= 0;	// tiempo final del evento
//@near uint8_t ED_eventType			= 0;	// tipo de evento
//@near uint16_t ED_tempAmbInit		= 0;	// Copia de temperatura ambiente
//@near uint16_t ED_tempEvaEnd		= 0;	// Copia de temperatura evaporador
//@near uint8_t ED_voltInit				= 0;	// voltaje de AC
// Fin del bloque de evento deshielo

// Bloque de evento power-on
uint8_t BloqEventPwrOn [18] = {0};
//@near uint16_t EPo_timeInit_HW	= 0;	//
//@near uint16_t EPo_timeInit_LW	= 0;	// tiempo de inicio del evento
//@near uint16_t EPo_timeEnd_HW		= 0;	//
//@near uint16_t EPo_timeEnd_LW		= 0;	// tiempo final del evento
//@near uint8_t EPo_eventType			= 0;	// tipo de evento
//@near uint16_t EPo_tempAmbInit	= 0;	// Copia de temperatura ambiente
//@near uint16_t EPo_tempEvaEnd		= 0;	// Copia de temperatura evaporador
//@near uint8_t EPo_voltInit			= 0;	// voltaje de AC
// Fin del bloque de evento power-on

uint8_t timeTxTWF			= 0;	//
_Bool flagsEventWF[8] = {0};// uint8_t flagsEventWF		= 0;
_Bool flagsTX2[8] = {0};// uint8_t flagsTX2	= 0;	// banderas para transmisi�n
uint16_t timeoutTWF = 0;

uint16_t 	timeDataWF_H		= 0;	//	contador en segundos para loggear datos
_Bool 	flagsWIFI[8] = {0};  // banderas Wifi

uint8_t bufferWifiTx[30] = {0};

uint8_t blockLoggWifi			= 0;
uint8_t byteLoggWifi			= 0;
uint16_t cntRegTxWifi			= 0;

uint8_t timeoutTxWifi			= 0;
uint8_t delayTxLoggWifi			= 0;


// Flags:  uint8_t flagsLogger2			= 0;	// banderas para logger
_Bool  		flagsLogger2 [8] = {0};	// banderas para logger

uint16_t 	cnt_gen_ms		= 0;					//tiempo de buzzer

uint16_t t_buzzer		= 0;

uint8_t edo_buzzer		= 0;
uint16_t buzzer_ton		= 0;
uint16_t buzzer_toff	= 0;


_Bool ban_buzzer[8] = {0};		// uint8_t ban_buzzer		= 0;
_Bool flagsBuzzer[8] = {0};		// uint8_t flagsBuzzer		= 0;

uint16_t 	 silencioAlarmH		= 0;

_Bool flagsTC[3] = {0};		// uint8_t flagsTC		= 0;


uint8_t bufferTxControl[144] = "";

uint16_t numRegTx = 0;
uint16_t cntRegDATA			= 0;	//
uint16_t cntRegEVENT			= 0;	//

uint16_t 	cntReg			= 0;	//
uint16_t 	*cntRegPNT		= 0;	//


_Bool  	estado1 [8]= {0};
_Bool  	estado2 [8]= {0};
//uint8_t		estado1 = 0;
_Bool  	estado3 [8]= {0};
_Bool  	estado4 [8]= {0};

uint8_t		cntMsgCmd = 0;
uint8_t		numMsg = 0;
uint8_t		retLampOff = 0;


_Bool  	flagsTime [8]= {0};		// banderas para tiempo


uint8_t timeBCD_year			= 0;
uint8_t timeBCD_month			= 0;
uint8_t timeBCD_day				= 0;
uint8_t timeBCD_hour			= 0;
uint8_t timeBCD_min				= 0;
uint8_t timeBCD_sec				= 0;

RTC_TimeTypeDef hRtcTime;
RTC_DateTypeDef hRtcDate;

uint16_t time_year		= 0;
uint8_t time_month		= 0;
uint8_t time_day		= 0;
uint8_t time_hour		= 0;
uint8_t time_min		= 0;
uint8_t time_sec		= 0;



uint32_t timeUNIX		= 0;
//@near uint32_t daysToMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
uint32_t daysToMonth[12] = {0,31,59,90,120,151,182,213,243,273,304,334};
uint8_t leapYears		= 0;

uint8_t timeBuzzOn		= 0;
uint8_t retPowerOn		= 0;

uint16_t	tsac_w = 0;
uint16_t	tDisplay_w = 0;


uint8_t debBtn2F3		= 0;
_Bool  	flagsBattery[8]	= {0};
//uint8_t flagsBattery		= 0;

uint8_t cntHoldP			= 0;
uint8_t typeClock		= 0;

uint8_t 	fCOM = 0;
uint16_t 	timeClearComst = 0;
uint16_t 	timeClearComst1 = 0;     //RM_20240425 Segundo timer para estado de comunicaci�n MODBUS

uint8_t countResetI2C = 0;

GPIO_TypeDef * PFULLDEF_Px_PORT[5] = {PFULLDEF_P1_PORT, PFULLDEF_P2_PORT, PFULLDEF_P3_PORT, PFULLDEF_P4_PORT, PFULLDEF_P5_PORT};
uint8_t PFULLDEF_Px_PIN[5] = {PFULLDEF_P1_PIN, PFULLDEF_P2_PIN, PFULLDEF_P3_PIN, PFULLDEF_P4_PIN, PFULLDEF_P5_PIN};

GPIO_TypeDef * PFULLDEF_MPx_PORT[4] = {PFULLDEF_MP0_PORT, PFULLDEF_MP1_PORT, PFULLDEF_MP2_PORT, PFULLDEF_MP3_PORT};
uint8_t PFULLDEF_MPx_PIN[4] = {PFULLDEF_MP0_PIN, PFULLDEF_MP1_PIN, PFULLDEF_MP2_PIN, PFULLDEF_MP3_PIN};

_Bool	FlagPuertaX [sizeMPx*sizePx] = {0}; // FlagPuertaX [Numero de Puertos MP][Numero de Puertos P] CGM 13/01/2025
uint8_t	FlagPuerta10Times [sizeMPx*sizePx] = {0}; // Bandera que muestrea 10 veces
uint8_t countWaitPuerta = 0;
_Bool firstFlagPuerta1 = 1;
uint8_t countMPx = 0;

_Bool    bandera_RTC = 0;
uint32_t Count_Test2 = 0;    //JTA eliminar buzzer inicial
_Bool    bandera_act_fw_j = 0;
uint32_t timeReg;
uint32_t dateReg;
// uint8_t 	prescalaI2c = 0;     //Variable prescala I2C temporal

uint8_t timeBCD_sec_ANT		= 0;

_Bool flagsTxControl[8]		= {0};
uint8_t delayComStat		= 0;
uint8_t DevLock = 0 ;
uint8_t statComFlag = 0 ;
uint8_t statComWIFIFlag = 0 ;
uint16_t cntSetName = 0 ;
//uint8_t difName[50] = "BLE_AT+NAMEIMBERA-CTOF-F\r\n";
uint8_t difName[50] = "BLE_AT+NAMEIMBERA-HEALTH\r\n";
uint8_t timeTxTBLE			= 0;	//
uint16_t timeoutTBLE = 0;

uint8_t timePreDh_h = 0;
uint8_t timePreDh_l = 0;

// Bloque de evento WiFi EX
uint8_t BloqEventWiFiEx [18] = {0};
//uint16_t comandoWF = 0;	//
//uint8_t softVersion1WF = 0;	// versión del software
//uint8_t softVersion2WF = 0;	// versión del software
//// Bloque de evento WiFi
//uint16_t WF_timeInit_HW		= 0;	//
//uint16_t WF_timeInit_LW		= 0;	// tiempo de inicio del evento
//uint16_t WF_timeEnd_HW		= 0;	//
//uint16_t WF_timeEnd_LW		= 0;	// tiempo final del evento
//uint8_t WF_eventType			= 0;	// tipo de evento
//uint16_t WF_tempAmbInit		= 0;	// Copia de temperatura ambiente
//uint16_t WF_tempEvaEnd		= 0;	// Copia de temperatura evaporador
//uint8_t WF_voltInit				= 0;	// voltaje de AC
//// Fin del bloque de evento WiFi



// para reservar memoria Flash dedicada a Logger
//#pragma section @near {eventLogger}
__attribute__((section(".eventLogger"))) uint8_t eventLogger[20480-1] = {0};
__attribute__((section(".eventLogger"))) uint8_t eventLoggerFin = 0;


//#pragma section @near {dataLogger}
__attribute__((section(".dataLogger"))) uint8_t dataLogger[12288-1] = {0};
__attribute__((section(".dataLogger"))) uint8_t dataLoggerFin = 0;


// #pragma section @near {varFlash}
// Nota: Esta seccion debe ir en Flash
__attribute__((section(".varFlash"))) uint8_t  versionFirm1 = 0;
__attribute__((section(".varFlash"))) uint8_t  versionFirm2 = 01;
//@near uint8_t versionFirm2 = 02;  //RM_20230908 VFW 0.002 Ajuste en calibración y envio de MAC a llave
__attribute__((section(".varFlash"))) uint8_t  fm_hardware = 02;
__attribute__((section(".varFlash"))) uint8_t  fm_modelo0 = 'F';  //'E'
__attribute__((section(".varFlash"))) uint8_t  fm_modelo  = '7';  //'8'

__attribute__((section(".BOOTLOADER"))) uint32_t crudo_bootloader[1024]= {

0x20008000,
0x0801F339,
0x0801F2AD,
0x0801F2B5,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x0801F2BD,
0x00000000,
0x00000000,
0x0801F2C7,
0x0801F2D1,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x0801F389,
0x4C06B510,
0x2B007823,
0x4B05D107,
0xD0022B00,
0xE0004804,
0x2301BF00,
0xBD107023,
0x20000090,
0x00000000,
0x0801F878,
0xB5104B04,
0xD0032B00,
0x48044903,
0xBF00E000,
0x46C0BD10,
0x00000000,
0x20000094,
0x0801F878,
0xAF00B580,
0x8F4FF3BF,
0x4B0446C0,
0x60DA4A04,
0x8F4FF3BF,
0x46C046C0,
0x46C0E7FD,
0xE000ED00,
0x05FA0004,
0xB089B590,
0x6078AF00,
0xF992F000,
0xD0011E03,
0xF8B0F000,
0x193B2410,
0x23100018,
0x2100001A,
0xFB68F000,
0x187B0021,
0x60DA2201,
0x687A187B,
0x187B609A,
0x601A2202,
0x60FB2300,
0x18FA230C,
0x0011187B,
0xF0000018,
0xF000FA21,
0x1E03F995,
0xF000D001,
0x46C0F88F,
0xB00946BD,
0x0000BD90,
0xB082B580,
0xF000AF00,
0xB672F863,
0x4B2A46C0,
0x601A4A2A,
0x607B2300,
0x4B29E049,
0xF0000018,
0x4B28FB29,
0x0018681B,
0xFFBAF7FF,
0xF950F000,
0xD0011E03,
0xF86EF000,
0x603B2300,
0x4B1EE020,
0x681A681B,
0x4920685B,
0x604B600A,
0x4B1F46C0,
0x4B1D6819,
0x685B681A,
0xF0002001,
0x1E03F8EB,
0x4B15D1F5,
0x3308681B,
0x4B13001A,
0x4B17601A,
0x3308681B,
0x4B15001A,
0x683B601A,
0x603B3301,
0x2BFF683B,
0xF000D9DB,
0x1E03F947,
0xF000D001,
0x4B0CF841,
0x3340681B,
0xF7FF0018,
0x4B09FF81,
0x1C5A681B,
0x601A4B07,
0x3301687B,
0x687B607B,
0xD9B22B3D,
0xFF62F7FF,
0x200000C0,
0x08020000,
0x200000AC,
0x200000D0,
0x200000C8,
0x20000000,
0xAF00B580,
0x4A0E4B0D,
0x4B0C601A,
0x605A2200,
0x22FA4B0A,
0x60DA0112,
0x22FA4B08,
0x609A0112,
0x22004B06,
0x4B05611A,
0xF0000018,
0x1E03FA31,
0xF000D001,
0x46C0F807,
0xBD8046BD,
0x200000AC,
0x40003000,
0xAF00B580,
0x46C0B672,
0xE7FD46C0,
0xAF00B580,
0xE7FD46C0,
0xAF00B580,
0xE7FD46C0,
0xAF00B580,
0x46BD46C0,
0xB580BD80,
0x46C0AF00,
0xBD8046BD,
0xAF00B580,
0xF85AF000,
0x46BD46C0,
0x0000BD80,
0xB082B580,
0x4B12AF00,
0x05122280,
0x4A11609A,
0x58D32380,
0x4B0F607B,
0x603B6A1B,
0x2380687A,
0x4013025B,
0x683BD010,
0x401322FF,
0xD00B2BCC,
0x22FF683B,
0x2BAA4013,
0x4B06D006,
0x4B05681A,
0x02C92180,
0x601A430A,
0x46BD46C0,
0xBD80B002,
0xE000ED00,
0x40022000,
0x4685480D,
0xFFD0F7FF,
0x490D480C,
0x23004A0D,
0x58D4E002,
0x330450C4,
0x428C18C4,
0x4A0AD3F9,
0x23004C0A,
0x6013E001,
0x42A23204,
0xF000D3FB,
0xF7FFFA63,
0xE7FEFF0F,
0x20008000,
0x20000000,
0x20000090,
0x0801F8A8,
0x20000090,
0x200000F4,
0x0000E7FE,
0xAF00B580,
0x681A4B04,
0x681B4B04,
0x4B0218D2,
0x46C0601A,
0xBD8046BD,
0x200000D4,
0x20000004,
0xAF00B580,
0x681B4B02,
0x46BD0018,
0x46C0BD80,
0x200000D4,
0xB086B5B0,
0x60F8AF00,
0x603A60B9,
0x4B21607B,
0x2B01781B,
0x2302D101,
0x4B1EE038,
0x701A2201,
0x22004B1C,
0x2517605A,
0x23FA197C,
0x0018009B,
0xF874F000,
0x70230003,
0x781B197B,
0xD11F2B00,
0x2B0168FB,
0x683AD106,
0x68B9687B,
0xF0000008,
0xE005F8B3,
0x68BB683A,
0x00180011,
0xFA38F000,
0x18FC2317,
0x009B23FA,
0xF0000018,
0x0003F857,
0x4B097023,
0x68FB695A,
0x4B0743D9,
0x615A400A,
0x22004B04,
0x2317701A,
0x781B18FB,
0x46BD0018,
0xBDB0B006,
0x200000D8,
0x40022000,
0xB082B580,
0x1DFBAF00,
0x701A2200,
0x695B4B0B,
0xDA0C2B00,
0x4A0A4B09,
0x4B08609A,
0x609A4A09,
0x695B4B06,
0xDA022B00,
0x22011DFB,
0x1DFB701A,
0x0018781B,
0xB00246BD,
0x46C0BD80,
0x40022000,
0x45670123,
0xCDEF89AB,
0xB082B580,
0x1DFBAF00,
0x701A2201,
0x695A4B09,
0x21804B08,
0x430A0609,
0x4B06615A,
0x2B00695B,
0x1DFBDA02,
0x701A2200,
0x781B1DFB,
0x46BD0018,
0xBD80B002,
0x40022000,
0xB084B580,
0x6078AF00,
0xFF62F7FF,
0x687B0002,
0x60FB189B,
0x025B2380,
0xE00760BB,
0xFF58F7FF,
0x68FB0002,
0xD8014293,
0xE02A2303,
0x691B4B17,
0x401368BA,
0x4B15D1F2,
0x4A15691B,
0x60BB4013,
0x4A144B12,
0x68BB611A,
0xD0042B00,
0x68BA4B12,
0x2301605A,
0xF7FFE015,
0x0002FF3B,
0x189B687B,
0xE00760FB,
0xFF34F7FF,
0x68FB0002,
0xD8014293,
0xE0062303,
0x691A4B05,
0x02DB2380,
0xD1F14013,
0x00182300,
0xB00446BD,
0x46C0BD80,
0x40022000,
0x000083FA,
0x000083FB,
0x200000D8,
0xB084B5B0,
0x60F8AF00,
0x607B603A,
0x695A4B0B,
0x21014B0A,
0x615A430A,
0x683A68FB,
0xF3BF601A,
0x46C08F6F,
0x001C687B,
0x001D2300,
0x330468FB,
0x601A0022,
0x46BD46C0,
0xBDB0B004,
0x40022000,
0xB084B5B0,
0x6078AF00,
0x4B326039,
0x2B01781B,
0x2302D101,
0x4B2FE059,
0x701A2201,
0x22004B2D,
0x250F605A,
0x23FA197C,
0x0018009B,
0xFF7AF7FF,
0x70230003,
0x193B002C,
0x2B00781B,
0x687BD13F,
0x605A2201,
0x681B687B,
0xD10A2B04,
0xF846F000,
0x23FA193C,
0x0018009B,
0xFF64F7FF,
0x70230003,
0x683BE02D,
0x42522201,
0x687B601A,
0x60BB689B,
0x68BBE017,
0xF0000018,
0x250FF841,
0x23FA197C,
0x0018009B,
0xFF4EF7FF,
0x70230003,
0x781B197B,
0xD0032B00,
0x68BA683B,
0xE00A601A,
0x330168BB,
0x687B60BB,
0x687B689A,
0x18D368DB,
0x429A68BA,
0x4B09D3DF,
0x4B08695A,
0x438A2102,
0x4B05615A,
0x701A2200,
0x18FB230F,
0x0018781B,
0xB00446BD,
0x46C0BDB0,
0x200000D8,
0x40022000,
0xAF00B580,
0x695A4B04,
0x49044B03,
0x615A430A,
0x46BD46C0,
0x46C0BD80,
0x40022000,
0x00010004,
0xB084B580,
0x6078AF00,
0x695B4B08,
0x40134A08,
0x687B60FB,
0x68FB00DA,
0x4B04431A,
0x430A4905,
0x46C0615A,
0xB00446BD,
0x46C0BD80,
0x40022000,
0xFFFFFC07,
0x00010002,
0xB084B580,
0x6078AF00,
0x2B00687B,
0x2301D101,
0x687BE06D,
0xF0000018,
0x687BF875,
0x4A36681B,
0x687B601A,
0x4A35681B,
0x687B601A,
0x687A681B,
0x605A6852,
0x681B687B,
0x6892687A,
0xF7FF609A,
0x0003FE3F,
0xE00E60FB,
0xFE3AF7FF,
0x68FB0002,
0x2B211AD3,
0x687BD907,
0x68DB681B,
0x40132202,
0x2303D001,
0x687BE043,
0x68DB681B,
0x40132202,
0x687BD1EA,
0x2B00691B,
0x687BD105,
0x2280681B,
0x615A01D2,
0x687BE007,
0x687B691A,
0x21C0681B,
0x430A0209,
0xF7FF615A,
0x0003FE13,
0xE00E60FB,
0xFE0EF7FF,
0x68FB0002,
0x2B211AD3,
0x687BD907,
0x68DB681B,
0x4013220F,
0x2303D001,
0x687BE017,
0x68DB681B,
0x4013220F,
0x687BD1EA,
0x691A681B,
0x68DB687B,
0xD005429A,
0x681B687B,
0x68D2687A,
0xE003611A,
0x681B687B,
0x601A4A05,
0x00182300,
0xB00446BD,
0x46C0BD80,
0x0000CCCC,
0x00005555,
0x0000AAAA,
0xB082B580,
0x6078AF00,
0x46BD46C0,
0xBD80B002,
0xB082B580,
0x6078AF00,
0x681B687B,
0x601A4A03,
0x00182300,
0xB00246BD,
0x46C0BD80,
0x0000AAAA,
0x18820003,
0xD1004293,
0x70194770,
0xE7F93301,
0x2600B570,
0x4D0D4C0C,
0x10A41B64,
0xD10942A6,
0xF0002600,
0x4C0AF819,
0x1B644D0A,
0x42A610A4,
0xBD70D105,
0x58EB00B3,
0x36014798,
0x00B3E7EE,
0x479858EB,
0xE7F23601,
0x0801F8A0,
0x0801F8A0,
0x0801F8A4,
0x0801F8A0,
0x46C0B5F8,
0xBC08BCF8,
0x4770469E,
0x46C0B5F8,
0xBC08BCF8,
0x4770469E,
0x4802B401,
0xBC014684,
0xBF004760,
0x20000009,
0x0801F0E9,
0x0801F0C1,
0x08000000,
0x00000001,
0xB088B580,
0x6078AF00,
0x231F6039,
0x220018FB,
0x687B701A,
0x683B61BB,
0x4B1A617B,
0x4B19695A,
0x02C92180,
0x615A430A,
0x8310F3EF,
0x68FB60FB,
0xB672613B,
0xE00F46C0,
0x69BB697A,
0x601A6812,
0x3304697B,
0x69BB617B,
0x61BB3304,
0x187B211F,
0x187B781A,
0x701A3201,
0x18FB231F,
0x2B3F781B,
0x46C0D9EA,
0x691A4B07,
0x025B2380,
0xD1F94013,
0x60BB693B,
0xF38368BB,
0x46C08810,
0x46BD46C0,
0xBD80B008,
0x40022000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000};


uint16_t   foovar1[10];
uint16_t   *foovar2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART4_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
static void check_Tick_1ms(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
unsigned long millis(){

    return timemilis;
}

void reconfigura_perif(void)
{
//	HAL_IWDG_Refresh(&hiwdg);
	initEEPROMEmulated(); // Init EEPROM Emulated

	HAL_Init();
	SystemClock_Config();

	configura_perif_2();

	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

}

void configura_perif_2(void)
{
	  MX_GPIO_Init();
	  MX_DMA_Init();
	  MX_ADC1_Init();
	  MX_I2C1_Init();
	  MX_IWDG_Init();
	  MX_TIM3_Init();
	  MX_TIM1_Init();
	  MX_TIM6_Init();
	  MX_USART2_UART_Init();
	  MX_USART4_UART_Init();


}

//**********************************************************************************************************************************************************
//**********************************************************************************************************************************************************
static void check_Tick_1ms(void)
{

				presc_tick_1ms++;
				if(presc_tick_1ms >= 4)							// contador prescala 1ms =
					{
						presc_tick_1ms = 0;
						tick_1ms = 1;							// Manuel, levanta flag 1ms
						presc_tick_10ms++;
						timemilis++;							// variable contadora de milisegundos
					}
				if(presc_tick_10ms >= 10)						// contador prescala 10ms
					{
						presc_tick_10ms = 0;
						tick_10ms =1;							// flag base de tiempo 10ms
						presc_tick_100ms++;
					}
				if(presc_tick_100ms >= 10)						// contador prescala 100ms
					{
						presc_tick_100ms = 0;
						tick_100ms =1;							// flag base de tiempo 100ms
						presc_tick_1s++;
					}
				if(presc_tick_1s >= 10)							// contador prescala 1s
					{
						presc_tick_1s = 0;
						tick_1s =1;								// flag base de tiempo 1s
					}
		//Manuel 19-FEB-2021:		}
}
//**********************************************************************************************************************************************************
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	//initEEPROMEmulated(); // Init EEPROM Emulated
	timeRstBLE = 1;
	inicio ();

	ProcesosC = 6;


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  configura_perif_2();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  MX_RTC_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_USART4_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  initEEPROMEmulated(); // Init EEPROM Emulated

  Modbus_ModbusSalave ();
   //ModbusMaster_begin(eePlantilla[eeAddModBus]);		// Manuel 23-MAR-2022	ModbusMaster_begin(222);
  ModbusMaster_begin(reePlantilla[eeAddModBus]);
  TR485_Trasnmiting = 0;														//17-DIC-2021		El dispositivo inicialmente escucha
  HAL_GPIO_WritePin(GPIOC, PFULLDEF_MAX485_DE_AND_RE_NEG, GPIO_PIN_RESET);      //17-DIC-2021 El Driver inicalmente Escucha
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   BluetoothState = 3;			// Maquina de estados Bluetooth 1:Configuracion 2:Obtencion parametros 3:Tx/RX
     timeRstBLE = 8;
     BluetoothState = 1;
     HAL_GPIO_WritePin(PFULLDEF_VSEN, GPIO_PIN_SET);      //02-Jul-2024:  Habilita VSEN

     HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
     HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

     HAL_TIM_PWM_Start (&htim3,TIM_CHANNEL_2);			// Enciende PWM   JTA eliminar buzer inicial
     while(Count_Test2 < 260000)
     {
    	 HAL_IWDG_Refresh(&hiwdg);
   		Count_Test2++;//  eliminar JTA buzzer
   		if(Count_Test2 == 259999 ) //eliminar JTA buzzer
   		{
   			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2); //eliminar JTA buzzer
   			//Count_Test2 = 255;//eliminar JTA buzzer
   		}
     }
     Count_Test2 = 0;

     asm ("nop");

     for (int i = 0; i < 1000; i++)
     {
   	  HAL_Delay (1);
   	  HAL_IWDG_Refresh(&hiwdg);
     }


while (1)
{

   testTimmingProcess:
   	ProcesosC++;
       while ( !(TIM6->SR & 0x0001) ){
   		// Espera hasta que se cumpla el overflow del timer 6
   		// 125 us
   		asm ("nop");
   	}
   	// Borra bandera de overflow
   	TIM6->SR &= ~TIM_SR_UIF;
   	HAL_IWDG_Refresh( &hiwdg );

   	//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);	 //28-May-2024: Salida IO6 toogle test
     	RndNumber++;       //RM_20240304 Para agregar PASSWORD de seguridad BLE
         asm ("nop");


       main10();					// ASM: <<<-- TRADUCCION COMPLETA -->>>

       muestreo();

     	if(StateSleep == 0x55){
     		// C: Pendiente a Adaptar
     		//;Restablece el Tiempo RTC
     		asm ("nop");
     		asm ("nop");
     	}

     	retardoDeActuadores();		// ASM: <<<-- TRADUCCION COMPLETA -->>>

     	asm ("nop");
     	// procesa la base de tiempo para C y modbus
     	check_Tick_1ms();

     	/*
     	Falta parte por insertar
     	 */
     	if(timeRstBLE){
     		/* USART Disable */
     		// USART_Cmd(USART2, DISABLE);
           HAL_GPIO_WritePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI, GPIO_PIN_SET);      //28-May-2024:  Enciende Modulo WIFI
           //GPIOC->BSRR = GPIO_BSRR_BS_6;
     		BluetoothState = 1;
     	}
     	else{
     		/* USART Enable */
     		// USART_Cmd(USART2, ENABLE);
     		HAL_GPIO_WritePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI, GPIO_PIN_RESET);      //28-May-2024:  Apaga Modulo WIFI
     		//GPIOC->BSRR = GPIO_BSRR_BR_6;

     		// State Machine Bluetooth    10-MAR-2022
     		switch(BluetoothState){
     			case 1:
     				SetUpBluetooth_Ble();		break;		// 1:Configuracion  .... (( Adaptando ))
     			case 2:
     				GetParamBluetooth_Ble();	break;		// 2:Obtencion parametros
     			case 3:
     				TransmitReceive_Ble();		break;		// 3:transmision-recepcion
     		}
     	}

     	Read_Inpunts_ble();				// 14-Mar-2022

   //  	if (device_conected){
   //  		//ContadorDebugger++;
   //  		// #pragma asm
   //  		if(!flagsTX [7])
   //  			timeOutRst = 241;				// carga time out de resetcon 60 segundos
   //  		flagsTX [7] = 1;							// levanta bandera de dispositivo conectado
   //
   //  		if(flagsLogger2 [2])
   //  			flagsTX [7] = 0;
   //  	}
   //  	else{
   //  		if(flagsTX [7]){				// sólo si viene de una desconexión cancela la transmisión que estuviera en progreso
   //  			keyTx = 0;// en caso de desconexion cancela toda transmisión
   //  			//flagsTX = 0;
   //  			for(uint8_t i = 0; i < 8 ; i++ )
   //  				flagsTX [i] = 0;
   //  			// flagsRxFirm = 0;
   //  			for(uint8_t i = 0; i < 8 ; i++ )
   //  				flagsRxFirm [i] = 0;
   //  			flagsLogger [4] = 0;				// permite loggeo de eventos
   //  			flagsLogger [5] = 0;				// permite loggeo de datos
   //  		}
   //  		flagsTX [7] = 0;				// borra bandera de dispositivo conectado
   //  	}

     	if(tick_1s){
     	  asm ("nop");
     	  Count_Test++;
     	  // HAL_GPIO_TogglePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI);	//28-May-2024: Modulo WIFI toogle test
     	  // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);      //28-May-2024:  Enciende Modulo WIFI

     	  // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);	//PA7
     	  // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);	//PB0

     //	  uint8_t tx_buffWifiBle [] = "Cominicacion mandada desde DMA-UART2 \n";
     //	  HAL_UART_Transmit_DMA(&huart2, tx_buffWifiBle, strlen((char*)tx_buffWifiBle) );

           /*   Prueba de trasnmision por modbus
     	  HAL_GPIO_WritePin(GPIOC, PFULLDEF_MAX485_DE_AND_RE_NEG, GPIO_PIN_SET);      //28-May-2024:  Habilita trasnmision por modbus
     	  uint8_t tx_buffModbus [] = "Test de comunicacion por modbus \n";
     	  HAL_UART_Transmit_DMA(&huart4, tx_buffModbus, strlen((char*)tx_buffModbus) );
     	  */
     	    //  *** Prueba de activacion de rele compresor
     	    //portX[rel_co] =  portX[rel_co] 1;
     	    //  *** Prueba de activacion de rele compresor
     	}

     	if(tick_1ms == 1){
     		// tiempo para rutina buzzer
     		// #pragma asm
     		cnt_gen_ms++;//Incrementa el contador general de ms cada 1ms
     		// prescalaI2c++;
     	}
//   			    HAL_RTC_GetTime (&hrtc, &hRtcTime, RTC_FORMAT_BCD);
//   			    HAL_RTC_GetDate (&hrtc, &hRtcDate, RTC_FORMAT_BCD);
//   			    timeBCD_year = hRtcDate.Year;
//   		 	    timeBCD_month = hRtcDate.Month;
//   			    timeBCD_day = hRtcDate.Date;
//   			    timeBCD_hour = hRtcTime.Hours;
//   			    timeBCD_min = hRtcTime.Minutes;
//   			    timeBCD_sec = hRtcTime.Seconds;

   				uint32_t tmpreg1 = (uint32_t)(READ_REG(RTC->TR) & RTC_TR_RESERVED_MASK);
     			uint32_t datetmpreg1 = (uint32_t)(READ_REG(RTC->DR) & RTC_DR_RESERVED_MASK);

     		  	timeBCD_year = (uint8_t)((datetmpreg1 & (RTC_DR_YT | RTC_DR_YU)) >> RTC_DR_YU_Pos);
     		  	timeBCD_month = (uint8_t)((datetmpreg1 & (RTC_DR_MT | RTC_DR_MU)) >> RTC_DR_MU_Pos);
     		  	timeBCD_day = (uint8_t)((datetmpreg1 & (RTC_DR_DT | RTC_DR_DU)) >> RTC_DR_DU_Pos);
     		  	timeBCD_hour = (uint8_t)((tmpreg1 & (RTC_TR_HT | RTC_TR_HU)) >> RTC_TR_HU_Pos);
       	  		timeBCD_min = (uint8_t)((tmpreg1 & (RTC_TR_MNT | RTC_TR_MNU)) >> RTC_TR_MNU_Pos);
     		  	timeBCD_sec = (uint8_t)((tmpreg1 & (RTC_TR_ST | RTC_TR_SU)) >> RTC_TR_SU_Pos);

     		  	timeBCD_to_UNIX();
     		  	//------------------------------------------------------------------------------------------
     		  	calculando_tiempo_UNIX ();
   //------------------------------------------------------------------------------------------
//     		  		if(!flagsTime[f_timeConfigRTC]){
//     		  			goto noActTime;
//     		  		}

     		  		timeSeconds_HW = (uint16_t) ((timeUNIX)>>16);
     		  		timeSeconds_LW = (uint16_t) (timeUNIX&0xFFFF);

     		  		if(timeBCD_sec_ANT == timeBCD_sec){
     		  			goto no_inc_cnt_sec;
     		  		}
     		  		timeBCD_sec_ANT = timeBCD_sec;
     		  		decword(&cntLogger_H);
     		  no_inc_cnt_sec:
     		  noActTime:
   		  asm ("nop");

     	  	switch(ProcesosC)
     	  	{
     	  		case 0:
     	  			comunicacion();
     	  			break;		// ASM: Pendiente a traducir
     	  		case 1:
     	  			voltmetro();
     	  			break;		// ASM: Pendiente a traducir
     	  		case 2:
     	  			temper();
     	  			break;		// ASM: Pendiente a traducir ..... Julio Torres
     	  		case 3:
     	  //			if(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC)){
     	  //				 asm ("nop");
     	  //			}
     	  			if(USART4->ISR & USART_ISR_TC){
     	  			      HAL_GPIO_WritePin(GPIOC, PFULLDEF_MAX485_DE_AND_RE_NEG, GPIO_PIN_RESET);      //17-DIC-2021 El Driver inicalmente Escucha
     	  				 //GPIOC->BSRR = GPIO_BSRR_BR_2;
     	  			}
     	  			ModbusMap ();
     	  			noctar ();
     	  			break;		// ASM: Pendiente a traducir
     	  		case 4:
     	  			if(bandera_act_fw_j == 0)    //Parche
     	  			{
     	  				refrigera();
     	  			}
     	  			break;		// ASM: Pendiente a traducir
     	  		case 5:
     	  			display();
     	  			break;		// ASM: Pendiente a traducir
     	  		case 6:
     	  			tiempo ();				// ASM: <<<-- TRADUCCION COMPLETA -->>> 15-Jul-2024
     	  			if(bandera_act_fw_j == 0)    //Parche
     	  			{
     	  				logger ();				// ASM: Pendiente a traducir
     	  			}
    	  			tx_control ();			// ASM: "Faltan Comandos a Traducir"
   //
     	  			if ( keyWrFirm == 0xAA){
     	  			    asm ("nop");
     	  				if	( keyTx == 00 )	{
     	  				    asm ("nop");
   	  					bootloader();
     	  				}
     	  			}
     	  			break;		// ASM: Pendiente a traducir
     	  		case 7:
     	  			watch();
     	  			ProcesosC = 255;
     	  			break;		// ASM: Pendiente a traducir
     	  		default:
     	  		    asm ("nop");
     	  			for (;;)      ;// Watch dog Reset
     	  		break;

     	  	}

   	  	// clear time flags
   	  	tick_1ms = 0;						// flag base de tiempo 1ms
   	  	tick_10ms = 0;					// flag base de tiempo 10ms
   	  	tick_100ms = 0;					// flag base de tiempo 100ms
   	  	tick_1s = 0;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 7;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */
	uint8_t val_analog = 0x03;
	uint32_t wait_loop_index = 0;
	uint32_t tmp_cfgr1 = 0;
	uint32_t tmp_cfgr2 = 0;
	uint32_t AWDThresholdHighValue = 0;
	uint32_t AWDThresholdLowValue = 0;

	//ACTIVACION DE RELOJ
	MODIFY_REG(RCC->CCIPR, RCC_CCIPR_ADCSEL, (uint32_t)(RCC_ADCCLKSOURCE_SYSCLK));
	__HAL_RCC_ADC_CLK_ENABLE();

	//PINES ANALOGICOS PARA CANALES DE MUESTRA

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// ---------------------- GPIOC ----------------------
	// PC0
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE0_Msk, val_analog);
	// PC1
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE1_Msk, val_analog);
	// PC4
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE4_Msk, val_analog);

	// Sin pull-up/pull-down para PC0, PC1, PC4
	CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD4_Msk);

	// ---------------------- GPIOA ----------------------
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE5_Msk, val_analog);
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE6_Msk, val_analog);
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE7_Msk, val_analog);

	CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD5_Msk | GPIO_PUPDR_PUPD6_Msk | GPIO_PUPDR_PUPD7_Msk);

	// ---------------------- GPIOB ----------------------
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE1_Msk, val_analog);
	CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD1_Msk);

	//Activar el regulador del ADC
	MODIFY_REG(ADC1->CR, ADC_CR_BITS_PROPERTY_RS, ADC_CR_ADVREGEN);

	wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US / 10UL) * ((SystemCoreClock / (100000UL * 2UL)) + 1UL));
	while (wait_loop_index != 0UL)
	{
		wait_loop_index--;
	}

	//Llenar el registro ADC_CFGR1

	tmp_cfgr1 |= 	   (ADC_RESOLUTION_10B                         |
	                    ADC_CFGR1_AUTOWAIT((uint32_t)DISABLE)      |
	                    ADC_CFGR1_AUTOOFF((uint32_t)DISABLE)       |
	                    ADC_CFGR1_CONTINUOUS((uint32_t)DISABLE)    |
	                    ADC_CFGR1_OVERRUN(ADC_OVR_DATA_PRESERVED)  |
	                    ADC_DATAALIGN_RIGHT                        |
	                    ADC_SCAN_SEQ_MODE(ADC_SCAN_SEQ_FIXED)      |
	                    ADC_CFGR1_DMACONTREQ((uint32_t)DISABLE));
	MODIFY_REG(ADC1->CFGR1,
	                 ADC_CFGR1_RES       |
	                 ADC_CFGR1_DISCEN    |
	                 ADC_CFGR1_CHSELRMOD |
	                 ADC_CFGR1_AUTOFF    |
	                 ADC_CFGR1_WAIT      |
	                 ADC_CFGR1_CONT      |
	                 ADC_CFGR1_OVRMOD    |
	                 ADC_CFGR1_EXTSEL    |
	                 ADC_CFGR1_EXTEN     |
	                 ADC_CFGR1_ALIGN     |
	                 ADC_CFGR1_SCANDIR   |
	                 ADC_CFGR1_DMACFG,
	                 tmp_cfgr1);

	//Llenar el registro ADC_CFGR2

	tmp_cfgr2 |= ((ADC_CLOCK_SYNC_PCLK_DIV1 & ADC_CFGR2_CKMODE) |
	                    	ADC_TRIGGER_FREQ_LOW);

	MODIFY_REG(ADC1->CFGR2,
	                 ADC_CFGR2_CKMODE |
	                 ADC_CFGR2_LFTRIG |
	                 ADC_CFGR2_OVSE   |
	                 ADC_CFGR2_OVSR   |
	                 ADC_CFGR2_OVSS   |
	                 ADC_CFGR2_TOVS,
	                 tmp_cfgr2);
	//Configuracion de canales

	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_0 & ADC_CHANNEL_ID_BITFIELD_MASK));
	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_1 & ADC_CHANNEL_ID_BITFIELD_MASK));
	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_9 & ADC_CHANNEL_ID_BITFIELD_MASK));
	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_10 & ADC_CHANNEL_ID_BITFIELD_MASK));
	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_14 & ADC_CHANNEL_ID_BITFIELD_MASK));
	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_15 & ADC_CHANNEL_ID_BITFIELD_MASK));
	SET_BIT(ADC1->CHSELR, (ADC_CHANNEL_18 & ADC_CHANNEL_ID_BITFIELD_MASK));

	//Watch_Dogs

	SET_BIT(ADC1 -> AWD2CR, (1UL << __LL_ADC_CHANNEL_TO_DECIMAL_NB(ADC_ANALOGWATCHDOG_2)));
	WRITE_REG(ADC1->ISR, LL_ADC_FLAG_AWD2);
	CLEAR_BIT(ADC1->IER, LL_ADC_IT_AWD2);

	__IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADC1->AWD1TR,
	                                             (((ADC_ANALOGWATCHDOG_2 & ADC_AWD_TRX_REGOFFSET_MASK))
	                                              >> (ADC_AWD_TRX_REGOFFSET_BITOFFSET_POS))
	                                             + ((ADC_AWD_CR3_REGOFFSET & ADC_ANALOGWATCHDOG_2)
	                                                >> (ADC_AWD_CRX_REGOFFSET_BITOFFSET_POS + 1UL))
	                                            );

	MODIFY_REG(*preg,
	             ADC_AWD1TR_HT1 | ADC_AWD1TR_LT1,
	             (AWDThresholdHighValue << ADC_TR1_HT1_BITOFFSET_POS) | AWDThresholdLowValue);

	SET_BIT(ADC1->AWD3CR, (1UL << __LL_ADC_CHANNEL_TO_DECIMAL_NB(ADC_ANALOGWATCHDOG_3)));
	WRITE_REG(ADC1->ISR, LL_ADC_FLAG_AWD3);
	CLEAR_BIT(ADC1->IER, LL_ADC_IT_AWD3);

	__IO uint32_t *preg1 = __ADC_PTR_REG_OFFSET(ADC1->AWD1TR,
	                                             (((ADC_ANALOGWATCHDOG_3 & ADC_AWD_TRX_REGOFFSET_MASK))
	                                              >> (ADC_AWD_TRX_REGOFFSET_BITOFFSET_POS))
	                                             + ((ADC_AWD_CR3_REGOFFSET & ADC_ANALOGWATCHDOG_3)
	                                                >> (ADC_AWD_CRX_REGOFFSET_BITOFFSET_POS + 1UL))
	                                            );

	MODIFY_REG(*preg1,
	             ADC_AWD1TR_HT1 | ADC_AWD1TR_LT1,
	             (AWDThresholdHighValue << ADC_TR1_HT1_BITOFFSET_POS) | AWDThresholdLowValue);

  /* USER CODE END ADC1_Init 0 */

//  ADC_ChannelConfTypeDef sConfig = {0};
//  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
//  hadc1.Instance = ADC1;
//  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
//  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
//  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//  hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
//  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//  hadc1.Init.LowPowerAutoWait = DISABLE;
//  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
//  hadc1.Init.ContinuousConvMode = DISABLE;
//  hadc1.Init.NbrOfConversion = 1;
//  hadc1.Init.DiscontinuousConvMode = DISABLE;
//  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//  hadc1.Init.DMAContinuousRequests = DISABLE;
//  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
//  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
//  hadc1.Init.OversamplingMode = DISABLE;
//  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_LOW;
//  if (HAL_ADC_Init(&hadc1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_0;
//  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_1;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_9;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_10;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_14;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_15;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Regular Channel
//  */
//  sConfig.Channel = ADC_CHANNEL_18;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure the WatchDogs 2
//  */
//  AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
//  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
//  if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure the WatchDogs 3
//  */
//  AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_3;
//  if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE BEGIN ADC1_Init 2 */

	  uint32_t backup_setting_cfgr1;
	  uint32_t calibration_index;
	  uint32_t calibration_factor_accumulated = 0;
	  uint32_t wait_loop_index1 = 0UL;

	  if ((ADC1->CR & ADC_CR_ADEN) == 0UL)
	  {
	  	backup_setting_cfgr1 = ADC1->CFGR1 & (ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_AUTOFF);
	  	ADC1->CFGR1 &= ~(ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_AUTOFF);

	     	for (calibration_index = 0UL; calibration_index < 8UL; calibration_index++)
	      	{
	     		ADC1->CR &= ~ADC_CR_BITS_PROPERTY_RS;    // Limpiar los bits de la propiedad rs
	     		ADC1->CR |= ADC_CR_ADCAL;                // Establecer el bit ADC_CR_ADCAL para iniciar la calibración

	     		while ((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL)
	     		{
	     			wait_loop_index1++;
	     		}

	     		calibration_factor_accumulated += ADC1->CALFACT & ADC_CALFACT_CALFACT;
	      	}
	  	calibration_factor_accumulated /= calibration_index;

	  	ADC1->CR &= ~ADC_CR_BITS_PROPERTY_RS;    // Limpiar los bits de la propiedad rs
	  	ADC1->CR |= ADC_CR_ADEN;                  // Establecer el bit ADC_CR_ADEN para habilitar el ADC


	  	ADC1->CALFACT &= ~ADC_CALFACT_CALFACT;         // Limpiar los bits de calibración existentes
	  	ADC1->CALFACT |= calibration_factor_accumulated; // Establecer el nuevo factor de calibración


	  	ADC1->CR &= ~ADC_CR_BITS_PROPERTY_RS;     // Limpiar los bits de la propiedad rs
	  	ADC1->CR |= ADC_CR_ADDIS;                  // Establecer el bit ADC_CR_ADDIS para deshabilitar el ADC

	  	ADC1->CFGR1 |= backup_setting_cfgr1;  // Establecer los bits de CFGR1 según el valor de backup_setting_cfgr1
	  }
  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x201060FF;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Window = 800;
  hiwdg.Init.Reload = 800;
  hiwdg.Init.EWI = 0;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */


  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  /*if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x2000, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 14000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 7000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 14000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 7000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 31;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 438;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */
  TIM6->CR1 |= TIM_CR1_CEN;
  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 38400;
  huart4.Init.WordLength = UART_WORDLENGTH_9B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_EVEN;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX_OVR_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX_OVR_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX_OVR_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_3
                          |GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC2 PC3 PC6 PC8
                           PC9 PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB10 PB12 PB3
                           PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_3
                          |GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB14 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /*HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  HAL_GPIO_WritePin(PFULLDEF_MP1, GPIO_PIN_SET);// Activa la selección de MP1
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
  HAL_GPIO_WritePin(PFULLDEF_VSEN, GPIO_PIN_SET);
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
