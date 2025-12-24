#include "main_app.h"

/*
 * -------------------------------------------------------------------------
 * CRITICAL VARIABLE: 'status'
 * -------------------------------------------------------------------------
 * Why 'volatile'?
 * This variable is modified inside an Interrupt Service Routine (ISR) but
 * read inside the main loop. These are two different contexts.
 *
 * SCENARIO 1: WITHOUT 'volatile'
 * The C Compiler (with optimization on) analyzes the 'main' function. It sees
 * that 'status' is initialized to 0 and is NOT modified inside the 'while(!status)'
 * loop. The compiler thinks: "Since this variable never changes in this scope,
 * I will just load '0' into a CPU register and never check the RAM address again."
 * RESULT: The program hangs in the while loop FOREVER, even if you press the button
 * and the ISR changes the value in RAM. The CPU never sees the change.
 *
 * SCENARIO 2: WITH 'volatile'
 * The keyword tells the compiler: "Do not optimize access to this variable. Its
 * value can change unexpectedly (by hardware or ISR) at any time."
 * RESULT: The compiler generates code that forces the CPU to read the actual
 * value from the RAM address during every single iteration of the while loop.
 * When the ISR updates the RAM, the main loop sees it immediately.
 * -------------------------------------------------------------------------
 */
volatile uint8_t status = 0;

GPIO_InitTypeDef hgpio;

int main(void){

    // Initialize the HAL Library
	HAL_Init();

    // Configure the system clock (implementation is TODO below)
	SystemClockConfig();

    // Initialize Peripherals
	LED_Init();
	Button_Init();

    /*
     * ---------------------------------------------------------------------
     * THE BLOCKING LOOP
     * ---------------------------------------------------------------------
     * This loop waits for the flag to become TRUE (1).
     *
     * Assembly Logic (Simplified):
     *
     * IF volatile IS USED:
     * Loop_Start:
     * LDR  R0, [Address_of_status]   <-- Instruction to fetch from RAM
     * CMP  R0, #0                    <-- Compare with 0
     * BEQ  Loop_Start                <-- If equal, jump back to start
     *
     * IF volatile IS NOT USED (Optimization Trap):
     * LDR  R0, [Address_of_status]   <-- Loads '0' once before loop
     * Loop_Start:
     * CMP  R0, #0                    <-- Checks the register (always 0)
     * BEQ  Loop_Start                <-- Infinite loop. RAM is ignored.
     * ---------------------------------------------------------------------
     */
		while(!status){
            // CPU is stuck here until Button Interrupt fires.
            // Without volatile, it stays here forever even after button press.
		}

    /*
     * If we reach here, it means 'status' became 1.
     * We turn on the LED on PA5.
     */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

    // Main Super Loop (End of program logic)
	while(TRUE);

	return 0;
}

/*
 * -------------------------------------------------------------------------
 * INTERRUPT SERVICE ROUTINE (CALLBACK)
 * -------------------------------------------------------------------------
 * This function is called asynchronously when the hardware detects a falling
 * edge on the button pin. This happens outside the normal flow of 'main'.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == GPIO_PIN_13){
        // Toggle the status variable
		if(status){
			status = 0;
		}
		else{
            /*
             * This write operation updates the value in the RAM Memory Address.
             * Because 'status' is volatile, the main loop (which is constantly
             * reading that RAM address) will notice this change immediately.
             */
			status = 1;
		}
	}
}

// ISR Handler for External Interrupt lines 10 to 15
void EXTI15_10_IRQHandler(void){
    // Call the HAL handler which clears flags and calls the Callback above
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void LED_Init(void){
    // Enable Clock for GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA5 as Output (Push-Pull) for the LED
	hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Pin = GPIO_PIN_5;

	HAL_GPIO_Init(GPIOA, &hgpio);
}

void Button_Init(void){
    // Enable Clock for GPIOC
	__HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure PC13 as Input with Interrupt capability
    // Trigger on Falling Edge (Button Press usually pulls to GND)
	hgpio.Mode = GPIO_MODE_IT_FALLING;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pin = GPIO_PIN_13;

	HAL_GPIO_Init(GPIOC, &hgpio);

    // Enable the Interrupt Vector in NVIC (Nested Vector Interrupt Controller)
    // Priority 0 (Highest)
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
}

void SystemClockConfig(void){
	//TODO: Configure system clock (currently using default HSI)
}

void Error_Handler(void){
    // Trap CPU in case of error
	while(TRUE);
}

void SysTick_Handler(void){
    // Necessary for HAL_Delay and Timeout management
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
