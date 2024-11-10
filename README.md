# bus_reseaux



Ceci est le compte-rendu du tp de bus et réseau



[TOC]

------



## 2. TP1- Bus I2C

#### 2.1 Capteur BMP280

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
![Capture d’écran](./docs_annexes/img/Capture%20d’écran%20du%202024-10-11%2009-05-37.png)



Attention au câblage !!! Surce d'erreur et de grande perte de temps

à contrôler 

-les pins GPIO sur lequels l'i2c est configurée dans l'ioc

-les pins PHYSIQUE sur lesquels on branche SCL cable jaune et SDA cable blanc   : voir la photo

-les pins physiques sur lesquels on branche VCC (3.3V) cable rouge et GND sur un pin GND



PHOTO du câblage sur la STM32









image-20241011090144010-I2C : 

​	![image-20241011090144010](./docs_annexes/img/image_ioc.png)

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

![image-20241011102221878](./docs_annexes/img/capture_envoie_liaison_serie.png)

L'affichage est décalé car il manquait le \r pour le retour chariot 

```c
printf("Hello from STM32!\r\n");
```

Maintenant l'affichage est centré à gauche comme on peut le voir : 


![test echo avec retour](./docs_annexes/img/test echo avec retour.png)

sudo minicom -D /dev/ttyACM0

#### 2.3. Communication I²C


On va ajouter tout le code nécessaire pour manipuler le composant dans des fonctions dont la syntaxe pour rédiger leurs signatures sera BMP280_fonction_a_coder(). Elle seront déclarer dans le fichier header driver.h et implémenter dans le fichier qui sera appelé driver.c. 

 

**La première fonction que l’on a codé finalement dans la question précédente est la vérification ou confirmation de l’id de l’équipement I2C et sa réponse précisant son adresse sur le bus I2C. Nous avons donc implémenté la fonction checkID().** 



___

### CheckID

**code checkID() :** 

```c
int BMP280_checkID(void) {


	//FOnctionnel au 23 oct 18h59 <<<<<<< aussi 9 nov 19h34 organisé

	uint8_t buf[10];
	buf[0]=BMP280_ID_REG;

	retour = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 1, -1);

	printf("transmission in progress\r\n");

	if(retour != HAL_OK){

		printf(" \n problem during transmission \r\n");

		return 1;

	}
	else{
		printf("no problem during transmission | transmission complete -> reception in progress\r\n");
	}


	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, -1);
	if(retour != HAL_OK){

		printf(" \n problem during reception I2C\r\n");

		return 1;

	}
	else{

		printf("\n reception complete ...  : Register Id content : 0x%x\r\n", buf[0]);
	}

}
```



##### **Configuration du BMP280** mode normal - Pressure oversampling x16 - Temperature oversampling x2 

![](./docs_annexes/img/tab_registres.png)

tous les registres à configurer sont des sous parties du registre **ctrl_meas** : les bits 7 à 5 pOUr l‘oversampling de temperature , 4 à 2 pour l‘oversampling de la pression. 

 il faudra donc modifier les bits de bon poids à l’adresse du registre **ctrl_meas** pour modifier le paramètre voulu (mode power normal, oversampling) mais tous situés à l’adresse 0xF4 

Les paramètres binaires à mettre sont aussi décrits page 25 datasheet pour chaque paramètres d’oversampling et de mode voulu il y a un mot binaire associé 

![](./docs_annexes/img/registre cotrol measure.png)

##### Contrôle du mode d'alimentation

ll faut placer le composant en mode normal, les configurations possibles et leurs valeurs binaires associées du registre mode[1:0] sont décrites dans le tableau suivant page 15 : 
![mode normal for power mode](./docs_annexes/img/mode_alim_register.png )  	

| Adresse registre ctrl_meas | Valeur à écrire                                              |
| -------------------------- | ------------------------------------------------------------ |
| 0xF4                       | Les bits mode[1:0] doivent être configurés à 11 pour le mode normal. |

 

On souhaite paramétrer Pressure oversampling à x16 etTemperature oversampling à x2 . 

##### Contrôle de la mesure de pression 

page 12 : Pour activer l’oversampling de la pression (et la mesure) il faut sélectionner les bits [2:0] du registre de control osrs_p à l’adresse 0xF4. Les configurations possibles et valeurs associés du registre osrs_p ne pas sont décrites p.13 dans le tableau ci-dessous.

![page 12 détails configuration de la pression](./docs_annexes/img/page12 datasheet bmp.png)

 Mais dans le tableau 21 ci-dessous les exemples de configuration avec l'oversampling sont détaillés  page25 : 

