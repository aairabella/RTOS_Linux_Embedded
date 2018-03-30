# RTOS y Linux Embebido 

En este repositorio se encuentran los recursos de teoría, trabajos prácticos y ejercicios resueltos del curso **Programación avanzada de sistemas embebidos en RTOS y Linux Embebido**

## Organización del Repositorio

### practicos

En la carpeta `practicos` se encuentran los enunciados de los trabajos prácticos: 

```
practicos
├── a_FreeRTOS.pdf
├── b_FreeRTOS.pdf
└── c_PetaLinux.pdf
```

### software

En esta carpeta se encuentran los códigos `.c` de cada uno de los ejercicios de cada trabajo práctico. Están ordenados en sub-directorios, según los nombres de los trabajos prácticos presentados en la sección anterior. 

En los ejercicios donde se realizaban preguntas, fueron contestadas como comentarios dentro del mismo código fuente y además se detallan en la sección Desarrollo de los Ejercicios. 

### Teoría

Contiene los archivos de las presentaciones de clases:

```
teoria
├── a_freeRTOS.pdf
├── b_freeRTOS.pdf
└── c_PetaLinux.pdf
```


# Desarrollo de los ejercicios

## Ejercicios `a_FreeRTOS.pdf`

Se detallan a continuación la salida de consola y las respuestas a las preguntas de los ejercicios de la parte `a_FreeRTOS.pdf`:

*Ejercicio 1:*

¿Qué tarea tiene mayor prioridad?

```
// Esta tarea tiene mayor prioridad.
xTaskCreate( prvRxTask,
           ( const char * ) "GB",
             configMINIMAL_STACK_SIZE,
             NULL,
             tskIDLE_PRIORITY + 1,
             &xRxTask );
```

¿Cuántos elementos pueden ser almacenados en la cola?

```
// Solo puede almacenarse un elemento en esta cola.
xQueue = xQueueCreate( 	1, 
                        sizeof( HWstring ) );
```

Salida de consola del ejercicio: 

```
Hello from Freertos example main
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
Rx task received string from Tx task: Hello World
FreeRTOS Hello World Example PASSED
```

