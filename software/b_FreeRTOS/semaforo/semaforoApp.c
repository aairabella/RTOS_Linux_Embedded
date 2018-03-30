
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xuartps.h"

void setupSystem();

int counter = 0;
/*----------------------------------------------------------------------*/
// GPIOs Definitions

#define ID_BTN XPAR_AXI_GPIO_0_DEVICE_ID
#define ID_LED XPAR_AXI_GPIO_1_DEVICE_ID
#define ledsAddr XPAR_AXI_GPIO_1_BASEADDR
#define LED_CHANNEL 1

XGpio leds, btn;

xSemaphoreHandle semHandler;

/*----------------------------------------------------------------------*/

void setupSystem(){

	int Status;
	// Initialize the GPIO driver
	print("-- Initialize the GPIO driver\n\r");
	Status = XGpio_Initialize(&leds, ID_LED);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XGpio_Initialize(&btn, ID_BTN);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	XGpio_SetDataDirection(&leds,1,0x0);
	XGpio_SetDataDirection(&btn,1,0xffffffff);

}

static TaskHandle_t xTaskLED, xTaskPRINT;


static void taskLED( void *pvParameters )
{
    for( ;; )
    {
    	counter++;
        XGpio_DiscreteWrite(&leds, 1, counter);

		if (counter % 8 == 0){
		// Give Semaphore
        	xSemaphoreGive( semHandler );

			xil_printf("Semaphore given!\n\r");
	    }

	vTaskDelay( 100 );
    }
}
static void taskPRINT( void *pvParameters )
{
	for( ;; )
	{
		if( xSemaphoreTake( semHandler, ( portTickType ) 1 ) == pdTRUE )
        {
        	xil_printf("Found a modulus(8) number: Counter Value: %d\n\r", counter);
        }
	}
	//vTaskDelay( 2 );
}

void takeSemaphore( xSemaphoreHandle semHandler ){

	if( xSemaphoreTake( semHandler, ( portTickType ) 1 ) == pdTRUE )
    	xil_printf("The semaphore is mine! Hello!\n\r", counter);
	return;
}

int main ( void ){


    xil_printf("---- Start of the Program ----\r\n");
    setupSystem();

    vSemaphoreCreateBinary( semHandler );


    print("-- Tasks creation\n\r");

    xTaskCreate  ( taskLED, 					/* The function that implements the task. */
               ( const char * ) "LED", 	/* Text name for the task, provided to assist debugging only. */
                 configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
                 NULL, 						/* The task parameter is not used, so set to NULL. */
                 tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
                 &xTaskLED );

    xTaskCreate  ( taskPRINT, 					/* The function that implements the task. */
                 ( const char * ) "PRINT", 	/* Text name for the task, provided to assist debugging only. */
                 configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
                 NULL, 						/* The task parameter is not used, so set to NULL. */
                 tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
                 &xTaskPRINT );

    // Start semaphore taken:

    takeSemaphore( semHandler );

    // Start scheduler
    vTaskStartScheduler();

    // Will only reach here if there was insufficient memory to create the idle task
    return 0;

}


