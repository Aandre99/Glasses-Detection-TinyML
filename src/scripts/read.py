import serial
import numpy as np
import matplotlib.pyplot as plt
import time

ser = serial.Serial('COM7', 115200)

def receive_image_from_serial(port='COM3', baudrate=9600, image_size=(64, 64, 1)):
	total_bytes = image_size[0] * image_size[1]
	
	# Ler os dados da porta serial
	image_data = ser.read(total_bytes)

	# Converter os dados recebidos em uma matriz numpy
	image = np.frombuffer(image_data, dtype=np.int8).reshape(image_size)
	
	return image

def plot_image(image):
    plt.imshow(image, cmap='gray')
    plt.draw()
    plt.title('Received Image')
    plt.axis('off')
    plt.pause(0.01)
      


# Exemplo de uso
if __name__ == "__main__":
	
    while(1):
        #time.sleep(2)
        
        ser.write('s'.encode('utf-8'))
        image = receive_image_from_serial()
        plot_image(image)
        s = ser.readline().decode('utf-8')
        print(s)
        ser.flushInput()