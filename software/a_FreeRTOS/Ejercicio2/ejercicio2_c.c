#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xil_printf.h"
#include "xparameters.h"

#define TIMER_ID    1
#define DELAY_10_SECONDS    10000UL
#define DELAY_3_SECONDS     3000UL
#define DELAY_1_SECOND      1000UL
#define TIMER_CHECK_THRESHOLD   2


static void prvTxTask( void *pvParameters );
static void prvRxTask( void *pvParameters );
static void vTimerCallback( TimerHandle_t pxTimer );

static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;
static QueueHandle_t xQueue = NULL;
static TimerHandle_t xTimer = NULL;
char HWstring[15] = "Hello World";
long RxtaskCntr = 0;

const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );


int main( void )
{
    const TickType_t x3seconds = pdMS_TO_TICKS( DELAY_3_SECONDS );

    xil_printf( "\n\rHello from Freertos example main\r\n" );

    xTaskCreate( prvTxTask,
                 ( const char * ) "Tx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY,
                 &xTxTask );

    // Esta tarea tiene mayor prioridad.
    xTaskCreate ( prvRxTask,
                 ( const char * ) "GB",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY,
                 &xRxTask );


    // Solo puede almacenarse un elemento en esta cola.
    xQueue = xQueueCreate(  1,
                            sizeof( HWstring ) );


    configASSERT( xQueue );

    xTimer = xTimerCreate( (const char *) "Timer",
    		                x3seconds,
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
        xil_printf("Task Tx executed, pre delay\r\n");
        vTaskDelay( x1second );
        xil_printf("Task Tx executed, post delay\r\n");

        xQueueSend( xQueue,
                    HWstring,
                    0UL );
        xil_printf("Task Tx executed, post Queue Send\r\n");

    }
}

static void prvRxTask( void *pvParameters )
{
char Recdstring[15] = "";
    for( ;; )
    {
        xil_printf("Task Rx executed, pre delay\r\n");
        vTaskDelay( x1second );
        xil_printf("Task Rx executed, post delay\r\n");

        xQueueReceive(  xQueue,
                        Recdstring,
                        portMAX_DELAY );
        xil_printf("Task Rx executed, post Queue Receive\r\n");


        xil_printf( "Rx task received string from Tx task: %s\r\n", Recdstring );
        RxtaskCntr++;
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

