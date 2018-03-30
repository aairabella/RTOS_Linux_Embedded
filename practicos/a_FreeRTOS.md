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



### Tercera Parte: Asignar la misma prioridad a ambas tareas

