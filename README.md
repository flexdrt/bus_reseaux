# bus_reseaux



Ceci est le compte-rendu du tp de bus et réseau



[TOC]

------



#### 2. TP1- Bus I2C

##### 2.1 Capteur BMP280

Mise en œuvre du BMP280 

1. Quelles sont les adresses I²C possibles pour ce composant.

   On peut lire en page 28 de la datasheet (link ) que l'équipement possède une adresse sur 7 bits qui est '111011x' . Les 6 premiers bits, ceux de poids fort, sont différents de 'x'. Le caractère 'x' sert à indiquer que le bit de poids faible n'est pas fixé et que l'utilisateur doit le fixer et le configurer en l'adressant avec une adresse personalisée. 
   Pour 

   

   Toutes les communications avec l'équipement sont réalisées en lisant et en écrivant dans des registres. Ces registres sont sur une largeur de 8 bits et sont décrits dans le tableau suivant. 

   ![](C:\Users\gfjk\Downloads\bus_reseaux-main\bus_reseaux-main\Capture d'écran 2024-10-11 114528.png)

2. Identifier le registre et la valeur permettant d'identifier ce composant 


Le registre d'identification (ID) est à l'adresse 0xD0, et sa valeur est 0x58 pour le BMP280. 



3. Identifier le registre et la valeur permettant de placer le composant en mode normal 

​	Le registre de contrôle ctrl_meas (adresse 0xF4) permet de définir le mode de fonctionnement. Les bits 	mode[1:0] doivent être configurés à 11 pour le mode normal. 

 

4. Identifier les registres contenant l'étalonnage du composant 

Les données d'étalonnage sont stockées dans les registres de 0x88 à 0xA1 

 

5. Identifier les registres contenant la température (ainsi que le format) 

Les données de température sont réparties sur trois registres : 0xFA (MSB), 0xFB (LSB), et 0xFC (XLSB). 

Le format est une valeur non signée de 20 bits : le MSB contient les bits [19:12], le LSB les bits [11:4], et le XLSB les bits [3:0]. 

 

6. Identifier les registres contenant la pression (ainsi que le format) 

Les données de pression sont également réparties sur trois registres : 0xF7 (MSB), 0xF8 (LSB), et 0xF9 (XLSB). 

Le format est une valeur non signée de 20 bits, similaire à celui des données de température. 

 

7. Identifier les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits. 

La fonction pour la compensation de la température est bmp280_compensate_T_int32 et celle pour la pression est bmp280_compensate_P_int64. Ces fonctions utilisent des entiers 32 bits pour les calculs de compensation. 

 



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

*****************Problèmes cube ide****************************

```bash
sudo apt-get install libncurses5
Lecture des listes de paquets... Fait
Construction de l'arbre des dépendances... Fait
Lecture des informations d'état... Fait      
Les paquets supplémentaires suivants seront installés : 
  libtinfo5
Les NOUVEAUX paquets suivants seront installés :
  libncurses5 libtinfo5
0 mis à jour, 2 nouvellement installés, 0 à enlever et 6 non mis à jour.
Il est nécessaire de prendre 207 ko dans les archives.
Après cette opération, 883 ko d'espace disque supplémentaires seront utilisés.
Souhaitez-vous continuer ? [O/n] o
Réception de :1 http://fr.archive.ubuntu.com/ubuntu jammy-updates/universe amd64 libtinfo5 amd64 6.3-2ubuntu0.1 [100 kB]
Réception de :2 http://fr.archive.ubuntu.com/ubuntu jammy-updates/universe amd64 libncurses5 amd64 6.3-2ubuntu0.1 [107 kB]
207 ko réceptionnés en 0s (974 ko/s)   
Sélection du paquet libtinfo5:amd64 précédemment désélectionné.
(Lecture de la base de données... 283299 fichiers et répertoires déjà installés.)
Préparation du dépaquetage de .../libtinfo5_6.3-2ubuntu0.1_amd64.deb ...
Dépaquetage de libtinfo5:amd64 (6.3-2ubuntu0.1) ...
Sélection du paquet libncurses5:amd64 précédemment désélectionné.
Préparation du dépaquetage de .../libncurses5_6.3-2ubuntu0.1_amd64.deb ...
Dépaquetage de libncurses5:amd64 (6.3-2ubuntu0.1) ...
Paramétrage de libtinfo5:amd64 (6.3-2ubuntu0.1) ...
Paramétrage de libncurses5:amd64 (6.3-2ubuntu0.1) ...
Traitement des actions différées (« triggers ») pour libc-bin (2.35-0ubuntu3.8) ...

```



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

Utiliser la commande suivante pour lancer `minicom` sur le bon port :

```bash
sudo minicom -D /dev/ttyACM0
```

![image-20241011102221878](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/capture_envoie_liaison_serie.png)

L'affichage est décalé car il manquait le \r pour le retour chariot 

```c
printf("Hello from STM32!\r\n");
```

Maintenant l'affichage est centré à gauche comme on peut le voir : 


![test echo avec retour](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/test echo avec retour.png)



#### 2.3. Communication I²C



