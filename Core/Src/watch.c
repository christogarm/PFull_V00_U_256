/*
 * Ya se Adapto a CTOFF 06/06/2025
 */

#include "main.h"
#include "customMain.h"

int16_t int_limsual_w;
int16_t int_tdev_w;
int16_t int_difTempAl_w;
int16_t int_liminal_w;


void watch (void){

//	;--------------------------------------------------------------------------------------------------
//	;							Supervisión falla compresor. (sensor 1 y sensor de corriente)
	if(!trefst2[f_retCo])		//btjf		trefst2,#f_retCo,ask_falla_co;	// se disparo falla ? No, pregunta por condiciones de falla
		goto ask_falla_co;
	GPIOR0[f_comp] = 0;			//bres		GPIOR0,#f_comp;									// Si se disparo la falla manten el compresor apagado.
	//ldw			X,timeAlarmRetCo_w
	if(timeAlarmRetCo_w)		//tnzw		X;												// Terminó el tiempo de alarma ?
		goto fin_falla_co;					// jrne		fin_falla_co;							// No, sal de rutina
	trefst2[f_retCo] = 0;			//bres		trefst2,#f_retCo;					// termina falla de Co
	goto fin_falla_co;				//jra			fin_falla_co

ask_falla_co:
	trefst2[f_retCo] = 0;	//bres		trefst2,#f_retCo;								// limpia bandera de falla de compresor

	if(!GetRegFlagState(Plantilla[numSens], f_senCo))//	btjf		numSens,#f_senCo,fin_falla_co; con sensor de corriente deshabilitado no supervises falla de compresor por temperatura y corriente
		goto fin_falla_co;//
	if(trefst[f_sac])	//	btjt		trefst,#f_sac,fin_falla_co;	/
		goto fin_falla_co;
	if(trefst[f_saa])	//	btjt		trefst,#f_saa,fin_falla_co;	/ en caso de fallas de sensor no verifiques alarma
		goto fin_falla_co;//
	if(!portX[rel_co])//	btjf		puertoa,#rel_co,fin_falla_co;	/ El compresor esta encendido? No, no verifiques alarma
		goto fin_falla_co;//
	//	ldw			X,tdevl;									// Carga temperatura (sensor 1)
	uint16_t tdevl_W = (uint16_t) ((tdevl << 8) + tdevf);
	uint16_t tempRetCo_W = (uint16_t) ((Plantilla[tempRetCo_H] << 8) + Plantilla[tempRetCo_L]);
	if(tdevl_W < tempRetCo_W)		//	cpw			X,tempRetCo
		goto fin_falla_co;			//	jrult		fin_falla_co;							// temperatura de sensor 1 es menor a latemperatura de falla ? si, termina con bandera de falla en cero
	//
	//	ldw			X,variable_corriente
	if(variable_corriente >= tempRetCo_W)	//	cpw			X,alarmaRetCo
		goto ask_time_falla_co;				//	jruge		ask_time_falla_co;				// corriente es mayor a corriente de falla ? si, pregunt si la corriente estuvo en falla el tiempo suficiente
load_time_falla_co:
	cntRetCo  = Plantilla[timeRetCo];		//mov			cntRetCo,timeRetCo;				// no, carga tiempo de corriente en falla
	goto fin_falla_co;// jra			fin_falla_co
ask_time_falla_co:
	if(cntRetCo)				//tnz			cntRetCo;
		goto fin_falla_co;		//jrne		fin_falla_co;							// la corriente en falla ya duro el tiempo suficiente para entrar en falla ? no, aun no indiques falla.

	trefst2[f_retCo] = 1;		// bset		trefst2,#f_retCo;					// indica falla de compresor
	//ldw			X,#600;										// carga tiempo que va durar la falla en segundos (600 = 10min)
	timeAlarmRetCo_w = 600;		//ldw			timeAlarmRetCo_w,X

fin_falla_co:

//	;--------------------------------------------------------------------------------------------------
//	;							Supervisión falla inyeción. (sensor 3)
//	;--------------------------------------------------------------------------------------------------

falla_iny:
	trefst[f_iny] = 0;		//	bres		trefst,#f_iny;								// limpia bandera de falla de inyección
	//
	if(!GetRegFlagState(Plantilla[numSens], f_sen3))	//	btjf		numSens,#f_sen3,fin_falla_iny; / si el sensor 3 está deshabilitado no supervises falla de inyección
		goto fin_falla_iny;//
	if(trefst2[f_s3short])//	btjt		trefst2,#,fin_falla_iny;	Si hay alguna falla con el sensor de inyección no ejecutes falla de inyección
		goto fin_falla_iny;
	if(trefst2[f_s3open])	//	btjt		trefst2,#f_s3open,fin_falla_iny
		goto fin_falla_iny;
		//
	//	ldw			X,tret_w;									// Carga temperatura de Inyección (sensor 3)
	int16_t alarmaIny =  (int16_t) ((Plantilla[alarmaIny_H] << 8) + Plantilla[alarmaIny_L]);
	if((int16_t)tret_w < alarmaIny)		//	cpw			X,alarmaIny
		goto fin_falla_iny;		//	jrult		fin_falla_iny;						// temperatura de inyección es menor a temperatura de falla ? Sí, termina con bandera de falla en cero
	trefst[f_iny] = 1;			//	bset		trefst,#f_iny;								// limpia bandera de falla de inyección

fin_falla_iny:

//;------------------------------------------------------------------------------------------------------------------
//;							supervicion de timeout token wifi - ble
//;------------------------------------------------------------------------------------------------------------------
			//ldw		X,timeoutTBLE;					/ No,checa time out
			//cpw		X,#1;
			if(timeoutTBLE == 1)//jreq	resetComMod;					/
				goto resetComMod;
			//ldw		X,timeoutTWF;					/ No,checa time out
			//cpw		X,#1;
			if(timeoutTWF != 1)//jrne	fallas_temp;					/ No, continúa
				goto fallas_temp;
resetComMod:
			timeTxTBLE = 20;	//mov		timeTxTBLE,#20
			flagsTX2[2] =0;		//bres	flagsTX2,#2;					/ sí, cancela bandera de token recibido
			//ldw		X,#300;
			timeoutTBLE = 300;	//ldw		timeoutTBLE,X;					/ carga time out de Token BLE (5 min)

			timeTxTWF = 30;			//mov		timeTxTWF,#30
			flagsTX2 [1] = 0; 		//bres	flagsTX2,#1;					/ sí, cancela bandera de token recibido
			timeRstBLE = 5;			//mov		timeRstBLE,#5;				/ carga 5 segundos de apagado para BLE (reinicia el modulo wifi)
			//ldw		X,#300;
			timeoutTWF = 300;		//ldw		timeoutTWF,X;					/ carga time out de Token (5 min)
			keyTx = 0;				//clr		keyTx  ;// en caso de desconexion cancela toda transmisión
			for(uint8_t i=0; i<8; i++){
				flagsTX[i] = 0;		//clr		flagsTX
				if(flagsRxFirm[0])
					reinicio_valores_act_fw();
				flagsRxFirm[i]= 0;	//clr		flagsRxFirm
				flagsTxControl[i] = 0; // clr		flagsTxControl
				flagsWIFI[i] = 0; 		//clr		flagsWIFI
			}


			flagsLogger[4] = 0;			//bres	flagsLogger,#4;				// permite loggeo de eventos
			flagsLogger[5] = 0;			//bres	flagsLogger,#5;				// permite loggeo de datos


//;------------------------------------------------------------------------------------------------------------------
fallas_temp:

	if(trefst[f_sac])	//btjt		trefst,#f_sac,noAlarmTemp;	/
		goto noAlarmTemp;
	if(!trefst[f_saa])	//btjf		trefst,#f_saa,tempAmbHiAlarm;	/ en caso de fallas de sensor no verifiques alarmas de temperatura
		goto tempAmbHiAlarm;
noAlarmTemp:
	trefst2[f_ambLo] = 0;		//bres		trefst2,#f_ambLo
	trefst2[f_ambHi] = 0;		//bres		trefst2,#f_ambHi
	goto tempAlarmFin;			//jra			tempAlarmFin

tempAmbHiAlarm:
	//ldw			X,limsual_w;
	int_limsual_w = (int16_t)((Plantilla[limsual_H] << 8) + Plantilla[limsual_L]);
	int_tdev_w = (int16_t)((tdevl << 8) + tdevf);
	//cpw			X,tdevl
	if(int_limsual_w > int_tdev_w) //jrsgt		HiAlarmHist
		goto HiAlarmHist;
	goto tempAmbLoAlarm;			//jra			tempAmbLoAlarm

HiAlarmHist:
	int_difTempAl_w = (int16_t)((Plantilla[difTempAl_H] << 8) + Plantilla[difTempAl_L]);
	//subw		X,difTempAl
	//cpw			X,tdevl
	if((int_limsual_w - int_difTempAl_w) < int_tdev_w)//jrslt		tempAmbLoAlarm
		goto tempAmbLoAlarm;
	trefst2[f_ambHi] = 0;		//bres		trefst2,#f_ambHi

tempAmbLoAlarm:
	int_liminal_w = (int16_t)((Plantilla[liminal_H] << 8) + Plantilla[liminal_L]);//	ldw			X,liminal_w;
	//	cpw			X,tdevl
	if(int_liminal_w < int_tdev_w)	//	jrslt		LoAlarmHist
		goto LoAlarmHist;
	trefst2[f_ambLo] = 1;	//	bset		trefst2,#f_ambLo
	goto tempAlarmFin;		//	jra			tempAlarmFin
LoAlarmHist:
	//addw		X,difTempAl
	//	cpw			X,tdevl
	if((int_liminal_w + int_difTempAl_w) > int_tdev_w)//	jrsgt		tempAlarmFin
		goto tempAlarmFin;
	trefst2[f_ambLo] = 0;	//	bres		trefst2,#f_ambLo
tempAlarmFin:

	chk_var();		// call		chk_var

	// call		mcuset	;			/ Refresca los valores de los registros IO que no deben cambiar

	//SIM	;					/ Deshabilita interrupciones

	param2eeprom();	//call		param2eeprom

	memodriver();	//call		memodriver	;		/ Refresca los parámetros de operación del refrigerador uno a la vez cada ms

	if(!flagsb[f_excom])// btjf		flagsb,#f_excom,no_rstdpy
		goto no_rstdpy;

	//tnz			cnt_ecom
	if(cnt_ecom)		//jrne		watch00
		goto watch00;

	portX[dp_sw] = 0; 	//	bres		puertob,#dp_sw                  ;21/08 comentado pruebas    RM_20220622    Para que resetee la alimentación del display cuando no esta conectado
	//	tnz			cnt_dpyrst
	if(cnt_dpyrst == 0)//	jreq		no_rstdpy
		goto no_rstdpy;

	goto watch01;		// jra			watch01

no_rstdpy:
	portX[dp_sw] = 1;		// bset		puertob,#dp_sw					;Manuel 23-FEB-2022		bset		puertoa,#dp_sw
	cnt_ecom = 200;			// mov			cnt_ecom,#200
watch00:
	cnt_dpyrst = 100;		// mov			cnt_dpyrst,#100
watch01:
	//	tnz			maxwork
	if(Plantilla[maxwork])	//	jrne		co_ex_rev
		goto co_ex_rev;
	trefst2[f_co_ex] = 0;	//	bres		trefst2,#f_co_ex
	goto fin_co_ex;			//	jra			fin_co_ex

co_ex_rev:
	if(portX[rel_co])	//	btjt		puertoa,#rel_co,ask_coex;	/ El compresor esta encendido?   ;RM_20220714  Cambio de puertos para cargas
		goto ask_coex;
	wreg = Plantilla[maxwork];	//	mov			wreg,maxwork;							/ Carga el tiempo de compresor exhausto
	BaBentre10_math();			//	call		BaBentre10_math;					/ Convierte el dato a minutos
	//cntworkl = resull;	//	mov			cntworkl,resull;
	cntworkh = (uint16_t) ((resulh << 8) + resull);;	//	mov			cntworkh,resulh;
ask_coex:
	if(trefst2[f_co_ex])	//	btjt		trefst2,#f_co_ex,time_rest_co
		goto time_rest_co;
	//	ld			A,cntworkh
	//	or			A,cntworkl
	if((cntworkh))	//	jrne		let_comp_work
		goto let_comp_work;
	trefst2[f_co_ex] = 1;		//	bset		trefst2,#f_co_ex
	goto off_comp_exha;			//	jra			off_comp_exha
let_comp_work:
	wreg = Plantilla[exhausted];	//	mov			wreg,exhausted;						/ Carga el tiempo de descanzo de compresor exhausto
	//	tnz			wreg
	if(wreg)//	jrne		conv_ex_tm
		goto conv_ex_tm;
	wreg = 1;	//	mov			wreg,#1
conv_ex_tm:
	BaBentre10_math();	// call		BaBentre10_math;					/ Convierte el dato a minutos
	//cntexhal = resull;	//	mov			cntexhal,resull;
	cntexhah = (uint16_t) ((resulh << 8) + resull);	//	mov			cntexhah,resulh;
	goto fin_co_ex;		//	jra			fin_co_ex

time_rest_co:
	//	ld			A,cntexhah
	//	or			A,cntexhal
	if(cntexhah)	//	jrne		off_comp_exha
		goto off_comp_exha;
	trefst2[f_co_ex] = 0;	//	bres		trefst2,#f_co_ex
	goto fin_co_ex;			//	jra			fin_co_ex
off_comp_exha:
	GPIOR0[f_comp] = 0;		//bres	GPIOR0,#f_comp
fin_co_ex:


////;------------------------- Control del Activacion de Cargas
watch50:

				portX[rel_co] = 0;			//		bres		puertoa,#rel_co; /Apaga los relevadores    ;RM_20220623  Cambio de puertos para cargas
				portX[rel_dh] = 0;			//		bres		puertoe,#rel_dh;                           ;RM_20220623  Cambio de puertos para cargas
				portX[rel_lz] = 0;			//		bres		puertoe,#rel_lz;		// Manuel 10-MAR-2022  ;RM_20220623  Cambio de puertos para cargas
				portX[rel_fn] = 0;			//		bres		puertoa,#rel_fn;		// Manuel 10-feb-2022  ;RM_20220623  Cambio de puertos para cargas
				//portX[led_pot] = 0;			//		bres		puertoa,#led_pot;

				if(!GPIOR1[f_led]){
					goto	jmp_watch_02;
				}
				//portX[led_pot] = 1;			// 		bset		puertoa,#led_pot;
jmp_watch_02:

				if(timerOffManto != 0){
					goto	jmp_watch_12;			//jrne		jmp_watch_12;			/ si hay tiempo de apagado de mantenimiento manten las cargas apagadas.
				}

//;------------------------- Control del retardo Compresor
				if(!GPIOR0[f_comp]){
					goto	jmp_watch_03_00;
				}
				if(retzc_ms_compresor != 0){
					goto	jmp_watch_03;				//jrne    jmp_watch_03                    ;NO, espera a que se cumpla
				}
				portX[rel_co] = 1;					// bset		puertoa,#rel_co; /Enciende el compresor    ;RM_20220623  Cambio de puertos para cargas
				goto jmp_watch_03;					// jra			jmp_watch_03
jmp_watch_03_00:
				retzc_ms_compresor = tms_retcz_compresor;			// 	mov     retzc_ms_compresor,#tms_retcz_compresor       ;Carga retardo para encendido de compresor

jmp_watch_03:

				if(GPIOR0[f_comp]){				//btjt  	GPIOR0,#f_comp,jmp_watch_05;
					goto	jmp_watch_05;
				}
				if(GPIOR1[f_fan]){				//btjt		GPIOR1,#f_fan,jmp_watch_04
					goto	jmp_watch_04;
				}
				ret_fan_co = 15;					//mov			ret_fan_co,#15;	/ Si, el compresor esta apagado carga retard
				goto	jmp_watch_05;				//jra			jmp_watch_05
jmp_watch_04:
				ret_fan_co = 0;						//clr			ret_fan_co
jmp_watch_05:

//;------------------------- Control del retardo deshielo
watch55:		if(!GPIOR0[f_dh]){				//btjf		GPIOR0,#f_dh,jmp_watch_07_00;
					goto	jmp_watch_07_00;
				}
				if(retzc_ms_deshielo != 0){				// tnz			retzc_ms_deshielo              ;¿Ya se cumplió el retardo al cruce por cero para el deshielo?
					goto	jmp_watch_07;				// jrne    jmp_watch_07                    ;NO, espera a que se cumpla
				}
				portX[rel_dh] = 1 ;						// bset		puertoe,#rel_dh; Enciende el deshielo    ;RM_20220623  Cambio de puertos para cargas
				goto jmp_watch_07;			    		// jra			jmp_watch_07

jmp_watch_07_00:
				retzc_ms_deshielo = tms_retcz_deshielo;	 	//mov     retzc_ms_deshielo,#tms_retcz_deshielo       ;Carga retardo para encendido de deshielo

jmp_watch_07:

//;------------------------- Control del retardo Lampara
watch60:		if(!GPIOR0[f_lamp]){				//btjf		GPIOR0,#f_lamp,jmp_watch_11_00;
					goto	jmp_watch_11_00;
				}
				if(retzc_ms_lampara != 0){				//;¿Ya se cumplió el retardo al cruce por cero para lampara?
					goto	jmp_watch_11;				// jrne    jmp_watch_11                    ;NO, espera a que se cumpla
				}
				portX[rel_lz] = 1 ;						// bset		puertoe,#rel_lz; // Manuel 10-Mar-2022	Enciende la lampara   ;RM_20220623  Cambio de puertos para cargas
				goto	jmp_watch_11;					// jra			jmp_watch_11

jmp_watch_11_00:
				retzc_ms_lampara = tms_retcz_lampara;		//	mov     retzc_ms_lampara,#tms_retcz_lampara       ;Carga retardo para encendido de lampara

//;------------------------- Control del retardo Ventilador
jmp_watch_11:
				if(!GPIOR1[f_fan]){
					goto jmp_watch_12_00;
				}
				if(retzc_ms_ventilador != 0){		//tnz			retzc_ms_ventilador              ;¿Ya se cumplió el retardo al cruce por cero para ventilador?
					 goto jmp_watch_12;				//	jrne    jmp_watch_12                    ;NO, espera a que se cumpla
				}
				portX[rel_fn] = 1 ;					// bset		puertoa,#rel_fn; // Manuel 01-feb-2022	Enciende ventilador
				goto jmp_watch_12;					// jra			jmp_watch_12


jmp_watch_12_00:
				retzc_ms_ventilador = tms_retcz_ventilador;			//		mov     retzc_ms_ventilador,#tms_retcz_ventilador       ;Carga retardo para encendido de ventilador

jmp_watch_12:

	if(portX[rel_co])		//	btjt		puertoa,#rel_co,jmp_watch_13; / sí el compresor está apagado manten cargado el tiempo de deficiencia  ;RM_20220623  Cambio de puertos para cargas
		goto jmp_watch_13;
	wreg = Plantilla[timeDefi];	//	mov			wreg,timeDefi;	/ Toma el tiempo de deficiencia
	//	ldw			X,#$003C;				/ Número de segundos por minuto
	//	ld      A,wreg
	//	mul			X,A;						/ Multiplicalos
	//	ldw     resulh,X
	//	mov			cntDefi_L,resull;
	//	mov			cntDefi_H,resulh;	/ Carga el tiempo de deficiencia en segundos
	cntDefi_H = wreg * 0x003C;
	//	subw		X,#1
	cntDefi1_H = cntDefi_H - 1;	//	ldw			cntDefi1_H,X;		/ establece el tiempo de deficiencia inicial para sensor 1

jmp_watch_13:

	//;---------------------------------------------------------------
	//;						Prueba de cargas                          //RM_20240816 Prueba de cargas
	//;---------------------------------------------------------------
	//	tnz			tiempoPrCargas
	if(tiempoPrCargas == 0)//	jreq		noPrCargas
		goto noPrCargas;
	portX[rel_co] = 1;	//	bset		puertoa,#rel_co; /enciende los relevadores
	portX[rel_dh] = 1;	//	bset		puertoe,#rel_dh;
	portX[rel_lz] = 1;	//	bset		puertoe,#rel_lz;
	portX[rel_fn] = 1;	//	bset		puertoa,#rel_fn;

noPrCargas:

	asm("nop");
//;------------------------- Revision de la comunicacion


//;------------------------- Revision de verifica_eeprom:



//;------------------------- Revision de verifica_ram:
verifica_ram:
	//	ld      A,dato_seg1        ;Verifica las variables de seguridad de EEPROM
	if(Plantilla[dato_seg1] == 0xAA)//	cp      A,#$AA             ;/
		goto v_ram2;	//	jreq    v_ram2             ;/
	goto espera_reset;		//	jp      espera_reset     ;/
v_ram2:
		//ld      A,dato_seg2        ;/
 	if(Plantilla[dato_seg2] == 0x66)	// cp      A,#$66             ;/
 		goto v_ram3;		//jreq    v_ram3             ;/
 	goto espera_reset;		//jp      espera_reset     ;/
v_ram3:
	//	ld      A,dato_seg3        ;/
	if(Plantilla[dato_seg3] == 0xCC)//	cp      A,#$CC             ;/
		goto verifica_ob;			//	jreq    verifica_ob        ;¿Son iguales? SI, Continua
	goto espera_reset;				//	jp      espera_reset     ;NO, Resetea el MCU

//;------------------------- Revision de los Option Bytes
verifica_ob:       //;10 Option Bytes localizados de la dirección $4800 a la $480A
	/*
	 * Descomentar cuando se realice la proteccion de memoria
	 */
	//if((FLASH->OPTR & 0xFF) == 0xAA || (FLASH->OPTR & 0xFF) == 0xCC)		// Read Proteccion Level 1
	//	goto espera_reset;
	if((FLASH->OPTR & 0x700) != 0)	// BOR Level 0
		goto espera_reset;
	if(( (FLASH->OPTR >> 16) & 0x1 ) == 0x1)	// Software Independent Watchdog
		goto espera_reset;
	if(( (FLASH->OPTR >> 17) & 0x1 ) == 0)
		goto end_watch2;

espera_reset:		          	//;Loop infinito para que el IWDG resetee el MCU
	goto espera_reset;			//jra    espera_reset     ;/

end_watch2:
	return;

}
