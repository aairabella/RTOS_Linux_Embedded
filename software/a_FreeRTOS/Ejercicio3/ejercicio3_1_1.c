#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xil_printf.h"
#include "xparameters.h"

#define TIMER_ID    1
#define DELAY_10_SECONDS    10000UL
#define DELAY_1_SECOND      1000UL
#define TIMER_CHECK_THRESHOLD   9


static void prvTxTask( void *pvParameters );
static void prvRxTask( void *pvParameters );
static void vTimerCallback( TimerHandle_t pxTimer );

static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;
static QueueHandle_t xQueue = NULL;
static TimerHandle_t xTimer = NULL;
char HWstring[15] = "Hello World";
long RxtaskCntr = 0;
UBaseType_t InQueue = 0;

const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );


int main( void )
{
    const TickType_t x10seconds = pdMS_TO_TICKS( DELAY_10_SECONDS );

    xil_printf( "\n\r\n\rHello from Freertos example main\r\n" );

    xTaskCreate( prvTxTask,
                 ( const char * ) "Tx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY+1,
                 &xTxTask );

    // Esta tarea tiene mayor prioridad.
    xTaskCreate ( prvRxTask,
                 ( const char * ) "Rx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY,
                 &xRxTask );


    // Solo puede almacenarse un elemento en esta cola.
    xQueue = xQueueCreate(  10,
                            sizeof( HWstring ) );


    configASSERT( xQueue );

    xTimer = xTimerCreate( (const char *) "Timer",
    		                x10seconds,
                            pdFALSE,
                            (void *) TIMER_ID,
                            vTimerCallback);

    configASSERT( xTimer );

    xTimerStart( xTimer, 0 );

    vTaskStartScheduler();

    for( ;; );
}

static void prvTxTask( void *pvParameters )
{
    for( ;; )
    {
        //xil_printf("Task Tx executed, pre delay\r\n");
        vTaskDelay( 100 );
        //xil_printf("Task Tx executed, post delay\r\n");

        xQueueSend( xQueue,
                    HWstring,
                    0UL );
        //xil_printf("Task Tx executed, post Queue Send\r\n");

    }
}

static void prvRxTask( void *pvParameters )
{
char Recdstring[15] = "";
    for( ;; )
    {
        //xil_printf("Task Rx executed, pre delay\r\n");
        vTaskDelay( 200 );
        //xil_printf("Task Rx executed, post delay\r\n");

        xQueueReceive(  xQueue,
                        Recdstring,
                        portMAX_DELAY );
        xil_printf("Task Rx executed, post Queue Receive\r\n");


        xil_printf( "Rx task received string from Tx task: %s\r\n", Recdstring );
        RxtaskCntr++;
        InQueue = uxQueueMessagesWaiting( xQueue );
        xil_printf("\n\rIn Queue: %d\r\n", InQueue);
    }
}

static void vTimerCallback( TimerHandle_t pxTimer )
{
    long lTimerId;
    configASSERT( pxTimer );

    lTimerId = ( long ) pvTimerGetTimerID( pxTimer );

    if (lTimerId != TIMER_ID) {
        xil_printf("FreeRTOS Hello World Example FAILED");
    }

    if (RxtaskCntr >= TIMER_CHECK_THRESHOLD) {
        xil_printf("FreeRTOS Hello World Example PASSED");
    } else {
        xil_printf("FreeRTOS Hello World Example FAILED");
    }

    vTaskDelete( xRxTask );
    vTaskDelete( xTxTask );
}

