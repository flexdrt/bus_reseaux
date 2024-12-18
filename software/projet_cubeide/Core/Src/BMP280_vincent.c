#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "BMP280_vincent.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;





HAL_StatusTypeDef retour; //Permet de verifier si les fonctions I2C s'exécutent correctement

uint8_t RxBuff[RX_BUFF_SIZE]={0};



//déclaration du pHeader
CAN_TxHeaderTypeDef pHeader;
uint8_t aData[3];  // Tableau pour les données à transmettre
uint32_t pTxMailbox; // Variable pour stocker l'indice de la boîte aux lettres CAN


int K_pid = 0;
int a_pid = 0;
int setK=0;

uint8_t ctrl_meas = 0xF4;
BMP280_S32_t temp_uncompen;
BMP280_S32_t temp_compen;

BMP280_S32_t pres_uncompen;
BMP280_S32_t pres_compen;


//registre etalo temperature
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

//registre etalo pression
short dig_P1 = 0;
signed short dig_P2 = 0;
signed short dig_P3 = 0;
signed short dig_P4 = 0;
signed short dig_P5 = 0;
signed short dig_P6 = 0;
signed short dig_P7 = 0;
signed short dig_P8 = 0;
signed short dig_P9 = 0;

//uint8_t config = (0b010 << 5) | (0b101 << 2) | (0b11);


//envoyer une trame avec l'adresse du registre à l'aide de la fonction HAL_I2C_Master_Transmit().
//buf[0]=BMP_ID_REG;
//HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 1, -1);

//HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, -1);
//printf("Idreg: 0x%x\r\n", buf[0]); 

int BMP280_checkID(void) {


	//FOnctionnel au 23 oct 18h59 <<<<<<< aussi 9 nov 19h34 organisé

	uint8_t buf[10];
	buf[0]=BMP280_ID_REG;

	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 1, -1);

	//printf("checkID : transmission in progress\r\n");

	if(retour != HAL_OK){

		printf(" \n checkID : problem during transmission \r\n");

		return 1;

	}
	else{
		//%%printf("no problem during transmission ... transmission complete -> reception in progress\r\n");
	}


	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, -1);
	if(retour != HAL_OK){

		printf(" \n checkID : problem during reception I2C\r\n");

		return 1;

	}
	else{

		//printf("\n reception complete ...  : Register Id content : 0x%x\r\n", buf[0]);
	}

}




void BMP280_config(void) {
	uint8_t part1 = 0b010 << 5;  // Décalage de 5 bits
	uint8_t part2 = 0b101 << 2;  // Décalage de 2 bits
	uint8_t part3 = 0b11;        // Pas de décalage
	/*
	 * ^
	 *
	 *
	 * printf("Part1 (0b010 << 5) = %u\r\n", part1);   // Affiche 64
	printf("Part2 (0b101 << 2) = %u\r\n", part2);   // Affiche 20
	printf("Part3 (0b11)       = %u\r\n", part3);   // Affiche 3



	 */
	uint8_t sumPart=part1+part2+part3;
	//^^printf(" sum of parts = Part1+Part2+Part3 = %u \r\n",sumPart);   // Affiche 3
	uint8_t config = part1 | part2 | part3;


	uint8_t buf[2];


	buf[0] = ctrl_meas;

	buf[1] = config;  //mot binaire a ecrire pour conf correctement 

	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 2,HAL_MAX_DELAY);

	//%%printf("configuration : transmission in progress\r\n");

	if (retour != HAL_OK) {

		printf("\n config: problem during transmission \r\n\r\n");

	}

	else{
		printf("config : no problem during transmission ... transmission complete -> reception in progress\r\n");
	}

	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, HAL_MAX_DELAY);

	if (retour != HAL_OK) {

		printf("\n config : problem during reception I2C\r\n");

	}
	//^^^printf("config = %u\r\n",config);
	if (buf[0] == config) {

		printf("\nconfig : well configured - configuration value = hexadecimal : 0x%x | decimal  : 0d%u\r\n", buf[0]);



	}



}

