






//*********************************************************************************************************************
//*********************************************** Macros de la Clase  ************************************************
//*********************************************************************************************************************

//---------------      Imbera Control solo de Lectura  --------------------------------------------
#define 	Eventos_AddressBegin  								0x0100     // Manuel 06-dic-2021: limite inferior Address de Eventos
#define 	Eventos_AddressEnd										0x010D		 // Manuel 06-dic-2021: limite superior Address de Eventos

#define 	Mediciones_AddressBegin  							0x2000     // Manuel 06-dic-2021: limite inferior Address de Mediciones
#define 	Mediciones_AddressEnd									0x2003		 // Manuel 06-dic-2021: limite superior Address de Mediciones

#define 	Alarmas_AddressBegin  								0x200     // Manuel 06-dic-2021: limite inferior Address de Alarmas
#define 	Alarmas_AddressEnd										0x208	    // Manuel 06-dic-2021: limite superior Address de Alarmas

#define 	DatosMAC_AddressBegin  								0x300     // Manuel 06-dic-2021: limite inferior Address de Datos MAC
#define 	DatosMAC_AddressEnd										0x302		 // Manuel 06-dic-2021: limite superior Address de Datos MAC

#define 	DatosFirmware_AddressBegin  					0x310     // Manuel 06-dic-2021: limite inferior Address de DatosFirmware
#define 	DatosFirmware_AddressEnd						  0x310		 // Manuel 06-dic-2021: limite superior Address de  DatosFirmware
//---------------      OXXO Control demo RW  --------------------------------------------------------------------------------------------
#define 	PNU_0x3000_AddressBegin  					0x3000     // Manuel  14-Ene-2022: limite inferior ID del cliente del refrigerador
#define 	PNU_0x3000_AddressEnd						  0x3000		 // Manuel 14-Ene-2022: limite superior ID del cliente del refrigerador

#define 	PNU_0x3002_AddressBegin  					0x3002     // Manuel  14-Ene-2022: limite inferior Unidad de temperatura
#define 	PNU_0x3002_AddressEnd						  0x3002		 // Manuel 14-Ene-2022:  limite superior Unidad de temperatura

#define 	PNU_0x3004_AddressBegin  					0x3004     // Manuel  14-Ene-2022: limite inferior	Temperatura Setpoint MODO NORMAL
#define 	PNU_0x3004_AddressEnd						  0x3004		 // Manuel 14-Ene-2022:  limite superior 	Temperatura Setpoint MODO NORMAL

#define 	PNU_0x3006_AddressBegin  					0x3006     // Manuel  14-Ene-2022: limite inferior	Temperatura Diferencial  MODO NORMAL
#define 	PNU_0x3006_AddressEnd						  0x3006		 // Manuel 14-Ene-2022:  limite superior 	Temperatura Diferencial  MODO NORMAL

#define 	PNU_0x3008_AddressBegin  					0x3008     // Manuel  14-Ene-2022: limite inferior	MODO NOCTURNO: duración en mins
#define 	PNU_0x3008_AddressEnd						  0x3008		 // Manuel 14-Ene-2022:  limite superior 	MODO NOCTURNO: duración en mins

#define 	PNU_0x300A_AddressBegin  					0x300A     // Manuel  14-Ene-2022: limite inferior	Consumo por variable
#define 	PNU_0x300A_AddressEnd						  0x300A		 // Manuel 14-Ene-2022:  limite superior 	Consumo por variable

#define 	PNU_0x300C_AddressBegin  					0x300C     // Manuel  14-Ene-2022: limite inferior	Compresor: tiempo minimo de apagado en mins
#define 	PNU_0x300C_AddressEnd						  0x300C		 // Manuel 14-Ene-2022:  limite superior 	Compresor: tiempo minimo de apagado en mins

#define 	PNU_0x300E_AddressBegin  					0x300E     // Manuel  14-Ene-2022: limite inferior	Compresor: tiempo minimo de encendido en mins
#define 	PNU_0x300E_AddressEnd						  0x300E		 // Manuel 14-Ene-2022:  limite superior 	Compresor: tiempo minimo de encendido en mins

