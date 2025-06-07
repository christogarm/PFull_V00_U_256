/* CGM 27/05/2025
 * Ya Adaptado a CTOFF
 */

#include "main.h"
#include "board_PFULLDEF.h"
#include "customMain.h"
#include "bluetooth.h"
//#include "math.h"

/*
 * 			CGM 07/05/2025
 * Analisis del Amplificador aplicado en el Proyecto Pfull (Ir a ver el Diagrama OP+1 del Sensor 5)
 * 			 _				   _   _			_	  _			     _
 * 			|	2*R169			| |		RTP		 |	 |	 R169		  |
 * 	Vo 	= 	| ---------- (VSEN) | | ------------ | - |	------ (VSEN) |
 * 			|_	 R170		   _| |_ R157 + RTP _|	 |_	 R170		 _|
 *
 * 		-- Sustitución de la ecuacion por una literal:
 * 				 _				   _			 _			  _	  			 _			     _
 * 				|	2*R169			|			|	  RTP	   |	 		|	 R169		  |
 * 		Alpha = | ---------- (VSEN) |;	Gamma =	| ------------ |	Beta =	|	------ (VSEN) |
 * 				|_	 R170		   _|			|_ R157 + RTP _|	 		|_	 R170		 _|
 *
 * 		 -- Ecuación Reducida y Despejada
 * 	Vo	=	Gamma * X - Beta		->		Gamma = (Vo / Alpha) - (Beta/Alpha)
 *
 * Ecuación Lineal Caracteristicas del RTP 1000
 *
 * 	RTP = 3.91 * T + 1000
 *
 * Igualacion de ambas ecuaciones en función del Voltaje de Salida del OPAMP
 * 		 _									  _
 *		| ( Gamma/(1 - Gamma) ) * R157 - 1000  |
 * 	T =	| ------------------------------------ |	[°C]
 * 		|_				   3.91				  _|
 */

// Definiciones y Variables utilizadas para reducir el codigo, Fueron redondeadas y calculadas en una calculadora
// Las variables se utilizan en una parte Entera y otra decimal

#define		alphaEnt_OP1		23
#define		alphaFrac_OP1		7455
#define		betaEnt_OP1			9
#define		betaFrac_OP1		94227
int			gammaEnt_OP1	=	0;
int			gammaFrac_OP1	=	0;

#define		const1Ent			0			// Parte Entera y Decimal para realizar un cambio de la muestra del ADC a terminos del Voltaje y se divide entre Alpha para reducir codigo
#define		const1Frac			14		// Operacion:	3.3/(1024*Alpha)
#define		const2Ent			255			// División entre la Ordenada al origen y la pendiente de la Ecuacion Lineal del RTP1000
#define		const2Frac			75448		// Operacion:	1000/3.91
#define		const3Ent			255			// Division entre la Resistencia 157 y la Pendiente
#define		const3Frac			75448		// Operación:	R157/3.91
#define		const4Ent			0			// Division entre Alpha y Beta
#define		const4Frac			43088		// Operación:	Alpha/Beta

// Definiciones para mejorar la resolucion de las operaciones
#define mulDigFrac 100000				// Multiplicador para saber cuantos decimales de resolución se desea tener en la suma, resta y multiplicacion, si cambian estos valores, se deben de modificar todas las constantes declaradas aqui o donde se utilizen las funciones de Suma, Multiplicacion y Division declaradas en este archivo
										// mulDigFrac = 1000;		xxxEnt = 4785;	xxxFrac = 123;		->	Valor Flotante = 4785.123
										// mulDigFrac = 100000;		xxxEnt = 4785;	xxxFrac = 12345;	->	Valor Flotante = 4785.12345

#define corrResDiv	32					// Este valor se utilza para realizar un corrimiento en el dividendo para se tenga mas resolución en los decimales
										// Nota: si se da valores muy altos puede provocar el desbordamiento del tipo de dato utilizado

// Variables para la toma de muestras, son de 16 bits debido a que el ADC esta configurado a 10 bits
uint16_t	muestraOP1[8] = {0};
uint16_t	muestraOP2[8] = {0};
uint16_t	muestraOP3[8] = {0};
uint16_t	muestraOP4[8] = {0};

// Promedios de las muestras
uint16_t	promedioOP1 = 0;
uint16_t	promedioOP2 = 0;
uint16_t	promedioOP3 = 0;
uint16_t	promedioOP4 = 0;

// Medición del Sensor
int		sensorEntOP1 	= 0;
int		sensorFracOP1 	= 0;
int		sensorEntOP2 	= 0;
int		sensorFracOP2 	= 0;
int		sensorEntOP3 	= 0;
int		sensorFracOP3 	= 0;
int		sensorEntOP4 	= 0;
int		sensorFracOP4 	= 0;