void BMP280_calib(void) {
	//**********à supprimer

	// uint8_t etalo = 0x88; //Adresse du premier registre contenant les valeurs d'étalonnage
	//buf_adr[0]=etalo; //buffer contenant l'adresse du registre d'étalonnage

	//**********************

	uint8_t buf_data[24];	//buffer pour les données reçues des registre

	//on demande la lecture avec le transmit

	//BMP280_TRIM_REG_MSB  est déclarée dans BMP280_vincent.h

	// l'adresse de la variable BMP280_TRIM_REG_MSB est passée a la fonction qui attends une adresse en paramètre (celle sur laquelle pointerait le pointeur pData)
	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_TRIM_REG_MSB,1, HAL_MAX_DELAY);

	if (retour != HAL_OK) {
		//printf("Probleme calib I2C\r\n");
		return;
	}
	//on lit la réponse qu'on stocke dans le buffer buf_data
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 24,HAL_MAX_DELAY);

	if (retour != HAL_OK) {
		//^printf("Problem calib I2C\r\n");;


	}
	else {	//on affiche les données d'étallonage reçues

	}
	dig_T1 = buf_data[0] | (buf_data[1] << 8);
	dig_T2 = buf_data[2] | (buf_data[3] << 8);
	dig_T3 = buf_data[4] | (buf_data[5] << 8);

	dig_P1 = buf_data[6] | (buf_data[7] << 8);
	dig_P2 = buf_data[8] | (buf_data[9] << 8);
	dig_P3 = buf_data[10] | (buf_data[11] << 8);
	dig_P4 = buf_data[12] | (buf_data[13] << 8);
	dig_P5 = buf_data[14] | (buf_data[15] << 8);
	dig_P6 = buf_data[16] | (buf_data[17] << 8);
	dig_P7 = buf_data[18] | (buf_data[19] << 8);
	dig_P8 = buf_data[20] | (buf_data[21] << 8);
	dig_P9 = buf_data[22] | (buf_data[23] << 8);



	for (int i = 0; i < 24; i++) {
		//^printf("i :%d\r",i);
		//printf("dans boucle for //registre d'etallonage calib= 0x%x soit %d \r\n ", buf_data[i],buf_data[i]);


	}

}

BMP280_S32_t BMP280_get_temperature() {

	BMP280_S32_t adc_T;

	//buffer pour les données reçues de la part des registres , ici 3 pour la température

	///uint8_t ! pas int!. Si int buf_data[3] :  valeur mal stockée = valeur température  incorrecte
	uint8_t buf_data[3];

	//on demande la lecture avec le transmit

	//BMP280_TEMP_REG_MSB   est déclarée dans BMP280_vincent.h

	// l'adresse de la variable BMP280_TEMP_REG_MSB  est passée a la fonction qui attend une adresse en paramètre (celle sur laquelle pointerait le pointeur pData)
	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_TEMP_REG_MSB, 1,
			HAL_MAX_DELAY);

	if (retour != HAL_OK) {
		//^printf("Probleme I2C\r\n");
	}
	//on lit la réponse qu'on stocke dans le buffer buf_data
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 3,HAL_MAX_DELAY);

	if (retour != HAL_OK) {

		//^printf("problem i2c");

		return 1;
	}

	adc_T = ((BMP280_S32_t) (buf_data[0]) << 12)| ( (BMP280_S32_t) (buf_data[1]) << 4)| ( (BMP280_S32_t) (buf_data[2]) >> 4);






	//printf("%05lX", adc_T);

	//^printf("%d \r\n", adc_T);

	return adc_T;
}

BMP280_S32_t BMP280_get_pressure() {

	BMP280_S32_t adc_P;


	//uint8_t ! pas int!. Si int buf_data[3] :  valeur mal stockée = valeur pression incorrecte
	uint32_t buf_data[3];//buffer pour 	les données reçues de la part des registres , ici 3 pour la pression

	//on demande la lecture avec le transmit

	//BMP280_PRES_REG_MSB est déclarée dans BMP280_vincent.h

	// l'adresse de la variable BMP280_PRES_REG_MSB est passée a la fonction qui attend une adresse en paramètre (celle sur laquelle pointerait le pointeur pData)
	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_PRES_REG_MSB, 1,HAL_MAX_DELAY);

	if (retour != HAL_OK) {
		//printf("problem i2c\r\n");
	}
	//on lit la réponse qu'on stocke dans le buffer buf_data
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 3,HAL_MAX_DELAY);

	if (retour != HAL_OK) {

		//^printf("problem i2c \r\n");

		return 1;
	}

	adc_P = ((BMP280_S32_t) (buf_data[0]) << 12) | ((BMP280_S32_t) (buf_data[1]) << 4) | ((BMP280_S32_t) (buf_data[2]) >> 4);

	//^printf("Pressure adc_P:    0d ");


	//printf("%05lX", adc_P);

	//^printf("%d\r\n", adc_P);



	return adc_P;
}


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
	BMP280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
//test

BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
//affiche une valeur cohérente truc

/*
 *
 *
 *
 * GET_P

251525343

26757075

div 256 *10⁵ 1

P=104519.828125_Pa soit P=1.045198


 *
 *
 *
 *
 */

{
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0){
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19; p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
	return (BMP280_U32_t)p;
}

/* affiche une valeur non cohérente
 *
 *
 * GET_P

228582208

4268442490

div 256 *10⁵ 166

P=16673603.000000_Pa soit P=166.736023
 */