![tab 21 conf pression oversampling ](./docs_annexes/img/tab 21 datasheet pression conf.png)

On va choisir le mot binaire ‘101’ qui correspond à oversampling x16 ce que l’on souhaite comme configuration. 

La valeur '101' à écrire dans le code est notée 0b101. 













##### Contrôle de la mesure de température

Pour activer l’oversampling de la température (et la mesure), il faut sélectionner les bits [2:0] du registre de control osrs_t toujours à l’adresse 0xF4 

Configurations possibles et valeurs possible du registre osrs_t sont tout de suite décrites dans le tableau 5 suivant page 13 :  

![tab 5 page 13](./docs_annexes/img/tab 5 page 13 temperature conf bmp.png)



Ou dans le tabelau  22 page 25 :

![tab 22 pge 25](./docs_annexes/img/tab 22 conf temperature page 25.png)

On va choisir le mot binaire ‘010’ qui correspond à oversampling x2, ce que l’on souhaite comme configuration. 

Il faut écrire dans les bits 7 à 5 du registre le mot binaire ’010’ qu’on notera dans le code C “0b010”. 

On va ajouter tout le code nécessaire pour configurer le composant dans une fonction BMP280_config() qui sera dans le fichier driver.c qui sera accompagné de son fichier driver.h 














En I²C, l'écriture dans un registre se déroule de la manière suivante : 

1. envoyer l'adresse du registre à écrire, suivi de la valeur du registre 
2. si on reçoit immédiatement, la valeur reçu, lorsque l’on essaie de lire le bus, sera la nouvelle valeur du registre 

**************************

Code bpm  



```c
uint8_t config = (0b010<<5)|(0b101<<2)|(0b11);  
```



(0b010<<5)pour remplir les bits [7:5] soit les bit 7 à 5 soit **osrs_t [2:0]** 

**`0b010 << 5`** : 

- `0b010` est `2` en décimal.
- Décalage de 5 bits vers la gauche : `0b01000000`, soit `64` en décimal



(0b101<<2) pour remplir les bits [4:2] (bits 4 à 2) soit **osrs_p [2:0]** 

**`0b101 << 2`** :

- `0b101` est `5` en décimal.
- Décalage de 2 bits vers la gauche : `0b00010100`, soit `20` en décimal.



(0b11) pour remplir les bits [1:0] soit **mode [1:0]** 

**`0b11`** :

- `0b11` est `3` en décimal et n'est pas décalé.



La valeur de `config` est donc **87** en décimal, soit **0x57** en hexadécimal, et **0b01010111** en binaire.



On lit bien sur l'USART la valeur renvoyé par le capteur suite à la configuration, affiché en hexadécimal grâce à %x :



*****

Pour historique/archive

****![image-capteur renvoit la bonne configuration (0d87 en décimal=0x57) 20241110011655406](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110011655406.png)

![image - renvoit la bonne configuration (0d87 en décimal et 0x57 en hexa)-20241110012753904](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110012753904.png)

****

Screenshot résultat [Dernier test après remis en arrière le code] 

![image-20241110122215211](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110122215211.png)

 

```c
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
	if (buf[1] == config) {

		printf("\nconfig : well configured - configuration value = hexadecimal : 0x%x | decimal  : 0d%u\r\n", buf[1]);



	}



}
```



**Printf - paramètres :**

utiliser le spécificateur de format %d pour afficher un entier 

**%u** est utilisé pour afficher le contenu de la variable en décimal (entiers non signé) Code b 

**%x** sert à afficher le contenu de la variable en hexadécimal  



___



### Étalonnage du composant

Pour récupérer les valeurs d’étalonnage, on envoie l’adresse la plus basse du registre des valeurs d’étalonnage et on réceptionne les 24 valeurs d’étalonnage.

![etalo tab17](./docs_annexes/img/tab17_etalo.png)

page 21 

| Adresse registres étalonnage                 | Valeur à écrire                                              |
| -------------------------------------------- | ------------------------------------------------------------ |
| 0x88[136 en décimal] à 0xA1 [160 en décimal] | Les bits mode[7:0] doivent être configurés à 11 pour le mode normal. |



Code pour l’étallonage 

On va faire une demande  de lecture qui commence avec transmit() puis read().