uint8_t countSel = 0;
uint8_t countOP1 = 0;
uint8_t countOP2 = 0;
uint8_t countOP3 = 0;
uint8_t countOP4 = 0;

uint16_t	tsacram [8] = {0};
uint16_t    tretram [8] = {0};
uint16_t    tambram [8] = {0};
uint16_t    tevaram [8] = {0};

uint16_t   curva = 0;
uint8_t    A = 0;
uint16_t   Y = 0;
uint16_t   X = 0;
//uint8_t    wreg = 0;
//uint8_t    waux = 0;
uint16_t   resul =  0;
uint16_t   tempo = 0;
uint16_t   raux = 0;
uint16_t   promant = 0;
uint16_t   ultimoprm = 0;
//uint16_t   tdev = 0;
//uint16_t   tDisplay_w = 0;
//uint16_t   tdevdpy_w = 0;
//aux
//uint16_t  limsup_w = 0;
//aux
//	uint8_t retcncfa = 0;
//	uint8_t retcncfe = 0;
//	uint8_t retcncfr = 0;
//	uint8_t retcncfc = 0;
//  uint8_t cntsame = 0;
//  uint8_t ret_up = 0;

//aux
//uint8_t trefst_aux = 0;
//uint8_t trefst2_aux = 0;
//aux
//////////////////////////////////////////////////////////////////////////////////////////////////7
//uint16_t adcram = 0;
//*************************************************************************************************

/*
 * Prototipos de las Funciones
 */

// Funciones para Reducir codigo
uint16_t promedio_8(uint16_t * muestras_);

// Operaciones Aritmeticas con numeros Enteros como si fueran Flotantes
void sumar(	int sum1Ent, int sum1Frac,
			int sum2Ent, int sum2Frac,
			int * ResulEnt, int * ResulFrac);

void multiplicar(	int factor1Ent, int factor1Frac,
					int factor2Ent, int factor2Frac,
					int * productoEnt, int * productoFrac);

void dividir(	int divisorEnt, int divisorFrac,
				int dividendoEnt, int dividendoFrac,
				int * cocienteEnt, int * cocienteFrac);

// Muestreo y Conversion de los OPAMPS a Temperatura
void muestreoOPAMx(	uint16_t * muestraOPx, uint16_t * promedioOPx, uint8_t *countOPx,
					int * sensorEntOPx, int * sensorFracOPx);