//{
//	BMP280_S64_t var1, var2, p;
//	var1 = ((BMP280_S64_t)t_fine) - 128000;
//	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
//	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
//	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
//	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
//	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
//	if (var1 == 0){
//		return 0; // avoid exception caused by division by zero
//	}
//	p = 1048576-adc_P;
//	p = (((p<<31)-var2)*3125)/var1;
//	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
//	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19; p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
//	return (BMP280_U32_t)p;
//}





HAL_StatusTypeDef tx_can(CAN_TxHeaderTypeDef pHeader2,uint8_t* aData2,uint32_t pTxMailbox2){

	HAL_StatusTypeDef error_out;
	//uint32_t pTxMailbox; // Variable pour stocker l'indice de la boîte aux lettres CAN

	// Envoi du message CAN avec l'angle de 90°
	HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan1, &pHeader2, aData2, &pTxMailbox2);
	error_out=status;
	// Vérification du statut d'envoi
	if (status != HAL_OK) {
		// Gérer l'erreur d'envoi
		printf("erreur HAL_CAN_AddTxMessageCAN\r\n");
	}
	else{
		printf("[tx_can]  envoi reussi\r\n");
	}

	return error_out;


}

void enable_can(){

	//********************Activer le module can***************************************************
	HAL_StatusTypeDef start_can= HAL_CAN_Start(&hcan1);
	if(start_can!= HAL_OK) {

		// Gérer l'erreur de démarrage
		printf("erreur start can_config CAN\r\n");

	}
	else{
		printf("[enable_can] start  CAN OK\r\n");
	}



	//*************************************déclaration du pHeader**********************************

	//CAN_TxHeaderTypeDef pHeader;
	//uint8_t aData[3];  // Tableau pour les données à transmettre

	// Configuration du champ pHeader
	pHeader.StdId = 0x61;          // Identifiant standard pour la commande "Angle"
	pHeader.IDE = CAN_ID_STD;      //(0x00000000U)  !< Standard Id
	pHeader.RTR = CAN_RTR_DATA;    //(0x00000000U)  !< Data frame
	pHeader.DLC = 2;
	pHeader.TransmitGlobalTime = DISABLE;

	aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
	aData[1] = 0x00;  // D1 : Angle positif

	//*************************************transmission au can de la configuration**********************************
	HAL_StatusTypeDef conf_errout=tx_can(pHeader, aData, pTxMailbox);
	// Vérification du statut d'envoi
	if (conf_errout != HAL_OK) {
		// Gérer l'erreur d'envoi
		printf("erreur config_base_ CAN\r\n");
	}
	else{
		printf("config base  envoi reussi\r\n");
	}

}


void can_setter(CAN_TxHeaderTypeDef pHeader, uint8_t* aData,uint32_t pTxMailbox){



	// CAN part start  while loop
	aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
	aData[1] = 1-aData[1];  //


	HAL_StatusTypeDef rota_out=tx_can(pHeader, aData, pTxMailbox);
	if (rota_out != HAL_OK) {
		// Gérer l'erreur d'envoi
		printf(" err rota CAN\r\n");
	}
	else {
		printf("change rotation complete \r\n");

	}


}


void can_change_temp(BMP280_U32_t temp_comp){


	enable_can();

	//TP4 partie CAN d
	//début *******************************************

// récupérer temps****************************
//	BMP280_S32_t temp_uncompen;
//
//	temp_comp=bmp280_compensate_T_int32(temp_uncompen); //récupérer la température compensé
//	//déclaration des variables contenant la température non compensée
//	temp_uncompen= BMP280_get_temperature(); //récupérer la température

	//déclaration des variables contenant la température  compensée
	BMP280_U32_t old_temp;


//********temp_comp = temperature *******************************************


	if(fabs(temp_comp-old_temp)>2){

		if(temp_comp>old_temp){//On bouge dans le sens 0x00 +

			aData[1]=0x00;
			aData[0]=(int)(10*(temp_comp-old_temp));
			HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);
			//printf("adata0= %d\r\n",aData[0]);

			old_temp=temp_comp;
		}
		else{//On bouge dans le sens 0x01 -
			aData[1]=0x01;
			aData[0]=(int)((old_temp-temp_comp));
			HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);
			//printf("adata0= %d\r\n",aData[0]);
			old_temp=temp_comp;
		}


	}

	//tx_can

	//	// Envoi du message CAN avec l'angle de 90°
	//	HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);
	//
	//	// Vérification du statut d'envoi
	//	if (status != HAL_OK) {
	//		// Gérer l'erreur d'envoi
	//		printf(" [can_setter] err while CAN\r\n");
	//	}

	//CAN oart finish while loop


	//old_temp = temp_comp;//mise à jour de la température


}

//*********************************************************************************************