La lecture se déroule avec l'envoie de l'adresse du registre 0x88 ( qui correspond au registre calib00 de calibration (étalonnage) la plus basse valeur des adresses de registres d'étallonage ) et on reçoit 1 octet correspondant au contenu du registre.

On ajoute la constante dans le fichier BMP280_vincent.h suivante :

```c
typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;
typedef int64_t BMP280_S64_t;

static const uint8_t BMP280_ADDR = 0x77 << 1; // Use 8-bit address
// static const uint8_t BMP280_ADDR = 0x76 << 1; // Use 8-bit address

static const uint8_t BMP280_ID_REG = 0xD0;//id du registre selon la doc

static const uint8_t BMP280_TRIM_REG_MSB = 0x88;
```

La fonction BMP280_calib() contient le code suivant pour avoir les registres d'étallonage : 

```c
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
		printf("\ni :%d\r\n",i);
		printf("dans boucle for //registre d'etallonage calib= %d \r\n ", buf_data[i]);


	}
	
}
```



```c
#include "stdio.h"
#include "stdlib.h"

#include "main.h"
#include "BMP280_vincent.h"

extern I2C_HandleTypeDef hi2c1;

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
  

  
 
  //envoyer une trame avec l'adresse du registre à l'aide de la fonction HAL_I2C_Master_Transmit().
  //buf[0]=BMP_ID_REG;
  //HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 1, -1);

  //HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, -1);
  //printf("Idreg: 0x%x\r\n", buf[0]); 
  
 int BMP280_checkID(void){ 

    uint8_t buffer[1]; 
	buf[0] = BMP280_ID_REG;
    retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_ADDR, &ID_address, 1, HAL_MAX_DELAY); 

    if(retour != HAL_OK){ 

        printf("Problème avec l'I2C\r\n"); 

        return 1; 

    } 

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buffer, 1, HAL_MAX_DELAY); 

    if(retour != HAL_OK){ 

        printf("Problème avec l'I2C\r\n"); 

        return 1; 

    } 

    printf("L'ID du capteur est 0x%x\r\n",buffer[0]); 

    return 0; 

} 		 
  
 int BMP280_config(void){ 

    //uint8_t buffer[1]; 

    uint8_t buf[2]; 

    buf[0]= ctrl_meas; 

    buf[1]= config;//mot binaire a ecrire pour conf correctement 

    retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_ADDR, buf, 2, HAL_MAX_DELAY); 

    if(retour != HAL_OK){ 

        printf("Problème avec l'envoie I2C\r\n"); 

        return 1; 

    } <

    retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, HAL_MAX_DELAY); 

    if(retour != HAL_OK){ 

        printf("Problème avec la lecture I2C\r\n"); 

        return 1; 

    } 

 

    if(buffer[0] == config){ 

        printf("La config reçue est bien reçue et elle vaut %d\r\n", buffer[0]); 

        return 0; 

    } 

    return 0; 

}

 void BMP280_calib(void){
	//**********à supprimer

	// uint8_t etalo = 0x88; //Adresse du premier registre contenant les valeurs d'étalonnage
	//buf_adr[0]=etalo; //buffer contenant l'adresse du registre d'étalonnage

	//**********************

	 buf_data[24];//buffer pour les données reçues des registre

	 //on demande la lecture avec le transmit

	 //BMP280_TRIM_REG_MSB  est déclarée dans BMP280_vincent.h

	 // l'adresse de la variable BMP280_TRIM_REG_MSB est passée a la fonction qui atteint une adresse en paramètre (celle sur laquelle pointerait le pointeur pData)
	 retour=HAL_I2C_Master_Transmit(&hi2c1,BMP280_address,&BMP280_TRIM_REG_MSB,1, HAL_MAX_DELAY);

	 if(retour != HAL_OK){
	 		printf("Problème avec le transmit() del'I2C\r\n");
	 	}
	 //on lit la réponse qu'on stocke dans le buffer buf_data
	 retour = HAL_I2C_Master_Receive(&hi2c1,BMP280_address, buf_data, 24, HAL_MAX_DELAY);

	   if(retour != HAL_OK){

	 ​    printf("Problème avec la lecture des données d’étallonage I2C \r\n");

	 ​    return 1;
	   }
	   else{//on affiche les données d'étallonage reçues

		   for(int i=0; i<25; i++){
			   printf("registre d'étallonage calib [%d] \r\n ",buf_	data[i]);

		   }
	   }
	   dig_T1 = buf_data[0]|(buf_data[1]<<8);
	   dig_T2 = buf_data[2]|(buf_data[3]<<8);
	   dig_T3 = buf_data[4]|(buf_data[5]<<8);
	   dig_P1 = buf_data[6]|(buf_data[7]<<8);
	   dig_P2 = buf_data[8]|(buf_data[9]<<8);
	   dig_P3 = buf_data[10]|(buf_data[11]<<8);
	   dig_P4 = buf_data[12]|(buf_data[13]<<8);
	   dig_P5 = buf_data[14]|(buf_data[15]<<8);
	   dig_P6 = buf_data[16]|(buf_data[17]<<8);
	   dig_P7 = buf_data[18]|(buf_data[19]<<8);
	   dig_P8 = buf_data[20]|(buf_data[21]<<8);
	   dig_P9 = buf_data[22]|(buf_data[23]<<8);
 }

 BMP280_get_temperature(){

}

```



il faut décaler l'octet écrit qui est de poids fort (c-a-d le deuxième octet reçu dans chaque couple d'octet par exemple pour buf_data[1] et buf_data[0] c'est buf_data[1] qui est l'octet de poids fort) dans chaque registre de calibration pour les stockés dans les variables t1,t2,t3,p1,...