void temper(void){


	cnttemp++;
	//A = cnttemp;

	if(cnttemp >= 50) 	   //Manuel 07-Mar-2022	$64;		/ cntemp es menor
		goto tempe05;  // Sí no tomes muestras
	goto fintemp;      // No, sincroniza la lectura con el dígito 1

tempe05:
		decwreg(&retcncfa); // Decrementa tiempo para liberar fallas
		decwreg(&retcncfe);
		decwreg(&retcncfr);
		//decwreg(&retcncfc);
temper_j00:
		cnttemp = 0;   	   // Inicia el contador de 100 ms

		/*CGM 09/05/2025
		 * Toma de Muestras de los OPAMPS
		 */
//		countSel++;
//		switch(countSel){
//			case 1:
//				ADC1->CHSELR = ADC_CHSELR_CHSEL14;
//				break;
//			case 2:
//				//ADC1->CHSELR = ADC_CHSELR_CHSEL8;
//				break;
//			case 3:
//				//ADC1->CHSELR = ADC_CHSELR_CHSEL4;
//				break;
//			case 4:
//				//ADC1->CHSELR = ADC_CHSELR_CHSEL15;
//				break;
//			default:
//				countSel = 0;
//				break;
//		}


//-------------------------------------------------------------------------------------------------
//-------------Tercer sensor de temperatura--------------------------------------------------------

											//ADC Temperatura Salida de aire		        // Habilitar el canal 0 en la secuencia de conversión usando la directiva definida
	ADC1->CHSELR = 0;
	ADC1->CHSELR |= ADC_CHSELR_CHSEL18;  // Canal 0
	capturaAD();

lookshort_S3:
	//Y_A();
	//Y = adcramh;
	//A = edorefri;

	if(edorefri == 1)
		goto cmp_man_01_s03;    		// En autoprueba deja estos límites
	if(adcramh < 950)                 	// Límite superior 942 (4.6v (rt = 27.4k rs = 55))
		goto lookopen_S03;      		// No, revise límite inferior
	goto sens_short_S03;

cmp_man_01_s03:
		if(adcramh < 0x01C4)         // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
			goto lookopen_S03; // No, revise límite inferior

sens_short_S03:
		//		BitSet(trefst2,4);     // Indica la falla (Sensor de salida en corto)
		//		BitClear(trefst2,5);   // Cancela la otras falla del sensor de salida
		Bset_Clear_trfst(&trefst2[0], &trefst2[0],4, 5);

		goto loadret_S03;      // Carga el retardo de cancelación

lookopen_S03:
		//A = edorefri;
		if(edorefri == 1)
			goto cmp_man_02_S03;   // En autoprueba deja estos límites

		if(adcramh >= 0x002B)            // Ajuste en comportamiento de la Falla del sensor evaporador
			goto sens_ok_S03;      // Sí, indica sensor OK
		goto sens_open_S03;

cmp_man_02_S03:
		if(adcramh >= 0x01AE)            // Límite inferior 0x1B4 /  con tolerancia del 1% en el probador.
			goto sens_ok_S03;      // Sí, indica sensor OK

sens_open_S03:
		Bset_Clear_trfst(&trefst2[0], &trefst2[0],5, 4);
//		BitSet(trefst2,5);         // Indica la falla (Sensor de salida abierto)
//		BitClear(trefst2,4);       // Cancela la otras falla del sensor de salida

loadret_S03:
		retcncfr = 0x0F;           // Carga retardo de cancelación de falla; 1.5 segundos
		goto tempeLoad_s03;

sens_ok_S03:
		//A = retcncfr;
		if(retcncfr != 0)                 // Ya se agotó el retardo?
			goto tempeLoad_s03;
		Bclear_Clear_trfst(&trefst2[0], &trefst2[0],4, 5);
//		BitClear(trefst2,4);       // Cancela banderas de falla de sensor de salida
//		BitClear(trefst2,5);	   // Cancela banderas de falla de sensor de salida

tempeLoad_s03:
		tretram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra
//-------------------------------------------------------------------------------------------------
									//ADC Temperatura Enfriador

		ADC1->CHSELR = 0;
		ADC1->CHSELR |= ADC_CHSELR_CHSEL10;  // Canal 0
		capturaAD ();

cmp_shr:
		//Y_A();
		//Y = adcramh;
		//A = edorefri;

		if(edorefri == 1)
			goto cmp_man_01;   // En autoprueba deja estos límites
		if(adcramh < 950)            // Límite superior 942 (4.6v (rt = 27.4k rs = 55))
			goto lookopen;     // No, revise límite inferior
		goto sens_short;

cmp_man_01:
		if(adcramh < 0x01C4)         // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
			goto lookopen;     // No, revise límite inferior

sens_short:
		Bset_Clear_trfst(&trefst[0], &trefst[0],0, 1);

//		BitSet(trefst,0);       //0x01;	/ Indica la falla (Sensor de ambiente en corto)
//		BitClear(trefst,1);     //0x02;	/ Cancela la otras falla del sensor de ambiente interno
		goto loadret;           // Carga el retardo de cancelación

lookopen:
		//A = edorefri;
		if(edorefri == 1)
			goto cmp_man_02;    // En autoprueba deja estos límites
		if(adcramh >= 0x002B)         // Ajuste en comportamiento de la Falla del sensor evaporador IJG 30/07/14
			goto sens_ok;       // Sí, indica sensor OK
		goto sens_open;

cmp_man_02:
		if(adcramh >= 0x01AE)         // Límite inferior 0x1B4 /  con tolerancia del 1% en el probador.
			goto sens_ok;       // Sí, indica sensor OK

sens_open:
		Bset_Clear_trfst(&trefst[0], &trefst[0],1, 0);
//		BitSet(trefst,1);      //0x02;	/ Indica la falla (Sensor de ambiente abierto)
		//BitSet(trefst_aux,1);      //0x02;	/ Indica la falla (Sensor de ambiente abierto)
//		BitClear(trefst,0);    //0x01;	/ Cancela la otras falla del sensor de ambiente interno
		//BitClear(trefst_aux,0);    //0x01;	/ Cancela la otras falla del sensor de ambiente interno

loadret:
		retcncfa = 0x0F;   	   // Carga retardo de cancelación de falla; 1.5 segundos
		cnthold = 0;           // Cancela el retardo de retención de display
		goto tempe13;

sens_ok:
		//A = retcncfa;
		if(retcncfa != 0)            // Ya se agotó el retardo?
			goto tempe13;
sens_ok10:
		Bclear_Clear_trfst(&trefst[0], &trefst[0],0, 1);
//		BitClear(trefst,0);   // Cancela banderas de falla de sensor de ambiente
//		BitClear(trefst,1);   // Cancela banderas de falla de sensor de ambiente

tempe13:
		tambram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra
//-------------------------------------------------------------------------------------------------
	//; ADC Temperatura Deshielo

		ADC1->CHSELR = 0;
		ADC1->CHSELR |= ADC_CHSELR_CHSEL14;  // Canal 0
		capturaAD ();

		//Y_A();
		//Y = adcramh;
		//A = edorefri;

		if(edorefri == 1)
			goto cmp_man_03;   // En autoprueba deja estos límites
		if(adcramh < 950)            // Límite superior 942 (4.6v (rt = 27.4k rs = 55))
			goto lookopene;    // No, revise límite inferior
		goto senshorte;

cmp_man_03:
	if(adcramh < 0x01C4)         // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
		goto lookopene;    // No, revise límite inferior

senshorte:
	Bset_Clear_trfst(&trefst[0], &trefst[0],2, 3);

//	BitSet(trefst,2);      //0x04   / Indica sensor del evaporador en corto
//	BitClear(trefst,3);     //0x28;	/ Cancela las otras dos fallas del sensor de evaporador
	goto loadret2;         // Carga retardo de cancelación de falla

lookopene:
	//A = edorefri;
	if(edorefri == 1)
		goto cmp_man_04;   // En autoprueba deja estos límites
	if(adcramh >= 0x002B)        // Ajuste en comportamiento de la Falla del sensor evaporador IJG 30/07/14
		goto sens_oke;     // Si, entrega el dato medido y cancela las banderas de falla
	goto sens_opene;

cmp_man_04:
	if(adcramh >= 0x01AE)        // Límite inferior 0x1B4 /  con tolerancia del 1% en el probador.
		goto sens_oke;     // Si, entrega el dato medido y cancela las banderas de falla

sens_opene:
	Bset_Clear_trfst(&trefst[0], &trefst[0],3, 2);
//	BitSet(trefst,3);   //0x08;	/ Indica sensor del evaporador abierto
//	BitClear(trefst,2); //0x24;	/ Cancela las otras dos fallas del sensor de evaporador

loadret2:
	retcncfe = 0x14;    // Carga retardo de cancelación de falla; 2 segundos
	goto tempe16;

sens_oke:
	//A = retcncfe;
	if(retcncfe != 0)          // Ya se agotó el retardo?
		goto tempe16;
	Bclear_Clear_trfst(&trefst[0], &trefst[0],2, 3);
//	BitClear(trefst,2); //0x0C;	/ Cancela indicaciones de falla del sensor del evaporador
//	BitClear(trefst,3); //0x0C;	/ Cancela indicaciones de falla del sensor del evaporador

tempe16:
	if(trefst[f_sdc]) //#f_sdc // El sensor del evaporador esta en corto?
	//if(GetRegFlagState(trefst_aux, 2)) //#f_sdc // El sensor del evaporador esta en corto?
		goto temper_j01;
	goto tempe16a;                 // No, revisa temperatura alta

temper_j01:
		adcramh = 0x000A;           // Carga el dato de temperatura del evaporador con < -50°C
tempe16a:
		tevaram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra

		cntmues++;         //Incrementa el contador de muestra
		A = cntmues;
		if(A >= 8)         // Ya fueron 8 muestras?
			goto tempe17;  // Si, obten el promedio
		goto fintemp;

tempe17:

	resul = promedio_8(&tretram[0]);
	linealiza();
	tret_w = tempo;    /// Entrega el dato de temperatura de salida linealizada

	resul = promedio_8(&tevaram[0]);

tempe24:

	linealiza();
	tevaf = lowByte(tempo);      // Entrega el dato de temperatura del evaporador a 10 bits
	teval = highByte(tempo);

	resul = promedio_8(&tambram[0]);

	if (resul == promant)
		goto tempe27;                //nota1: checar comparaciones
tempe25a:
	if (promant >= resul)
		goto tempe25b;            //La diferencia de promedios es positiva?

	//Y = Y - 0xFFFF;               // La diferencia es igual a -1?
	if (resul == 0xFFFF)
		goto vesiestab;  		  // Si, revisa estabilidad

	resul = resul - 1;
	goto tempe26;

tempe25b:
		if ((promant-resul) == 1)
			goto vesiestab;       // Si, revisa estabilidad
		resul = resul + 0x0001;          // Decrementa el promedio actual
		goto tempe26;

vesiestab:
		if (ultimoprm = resul)
			goto estable;  // El nuevo dato es igual a último?
		cntsame = 0;       // No, inicia el contador de promedios iguales
		goto noestab;

estable:
		cntsame++;         // Si, incrementa el contador

noestab:
		Y = resul;
		ultimoprm = Y;     // Almacena el último dato para la siguiente comparación
		A = cntsame;
		if (cntsame < 0x0F) // Ya esta estable por lo menos por 12 segundos?
			goto tempe29;

tempe26:
		X = resul;          // Almacena el dato anteriorpara la siguiente comparación
		promant = resul;
		linealiza();
		tdevf = lowByte(tempo);  // Entrega el dato de temperatura del ambiente en °C con fracción
		tdevl = highByte(tempo);

tempe27:
		ultimoprm = promant;  // Actualiza el dato del último promedio
		cntsame = 0;          // Inicia el contador de promedios iguales hacia arriba

tempe29:
		//ld			A,retcncfa;
		if(retcncfa == 0)			//cp			A,#$00;		/ Ya llegó a cero el retardo
			goto tempe30;			//jreq		tempe30;		/ Si
		//cp			A,#$09;		/ Ya se obtuvo un promedio correcto?
		if(retcncfa < 0x9)
			goto tempe39;	//jrult		tempe39;		/ Si, cargalo al display
		//;//manuel reduc...     jp			tempe40
		//
		//ldw			X,#$FE34
		tdevf = 0x34;		//ldw			tdevl,X
		tdevl = 0xFE;		//
		goto tempe40;		//jra			tempe40


tempe30:
		if(!flagsa[arran]) //#arran   // Estas en período de arranque?     nota 2: Cambiar flagsa a registro
			goto temper_j02;      // Si, toma el dato de temperatura medido
		goto tempe39;

temper_j02:
		Y = tdevdpy_w;         // manuel_math_change//   tdevdpyl;
		//Y = Y - tDisplay_w;    // w = TdeVdpy - TdeV
		Y = Y - tdevl;
		if (Y == 0)
			goto tempe40;
		else if ((GetRegFlagState(Y, 15)) == 0)                               //nota3 checar combinacion con JRSGT
			goto tempe39;      // La diferencia es positiva?

		A =  Plantilla [filtro];
		if(A == 0)
			goto tempe39;
		A = A << 1;
		ret_up++;              // 800 ms más de retardo

		if(A > ret_up)
			goto tempe50;

		Y = (Y ^ 0xFFFF) + 1;
		if (Y < 11)           // Se compara con 0xFE si se quieren dos decrementos abajo
			goto tempe37b;

		X = tdevl;
		X = X - 10;            //w = tdev - 1.0
		//waux = X;
		waux = X;
		goto tempe39a;

tempe37b:
		X = tdevdpy_w;     //manuel_math_change//  tdevdpyf;
		X = X + 1;         //manuel_math_change//  tdevdpyl;
		//waux = X;
		waux = X;
		goto tempe39a;

tempe39:
		wreg = tdevf;		//call		ld_tdev_to_wreg
		waux = tdevl;
		//clr			cnthold;	/ Cancela el retardo
		cnthold = 0;                  // Cancela el retardo

// manuel_math_change ............................. filtro digital
tempe39a:

		A = cnthold;
		if (A == 0)                   // Ya terminó el tiempo?
			goto tempe39b;            // Si, carga el dato a tdevdpy

		//Y = limsup_w;                 // manuel_math_change//   limsupl;/ Compara contra el límite superior para que congele en ese valor
		//Y = Y - waux;                 // tdev esta por arriba de limsup?
		//if(Y < 0)   //////////////////JRSLT              nota4: checar combinacion con JRSLT
		//if(GetRegFlagState(Y, 15))
		if(limsup_w < waux)
			goto tempe40;             // Si, congela el display en limsup
tempe39b:
//tdevdpyl,waux;	/ Almacena el dato anterior para la siguiente comparación
		/*
		 * No se hizo correctamente el cargado de la Palabra
		 */
		// X = waux;
		X = waux;

		tdevdpy_w = ((uint16_t) tdevl << 8) + tdevf;
tempe40:
		ret_up = 0;  // Inicia el retardo hacia arriba
tempe50:
		cntmues = 0;
		if (!trefst[f_sda]) //#f_sda
		//if (GetRegFlagState(trefst_aux, 3) == 0) //#f_sda
			goto tempe52;
		X = 0xFE34;
		tevaf = lowByte(X);      // Entrega el dato de temperatura del evaporador a 10 bits
		teval = highByte(X);

tempe52:
		if (!trefst2[f_s3open]) //#f_s3open
		//if (GetRegFlagState(trefst2_aux, 5) == 0) //#f_s3open
			goto tempe53;
		X = 0xFE34;
		tret_w = X;

tempe53:

fintemp:

		A = edorefri;

		if (A == 1)
			goto noClrSensFail;  // sí estás en autoprueba permite fallas

clrEvaFail:

		if( GetRegFlagState(Plantilla[numSens],f_sen2) )
			goto clrRetFail;
		Bclear_Clear_trfst(&trefst[0], &trefst[0],2, 3);

clrRetFail:
		if( GetRegFlagState(Plantilla[numSens],f_sen3) )
			goto noClrSensFail;
		Bclear_Clear_trfst(trefst2, trefst2,4, 5);
//		BitClear(trefst2,4);     // Cancela la fallas del sensor de retorno
//		BitClear(trefst2,5);

noClrSensFail:
		asm ("nop");




}

