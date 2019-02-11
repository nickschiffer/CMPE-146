#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"

#define SCENARIO 1

void vTaskOneCode(void *p)
{
    while(1)
    {
    	uart0_puts("aaaaaaaaaaaaaaaaaaaa");
    	vTaskDelay(100); // This sleeps the task for 100ms (because 1 RTOS tick = 1 millisecond)
    }
}

// Create another task and run this code in a while(1) loop
void vTaskTwoCode(void *p)
{
    while(1)
    {
    	uart0_puts("bbbbbbbbbbbbbbbbbbbb");
    	vTaskDelay(100);
    }
}

// You can comment out the sample code of lpc1758_freertos project and run this code instead
int main(int argc, char const *argv[])
{
    /// This "stack" memory is enough for each task to run properly (512 * 32-bit) = 2Kbytes stack
    const uint32_t STACK_SIZE_WORDS = 512;

    /**
     * Observe and explain the following scenarios:
     *
     * 1) Same Priority: A = 1, B = 1
     * 2) Different Priority: A = 2, B = 1
     * 3) Different Priority: A = 1, B = 2
     *
     * Turn in screen shots of what you observed
     * as well as an explanation of what you observed
     */
#if !defined SCENARIO
# error "SCENARIO is not defined."
#endif
#if SCENARIO == 1
    //Scenario 1
    xTaskCreate(vTaskOneCode,"Task A", STACK_SIZE_WORDS, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(vTaskTwoCode,"Task B", STACK_SIZE_WORDS, NULL, PRIORITY_LOW, NULL);
#endif
#if SCENARIO == 2
    //Scenario 2
    xTaskCreate(vTaskOneCode,"Task A", STACK_SIZE_WORDS, NULL, PRIORITY_MEDIUM, NULL);
    xTaskCreate(vTaskTwoCode,"Task B", STACK_SIZE_WORDS, NULL, PRIORITY_LOW, NULL);
#endif

#if SCENARIO == 3
    //Scenario 3
    xTaskCreate(vTaskOneCode,"Task A", STACK_SIZE_WORDS, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(vTaskTwoCode,"Task B", STACK_SIZE_WORDS, NULL, PRIORITY_MEDIUM, NULL);
#endif
    /* Start Scheduler - This will not return, and your tasks will start to run their while(1) loop */
    vTaskStartScheduler();

    return 0;
}
