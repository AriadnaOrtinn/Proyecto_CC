import serial
from tkinter import *
import threading
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from tkinter import messagebox
import time
import numpy as np

# CAMBIA 'COM3' por el puerto COM real de tu Arduino en el portátil
device = 'COM7' 
mySerial = serial.Serial(device, 9600)
nuevaLinea = False
linea = ""
trozos = []

MEDIA_EN_SAT = False    

def recepcion():
    global linea
    global nuevaLinea
    while True:
        if mySerial.in_waiting > 0:
            linea = mySerial.readline().decode('utf-8').rstrip()
            print(linea)
            nuevaLinea = True

def actualizar():
    global linea
    global nuevaLinea
    global trozos
    if nuevaLinea == True:
        trozos = linea.split(':')
        nuevaLinea = False

grafica_creada = False
canvas = None
ax = None
linea_grafica = None
linea_media = None
canvas_radar = None
linea_rastro = None
linea_actual = None

def AClick():
    global grafica_creada, canvas, ax, linea_grafica, linea_media
    global canvas_radar, linea_rastro, linea_actual
    
    IniciarButton.config(state=DISABLED)   
    threadRecepcion = threading.Thread(target=recepcion, daemon=True)
    threadRecepcion.start()
    
    if not grafica_creada:
        tituloInicio.grid_forget()          
        
        # Gráfica de Temperatura (Izquierda)
        fig = Figure(figsize=(5, 3), dpi=100)
        canvas = FigureCanvasTkAgg(fig, master=window)
        canvas.get_tk_widget().grid(row=0, column=0, columnspan=7, padx=8, pady=8, sticky=N+S+E+W)   
        ax = fig.add_subplot(111)
        ax.set_xlim(0, 100)
        ax.set_ylim(10, 50)
        ax.set_xlabel("Tiempo")
        ax.set_ylabel("Temperatura (°C)")
        linea_grafica, = ax.plot([], [], label="Temp (°C)")
        linea_media, = ax.plot([], [], color='red', linewidth=2, label="Valor medio")
        ax.legend(loc='upper left')

        # Gráfica Radar (Derecha)
        DIST_MAX = 50
        fig_radar = plt.Figure(figsize=(6, 4))
        ax_radar = fig_radar.add_subplot(111, projection='polar')
        ax_radar.set_title("Radar de Ultrasonido")
        ax_radar.set_thetamin(0)
        ax_radar.set_thetamax(180)
        ax_radar.set_ylim(0, DIST_MAX)
        ax_radar.set_rlabel_position(0)
        ax_radar.set_thetagrids(range(0, 180, 25))
        ax_radar.set_yticks(range(0, DIST_MAX + 1, 10))
        ax_radar.grid(True, color='lightgray')

        linea_rastro, = ax_radar.plot([], [], color='y', linewidth=1.5)
        linea_actual, = ax_radar.plot([], [], color='green', linewidth=2, marker='o')

        canvas_radar = FigureCanvasTkAgg(fig_radar, master=window)
        canvas_radar.get_tk_widget().grid(row=0, column=7, columnspan=7, padx=8, pady=8, sticky=N+S+E+W)   

        grafica_creada = True

    i = 0
    temperaturas = []
    eje_x = []
    medias_x = []     
    medias_y = [] 

    ultima_alerta_temp = 0
    ultima_alerta_hum = 0
    INTERVALO_ALERTA = 100 

    MAX_RASTRO = 15
    angulos_radar = []
    distancias_radar = []

    def actualizar_radar(angulo_grados, distancia):
        angulo = np.radians(angulo_grados)
        angulos_radar.append(angulo)
        distancias_radar.append(distancia)
        del angulos_radar[:-MAX_RASTRO]
        del distancias_radar[:-MAX_RASTRO]

        linea_rastro.set_data(angulos_radar, distancias_radar)
        linea_actual.set_data([0, angulo], [0, distancia])
        canvas_radar.draw_idle()

    while True:
        if nuevaLinea == True:
            actualizar()
            if len(trozos) > 0:
                try:
                    codigo = int(trozos[0])
                except ValueError:
                    window.update()
                    continue

                # CÓDIGO 0: Heartbeat
                if codigo == 0:
                    pass 

                # CÓDIGO 1: Temperatura y Humedad -> formato "1:t:xx.xx:h:yy.yy" (o con mayúsculas T/H)
                elif codigo == 1:
                    try:
                        temp_val = 0.0
                        hum_val = 0.0
                        letra_temp = 't'
                        letra_hum = 'h'

                        for idx, val in enumerate(trozos):
                            if val == 't' or val == 'T':
                                letra_temp = val
                                temp_val = float(trozos[idx + 1])
                            if val == 'h' or val == 'H':
                                letra_hum = val
                                hum_val = float(trozos[idx + 1])

                        eje_x.append(i)
                        temperaturas.append(temp_val)
                        i = i + 1
                        
                        linea_grafica.set_data(eje_x, temperaturas)
                        ax.set_xlim(0, max(100, i))

                        if not MEDIA_EN_SAT and len(temperaturas) > 0:     
                            ultimas = temperaturas[-10:]                         
                            medias_x.append(i - 1)                               
                            medias_y.append(sum(ultimas) / len(ultimas))         
                            linea_media.set_data(medias_x, medias_y)             

                        canvas.draw()

                        # Alarma basada estrictamente en el aviso 'T' del satélite
                        if letra_temp == 'T':
                            if time.time() - ultima_alerta_temp >= INTERVALO_ALERTA:
                                messagebox.showwarning("ALERTA", f"¡Temperatura máxima superada en SAT! ({temp_val} °C)")
                                ultima_alerta_temp = time.time()    
                        else:
                            ultima_alerta_temp = 0                  
            
                        # Alerta de humedad
                        if letra_hum == 'H':
                            if time.time() - ultima_alerta_hum >= INTERVALO_ALERTA:
                                messagebox.showwarning("ALERTA", f"¡Humedad máxima superada en SAT! ({hum_val} %)")
                                ultima_alerta_hum = time.time()
                        else:
                            ultima_alerta_hum = 0

                    except (ValueError, IndexError):
                        pass

                # CÓDIGO 2: Errores del DHT11
                elif codigo == 2:
                    if len(trozos) > 1:
                        err = trozos[1]
                        if err == '0':
                            messagebox.showwarning("Error", "Error con los datos de temperatura y humedad")
                        elif err == 'T':
                            messagebox.showwarning("Error", "Error con los datos de temperatura")
                        elif err == 'H':                                    
                            messagebox.showwarning("Error", "Error con los datos de humedad")

                # CÓDIGO 3: Medias calculadas en el Satélite
                elif codigo == 3 and MEDIA_EN_SAT:                        
                    try:                                                     
                        mediaT = float(trozos[1])                             
                        medias_x.append(max(i - 1, 0))                       
                        medias_y.append(mediaT)                               
                        linea_media.set_data(medias_x, medias_y)             
                        canvas.draw()                                        
                    except (ValueError, IndexError):                         
                        pass                        

                # CÓDIGO 4: Radar (Ángulo y Distancia)
                elif codigo == 4:   
                    try:
                        angulo = float(trozos[1])       
                        distancia = float(trozos[2])
                        if 0 <= distancia <= 50:
                            actualizar_radar(angulo, distancia)
                    except (ValueError, IndexError):
                        pass

        window.update() 