//*************************************************************************************************

void linealiza (void)
{
	tempo = 0;
	Y = resul;
	curva = Y;                  // Almacena el dato para ajuste de curva

	//  verifica en que rango de la curva cae
	if(curva >= 910)
		goto wise_hot_3;
	else if(curva >= 826)
		goto wise_hot_2;
	else if(curva >= 683)
		goto wise_hot_1;
	else if(curva >= 203)
		goto lil_principal;
	else
		goto curva_fria;


wise_hot_3:
	wise_hot_rep (83, 0xFFFF);
	goto fincurva;

wise_hot_2:
	wise_hot_rep (52, 0x93CC);
	goto fincurva;

wise_hot_1:
	wise_hot_rep (29, 0x4980);
	goto fincurva;

lil_principal:

	wise_hot_rep (19, 0x2ECC);
	goto fincurva;

curva_fria:
	wise_hot_rep (30, 0x3780);

fincurva:

	X = tempo;
	Y = X;                   //guarda una copia en Y
	if(GetRegFlagState(Y, 15))  //es dato positivo o negativo ?
		X = (X ^ 0xFFFF) + 1;


	A = highByte(X);          //guarda el valor de la parte alta
	waux = A;
	A = 10;

	X = lowByte(X) * A;
	A = highByte(X);
	X = 0;                    //borra la parte baja no interesa ya que son centesimas y milesimas etc.....
	X = A;

	tempo = X;                //esta es la parte de la decena

	//procesa la parte entera
	A = waux;                 //guarda el valor de l parte alta
	X = A;

	A = 10;
	X = lowByte(X) * A;

	X = X + tempo;

	if(GetRegFlagState(Y, 15))
	X = (X ^ 0xFFFF) + 1;

	tempo = X;              //esta es la parte de la decena
}

