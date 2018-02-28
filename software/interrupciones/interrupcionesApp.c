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

#define TIMER_ID	1
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define TIMER_CHECK_THRESHOLD	9
/*-----------------------------------------------------------*/

void setupSystem();
void Task_ISRHandler(void *p);

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
#define INTC_DEVICE_ID XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define INTC XScuGic
#define INTC_HANDLER XScuGic_InterruptHandler

#define BUTTON_INTERRUPT XGPIO_IR_CH1_MASK //XGPIO_IR_MASK

#define ID_BTN XPAR_AXI_GPIO_0_DEVICE_ID
#define ID_LED XPAR_AXI_GPIO_1_DEVICE_ID
#define ledsAddr XPAR_AXI_GPIO_1_BASEADDR
#define LED_CHANNEL 1
#define LED_DELAY 100000000
XScuGic xInterruptController; 	/* Interrupt controller instance */

XGpio leds, btn;
XUartPs Uart_Ps;
static INTC Intc;

static TaskHandle_t xTaskLED, xTaskISRHandler;

xSemaphoreHandle semHandler = NULL;

int counter = 0;
int buttons = 0;

static void taskLED( void *pvParameters )
{
    for( ;; )
    {
        XGpio_DiscreteWrite(&leds, 1, counter);



    	counter++;

		vTaskDelay( 100 );
    }
}

static void taskISRHandler( void *pvParameters )
{
	for( ;; )
	{
        if (xSemaphoreTake( semHandler, 1) == TRUE) {
    		XGpio_DiscreteWrite(&leds, 1, 0x1);
            xSemaphoreGive( semHandler );
        }

		vTaskDelay( 100 );
	}
}


/*-----------------------------------------------------------*/

int main ( void ){


	xil_printf("---- Start of the Program ----\r\n");

	setupSystem();

	vSemaphoreCreateBinary( semHandler ); // ( semHandler );


    print("-- Tasks creation\n\r");

    xTaskCreate  ( taskLED, 					/* The function that implements the task. */
                 ( const char * ) "LED", 	/* Text name for the task, provided to assist debugging only. */
                 configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
                 NULL, 						/* The task parameter is not used, so set to NULL. */
                 tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
                 &xTaskLED );

    xTaskCreate  ( taskISRHandler, 					/* The function that implements the task. */
                 ( const char * ) "LED", 	/* Text name for the task, provided to assist debugging only. */
                 configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
                 NULL, 						/* The task parameter is not used, so set to NULL. */
                 tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
                 &xTaskISRHandler );
	// Start scheduler
	vTaskStartScheduler();

	// Will only reach here if there was insufficient memory to create the idle task
	return 0;

}


//----------------------------------------------------------------------------

void GpioIsr(void *InstancePtr)
{

	static BaseType_t xHigherPriorityTaskWoken = 0;
	int a;

	XGpio *GpioPtr = (XGpio *)InstancePtr;

	// Disable the interrupt
	XGpio_InterruptDisable(GpioPtr, BUTTON_INTERRUPT);

	// There should not be any other interrupts occuring other than the the button changes
	if ((XGpio_InterruptGetStatus(GpioPtr) &BUTTON_INTERRUPT)!= BUTTON_INTERRUPT) {
		return;
	}

   // Read state of push buttons
	buttons = XGpio_DiscreteRead(&btn, 1);

    // Clear the interrupt such that it is no longer pending in the GPIO
    (void)XGpio_InterruptClear(GpioPtr, BUTTON_INTERRUPT);

    // Enable the interrupt
    XGpio_InterruptEnable(GpioPtr, BUTTON_INTERRUPT);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    xSemaphoreGiveFromISR( semHandler, &xHigherPriorityTaskWoken);

}

/*-----------------------------------------------------------*/

int SetupInterruptSystem(){
	int Result;
	INTC *IntcInstancePtr = &Intc;
	XScuGic_Config *IntcConfig;

	// Initialize the interrupt controller driver so that it is ready to use.
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);

	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Result = XScuGic_CfgInitialize(&xInterruptController, IntcConfig,IntcConfig->CpuBaseAddress);

	if (Result != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XScuGic_SetPriorityTriggerType(&xInterruptController, INTC_GPIO_INTERRUPT_ID, 0xA0, 0x3);


	// Connect the interrupt handler that will be called when an
	// interrupt occurs for the device.
	Result = XScuGic_Connect(&xInterruptController, INTC_GPIO_INTERRUPT_ID, (Xil_ExceptionHandler)GpioIsr, &btn);

	if (Result != XST_SUCCESS) {
		return Result;
	}

	// Enable the interrupt for the GPIO device.
	XScuGic_Enable(&xInterruptController, INTC_GPIO_INTERRUPT_ID);

	// Enable the GPIO channel interrupts so that push button can be
	// detected and enable interrupts for the GPIO device
	XGpio_InterruptEnable(&btn, BUTTON_INTERRUPT);
	XGpio_InterruptGlobalEnable(&btn);

	// Initialize the exception table and register the interrupt
	// controller handler with the exception table

	Xil_ExceptionInit();

return XST_SUCCESS;

}

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
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

			print("-- Interrupt service configuration\n\r");
			// Interrupt service configuration
			Status = SetupInterruptSystem();
			if (Status != XST_SUCCESS) {
				      return XST_FAILURE;
			}

			Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)INTC_HANDLER, &xInterruptController);

			Xil_ExceptionEnable();


}


