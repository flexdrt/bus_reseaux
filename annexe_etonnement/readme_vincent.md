

#### 2.2. Setup du STM32

Carte : STM32 NUCLEO-F446RE





I2C 

il faut activer l'i2c dans l'ioc pour configurer l'i2c

************************************************







L'identification du BMP280 consiste en la lecture du registre ID



En I²C, la lecture se déroule de la manière suivante:

1. **HAL_I2C_Master_Transmit()** : Cette fonction n'est pas utilisée ici pour envoyer des données spécifiques à l'esclave, mais plutôt pour envoyer **l'adresse du registre** que vous souhaitez lire. Vous envoyez l'adresse du registre dans lequel vous allez effectuer une lecture. C'est une pratique courante en I²C :

   - Le premier octet transmis est l'adresse de l'esclave (BMP_ADDR), avec le bit R/W réglé à 0 (pour indiquer une écriture).
   - Le second octet (buf) contient l'adresse du registre dans l'esclave BMP que vous voulez lire. Par exemple, il pourrait s'agir d'un registre de capteur BMP pour la pression ou la température.

   2.**HAL_I2C_Master_Receive()** : Après avoir spécifié l'adresse du registre avec l'appel à `HAL_I2C_Master_Transmit()`, vous utilisez **HAL_I2C_Master_Receive()** pour lire les données depuis ce registre. Ici, vous adressez à nouveau l'esclave, mais cette fois-ci avec le bit R/W réglé à 1 (indiquant une lecture), puis vous recevez les données dans `buf`.

dans cette séquence on doit d'abord indiquer l'adresse du registre (avec `Transmit()`), puis faire une lecture des données (avec `Receive()`

1. ##### envoyer l'adresse du registre ID

On va envoyer une trame à l'aide de la fonction HAL_I2C_Master_Transmit(). 

Le premier octet transmis sera l'adresse de l'esclave, L'adresse I2C du composant 0x77 car SDO est à la masse sur le PCB.

. 

**Ces 2 fonctions gère le  bit R/W, mais il faut quand même lui laisser la place dans l'adresse  I²C.** 

C'est pour cela qu'il faut décaler d'un bit à gauche l'adresse du composant : Je définis l'alias  

```c
#define BMP_ADDR (0x77 << 1)  // Décale 0x77 d'un bit vers la gauche
// DOC https://moodle.ensea.fr/pluginfile.php/5788/mod_resource/content/1/bst-bmp280-ds001.pdf page 29 
```



Le second octet représente l'adresse du 1er registre à écrire.  En l'occurrence, Le registre d'identification (ID) est à l'adresse 0xD0, et sa valeur est 0x58 pour le BMP280.  

| Adresse du registre | Contenu/valeur du registre |
| ------------------- | -------------------------- |
| 0xD0                | 0x58                       |

on déclare les variables suivantes 

```
uint8_t buf[10]
buf[0]=BMP_ID_REG
```

taille du buffer de réception des données : 10

On fait une définition de l'adresse du registre '

```
#define BMP_ID_REG 0xD0//id du registre selon la doc 
```









2. ##### recevoir 1 octet correspondant au contenu du registre

On va recevoir le contenu du registre précisé grâce au Transmit() précédent.

On doit lire  ce contenu avec la fonction HAL_I2C_Master_Receive(). On met dans le buffer buf les données envooées par l'esclave sur le bus. Puis on l'affiche dans le port série avec le printf.

```
  HAL_I2C_Master_Receive(&hi2c1, BMP_ADDR, buf, 1, -1);
  printf("Idreg: 0x%x\r\n", buf[0]);
```



![envoie registre id](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/envoie registre id.png)

On peut lire comme contenu du registre : 0x58. C 'est bien la valeur attendue pour le registre 0xD0



![image-20241023185954809](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/checkid() fonctionnel 0x58renvoyer.png)

![checkid()_sansancien_test_afficher_port_serie](/home/vincent/Documents/ese_3a/reseaux_bus_de_terrain/bus_reseaux/docs_annexes/img/23_oct_18h59/checkid()_sansancien_test_afficher_port_serie.png)



checkid() sans les anciens échec

**********



##### **Configuration du BMP280**



our commencer, nous allons  utiliser la configuration suivante: mode normal, Pressure oversampling  x16, Temperature oversampling x2

En I²C, l'écriture dans un registre se déroule de la manière suivante:



1. envoyer l'adresse du registre à écrire, suivi de la valeur du registre

Le registre de contrôle ctrl_meas (adresse 0xF4) permet de définir le mode de fonctionnement. Les bits mode[1:0] doivent être configurés à 11 pour le mode normal. 

0xF4



1. si on reçoit immédiatement, la valeur reçu sera la nouvelle valeur du registre