void mult1x2(void)
{
	resul = Y;                   // salva el valor a multiplicar

	A = highByte(resul);
	Y = A;
	A = wreg;                    //multiplicador

	Y = Y * A;                   // Realiza la multiplicación de las dieciseisenas

	raux = Y;                    //Y Guardalo

	A = lowByte(resul);
	Y = A;
	A = wreg;                    //multiplicador
	Y = Y*A;                     // Realiza la multiplicación de las unidades

	A = lowByte(Y);
	resul = A;
	A = highByte(Y);
	A = A + lowByte(raux);      //Suma dieciseisenas

	resul = (A << 8) + resul;
}

void capturaAD(void)
{
	ADC1->CR |= ADC_CR_ADEN;
	ADC1->ISR |= (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR);
	ADC1->CR |= ADC_CR_ADSTART;

	while ((ADC1->ISR & ADC_FLAG_EOC) == 0)
	{
	}
	ADC1->CR &= ~(ADC_IER_EOCIE | ADC_IER_EOSIE);

	for(uint8_t i=0; i < 10; i++);

	adcramh = ADC1->DR;
}

void wise_hot_rep (uint8_t wreg_p, uint16_t X_p)
{
	wreg = wreg_p;              // Multiplica el dato por el factor
	Y = curva;

	mult1x2();              // Multiplica

	X = resul;              // En tempo2 quedó el entero del dato de temperatura
	X = X - X_p;         // ordenada al origen en °C  (-255.996)
	tempo = X;
}

