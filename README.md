# bus_reseaux



Ceci est le compte-rendu du tp de bus et réseau

#### 2. TP1- Bus I2C

##### 2.1 Capteur BMP280

Mise en oeuvre du BMP280 

1. Quelles sont les adresses I²C possibles pour ce composant.

   On peut lire en page 28 de la datasheet (link ) que l'équipement possède une adresse sur 7 bits qui est '111011x' . Les 6 premiers bits, ceux de poids fort, sont différents de 'x'. Le caractère 'x' sert à indiquer que le bit de poids faible n'est pas fixé et que l'utilisateur doit le fixer et le configurer en l'adressant avec une adresse personalisée. 
   Pour 

   

   

   

2. Identifier le registre et la valeur permettant d'identifier ce composant

3. Identifier  le registre et la valeur permettant de placer le composant en mode normal

4. Identifier  les registres contenant l'étalonnage du composant

5. Identifier  les registres contenant la température (ainsi que le format)

6. Identifier  les registres contenant la pression (ainsi que le format)

7. Identifier les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits.

   edszadsxzs

dsdzs





#### 2.2. Setup du STM32

Carte : STM32 NUCLEO-F446RE

Page 35 datasheet : https://moodle.ensea.fr/pluginfile.php/5789/mod_resource/content/1/dm00105823-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf

![](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/Capture d’écran du 2024-10-11 09-05-37.png)

-I2C : 

​	![image-20241011090144010](/home/vincent/snap/typora/90/.config/Typora/typora-user-images/image-20241011090144010.png)

​	I2C1_SDA sur PB9 

​	I2C1_SCL sur PB8 

UART 2
	TX sur PA2 et RX  PA3



D'un UART pour la PI0. (TP2)

UART ? : 

​	PIN 



D'un CAN 



*************************************

##### **Redirection du print**

Afin de pouvoir facilement déboguer votre programme STM32, faites en  sorte que la fonction printf renvoie bien ses chaînes de caractères sur  la liaison UART sur USB, en ajoutant le code suivant au fichier  stm32f4xx_hal_msp.c:

```c
/* USER CODE BEGIN PV */
extern UART_HandleTypeDef huart2;
/* USER CODE END PV */
/* USER CODE BEGIN Macro */
#ifdef __GNUC__ /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf    set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END Macro */
/* USER CODE BEGIN 1 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END 1 */
```



*********************************************

##### **Test de la chaîne de compilation et communication UART sur USB**