```c
dig_T1 = buf_data[0]|(buf_data[1]<<8<<8);
```

On met sur 16 bit, soit 2 octets, l'octet contenu dans buf_data[1] alors qu'il était sur 1 octet auparavant. 

```
 (buf_data[1]<<8);
```

Cela nous permettra de faire un 'OU' logique avec l'autre octet  non décalé (buf_data[0]). L'octet de buf_data[0] sera convertit sur 16 bits en plaçant un octet de '0' en octet de poids fort (MSB) devant l'octet de donnée contenu dans buf_data[0]

exemple : 

0x0034 est égal à 0x34 sauf que l'on précise dans la première forme qu'il y a un octet de 0 à écrire avant d'écrire les bits en base 2 de 0x34.

**Valeur de `buf_data[0]` (LSB)** :

- 00000000 00110100 (0x0034)



Et vu que tout l'octet de buf_data[1] a été décalé vers le MSB. Il restera l'octet de poids fort une fois le 'OU' effectué .

Ainsi tout le contenu écrit dans buf_data [1] soit un octet entier sera simplement recopié lors de l'opération logique . 

On fait pareil pour chaque registre, on stocke dans un variable une fois le décalage fait.

```c
	   dig_T1 = buf_data[0]|(buf_data[1]<<8);
	   dig_T2 = buf_data[2]|(buf_data[3]<<8);
	   dig_T3 = buf_data[4]|(buf_data[5]<<8);
	   dig_P1 = buf_data[6]|(buf_data[7]<<8);
	   dig_P2 = buf_data[8]|(buf_data[9]<<8);
	   dig_P3 = buf_data[10]|(buf_data[11]<<8);
	   dig_P4 = buf_data[12]|(buf_data[13]<<8);
	   dig_P5 = buf_data[14]|(buf_data[15]<<8);
	   dig_P6 = buf_data[16]|(buf_data[17]<<8);
	   dig_P7 = buf_data[18]|(buf_data[19]<<8);
	   dig_P8 = buf_data[20]|(buf_data[21]<<8);
	   dig_P9 = buf_data[22]|(buf_data[23]<<8);
```



****





### Récupération de la température et de la pression 



Récupération de la température

On va coder une fonction qui va renvoyer le type  BMP280_S32_t. Pour cela, on va définir ce type dans le .h

```c
typedef int32_t BMP280_S32_t;
```

 Ainsi que l'adresse du registre contenant la température et pression 

```c
static const uint8_t BMP280_TEMP_REG_MSB = 0xFA;//Adresse du registre contenant la température
static const uint8_t BMP280_PRES_REG_MSB = 0xF7;//Adresse du registre contenant la pression
```





**<u>Il faut recevoir la température dans un buffer de type `uint8_t` et `int` sinon on obtiens des valeurs températures incorrectes</u>**



![image buffer de receive mauvais type valeurs temp et press° incorrectes -20241110152218446](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110152218446.png)

-> Comme on le voir ici  avant compensation on a 3422552063 comme valeur de température et 3456106495 comme valeur de pression







```c
BMP280_S32_t BMP280_get_temperature() {

	BMP280_S32_t adc_T;

	//buffer pour les données reçues de la part des registres , ici 3 pour la température

	///uint8_t ! pas int! : valeur mal stockée = valeur pression incorrecte
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
```







```c
BMP280_S32_t BMP280_get_pressure() {

	BMP280_S32_t adc_P;


	//uint8_t ! pas int!. Si int buf_data[3] :  valeur mal stockée = valeur pression incorrecte
	uint8_t buf_data[3];//buffer pour 	les données reçues de la part des registres , ici 3 pour la pression

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
```