#define 	PNU_0x3010_AddressBegin  					0x3010     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: habilitación de 3 o 5 fases
#define 	PNU_0x3010_AddressEnd						  0x3010		 // Manuel 14-Ene-2022:  limite superior 	Deshielo Programa 1: habilitación de 3 o 5 fases

#define 	PNU_0x3012_AddressBegin  					0x3012     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: duración fase 1 en mins
#define 	PNU_0x3012_AddressEnd						  0x3012		 // Manuel 14-Ene-2022:  limite superior 	Deshielo Programa 1: duración fase 1 en mins

#define 	PNU_0x3014_AddressBegin  					0x3014     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: duración fase 2 en mins
#define 	PNU_0x3014_AddressEnd						  0x3014		 // Manuel 14-Ene-2022:  limite superior 	Deshielo Programa 1: duración fase 2 en mins

#define 	PNU_0x3016_AddressBegin  					0x3016     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: duración fase 3 en mins
#define 	PNU_0x3016_AddressEnd						  0x3016		 // Manuel 14-Ene-2022:  limite superior 	Deshielo Programa 1: duración fase 3 en mins

#define 	PNU_0x3018_AddressBegin  					0x3018     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: periodo en mins para iniciar deshielo
#define 	PNU_0x3018_AddressEnd						  0x3018		 // Manuel 14-Ene-2022:  limite superior 	Deshielo Programa 1: periodo en mins para iniciar deshielo

#define 	PNU_0x301A_AddressBegin  					0x301A     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: temperatura para inicio de periodo de deshieloDeshielo Programa 1: temperatura para inicio de periodo de deshielo
#define 	PNU_0x301A_AddressEnd						  0x301A		 // Manuel 14-Ene-2022:  limite superior	Deshielo Programa 1: temperatura para inicio de periodo de deshieloDeshielo Programa 1: temperatura para inicio de periodo de deshielo 

#define 	PNU_0x301C_AddressBegin  					0x301C     // Manuel  14-Ene-2022: limite inferior	Deshielo Programa 1: temperatura para salir de deshielo  y verifica si entra a deshielo
#define 	PNU_0x301C_AddressEnd						  0x301C		 // Manuel 14-Ene-2022:  limite superior	Deshielo Programa 1: temperatura para salir de deshielo  y verifica si entra a deshielo 

#define 	PNU_0x301E_AddressBegin  					0x301E     // Manuel  14-Ene-2022: limite inferior	Modbus: dirección de la CIR
#define 	PNU_0x301E_AddressEnd						  0x301E		 // Manuel 14-Ene-2022:  limite superior 	Modbus: dirección de la CIR

#define 	PNU_0x3104_AddressBegin  					0x3104     // Manuel  14-Ene-2022: limite inferior	Alarma puerta abierta: tiempo de activación en mins
#define 	PNU_0x3104_AddressEnd						  0x3104		 // Manuel 14-Ene-2022:  limite superior 	Alarma puerta abierta: tiempo de activación en mins

#define 	PNU_0x3106_AddressBegin  					0x3106     // Manuel  14-Ene-2022: limite inferior	Alarma Congelamiento Programa 1: temperatura para entrar en alarma
#define 	PNU_0x3106_AddressEnd						  0x3106		 // Manuel 14-Ene-2022:  limite superior 	Alarma Congelamiento Programa 1: temperatura para entrar en alarma

#define 	PNU_0x3108_AddressBegin  					0x3108     // Manuel  14-Ene-2022: limite inferior	Alarma U 110V: voltaje para entrar en alarma
#define 	PNU_0x3108_AddressEnd						  0x3108		 // Manuel 14-Ene-2022:  limite superior	Alarma U 110V: voltaje para entrar en alarma

#define 	PNU_0x310A_AddressBegin  					0x310A     // Manuel  14-Ene-2022: limite inferior	Alarma U 110V: voltaje para salir de alarma 
#define 	PNU_0x310A_AddressEnd						  0x310A		 // Manuel 14-Ene-2022:  limite superior 	Alarma U 110V: voltaje para salir de alarma 