def BClick ():
    mySerial.write("2:0\n".encode('utf-8'))
    
def CClick ():
    mySerial.write("2:1\n".encode('utf-8'))

def MSClick():   
    global MEDIA_EN_SAT                      
    MEDIA_EN_SAT = True                      
    mySerial.write("3:1\n".encode('utf-8'))

def MPClick():  
    global MEDIA_EN_SAT                      
    MEDIA_EN_SAT = False                       
    mySerial.write("3:0\n".encode('utf-8'))
    
def DFClick():
    # Desfijar servomotor -> protocolo: código 4 con argumento -1 activa el barrido
    mySerial.write("4:-1\n".encode('utf-8')) 

def leer_numero(entrada, nombre):
    try:
        return float(entrada.get().replace(',', '.'))
    except ValueError:
        messagebox.showerror("Error", f"Introduce un número válido en: {nombre}")
        return None

def PeriodoTHClick():
    valor = leer_numero(entryPeriodoTH, "Periodo temp/hum")
    if valor is None: return
    if valor <= 0:
        messagebox.showerror("Error", "El periodo debe ser mayor que 0")
        return
    mySerial.write(f"1:{valor}\n".encode('utf-8')) 

def OrientacionClick():
    # Fijar ángulo específico del servomotor -> protocolo: código 4 con el ángulo (0 a 180)
    valor = leer_numero(entryOrientacion, "Orientación del sensor")
    if valor is None: return
    if valor < 0 or valor > 180:
        messagebox.showerror("Error", "La orientación debe estar entre 0 y 180 grados")
        return
    mySerial.write(f"4:{int(valor)}\n".encode('utf-8'))