***





### Compensation des valeurs 

On utilise le code mis à disposition par bosch dans la datasheet que l'on ajoute dans le fichier BMP280_vincent.c

Juste avant dans le fichier .h on déclare les types nécessaire au retour de ces fonctions et on déclare les fonctions : 

```c
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
```

Et on déclare ces fonctions : 

```c
//déclarations fonctions de compensation de bosch
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t );
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t);
```

Il faut modifier dans le corps des fonctions les signes tiret longs "–" (ou dash) par des tirets court classique  pour que le compilateur reconnaisse l'opération de soustraction "-"





Fonction de compensation de la température :

```c
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

```



Fonction de compensation de la pression :



```c
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

```



***

***

***







```C
/ Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
BMP280_S32_t var1, var2, T;
var1 = ((((adc_T>>3) – ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
var2 = (((((adc_T>>4) – ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) – ((BMP280_S32_t)dig_T1))) >> 12) *
((BMP280_S32_t)dig_T3)) >> 14;
t_fine = var1 + var2;
T = (t_fine * 5 + 128) >> 8;
return T;
}
“”–
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
BMP280_S64_t var1, var2, p;
var1 = ((BMP280_S64_t)t_fine) – 128000;
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
```

Lors de l'appel de ces fonctions il faudra passer les variables adc_T et adc_P en argument pour recevoir les valeurs compensées retournées par la fonction 



**Dans la boucle infinie du STM32,  récupérez les valeurs de la température et de la pression. Envoyez sur  le port série le valeurs 32 bit non compensées de la pression de la  température.** 

On appelle les fonctions dans la boucle while dans le fichier main.c 

```c
//dans la boucle while
*************************************************
BMP280_S32_t temp_uncompen;
BMP280_S32_t pres_uncompen;

temp_uncompen= BMP280_get_temperature();//récupérer la température
pres_uncompen=BMP280_get_pressure()//récupérer la pression

printf("valeur non compensée de la température %d \n\ valeur non compensée de la pression %d",temp_uncompen,pres_uncompen);

HAL_Delay(1000);

**************************************************
//aperçu de la boucle while du main.c
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  BMP280_S32_t temp_uncompen;
	  BMP280_S32_t pres_uncompen;

	  temp_uncompen= BMP280_get_temperature(); //récupérer la température
	  pres_uncompen=BMP280_get_pressure(); //récupérer la pression

	  printf("valeur non compensée de la température %d \n\ valeur non compensée de la pression %d",temp_uncompen,pres_uncompen);

	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}





```













****

![image-20241110153219501](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110153219501.png)

****

La température est renvoyée en **dixièmes de degré Celsius**, avec une **résolution de 0.01°C**. Vous avez mentionné qu'un **valeur de 5123** correspond à **51.23°C**.

#### Calcul de la température compensée :

Si la température compensée est de **2545**, cela signifie que :

Tempeˊrature compenseˊe en °C=2545100=25.45 °C\text{Température compensée en °C} = \frac{2545}{100} = 25.45 \, \text{°C}Tempeˊrature compenseˊe en °C=1002545=25.45°C

Donc, la température compensée serait ***<u>25.45°C</u>***.



******



La pression est renvoyée en **format Q24.8**, où les 24 bits représentent la partie entière de la pression et les 8 bits représentent la partie fractionnaire. 

Ainsi, la pression compensée est donnée sous la forme **P / 256**, où **P** est l'entier brut en **Pa**. 

la doc a mentionné qu'une valeur de **24674867** correspond à une pression de **963.86 hPa**.

Si la pression compensée est de **34601705** (en Pa), cela représente une valeur en **format Q24.8**. Pour obtenir la pression en **hPa**, nous devons diviser par 256 :

Pression compenseˊe en Pa=34601705256=135492.82 Pa\text{Pression compensée en Pa} = \frac{34601705}{256} = 135492.82 \, \text{Pa}Pression compenseˊe en Pa=25634601705=135492.82Pa = 1.3 bar environ c'est cohérent 

Ensuite, pour convertir la pression en **hPa** (hectopascals), nous divisons par 100 :

Pression compenseˊe en hPa=135492.82100=1354.93 hPa\text{Pression compensée en hPa} = \frac{135492.82}{100} = 1354.93 \, \text{hPa}Pression compenseˊe en hPa=100135492.82=1354.93hPa

Donc, la pression compensée serait **<u>*1354.93 hPa*</u>**.



Passé les pression en bar 

****

****

****

****



