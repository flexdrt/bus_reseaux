/**
 * @file BMP280_vincent.c
 * @brief Implémentation du pilote pour le capteur de température et de pression BMP280
 * @author Vincent LAKHMECHE
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "BMP280_vincent.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

/** @brief Variable pour vérifier le statut des communications I2C */
HAL_StatusTypeDef retour;

/** @brief Buffer de réception pour les communications UART */
uint8_t RxBuff[RX_BUFF_SIZE]={0};

/** @brief Déclaration des paramètres CAN */
CAN_TxHeaderTypeDef pHeader;  /*< Structure contenant les paramètres d'en-tête CAN */
uint8_t aData[3];            /*< Tableau pour les données à transmettre */
uint32_t pTxMailbox;         /*< Variable pour la boîte aux lettres CAN */

/** @brief Variables pour le contrôle PID */
int K_pid = 0;
int a_pid = 0;
int setK = 0;

/** @brief Registre de contrôle des mesures */
uint8_t ctrl_meas = 0xF4;

/** @brief Variables pour les mesures de température */
BMP280_S32_t temp_uncompen;  /< Température non compensée */
BMP280_S32_t temp_compen;    /< Température compensée */

/** @brief Variables pour les mesures de pression */
BMP280_S32_t pres_uncompen;  /< Pression non compensée */
BMP280_S32_t pres_compen;    /< Pression compensée */

/** @brief Coefficients de calibration pour la température */
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

/** @brief Coefficients de calibration pour la pression */
short dig_P1 = 0;
signed short dig_P2 = 0;
signed short dig_P3 = 0;
signed short dig_P4 = 0;
signed short dig_P5 = 0;
signed short dig_P6 = 0;
signed short dig_P7 = 0;
signed short dig_P8 = 0;
signed short dig_P9 = 0;

/**
 * @brief Vérifie l'identité du capteur BMP280
 * @return 1 en cas d'erreur, 0 sinon
 */
int BMP280_checkID(void) {
    uint8_t buf[10];
    buf[0] = BMP280_ID_REG;

    retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 1, -1);
    if(retour != HAL_OK) {
        printf("\ncheckID : problème lors de la transmission\r\n");
        return 1;
    }

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, -1);
    if(retour != HAL_OK) {
        printf("\ncheckID : problème lors de la réception I2C\r\n");
        return 1;
    }
    return 0;
}

/**
 * @brief Configure le capteur BMP280
 * Configuration : mode normal, suréchantillonnage x16 pour la pression,
 * x2 pour la température
 */
void BMP280_config(void) {
    // Construction du mot de configuration
    uint8_t part1 = 0b010 << 5;  // t_sb = 0.5ms
    uint8_t part2 = 0b101 << 2;  // filter = 16
    uint8_t part3 = 0b11;        // Mode normal

    uint8_t config = part1 | part2 | part3;
    uint8_t buf[2];

    buf[0] = ctrl_meas;
    buf[1] = config;

    retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 2, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        printf("\nconfig: problème lors de la transmission\r\n\r\n");
    }
    else {
        printf("config : transmission réussie\r\n");
    }

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        printf("\nconfig : problème lors de la réception I2C\r\n");
    }

    if (buf[0] == config) {
        printf("\nconfig : bien configuré - valeur de configuration = hex : 0x%x | dec : 0d%u\r\n", buf[0], buf[0]);
    }
}

/**
 * @brief Lit les coefficients de calibration du capteur
 */
void BMP280_calib(void) {
    uint8_t buf_data[24];

    retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_TRIM_REG_MSB, 1, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        return;
    }

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 24, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        return;
    }

    // Extraction des coefficients de calibration
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
}

/**
 * @brief Lit la valeur de température brute
 * @return Valeur brute de température sur 32 bits
 */
BMP280_S32_t BMP280_get_temperature(void) {
    BMP280_S32_t adc_T;
    uint8_t buf_data[3];

    retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_TEMP_REG_MSB, 1, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        return 1;
    }

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 3, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        return 1;
    }

    // Composition de la valeur 20 bits
    adc_T = ((BMP280_S32_t)(buf_data[0]) << 12) |
            ((BMP280_S32_t)(buf_data[1]) << 4) |
            ((BMP280_S32_t)(buf_data[2]) >> 4);

    return adc_T;
}

/**
 * @brief Lit la valeur de pression brute
 * @return Valeur brute de pression sur 32 bits
 */
BMP280_S32_t BMP280_get_pressure(void) {
    BMP280_S32_t adc_P;
    uint32_t buf_data[3];

    retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_PRES_REG_MSB, 1, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        return 1;
    }

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf_data, 3, HAL_MAX_DELAY);
    if (retour != HAL_OK) {
        return 1;
    }

    // Composition de la valeur 20 bits
    adc_P = ((BMP280_S32_t)(buf_data[0]) << 12) |
            ((BMP280_S32_t)(buf_data[1]) << 4) |
            ((BMP280_S32_t)(buf_data[2]) >> 4);

    return adc_P;
}

/** @brief Variable globale pour la compensation de température */
BMP280_S32_t t_fine;