/* 		CGM 06/05/2025
 * 		Se realiza un promedio de las  muestras guardadas en un arreglo de 8 elementos
 */

uint16_t promedio_8(uint16_t * muestras_){
	uint16_t promedioX = 0;
	for(uint8_t i = 0; i < 8; i++ ){
		promedioX +=  (*muestras_);
		muestras_++;
	}
	return (promedioX >> 3);
}



void Y_A (void)
{
	Y = adcramh;
	A = edorefri;
}

void Bset_Clear_trfst(uint8_t * trfst_1, uint8_t * trfst_2,uint8_t X, uint8_t Y)
{
	*(trfst_1 + X) = 1; // BitSet(trfst_1,X);   //#f_s4short   // Indica la falla (Sensor de salida en corto)
	//BitSet(trefst_aux,5);   //#f_s4short   // Indica la falla (Sensor de salida en corto)
	*(trfst_2 + Y) = 0; // BitClear(trfst_2,Y); //#f_s4open   // Cancela la otras falla del sensor de salida
}

void Bclear_Clear_trfst(uint8_t * trfst_3, uint8_t * trfst_4,uint8_t V, uint8_t W)
{
	*(trfst_3 + V) = 0;// BitClear(trfst_3,V);      // Cancela la fallas del sensor de evaporador
	//BitClear(trefst_aux,2);      // Cancela la fallas del sensor de evaporador
	*(trfst_4 + W) = 0;// BitClear(trfst_4,W);
}

/* 		CGM 06/05/2025
 *  Suma para dos numeros para tratar los datos enteros como si fueran flotantes
 *
 * _sumXEnt_:	Representa la parte Entera del Sumando.
 * _factorXFrac_:	Representa la parte Decimal del Factor; Las partes de Fraccionarias deben de ser 5 Digitos.
 * 						factorXFrac = 1 -> 0.00001	factorXFrac = 12 -> 0.00012		factorXFrac = 12345 -> 0.12345
 * _ResulEnt_: 	Almacena en la dirección dada el resultado de la parte entera de la Suma
 * _ResulFrac_: Almacena en la dirección dada el resultado de la parte decimal de la Suma
 */