## 3. TP2 - Interfaçage STM32 - Raspberry

#### 3.1. Mise en route du Raspberry PI Zéro

#### Premier démarrage

on flash la carte pour installer pi os lite 32 bits avec l'outil pi imager en précisant comme user (pi0) et comme mdp (password).

Pour le réseau, on configure ESE_Bus_Network pour le ssid et ilovelinux pour le mdp.
L'adresse IP est attribuée par le routeur : 192.168.88.237.



#### Modification de la configuration réseau de la PI zero :

Afin de pouvoir me connecter en ssh sur mon réseau privé au lieu du réseau de la salle de TP, je ajouter le réseau dans le fichier `wpa_supplicant.conf` qui se trouve dans `rootfs/etc/wpa_supplicant` . Pour cela, je monte la carte sd sur ma machine linux pour accèder à la partition boot qui contient ce fichier 

Une fois monté, deux partitions apparaissent : 

- rootfs
- bootfs 

On peut soit uiliser l'interface graphique de l'explorateur de fichier qui va se charger de monter le volume dès que l'on clique sur la partition 

![image-20241110184427471](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110184427471.png)





La partition rootfs contient toute l'arborescence (à partir de sa racine \ ), du système (raspbian en l'occurence).





Je ne peux pas ouvrir ce fichier sans sudo, grâce à Gedit je trouve le chemin complet depuis ma machine du fichier à ouvrir:

![image-20241110185901711](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110185901711.png)

Lorsque l'on monte une partition, elle est monté à partir du répertoire `/media/vincent/` comme on peut le voir en haut de la fenêtre.



J'ouvre en mode édition le fichier avec la commande

`sudo nano /media/vincent/rootfs/etc/wpa_supplicant/wpa_supplicant.conf`

J'écris dans le fichier les lignes suivantes :

```shell
update_config=1

#Ajout du réseau LAN pocket wifi
network={
 ssid="AIRBOX-69AE"
 psk="6wRp2Fr366H4"
}


```

![image-20241110185417222](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/image-20241110185417222.png)







dans bootfs je créé un fichier ssh vide et je copie le fichier wpa_supplicant.conf et j

```bash
vincent@Warhawk:/media/vincent/bootfs$ touch ssh
vincent@Warhawk:/media/vincent/bootfs$ sudo nano /media/vincent/rootfs/etc/wpa_supplicant/wpa_supplicant.conf
[sudo] Mot de passe de vincent : 
vincent@Warhawk:/media/vincent/bootfs$ pwd
/media/vincent/bootfs
vincent@Warhawk:/media/vincent/bootfs$ sudo cp /media/vincent/rootfs/etc/wpa_supplicant/wpa_supplicant.conf /media/vincent/bootfs

```





POur se connecter en ssh :

```bash
ssh pi0@192.168.1.150
```



#### 2.1 Loopback

On modifie les fichiers suivant pour pouvoir accèder à ce port série sur la pi zero :

-/boot/firmware/config.txt 
Pour activer le port série sur connecteur GPIO, sur la partition boot, modifiez le fichier config.txt pour ajouter à la fin les lignes suivantes:

```txt
enable_uart=1
dtoverlay=disable-bt
```

-/boot/firmware/cmdline.txt

```
console=serial0,115200
```

dans minicom il faut configurer les paramètre suivants en appuyant sur ctrl+A puis O





![minicom_options](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/tp2/minicom_options.png)



​							Serial port setup>F - Hardware Flow Control : no> il faut appuyer sur la touche F





Puis appuyer sur ctrl+A puis Z

![minicom ctrl z](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/tp2/minicom ctrl z.png)

Il faut appuyer sur E pour activer le local Echo on/off, ainsi lorsque l'on appuyer sur une touche elle est automatiquement renvoyé dans le terminal à la suite du caractère qu'on vient de taper comme montré ci-dessous dans la figure 'loopback test'

![minicom ctrl z](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/tp2/minicom ctrl z.png)





![loopback proof test](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/tp2/loopback proof test.png)

​											figure loopback test

Par exemple, en tapant t il se réaffiche grâce au echo qui affiche les caractères du rx sur le terminal.



#### Communication avec la STM32	





On a besoin d'un autre port UART, nous avons activé l'UART3 :

~~pin PC5 USART3_RX~~

~~pin PB10 USART3_TX~~





![Capture d’écran du 2024-10-18 10-38-24](/home/vincent/Images/Captures d’écran/Capture d’écran du 2024-10-18 10-38-24.png)

![apres changement_ioc_usart3](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/tp2/apres changement_ioc_usart3.png)



