


/*
 Definiciones de Puertos en tarjeta P Full
 */



//Entradas
#define PFULLDEF_STAT_VB					GPIO_PIN_11			//04-Jun-2024: VSEN
#define PFULLDEF_PWRC_VB					GPIO_PIN_12			// 02/12/2024: PWRC

#define PFULLDEF_P5							GPIOB,GPIO_PIN_4
#define PFULLDEF_MP1						GPIOB,GPIO_PIN_3


#define PFULLDEF_P1_PORT					GPIOC
#define PFULLDEF_P1_PIN						GPIO_PIN_13
#define PFULLDEF_P2_PORT					GPIOF
#define PFULLDEF_P2_PIN						GPIO_PIN_3
#define PFULLDEF_P3_PORT					GPIOB
#define PFULLDEF_P3_PIN						GPIO_PIN_7
#define PFULLDEF_P4_PORT					GPIOB
#define PFULLDEF_P4_PIN						GPIO_PIN_6
#define PFULLDEF_P5_PORT					GPIOB
#define PFULLDEF_P5_PIN						GPIO_PIN_4

#define PFULLDEF_MP0_PORT					GPIOC
#define PFULLDEF_MP0_PIN					GPIO_PIN_12
#define PFULLDEF_MP1_PORT					GPIOB
#define PFULLDEF_MP1_PIN					GPIO_PIN_3
#define PFULLDEF_MP2_PORT					GPIOC
#define PFULLDEF_MP2_PIN					GPIO_PIN_11
#define PFULLDEF_MP3_PORT					GPIOD
#define PFULLDEF_MP3_PIN					GPIO_PIN_2

//Salidas ----> GPIOA
#define PFULLDEF_rel_fn						GPIOA,GPIO_PIN_8  		//PA8:REL4		Salida	Relevador Ventilador
#define PFULLDEF_rel_co 					GPIOA,GPIO_PIN_9  		//PA9:REL1		Salida	Relevador Compresor
#define PFULLDEF_rel_dh						GPIOA,GPIO_PIN_10 		//PA10:REL3 	Salida	Deshielo

//Salidas ----> GPIOB
#define PFULLDEF_VSEN						GPIOB,GPIO_PIN_5		//04-Jun-2024: VSEN

//Salidas ----> GPIOC
#define PFULLDEF_dp_sw						GPIOC,GPIO_PIN_3		//PC3:dp_sw		Salida SW Display i2c Manuel
#define PFULLDEF_rel_lz 					GPIOC,GPIO_PIN_9		//PC9:REL5		Salida	Relevador Aux.Luz





#define PFULLDEF_FET_ON_OFF_WIFI			GPIO_PIN_6				//04-Jun-2024: FET ON/OFF WIFI
#define PFULLDEF_MAX485_DE_AND_RE_NEG		GPIO_PIN_2          //04-Jun-2024: / RS485A


#define PFULLDEF_IO6						GPIO_PIN_2          //10-Jul-2024: / Salida IO6
#define PFULLDEF_IO7						GPIO_PIN_10          //10-Jul-2024: / Salida IO7

