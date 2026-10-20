import serial
from tkinter import *
import threading
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from tkinter import messagebox
import time


device = 'COM5'
mySerial = serial.Serial(device, 9600)
def recepcion():
    threadRecepcion = threading.Thread (target = recepcion)
    threadRecepcion.start()

grafica_creada = False


def AClick ():
    global grafica_creada 
    
    #PAra que el titulo del principio desaparezca al pulsar INICIAR
    if not grafica_creada:
            tituloInicio.grid_forget()          # el título desaparece
            fig = Figure(figsize=(5, 3), dpi=100)
            canvas = FigureCanvasTkAgg(fig, master=window)
            canvas.get_tk_widget().grid(row=1, column=0, columnspan=3, padx=5, pady=5, sticky=N+S+E+W)
            ax = fig.add_subplot(111)
            ax.set_xlim(0, 100)
            ax.set_ylim(20, 30)
            ax.set_xlabel("Muestra")
            ax.set_ylabel("Temperatura (°C)")
            linea_grafica, = ax.plot([], [])
            grafica_creada = True

    

    # La gráfica incrustada en la interfaz
    fig = Figure(figsize=(6, 4), dpi=100)
    canvas = FigureCanvasTkAgg(fig, master=window)
    canvas.get_tk_widget().grid(row=1, column=0, columnspan=3, padx=5, pady=5, sticky=N+S+E+W)
    ax = fig.add_subplot(111)
    ax.set_xlim(0, 100)
    ax.set_ylim(20, 40)
    ax.set_xlabel("Tiempo")
    ax.set_ylabel("Temperatura (°C)")
    linea_grafica, = ax.plot([], [])


    # Grafica con los valores enviados desde COM5
    i = 0
    temperaturas = []
    eje_x = []
    while True:
        if mySerial.in_waiting > 0:
            linea = mySerial.readline().decode('utf-8').rstrip()
            print(linea)
            trozos = linea.split (':')

            eje_x.append (i)
            
            letra_temp = trozos[0]    
            temperatura = float (trozos[1])
            letra_hum = trozos[2]
            humedad = float(trozos[3])
            temperaturas.append (temperatura)
            i = i+1
            
            linea_grafica.set_data(eje_x, temperaturas)
            ax.set_xlim(0, max(100, i))
            canvas.draw()

            
            ultima_alerta_temp = 0
            ultima_alerta_hum = 0
            INTERVALO_ALERTA = 100 
          
            if letra_temp == 'T':
                if time.time() - ultima_alerta_temp >= INTERVALO_ALERTA:
                    messagebox.showwarning("ALERTA", f"¡Temperatura máxima! ({temperatura} °C)")
                    ultima_alerta_temp = time.time()    # se guarda al CERRAR la alerta
            else:
                ultima_alerta_temp = 0                  # vuelve a la normalidad
 
            #Alerta de humedad
            if letra_hum == 'H':
                if time.time() - ultima_alerta_hum >= INTERVALO_ALERTA:
                    messagebox.showwarning("ALERTA", f"¡Humedad máxima! ({humedad} %)")
                    ultima_alerta_hum = time.time()
            else:
                ultima_alerta_hum = 0



        window.update()


def BClick ():
    mySerial.write("Parar\n".encode('utf-8'))
    
def CClick ():
    mySerial.write("Reanudar\n".encode('utf-8'))
    


#Interfaz
window = Tk()
window.geometry("600x550")
window.rowconfigure(0, weight=1)
window.rowconfigure(1, weight=1)
window.rowconfigure(2, weight=0, minsize=150)

window.columnconfigure(0, weight=1)
window.columnconfigure(1, weight=1)
window.columnconfigure(2, weight=1)

tituloInicio = Label(window, text = "Pulse INICIAR para mostrar gráfica", font=("Courier", 20, "italic"))
tituloInicio.grid(row=0, column=0, columnspan=5, padx=5, pady=5, sticky=N + S + E + W)


# Botones
IniciarButton = Button(window, text="Iniciar", bg='maroon', fg="white",command=AClick)
IniciarButton.grid(row=2, column=0, padx=5, pady=5, sticky=N + S + E + W)
PararButton = Button(window, text="Parar", bg='pink', fg="white", command=BClick)
PararButton.grid(row=2, column=1, padx=5, pady=5, sticky=N + S + E + W)
ReanudarButton = Button(window, text="Reanudar", bg='purple', fg="white",command=CClick)
ReanudarButton.grid(row=2, column=2, padx=5, pady=5, sticky=N + S + E + W)

window.mainloop()