pin PC11 USART3_RX

pin PC10 USART3_TX



Afin de pouvoir facilement déboguer votre programme STM32, faites en  sorte que la fonction printf renvoie bien ses chaînes de caractères sur  la liaison UART sur USB, en ajoutant le code suivant au fichier  stm32f4xx_hal_msp.c: 

On va modifier la fonction **printf** pour quelle affiche sur les 2 ports série en même temps. On part de code : 

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

On rajoute l'écriture dans l'USART3 avec la fonction Transmit dans stm32f4xx_hal_msp.c:

```c
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
```



Ce qui donne le code suivant pour la fonction printf

```c
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
```

Communication entre la PI zero et le STM32

Le protocole de communication entre le Raspberry et la STM32 est le suivant:

| Requête ds résuu RPi | Réponse du STM | Commentaire                             |
| -------------------- | -------------- | --------------------------------------- |
| GET_T                | T=+12.50_C     | Température compensée sur 10 caractères |
| GET_P                | P=102300Pa     | Pression compensée sur 10 caractères    |
| SET_K=1234           | SET_K=OK       | Fixe le coefficient K (en 1/100e)       |
| GET_K                | K=12.34000     | Coefficient K sur 10 caractères         |
| GET_A                | A=125.7000     | Angle sur 10 caractères                 |

Pour écrire ce protocole on va écrire une fonction void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) dans BMP280_vincent.c. 

On va aussi déclarer en variable globale :

```c
***********************************
    // Définir des constantes pour les commandes
#define GET_T 0
#define GET_P 1
#define SET_K 2
#define GET_K 3
#define GET_A 4
#define UNKNOWN 5

uint8_t RxBuff[RX_BUFF_SIZE] = {0};
int K_pid = 0;
uint32_t coef;
BMP280_S32_t temp_uncompen;
BMP280_S32_t pres_uncompen;

// Fonction pour obtenir le code de commande
int getRequestCode(const char* request_pi) {
    if (strcmp(request_pi, "GET_T") == 0) return GET_T;
    else if (strcmp(request_pi, "GET_P") == 0) return GET_P;
    else if (strncmp(request_pi, "SET_K=", 6) == 0) return SET_K; //strncmp(RxBuff, "SET_K=", 6) pour vérifier que les 6 premiers caractères de la chaîne reçue correspondent bien à "SET_K=".
    //Si la commande est correcte, il suffit de prendre le reste de la chaîne, après ces 6 caractères, pour obtenir la valeur numérique.Vous pouvez utiliser atoi() (convertit une chaîne en entier) pour transformer cette partie de la chaîne en un entier.
Vous pouvez utiliser atoi() (convertit une chaîne en entier) pour transformer cette partie de la chaîne en un entier.
    else if (strcmp(request_pi, "GET_K") == 0) return GET_K;
    else if (strcmp(request_pi, "GET_A") == 0) return GET_A;
    return UNKNOWN;
}

```

la taille du tableau qui reçoit les données de l'UART est définit dans le fichier BMP280_vincent.h

```c
#define RX_BUFF_SIZE 5
```



