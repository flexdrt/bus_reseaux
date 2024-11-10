/*
 * BMP280_vincent.h
 *
 *  Created on: Oct 18, 2024
 *      Author: vincent
 */

#ifndef BMP280_VINCENT_H_
#define BMP280_VINCENT_H_

typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;
typedef int64_t BMP280_S64_t;

static const uint8_t BMP280_ADDR = 0x77 << 1; // Use 8-bit address
// static const uint8_t BMP280_ADDR = 0x76 << 1; // Use 8-bit address

static const uint8_t BMP280_ID_REG = 0xD0;//id du registre selon la doc
//uint8_t ctrl_meas = 0xF4;

/* code prof a supprimer une fois que mon code marche
 *
 * */

//static const uint8_t BMP280_ID_REG = 0xD0;
static const uint8_t BMP280_ID_LEN = 1;
static const uint8_t BMP280_ID_VAL = 0x58;


static const uint8_t BMP280_TRIM_REG_MSB = 0x88; //Adresse du registre contenant l'étallonage
static const uint8_t BMP280_TEMP_REG_MSB = 0xFA;//Adresse du registre contenant la température
static const uint8_t BMP280_PRES_REG_MSB = 0xF7;//Adresse du registre contenant la pression

//****************** déclarations fonctions**********************

int BMP280_checkID(void);
void BMP280_config();
int BMP280_init();
void  BMP280_calib(void);

BMP280_S32_t BMP280_get_temperature();
BMP280_S32_t BMP280_get_pressure();

//déclarations fonctions de compensation de bosch
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t );
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t);



#endif /* BMP280_VINCENT_H_ */
