# Ejercicios `a_FreeRTOS.pdf`

Se detallan a continuación las respuestas a las preguntas y la salida de consola de los ejercicios de la parte `a_FreeRTOS.pdf`:

## Ejercicio 1:

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

## Ejercicio 2

### Primera parte: *Invertir las prioridades de las tasks* 

Lo que debería ocurrir es que prvTxTask se ejecute primero que prvRxTask.

Se modificaron las tasks del ejercicio anterior para invertir las prioridades y poder *debuggear* que task se ejecuta primero: 

```
static void prvTxTask( void *pvParameters )
{
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );

    for( ;; )
    {
        xil_printf("Task Tx executed\r\n");
        vTaskDelay( x1second );

        xQueueSend( xQueue,
                    HWstring,
                    0UL );
    }
}

static void prvRxTask( void *pvParameters )
{
char Recdstring[15] = "";

    for( ;; )
    {
        xil_printf("Task Rx executed\r\n");
        xQueueReceive(  xQueue,
                        Recdstring,
                        portMAX_DELAY );

        xil_printf( "Rx task received string from Tx task: %s\r\n", Recdstring );
        RxtaskCntr++;
    }
}
```

Se agregaron las líneas `xil_printf("Task Tx executed\r\n");` y `xil_printf("Task Rx executed\r\n");`. 

La tarea `prvTXTask` se ejecuta primero, como se observa en la la siguiente salida de consola (se realizan comentarios junto a la línea de consola para comprender el comportamiento del programa: 

```
Hello from Freertos example main
Task Tx executed                                   // Primera ejecución de prvTXTask, por tener mayor prioridad. 
Task Rx executed                                   // Luego se ejecuta prvRXTask, la cual se bloquea cuando se hace la lectura del Queue
Task Tx executed                                   // Como prvRXTask está bloqueda por la lectura del Queue, se vuelve a ejecutar prvTXTask
Rx task received string from Tx task: Hello World  // prvRXTask continúa donde había quedado bloqueda, es decir, luego de la lectura del Queue
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed
Task Tx executed
Rx task received string from Tx task: Hello World
Task Rx executed

```

### Segunda Parte: Agregar `vTaskDelay( x1second )` a `RxTask`

Lo que sucede es que ahora se combinan los bloqueos de ambas tareas, sumado a que la tarea `prvRxTask` se bloquea además cuando lee la Queue. 

A continuación se muestra la salida de consola, detallando lo que sucede a cada paso. El código fué modificado para que el timer solo cuente 3 segundos, por lo cual se alcanza a enviar solo 2 mensajes. 

```
Hello from Freertos example main
Task Tx executed, pre delay                        // prvTxTask se bloquea por 1 segundo, no envia nada aún.
Task Rx executed, pre delay                        // prvRxTask se bloquea por 1 segundo, no recibe nada aún, no se bloquea. 
Task Tx executed, post delay                       // prvTxTask previo a ejecutar el envio a la Queue, luego del primer bloqueo.
Task Tx executed, post Queue Send                  // prvTxTask luego de ejecutar el envio a la Queue
Task Tx executed, pre delay                        // prvTxTask vuelve a ejecutarse debido a que prvRxTask continua bloqueda (no transcurrió un segundo). Se bloquea prvTxTask en ese momento. 
Task Rx executed, post delay                       // prvRxTask continua su ejecución luego del primer bloqueo.
Task Rx executed, post Queue Receive               // prvRxTask lee el mensaje almacenado en la Queue. Se bloquea hasta que el mensaje el leido. 
Rx task received string from Tx task: Hello World  // prvRxTask se desbloquea luego de leer la Queue y como prvTxTask continua bloqueada, prvRxTask puede continuar su ejecución. 
Task Rx executed, pre delay                        // prvRxTask se bloquea por nuevamente 1 segundo
Task Tx executed, post delay                       // prvTxTask sale del bloqueo.
Task Tx executed, post Queue Send                  // prvTxTask envia el segundo mensaje a la Queue
Task Tx executed, pre delay                        // prvTxTask vuelve a ejecutarse debido a que prvRxTask continua bloqueda (no transcurrió un segundo). Se bloquea prvTxTask en ese momento. 
Task Rx executed, post delay                       // prvRxTask continua su ejecución luego del segundo bloqueo.
Task Rx executed, post Queue Receive               // prvRxTask lee el mensaje almacenado en la Queue. Se bloquea hasta que el mensaje el leido. 
Rx task received string from Tx task: Hello World  // prvRxTask se desbloquea luego de leer la Queue y como prvTxTask continua bloqueada, prvRxTask puede continuar su ejecución. 
Task Rx executed, pre delay
FreeRTOS Hello World Example PASSED

```


### Tercera Parte: Asignar la misma prioridad a ambas tareas

El resultado es identico al caso anterior, debido a que la ejecución de las tareas está reglada por los tiempos que estas permanecen bloqueadas, más que por las prioridades de las mismas. 

```
Hello from Freertos example main
Task Tx executed, pre delay
Task Rx executed, pre delay
Task Tx executed, post delay
Task Tx executed, post Queue Send
Task Tx executed, pre delay
Task Rx executed, post delay
Task Rx executed, post Queue Receive
Rx task received string from Tx task: Hello World
Task Rx executed, pre delay
Task Tx executed, post delay
Task Tx executed, post Queue Send
Task Tx executed, pre delay
Task Rx executed, post delay
Task Rx executed, post Queue Receive
Rx task received string from Tx task: Hello World
Task Rx executed, pre delay
FreeRTOS Hello World Example PASSED

```

[Regresar a la página principal](../README.md) 