#define 	PNU_0x310C_AddressBegin  					0x310C     // Manuel  14-Ene-2022: limite inferior	Alarma -U 110V: voltaje para entrar en alarma
#define 	PNU_0x310C_AddressEnd						  0x310C		 // Manuel 14-Ene-2022:  limite superior	Alarma -U 110V: voltaje para entrar en alarma

#define 	PNU_0x310E_AddressBegin  					0x310E     // Manuel  14-Ene-2022: limite inferior	Alarma -U 110V: voltaje para salir de alarma 
#define 	PNU_0x310E_AddressEnd						  0x310E		 // Manuel 14-Ene-2022:  limite superior 	Alarma -U 110V: voltaje para salir de alarma 

#define 	PNU_0x3200_AddressBegin  					0x3200     // Manuel  14-Ene-2022: limite inferior	Comando activar Modo Nocturno
#define 	PNU_0x3200_AddressEnd						  0x3200		 // Manuel 14-Ene-2022:  limite superior	Comando activar Modo Nocturno

#define 	PNU_0x3202_AddressBegin  					0x3202     // Manuel  14-Ene-2022: limite inferior	Comando activar MODO DE AHORRO ES2 
#define 	PNU_0x3202_AddressEnd						  0x3202		 // Manuel 14-Ene-2022:  limite superior 	Comando activar MODO DE AHORRO ES2 

#define 	PNU_0x3204_AddressBegin  					0x3204     // Manuel  14-Ene-2022: limite inferior	Comando activar MODO VAULT
#define 	PNU_0x3204_AddressEnd						  0x3204		 // Manuel 14-Ene-2022:  limite superior	Comando activar MODO VAULT

#define 	PNU_0x3206_AddressBegin  					0x3206     // Manuel  14-Ene-2022: limite inferior	Comando desactivar MODO VAULT 
#define 	PNU_0x3206_AddressEnd						  0x3206		 // Manuel 14-Ene-2022:  limite superior 	Comando desactivar MODO VAULT 

#define 	PNU_0x3208_AddressBegin  					0x3208     // Manuel  14-Ene-2022: limite inferior	Comando activar MODO Servicio
#define 	PNU_0x3208_AddressEnd						  0x3208		 // Manuel 14-Ene-2022:  limite superior 	Comando activar MODO Servicio

#define 	PNU_0x320A_AddressBegin  					0x320A     // Manuel  14-Ene-2022: limite inferior	Comando Ventilador siempre ON con compresor OFF
#define 	PNU_0x320A_AddressEnd						  0x320A		 // Manuel 14-Ene-2022:  limite superior	Comando Ventilador siempre ON con compresor OFF

#define 	PNU_0x320C_AddressBegin  					0x320C     // Manuel  14-Ene-2022: limite inferior	Comando Ventilador activar ciclos ON-OFF con compresor OFF
#define 	PNU_0x320C_AddressEnd						  0x320C		 // Manuel 14-Ene-2022:  limite superior	Comando Ventilador activar ciclos ON-OFF con compresor OFF


//------------- 07-MAR-2022
#define 	AndyWRegisters_0x30_AddressBegin  					0x30     // Manuel  07-MAR-2022: Activacion/Desactivacion Nocturno
#define 	AndyWRegisters_0x30_AddressEnd						  0x30		 //

#define 	AndyWRegisters_0x31_AddressBegin  					0x31     // Manuel  07-MAR-2022: Activacion/Desactivacion Deshielo
#define 	AndyWRegisters_0x31_AddressEnd						  0x31		 //

#define 	AndyWRegisters_0x32_AddressBegin  					0x32     // Manuel  07-MAR-2022: Activacion/Desactivacion Lampara
#define 	AndyWRegisters_0x32_AddressEnd						  0x32		 //

//------------- 17-dic-2021
#define 	TestAddress_AddressBegin  						0x10     // Manuel 17-dic-2020: limite inferior Address de DatosFirmware
#define 	TestAddress_AddressEnd						  	0x10	 	// Manuel 17-dic-2020: limite superior Address de  DatosFirmware








