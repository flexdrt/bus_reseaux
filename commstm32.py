import serial
import time

ser = serial.Serial(
    port='/dev/ttyAMA0',      #Port série utilisé sur Raspberry Pi
    baudrate=115200
    timeout=1
)

def get_temperature():
    ser.write(b'GET_T\n')  # Envoyer la commande au STM32
    response = ser.readline().decode('utf-8').strip()  # Lire la réponse
    return response




def get_pressure():
    ser.write(b'GET_P\n')
    response = ser.readline().decode('utf-8').strip()
    return response




def set_coefficient_k(k_value):
    command = f'SET_K={k_value:04d}\n'  # Construire la commande avec le coefficient
    ser.write(command.encode('utf-8'))
    response = ser.readline().decode('utf-8').strip()
    return response




def get_coefficient_k():
    ser.write(b'GET_K\n')
    response = ser.readline().decode('utf-8').strip()
    return response





def main():
    while True:
        print("Choose a command:")
        print("1. Get Temperature")
        print("2. Get Pressure")
        print("3. Set Coefficient K")
        print("4. Get Coefficient K")
        print("5. Exit")
        
        choice = input("Enter your choice: ")
        
        if choice == '1':
            print("Temperature:", get_temperature())
        elif choice == '2':
            print("Pressure:", get_pressure())
        elif choice == '3':
            k_value = int(input("Enter the value of K (integer): "))
            print("Response:", set_coefficient_k(k_value))
        elif choice == '4':
            print("Coefficient K:", get_coefficient_k())
        elif choice == '5':
            print("0")
            break
        else:
            print("Invalid choice, please try again.")
        
        time.sleep(1)  # Pause d'une seconde entre chaque commande
