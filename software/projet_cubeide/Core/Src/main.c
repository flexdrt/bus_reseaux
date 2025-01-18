/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "stdio.h"
#include "BMP280_vincent.h"
//#include "stm32f4xx_hal_can.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BMP_ADDR (0x77 << 1)  // Décale 0x77 d'un bit vers la gauche
// DOC https://moodle.ensea.fr/pluginfile.php/5788/mod_resource/content/1/bst-bmp280-ds001.pdf page 29

#define BMP_ID_REG 0xD0 //id du registre selon la doc
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
extern uint8_t RxBuff[RX_BUFF_SIZE];


extern CAN_TxHeaderTypeDef pHeader;
extern uint8_t aData[3];  // Tableau pour les données à transmettre
extern uint32_t pTxMailbox;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__  With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf    set to 'Yes') calls __io_putchar()
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif  __GNUC__





/*
void can_change_temp(){

	//TP4 partie CAN d
	//début *******************************************


	BMP280_S32_t temp_uncompen;


	//déclaration des variables contenant la température non compensée
	temp_uncompen= BMP280_get_temperature(); //récupérer la température

	//déclaration des variables contenant la température  compensée
	BMP280_U32_t temp_comp,old_temp;


	temp_comp=bmp280_compensate_T_int32(temp_uncompen); //récupérer la température compensé




	if(fabs(temp_comp-old_temp)>40){

		// CAN part start  while loop
		aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
		aData[1] = 1-aData[1];  // D1 : Angle positif



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


	old_temp = temp_comp;//mise à jour de la température


}*/





/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_CAN1_Init();
	/* USER CODE BEGIN 2 */



	HAL_UART_Receive_IT(&huart1, RxBuff,RX_BUFF_SIZE);

	BMP280_S32_t temp_uncompen;
	BMP280_S32_t pres_uncompen;



	//déclaration des variables contenant la température non compensée
	BMP280_U32_t temp_comp;
	BMP280_U32_t pres_comp;

	BMP280_checkID();
	BMP280_config();
	BMP280_calib(); //récupère données étallonnage

	printf("main\r\n");



	//***************** COnnexion au moteur Avec fonctions du driver ******************************

	//enable_can();



	//***********************************************





	//********************FOnctionnel avec Mr.B**********************************************************************
	//TP4 partie CAN d
	//début *******************************************



	/*


	//Activer le module can
	HAL_StatusTypeDef start_can= HAL_CAN_Start(&hcan1);
	if(start_can!= HAL_OK) {

		// Gérer l'erreur d'envoi
		printf("erreur démarrage CAN\r\n");

	}
	 */

	/*
	//déclaration du pHeader
	CAN_TxHeaderTypeDef pHeader;
	uint8_t aData[3];  // Tableau pour les données à transmettre

	// Configuration du champ pHeader
	pHeader.StdId = 0x61;          // Identifiant standard pour la commande "Angle"
	pHeader.IDE = CAN_ID_STD;      //(0x00000000U)  !< Standard Id
	pHeader.RTR = CAN_RTR_DATA;    //(0x00000000U)  !< Data frame
	pHeader.DLC = 2;
	pHeader.TransmitGlobalTime = DISABLE;

	aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
	aData[1] = 0x00;  // D1 : Angle positif




	uint32_t pTxMailbox; // Variable pour stocker l'indice de la boîte aux lettres CAN

	// Envoi du message CAN avec l'angle de 90°
	HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);

	// Vérification du statut d'envoi
	if (status != HAL_OK) {
		// Gérer l'erreur d'envoi
		printf("erreur CAN\r\n");
	}
	else{
		printf("premiere conf reussi\r\n");
	}
	//fin partie CAN *******************************************

	//*********************************************************************************************

	 */


	//tx_can(pHeader, aData, pTxMailbox);


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

/*

		//printf("\n on rentre dans le while\r\n");

		//capter la température
		temp_uncompen= BMP280_get_temperature(); //récupérer la température
		temp_comp=bmp280_compensate_T_int32(temp_uncompen); //récupérer la température

		//printf("T=%ld%ld.%ld%ld_C\r\n",(temp_comp/1000)%10,(temp_comp/100)%10,(temp_comp/10)%10,temp_comp%10);

		//Les Affichages

		//Température

		printf("valeur non compensée de la température %u \r\n",temp_uncompen);

		printf("T=%ld%ld.%ld%ld_C\r\n",(temp_comp/1000)%10,(temp_comp/100)%10,(temp_comp/10)%10,temp_comp%10);

		//Pression
		pres_uncompen=BMP280_get_pressure(); //récupérer la pression
		pres_comp=bmp280_compensate_P_int64(pres_uncompen); //compenser la pression
		//HAL_Delay(1000);


		printf("P=%f_Pa  \r\n",((float)(pres_comp))/256);
*/







		/*
		//*******************************************Fonctionnel CAN avec Mr.B *********************


		// CAN part start  while loop
		aData[0] = 90;  // D0 : 90° en hexadécimal (0x5A)
		aData[1] = 1-aData[1];  //


		// Envoi du message CAN avec l'angle de 90°
		HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);

		// Vérification du statut d'envoi
		if (status != HAL_OK) {
			// Gérer l'erreur d'envoi
			printf(" err while CAN\r\n");
		}
		else {
			printf("change rotation complete \r\n");

		}

		//CAN oart finish while loop


		//*******************************************Fonctionnel CAN avec Mr.B *********************
		 */

		//*********************************************TP5 5.1**********************

		//Faire pivoter le moteur +90 puis -90 avec la fonction du driver
		//can_setter(pHeader, aData, pTxMailbox);



		//*********************************************TP5 5.2**********************
		//temp_uncompen= BMP280_get_temperature(); //récupérer la température
		//temp_comp=bmp280_compensate_T_int32(temp_uncompen); //récupérer la température

		//can_change_temp(temp_comp);

		//faire bouger le moteur en fonction de la température

		HAL_Delay(1000);





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
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 80;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief CAN1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN1_Init(void)
{

	/* USER CODE BEGIN CAN1_Init 0 */

	/* USER CODE END CAN1_Init 0 */

	/* USER CODE BEGIN CAN1_Init 1 */

	/* USER CODE END CAN1_Init 1 */
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 5;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CAN1_Init 2 */

	/* USER CODE END CAN1_Init 2 */

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
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

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
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

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
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : RASPBERRY_USART3_TX_Pin RASPBERRY_USART3_RX_Pin */
	GPIO_InitStruct.Pin = RASPBERRY_USART3_TX_Pin|RASPBERRY_USART3_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
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