void dial_pi(){

	if(strncmp(RxBuff, "GET_T", 5)==0) {


		//déclaration des variables contenant la température non compensée
		temp_uncompen= BMP280_get_temperature(); //récupérer la température

		//déclaration des variables contenant la température  compensée
		BMP280_U32_t temp_comp;


		temp_comp=bmp280_compensate_T_int32(temp_uncompen); //récupérer la température compensé


		//ancien printf
		//printf("%u \r\n",temp_comp);// AFFICHÉ TEMPÉRATURE compensée sur l'usart


		printf("T=%ld%ld.%ld%ld_C\r\n",(temp_comp/1000)%10,(temp_comp/100)%10,(temp_comp/10)%10,temp_comp%10);
		//T=+12.50_C 	Température compensée sur 10 cafficher aractères
		//GET_P 	P=102300Pa

		//HAL_Delay(1000);
		//interaction sur le moteur de la température
		can_change_temp(temp_comp);


	}

	if (strncmp(RxBuff,"GET_P",5)==0){

		//déclaration des variables contenant la pression non compensée
		//pres_uncompen= BMP280_get_pressure(); //récupérer la température

		//déclaration des variables contenant la pression  compensée
		BMP280_U32_t pres_comp;


		pres_uncompen=BMP280_get_pressure(); //récupérer la pression non compensée

		pres_comp=bmp280_compensate_P_int64(pres_uncompen); //compenser la pression


		//printf("%u \r\n",pres_uncompen);
		//printf("%u \r\n",pres_comp);
		//printf("div 256 *10⁵ %u \r\n", ((pres_comp) / (25600000) ));



		printf("P=%f_Pa  \r\n",((float)(pres_comp))/256);


	}

	if (strncmp(RxBuff,"SET_K=",10)==0){
		//K_pid=RxBuff[7];
		K_pid=0;

		printf("K %d",K_pid);

		//		BMP280_U32_t pres_comp;
		//		pres_uncompen=BMP280_get_pressure(); //récupérer la pression non compensée
		//
		//		pres_comp=bmp280_compensate_P_int64(pres_uncompen); //compenser la pression
		//
		//
		//		printf("%u \r\n",pres_uncompen);
		//
		//		printf("%u \r\n",pres_comp);


	}



	if(strncmp(RxBuff,"GET_A",5)==0){
		a_pid=145;
		printf("A=%d\r\n",a_pid);
	}

	// Réinitialiser RxBuff après chaque commande
	//memset(RxBuff, 0, RX_BUFF_SIZE);

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	dial_pi();
	HAL_UART_Receive_IT(&huart1, RxBuff, RX_BUFF_SIZE);


}

//*********************anciennes fonctions archives ********************


/*
 * void init_motor(CAN_TxHeaderTypeDef pHeader2,uint8_t* aData2,uint32_t pTxMailbox2)
{
	// Configuration du champ pHeader
	pHeader2.StdId = 0x61;          // Identifiant standard pour la commande "Angle"
	pHeader2.IDE = CAN_ID_STD;      //(0x00000000U)  !< Standard Id
	pHeader2.RTR = CAN_RTR_DATA;    //(0x00000000U)  !< Data frame
	pHeader2.DLC = 2;
	pHeader2.TransmitGlobalTime = DISABLE;

	aData2[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
	aData2[1] = 0x00;  // D1 : Angle positif

}
 */



/*void enable_can(){
//Activer le module can
	HAL_StatusTypeDef start_can= HAL_CAN_Start(&hcan1);
	if(start_can!= HAL_OK) {

		// Gérer l'erreur de démarrage
		printf("erreur start can_config CAN\r\n");

	}
	else{
		printf("[main] start  CAN OK");
	}

}

void tx_can(uint8_t * aData){

//déclaration du pHeader
	CAN_TxHeaderTypeDef pHeader;
	uint32_t pTxMailbox; // Variable pour stocker l'indice de la boîte aux lettres CAN
	// Configuration du champ pHeader
	pHeader.StdId = 0x61;          // Identifiant standard pour la commande "Angle"
	pHeader.IDE = CAN_ID_STD;      //(0x00000000U)  !< Standard Id
	pHeader.RTR = CAN_RTR_DATA;    //(0x00000000U)  !< Data frame
	pHeader.DLC = 2;
	pHeader.TransmitGlobalTime = DISABLE;

	aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
	aData[1] = 0x00;  // D1 : Angle positif



	// Envoi du message CAN avec l'angle de 90°
	HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);

	// Vérification du statut d'envoi
	if (status != HAL_OK) {
		// Gérer l'erreur d'envoi
		printf("erreur TX CAN\r\n");
	}
	else{
		printf("start TX can OK");
	}


}




void can_setter(){

	uint8_t aData[3];


	//on peut définirla vitesse aussi
	aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
	aData[1] = 0x00;  // D1 : Angle positif donc on ajoute 90 degrès

	tx_can(aData);


}*/
