#include <stdio.h>
#include "stdlib.h"

#include "main.h"
#include "BMP280_vincent.h"

extern I2C_HandleTypeDef hi2c1;
HAL_StatusTypeDef retour; //Permet de verifier si les fonctions I2C s'exécutent correctement
int RX_BUFF_SIZE = 500;
//uint8_t RxBuff[RX_BUFF_SIZE]={0};
int K_pid = 0;
int A_pid = 0;
uint8_t ctrl_meas = 0xF4;
BMP280_S32_t temp_uncompen;
BMP280_S32_t temp_compen;

BMP280_S32_t pres_uncompen;
BMP280_S32_t pres_compen;

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

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

	printf("checkID : transmission in progress\r\n");

	if(retour != HAL_OK){

		printf(" \n checkID : problem during transmission \r\n");

		return 1;

	}
	else{
		printf("no problem during transmission ... transmission complete -> reception in progress\r\n");
	}


	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, -1);
	if(retour != HAL_OK){

		printf(" \n checkID : problem during reception I2C\r\n");

		return 1;

	}
	else{

		printf("\n reception complete ...  : Register Id content : 0x%x\r\n", buf[0]);
	}

}




void BMP280_config(void) {
	uint8_t part1 = 0b010 << 5;  // Décalage de 5 bits
	uint8_t part2 = 0b101 << 2;  // Décalage de 2 bits
	uint8_t part3 = 0b11;        // Pas de décalage
	printf("Part1 (0b010 << 5) = %u\r\n", part1);   // Affiche 64
	printf("Part2 (0b101 << 2) = %u\r\n", part2);   // Affiche 20
	printf("Part3 (0b11)       = %u\r\n", part3);   // Affiche 3
	uint8_t sumPart=part1+part2+part3;
	printf(" sum of parts = Part1+Part2+Part3 = %u \r\n",sumPart);   // Affiche 3
	uint8_t config = part1 | part2 | part3;


	uint8_t buf[2];


	buf[0] = ctrl_meas;

	buf[1] = config;  //mot binaire a ecrire pour conf correctement 

	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 2,HAL_MAX_DELAY);

	printf("configuration : transmission in progress\r\n");

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
	//printf("config = %u\r\n",config);
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
		printf("Probleme calib I2C\r\n");
		return;
	}
	//on lit la réponse qu'on stocke dans le buffer buf_data
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 24,HAL_MAX_DELAY);

	if (retour != HAL_OK) {
		printf("Problem calib I2C\r\n");;


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
		printf("i :%d\r",i);
		printf("dans boucle for //registre d'etallonage calib= 0x%x soit %d \r\n ", buf_data[i],buf_data[i]);


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
		printf("Probleme I2C\r\n");
	}
	//on lit la réponse qu'on stocke dans le buffer buf_data
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 3,HAL_MAX_DELAY);

	if (retour != HAL_OK) {

		printf("problem i2c");

		return 1;
	}

	adc_T = ((BMP280_S32_t) (buf_data[0]) << 12)| ( (BMP280_S32_t) (buf_data[1]) << 4)| ( (BMP280_S32_t) (buf_data[2]) >> 4);
	printf("Temperature adc_T: 0d ");



	//printf("%05lX", adc_T);

	printf("%d \r\n", adc_T);

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
		printf("problem i2c\r\n");
	}
	//on lit la réponse qu'on stocke dans le buffer buf_data
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 3,HAL_MAX_DELAY);

	if (retour != HAL_OK) {

		printf("problem i2c \r\n");

		return 1;
	}

	adc_P = ((BMP280_S32_t) (buf_data[0]) << 12) | ((BMP280_S32_t) (buf_data[1]) << 4) | ((BMP280_S32_t) (buf_data[2]) >> 4);

	printf("Pressure adc_P:    0d ");


	//printf("%05lX", adc_P);

	printf("%d\r\n", adc_P);



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


BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
	return (BMP280_U32_t)p;
}