# Interfaz gráfica principal
window = Tk()
window.title("Estación de Control Terrestre (GCS)")
window.geometry("1100x750")   
window.minsize(1000, 700)     
window.rowconfigure(0, weight=1)                
window.rowconfigure(1, weight=0, minsize=90)    
window.rowconfigure(2, weight=0, minsize=80)    

for c in range(12):
    window.columnconfigure(c, weight=1, uniform="col")

tituloInicio = Label(window, text="Pulse INICIAR para mostrar gráfica", font=("Courier", 20, "italic"))
tituloInicio.grid(row=0, column=0, columnspan=12, padx=5, pady=5, sticky=N + S + E + W)

IniciarButton = Button(window, text="Iniciar", bg='maroon', fg="white", command=AClick, wraplength=120)
IniciarButton.grid(row=1, column=0, columnspan=2, padx=6, pady=8, sticky=N + S + E + W)
PararButton = Button(window, text="Parar", bg='pink', fg="white", command=BClick, wraplength=120)
PararButton.grid(row=1, column=2, columnspan=2, padx=6, pady=8, sticky=N + S + E + W)
ReanudarButton = Button(window, text="Reanudar", bg='purple', fg="white", command=CClick, wraplength=120)
ReanudarButton.grid(row=1, column=4, columnspan=2, padx=6, pady=8, sticky=N + S + E + W)
DesfijarButton = Button(window, text="Desfijar Servomotor (Barrido)", bg='blue', fg="white", command=DFClick, wraplength=120)
DesfijarButton.grid(row=1, column=6, columnspan=2, padx=6, pady=8, sticky=N + S + E + W)

MediaSATButton = Button(window, text="Calcular medias en SAT", bg='brown', fg="white", command=MSClick, wraplength=120)
MediaSATButton.grid(row=1, column=8, columnspan=2, padx=6, pady=8, sticky=N + S + E + W)
MediaPYButton = Button(window, text="Calcular medias en estación", bg='brown', fg="white", command=MPClick, wraplength=120)
MediaPYButton.grid(row=1, column=10, columnspan=2, padx=6, pady=8, sticky=N + S + E + W)

def crear_control(columna, columnspan, titulo, comando):
    marco = LabelFrame(window, text=titulo)
    marco.grid(row=2, column=columna, columnspan=columnspan, padx=8, pady=8, sticky=N + S + E + W)
    entrada = Entry(marco, width=8, justify=CENTER)
    entrada.pack(side=LEFT, padx=8, pady=8, expand=True)
    Button(marco, text="Aplicar", command=comando).pack(side=LEFT, padx=8, pady=8, expand=True)
    return entrada

# Solo 3 controles de configuración ahora (Periodo T/H, Orientación/Fijar Ángulo)
entryPeriodoTH = crear_control(1, 4, "Periodo temp/hum (s)", PeriodoTHClick)
entryOrientacion = crear_control(7, 4, "Fijar Ángulo (0-180°)", OrientacionClick)

window.mainloop()