void sumar(	int sum1Ent, int sum1Frac,
			int sum2Ent, int sum2Frac,
			int * ResulEnt, int * ResulFrac)
{
	// se almacenan la parte entera y fraccionaria
	int64_t sum1Ent_ = (int64_t) (sum1Ent*mulDigFrac + sum1Frac);
	int64_t sum2Ent_ = (int64_t) (sum2Ent*mulDigFrac + sum2Frac);

	// Se realiza la suma o la resta dependiendo de la diferencia de signos
	int64_t Suma = sum1Ent_ + sum2Ent_;

	int SumaEnt = (int) (Suma/mulDigFrac);
	int SumaFrac =  (Suma - SumaEnt*mulDigFrac);

	*ResulEnt = SumaEnt;
	*ResulFrac = SumaFrac;
}

/*		CGM 05/05/2025
 * Multiplicador para dos Factores para tratar los datos enteros como si fueran flotantes.
 *
 * _factorXEnt_:	Representa la parte Entera del Factor.
 * _factorXFrac_:	Representa la parte Decimal del Factor; Las partes de Fraccionarias deben de ser 5 Digitos.
 * 						factorXFrac = 1 -> 0.00001	factorXFrac = 12 -> 0.00012		factorXFrac = 12345 -> 0.12345
 * _productoEnt_: 	Almacena en la dirección dada el resultado de la parte entera de la multiplicación
 * _productoFrac_: 	Almacena en la dirección dada el resultado de la parte decimal de la multiplicación
 */
void multiplicar(	int factor1Ent, int factor1Frac,
					int factor2Ent, int factor2Frac,
					int * productoEnt, int * productoFrac)
{
	// Factores con sus respectivas Partes Enteras y Fraccionarias
	int64_t Factor1 = (int64_t) (factor1Ent * mulDigFrac + factor1Frac);
	int64_t Factor2 = (int64_t) (factor2Ent * mulDigFrac + factor2Frac);

	// Multiplicación y acomodo de los Resultados
	int64_t Resultado = Factor1 * Factor2;
	int64_t ResultadoEnt = (Resultado/(mulDigFrac))/mulDigFrac;
	int64_t ResultadoFrac = (Resultado - (ResultadoEnt*mulDigFrac*mulDigFrac))/mulDigFrac;

	// Almacena los Resultados
	*productoEnt = (int) ResultadoEnt;
	*productoFrac = (int) ResultadoFrac;
}

/*		CGM 05/05/2025
 * División de dos numeros
 * _divisorEnt_: Parte entera del Divisor
 * _divisorEnt_: Parte decimal del Divisor
 * _dividendoFrac_: Parte entera del Dividento
 * _dividendoFrac_: Parte decimal del Dividento
 * _cocienteEnt_: Parte Entera del Resultado
 * _cocienteFrac_: Parte Decimal del Resultado
 */
void dividir(	int divisorEnt, int divisorFrac,
				int dividendoEnt, int dividendoFrac,
				int * cocienteEnt, int * cocienteFrac)
{
	// Divisor y Dividendo con sus respectivas Partes Enteras y Fraccionarias
	int64_t divisor = (int64_t) (divisorEnt * mulDigFrac + divisorFrac);
	int64_t dividendo = (int64_t) (dividendoEnt * mulDigFrac + dividendoFrac);

	// Divison y acomodo de los Resultados
	//int64_t Resultado = dividendo * mulDigFracDiv / divisor;
	int64_t Resultado = (dividendo << corrResDiv) / divisor;
	//int ResultadoEnt = (int) (Resultado/mulDigFracDiv);
	int ResultadoEnt = (int) (Resultado >> corrResDiv);
	//int ResultadoFrac = (int) ((Resultado - ResultadoEnt*mulDigFracDiv)/(divDigFrac));
	int ResultadoFrac = (int) (((Resultado - (ResultadoEnt << corrResDiv))*mulDigFrac) >> corrResDiv);

	// Almacena los Resultados
	*cocienteEnt = ResultadoEnt;
	*cocienteFrac = ResultadoFrac;
}