```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     // Assurez-vous que RxBuff est null-terminated avant de l'utiliser
    RxBuff[RX_BUFF_SIZE - 1] = '\0';

    // Obtenir le code de la requête
    int request_code = getRequestCode((const char*)RxBuff);
    
    switch (request_code) {
        case GET_T: {
            temp_uncompen = BMP280_get_temperature();
            BMP280_S32_t temp_compen = bmp280_compensate_T_int32(temp_uncompen);
            printf("T=%ld%ld.%ld%ld_C\r\n",
                (temp_compen / 1000) % 10,
                (temp_compen / 100) % 10,
                (temp_compen / 10) % 10,
                temp_compen % 10);
            break;
        }
        case GET_P: {
            pres_uncompen = BMP280_get_pressure();
            BMP280_S32_t pres_compen = bmp280_compensate_P_int64(pres_uncompen);
            printf("P=%f_Pa\r\n", ((float)(pres_compen)) / 256);
            break;
        }
        case SET_K: {//a modifier
            coef = atoi((const char*)RxBuff + 6); // Vous pouvez utiliser atoi() (convertit une chaîne en entier) pour transformer cette partie de la chaîne en un entier.
            printf("SET_K=OK, coef=%d\r\n", coef);
            break;
        }
        case GET_K: {
            printf("K=%d\r\n", coef);
            break;
        }
        case GET_A: {
            // Ajouter votre logique pour GET_A
            printf("GET_A: angle value\r\n");
            break;
        }
        default: {
            printf("\r\nCommande inconnue\r\n");
            break;
        }
    }
    //***************
    request_pi;
    if(strcmp(RxBuff,request_rpi)==0))
    switch(){
        case request_pi=GET_T:
            //récupérer la température 
            BMP280_S32_t temp_uncompen; 
        	temp_uncompen= BMP280_get_temperature(); 
            //compenser la température sur 10 caractères
            BMP280_S32_t temp_compen;
            temp_compen=bmp280_compensate_T_int32(temp_uncompen);
            //afficher la température sur le port série
            printf("T=%ld%ld.%ld%ld_C\r\n",(temp/1000)%10,(temp/100)%10,(temp/10)%10,temp%10);
            break;
        case request_pi='GET_P':
        	pres_uncompen=BMP280_get_pressure(); 
            //compenser la pression sur 10 caractères
            BMP280_S32_t pres_compen;
            pres_compen=bmp280_compensate_P_int64(pres_uncompen)
            //afficher la pression compensée sur le port série
            printf("P=%f_Pa\r\n",((float)(pres_compen))/256);
            break;
        case request_pi'SET_K':
        	K_pid=0;
		//printf("\r\nSET_K=OK\r\n");
		//coef=atoi(RxBuff);
		coef=atoi(RxBuff);
		printf("%d\r\n",atoi(RxBuff));
		//printf("%d\r\n",coef);
            break;
        case GET_K:
            get_k();
            break();
        case GET_A:
            get_a();
            break();    
            
            
    }
    //buffer=get_t :>> temp_uncompen= BMP280_get_temperature(); 
    //buffer=get_p :>>  pres_uncompen=BMP280_get_pressure(); 	//récupérer la pression
    
    //buffer=set_k  :>>
    //buffer=get_k renvoie la valeur de K de l'asservissement du moteur sur 10 caractères :>>
    //buffer=get_a renvoie la valeur de a de l'asservissement du moteur sur 10 caractères BMP280_config() :>>
  HAL_UART_Receive_IT(&huart1, RxBuff, RX_BUFF_SIZE);  
}
```









```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(strcmp(RxBuff,"GET_T")==0){
		//printf("\r\nGET_T\r\n");
		nc_temp = BMP280_get_temp();
		temp=bmp280_compensate_T_int32(nc_temp);
		printf("T=%ld%ld.%ld%ld_C\r\n",(temp/1000)%10,(temp/100)%10,(temp/10)%10,temp%10);
	}
	else if(strcmp(RxBuff,"GET_P")==0){
		//printf("\r\nGET_P\r\n");
		nc_pres = BMP280_get_pres();
		pres=bmp280_compensate_P_int64(nc_pres);
		printf("P=%f_Pa\r\n",((float)(pres))/256);
	}
	else if(strcmp(RxBuff,"SET_K")==0){
		setK=1;
	}
	else if(setK==1){
		setK=0;
		//printf("\r\nSET_K=OK\r\n");
		//coef=atoi(RxBuff);
		coef=atoi(RxBuff);
		printf("%d\r\n",atoi(RxBuff));
		//printf("%d\r\n",coef);

	}
	else if(strcmp(RxBuff,"GET_K")==0){
		//printf("\r\nGET_K\r\n");
		printf("K=%d\r\n",coef);
	}
	else if(strcmp(RxBuff,"GET_A")==0){
		//printf("\r\nGET_A\r\n");
	}
	else{
		printf("\r\nCommande inconnue\r\n");
	}
	HAL_UART_Receive_IT(&huart1, RxBuff, RX_BUFF_SIZE);
}
```
crétaion de l'environnement à l'aide des commandes au tableau: voir photo pipenv 25/10



utiliser pipenv 



Suite à un problème de version de blinker, on procéde à l'installation sans le requirements.txt, package par package.





Quel est le rôle du décorateur `@app.route`?

Le décorateur permet d'accéder de se positionner dans le le lien /api/welcome/.



Capture d'écran





Quel est le role du fragment `<int:index>`?

Le fragment permet l'affichage de chaque lettre de Welcome, de 0 à 6 donc, none ensuite.













## TP4 :CAN



Il faut activer le CAN sur les pin PB8 et PB9 dans l'ioc. 

Il faut configurer la clock à 80 MHz dans clock configuration avec l'ide qui calcule tout tout seul

La configuration a mettre est déterminé à partir du site  

![image-20241108173330654](/home/vincent/snap/typora/90/.config/Typora/typora-user-images/image-20241108173330654.png)





