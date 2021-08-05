# TFGProject
Código para la implementación de un generador de tráfico de red con lenguaje de programación C


Para poder ejecutar la aplicación se deben compilar los archivos de la siguiente forma:

Generador:
    gcc UDPGenerator.c myutils.c -o generator -lm
    
Colector:
    gcc UDPCollector.c myutils.c -o colelctor -lm
    

La ejecución consiste en:
1. Primero ejecutar la parte del generador, indicando el # de paquetes que se desean generar, la longitud que tendrán y el ancho de banda para
  el generador. Por ejemplo:
      
        ./generator 5000 1200 150

2. El colector se ejecuta de la siguiente forma después de iniciar el generador:

        ./collector
        
        