void muestreoOPAMx(	uint16_t * muestraOPx, uint16_t * promedioOPx, uint8_t *countOPx,
					int * sensorEntOPx, int * sensorFracOPx){
	capturaAD();							// Tomamos la muestra
	muestraOPx[*countOPx] = adcramh; 		// La almacenamos en un buffer
	if(*countOPx == 8){
		// Obtenemos un Promedio de las muestras
		*promedioOPx = 0;
		for(uint8_t i; i<8; i++)
			*promedioOPx += muestraOPx[i];
		*promedioOPx = (*promedioOPx >> 3); // Es igual que dividir entre 8

		// Conversion de la muestra de ADC a Temperatura

		int AxEnt = 0;
		int AxFrac = 0;

		// Ax = promedioOPX * ( 3.3/(1024*Alpha) )
		multiplicar(	*promedioOPx,	00000,
						const1Ent,		const1Frac,
						&AxEnt,			&AxFrac);
		// Gamma = Ax + Beta/Alpha
		sumar(	AxEnt,			AxFrac,
				const4Ent,		const4Frac,
				&gammaEnt_OP1, 	&gammaFrac_OP1);
		// Ax = 1 - Gamma
		sumar(	1,					00000,
				-gammaEnt_OP1,		-gammaFrac_OP1,
				&AxEnt, 			&AxFrac);
		// Ax = Gamma/Ax
		dividir(AxEnt,		AxFrac,
				gammaEnt_OP1,	gammaFrac_OP1,
				&AxEnt,		&AxFrac);
		// Ax = Ax * (R157/3.91)
		multiplicar(	AxEnt,		AxFrac,
						const3Ent,	const3Frac,
						&AxEnt,		&AxFrac);

		// sensorEntOP1 = Ax - (1000/3.91)
		sumar(	AxEnt,			AxFrac,
				-const2Ent,		-const2Frac,
				sensorEntOPx, 	sensorFracOPx);
	}
	else{
		*countOPx += 1;
	}

}


/*
 *
 */


/*
 * NOTAS:   Primero encontrar los canales de ADC para definir bien las conexiones
 * 			Bateria Pendiente
 * 			Definicion de tdev
 * 			Uso de flagsa
 * 			subw y jreq en conjunto
 */

/*
 * #include "main.h"
#include "temper.h"
#include "customMain.h"
#include "math.h"


float       prueba1 = 0.0;
float       adc_f = 0.0;
int         tsac_w_t = 0;
int16_t     tsac_w = 0;

uint16_t    resul = 0;
uint16_t	tsacram [8] = {0};


*************************************************************************************************
uint16_t capturaAD (void)
{
	//Aqui se envian los valores de ADC a las variables correspondientes
	uint16_t   adcram = 0;
	asm("nop");
	//adcram++;
	ADC_Select_CH6 ();
	HAL_ADC_Start(&hadc); // start the adc
	HAL_ADC_PollForConversion(&hadc, 1000); // poll for conversion
	adcram = HAL_ADC_GetValue(&hadc); // get the adc value
	HAL_ADC_Stop(&hadc);
	//Reemplazar por el llamado a funcion o valor de ADC
	//adcram = 0x0FFF;
	return adcram;
}
*************************************************************************************************


void temper(void){
	asm ("nop");

	cnttemp++;
	if(cnttemp == 50)
	{
		//tempe05:
		//ldw			X,#retcncfa;/ Decrementa tiempo para liberar fallas
		//call		decwreg;
		//ldw			X,#retcncfe;
		//call		decwreg;
		//ldw			X,#retcncfr;
		//call		decwreg;
		//ldw			X,#retcncfc;
		//call		decwreg
		cnttemp = 0;
		ADC_Select_CH6();
		// RGM_16/JUL/24:	 capturaAD();
		tsacram[cntmues] = capturaAD();		//	adcram;
		cntmues++;
		if(cntmues == 8)
		tempe17();
	}

}




	//promedio_muestras = promedio_muestras + adcram; ;
	//if(muestras == 8)
	//{
		//curva = promedio_muestras / 8;

		//muestras = 0;
		//promedio_muestras = 0;
		//linealiza();
	//}

void tempe17 (void)
{
	cntmues = 0;
	prom8mues();
	linealiza();

}
void prom8mues(void)
{
	uint16_t resul_aux = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		resul_aux = resul_aux + tsacram[i];
	}
	resul = resul_aux / 8;

}

void linealiza(void)
{
	adc_f = (float) resul;

	if(resul <= 610)
	{
		prueba1 = (0.729727807*adc_f)-462.65612753;
		//prueba1 = (0.726602342*adc_f)-461.468794122;
		tsac_w_t = prueba1;
		tsac_w = tsac_w_t;
		asm ("nop");



	}
	else if(resul > 610)
	{
		prueba1 = (429.8275063742 + (-2.009354582 * adc_f) + (0.002112554* adc_f * adc_f));
		//prueba1 = (289.9866632992 + (-1.643205579 * adc_f) + (0.001875605* adc_f * adc_f));
		tsac_w_t = prueba1;
		tsac_w = tsac_w_t;
		asm ("nop");

	}

}

void ADC_Select_CH6()
{
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc.Instance->CHSELR = (uint32_t) 0;
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&hadc, &sConfig);
}

int calcula_temperatura (int m, int b)
{
	int32_t temperatura = 0;

	temperatura = resul;
	temperatura = (m * temperatura) / 100;

	temperatura = temperatura - b;
	temperatura = temperatura /10;

	return temperatura;

 */