/**
 * @brief Compense la valeur de température selon l'algorithme du fabricant
 * @param adc_T Valeur brute de température
 * @return Température en degrés Celsius x100
 */
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T) {
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *
            ((BMP280_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

/**
 * @brief Compense la valeur de pression selon l'algorithme du fabricant
 * @param adc_P Valeur brute de pression
 * @return Pression en Pascal (format Q24.8)
 */
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P) {
    BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
    var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
    var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
    var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
    if (var1 == 0) {
        return 0;  // Évite la division par zéro
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
    return (BMP280_U32_t)p;
}

/**
 * @brief Transmet un message via CAN
 * @param pHeader2 En-tête du message
 * @param aData2 Données à transmettre
 * @param pTxMailbox2 Boîte aux lettres utilisée
 * @return Statut de transmission
 */
HAL_StatusTypeDef tx_can(CAN_TxHeaderTypeDef pHeader2, uint8_t* aData2, uint32_t pTxMailbox2) {
    HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan1, &pHeader2, aData2, &pTxMailbox2);
    if (status != HAL_OK) {
        printf("erreur HAL_CAN_AddTxMessage\r\n");
    }
    else {
        printf("[tx_can] envoi réussi\r\n");
    }
    return status;
}












/**
 * @brief Active et configure le périphérique CAN
 */
void enable_can(void) {
    HAL_StatusTypeDef start_can = HAL_CAN_Start(&hcan1);
    if(start_can != HAL_OK) {
        printf("erreur start can_config CAN\r\n");
    }
    else {
        printf("[enable_can] start CAN OK\r\n");
    }

    // Configuration de l'en-tête CAN
    pHeader.StdId = 0x61;          // ID standard pour la commande "Angle"
    pHeader.IDE = CAN_ID_STD;      // Format standard
    pHeader.RTR = CAN_RTR_DATA;    // Trame de données
    pHeader.DLC = 2;               // 2 octets de données
    pHeader.TransmitGlobalTime = DISABLE;

    // Configuration initiale des données
    aData[0] = 90;   // Angle de 90°
    aData[1] = 0x00; // Angle positif

    // Envoi de la configuration de base
    HAL_StatusTypeDef conf_errout = tx_can(pHeader, aData, pTxMailbox);
    if (conf_errout != HAL_OK) {
        printf("erreur config_base_ CAN\r\n");
    }
    else {
        printf("config base envoi réussi\r\n");
    }
}



/**
 * @brief Modifie la position du moteur via CAN
 * @param pHeader En-tête de la trame CAN
 * @param aData Données à transmettre
 * @param pTxMailbox Boîte aux lettres utilisée
 */
void can_setter(CAN_TxHeaderTypeDef pHeader, uint8_t* aData, uint32_t pTxMailbox) {
    // Configuration des données pour la rotation
    aData[0] = 90;  // Angle de 90°
    aData[1] = 1-aData[1];  // Inverse la direction

    // Envoi de la commande de rotation
    HAL_StatusTypeDef rota_out = tx_can(pHeader, aData, pTxMailbox);
    if (rota_out != HAL_OK) {
        printf("err rota CAN\r\n");
    }
    else {
        printf("change rotation complete\r\n");
    }
}

/**
 * @brief Ajuste la position du moteur en fonction de la température
 * @param temp_comp Température compensée
 */
void can_change_temp(BMP280_U32_t temp_comp) {
    // Initialisation du CAN
    enable_can();

    // Variable pour stocker l'ancienne température
    static BMP280_U32_t old_temp;

    // Vérifie si le changement de température est significatif (>2°C)
    if(fabs(temp_comp-old_temp) > 2) {
        if(temp_comp > old_temp) {
            // Mouvement dans le sens positif
            aData[1] = 0x00;
            aData[0] = (int)(10*(temp_comp-old_temp));
            HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);
            old_temp = temp_comp;
        }
        else {
            // Mouvement dans le sens négatif
            aData[1] = 0x01;
            aData[0] = (int)((old_temp-temp_comp));
            HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);
            old_temp = temp_comp;
        }
    }
}

/**
 * @brief Gère le dialogue avec le Raspberry Pi
 * Traite les commandes reçues via UART
 */
void dial_pi(void) {
    // Commande de lecture de température
    if(strncmp(RxBuff, "GET_T", 5) == 0) {
        // Lecture de la température
        temp_uncompen = BMP280_get_temperature();
        BMP280_U32_t temp_comp = bmp280_compensate_T_int32(temp_uncompen);

        // Affichage formaté de la température : T=+12.50_C
        printf("T=%ld%ld.%ld%ld_C\r\n",
               (temp_comp/1000)%10,
               (temp_comp/100)%10,
               (temp_comp/10)%10,
               temp_comp%10);

        // Ajustement du moteur selon la température
        can_change_temp(temp_comp);
    }

    // Commande de lecture de pression
    if(strncmp(RxBuff, "GET_P", 5) == 0) {
        // Lecture et compensation de la pression
        pres_uncompen = BMP280_get_pressure();
        BMP280_U32_t pres_comp = bmp280_compensate_P_int64(pres_uncompen);

        // Affichage de la pression en Pascal
        printf("P=%f_Pa\r\n", ((float)(pres_comp))/256);
    }

    // Commande de réglage du coefficient K
    if(strncmp(RxBuff, "SET_K=", 10) == 0) {
        K_pid = 0;
        printf("K %d", K_pid);
    }

    // Commande de lecture de l'angle
    if(strncmp(RxBuff, "GET_A", 5) == 0) {
        a_pid = 145;
        printf("A=%d\r\n", a_pid);
    }
}

/**
 * @brief Callback de réception UART
 * @param huart Pointeur vers la structure UART concernée
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    dial_pi();
    HAL_UART_Receive_IT(&huart1, RxBuff, RX_BUFF_SIZE);
}