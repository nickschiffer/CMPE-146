/*
 * Nickolas Schiffer #012279319
 * CMPE 146 S19
 * Watchdog Lab
 */

/**
 * @file
 * @brief This is the application entry point.
 */

#include <stdio.h>
#include "io.hpp"
#include <tasks.hpp>
#include <storage.hpp>
#include <string>
#include <event_groups.h>
#include <ff.h>
#include <cstring>
#include <task.h>
#include <str.hpp>

#define PRODUCER_BIT (1 << 1)
#define CONSUMER_BIT (1 << 2)

typedef struct {
    uint16_t avg;
    uint64_t time;
} light_entry_t;

typedef struct {
    QueueHandle_t *queue;
    EventGroupHandle_t *event_group;

} task_params_t;

QueueHandle_t sensor_queue = xQueueCreate(100, sizeof(light_entry_t));
EventGroupHandle_t event_group = xEventGroupCreate();

TaskHandle_t producer_handle = NULL, consumer_handle = NULL, watchdog_handle = NULL, cpu_info_handle = NULL;

CMD_HANDLER_FUNC(taskHandler) {

    auto params = (char *) cmdParams.c_str();
    TaskHandle_t task_to_be_changed = NULL;
    printf("command: %s\n", params);
    char *pch = NULL;
    uint8_t numtasks = 0;

    char trunc[configMAX_TASK_NAME_LEN] = { 0 };

    pch = strtok(params, " ");

    if (strcasecmp(pch, "suspend") == 0) {
        while ((pch = strtok(NULL, " ")) != NULL) {
            if (strlen(pch) > configMAX_TASK_NAME_LEN - 1) {
                strncpy(trunc, pch, configMAX_TASK_NAME_LEN - 1);
                if ((task_to_be_changed = xTaskGetHandle(trunc)) != NULL) {
                    vTaskSuspend(task_to_be_changed);
                    numtasks++;
                }
                else {
                    printf("\"%s\" is not a valid task.\n", pch);
                }
            }
            else if ((task_to_be_changed = xTaskGetHandle(pch)) != NULL) {
                vTaskSuspend(task_to_be_changed);
                numtasks++;
            }
            else {
                printf("\"%s\" is not a valid task.\n", pch);
            }
        }
        if (numtasks) {
            printf("%d task%s suspended.\n", numtasks, (numtasks > 1) ? "s" : "");
            return true;
        }

        return false;

    }
    else if (strcasecmp(pch, "resume") == 0) {
        while ((pch = strtok(NULL, " ")) != NULL) {
            if (strlen(pch) > configMAX_TASK_NAME_LEN - 1) {
                strncpy(trunc, pch, configMAX_TASK_NAME_LEN - 1);
                if ((task_to_be_changed = xTaskGetHandle(trunc)) != NULL) {
                    vTaskResume(task_to_be_changed);
                    numtasks++;
                }
                else {
                    printf("\"%s\" is not a valid task.\n", pch);
                }
            }
            else if ((task_to_be_changed = xTaskGetHandle(pch)) != NULL) {
                vTaskResume(task_to_be_changed);
                numtasks++;
            }
            else {
                printf("\"%s\" is not a valid task.\n", pch);
            }
        }
        if (numtasks) {
            printf("%d task%s resumed.\n", numtasks, (numtasks > 1) ? "s" : "");
            return true;
        }

        return false;


    }
    else {
        return false;
    }
}

void vTaskGetHandle(void *pvParamters)
{
    std::string handle_name = "producer";
    TaskHandle_t handle1 = NULL, handle2 = NULL, handle3 = NULL, handle4 = NULL;
    while (1) {
        handle1 = xTaskGetHandle("producer");
        handle2 = xTaskGetHandle("consumer");
        handle3 = xTaskGetHandle("get_handle");
        handle4 = xTaskGetIdleTaskHandle();
        printf("%X\n%X\n%X\n%X\n\n", handle1, handle2, handle3, handle4);
        vTaskDelay(1000);
    }
}

void vTokenizer(void *pvParameters)
{

    while (1) {
        str string = "Hello my name is Simon";
        str *token = nullptr;

        while (1) {
            auto t = string.getToken(" ", false);
            if (t != NULL) {
                puts(t->c_str());
            }
            else {
                break;
            }

        }
        vTaskDelay(100);
    }
}

void vCPUUsageTask(void *pvParameters)
{
    std::string file = "";
    char buffer[BUFSIZ + 1];

    while (1) {
        const int delayInMs = 60000;

        snprintf(buffer, BUFSIZ, "CPU Info Logging started at %lums\n", xTaskGetMsCount());
        file += buffer;

        if (delayInMs > 0) {
            vTaskResetRunTimeStats();
            vTaskDelayMs(delayInMs);
        }

        // Enum to char : eRunning, eReady, eBlocked, eSuspended, eDeleted
        const char * const taskStatusTbl[] = { "RUN", "RDY", "BLK", "SUS", "DEL" };

        // Limit the tasks to avoid heap allocation.
        const unsigned portBASE_TYPE maxTasks = 16;
        TaskStatus_t status[maxTasks];
        uint32_t totalRunTime = 0;
        uint32_t tasksRunTime = 0;
        const unsigned portBASE_TYPE uxArraySize = uxTaskGetSystemState(&status[0], maxTasks, &totalRunTime);
        snprintf(buffer, BUFSIZ, "%10s Sta Pr Stack CPU%%          Time\n", "Name");
        file += buffer;
        for (unsigned priorityNum = 0; priorityNum < configMAX_PRIORITIES; priorityNum++) {
            /* Print in sorted priority order */
            for (unsigned i = 0; i < uxArraySize; i++) {
                TaskStatus_t *e = &status[i];
                if (e->uxBasePriority == priorityNum) {
                    tasksRunTime += e->ulRunTimeCounter;

                    const uint32_t cpuPercent = (0 == totalRunTime) ? 0 : e->ulRunTimeCounter / (totalRunTime / 100);
                    const uint32_t timeUs = e->ulRunTimeCounter;
                    const uint32_t stackInBytes = (4 * e->usStackHighWaterMark);

                    snprintf(buffer, BUFSIZ, "%10s %s %2u %5u %4u %10u us\n", e->pcTaskName, taskStatusTbl[e->eCurrentState], e->uxBasePriority, stackInBytes,
                            cpuPercent, timeUs);
                    file += buffer;
                }
            }
        }

        /* Overhead is the time not accounted towards any of the tasks.
         * For example, when an ISR happens, that is not part of a task's CPU usage.
         */
        const uint32_t overheadUs = (totalRunTime - tasksRunTime);
        const uint32_t overheadPercent = overheadUs / (totalRunTime / 100);
        snprintf(buffer, BUFSIZ, "%10s --- -- ----- %4u %10u luS\n", "(overhead)", overheadPercent, overheadUs);
        file += buffer;
        if (uxTaskGetNumberOfTasks() > maxTasks) {
            snprintf(buffer, BUFSIZ, "** WARNING: Only reported first %lu tasks\n", maxTasks);
            file += buffer;
        }
        file += "\n\n\n";
        Storage::append("1:cpu.txt", file.c_str(), file.size());
        file.clear();
        memset(buffer, 0, sizeof(buffer));
        puts("CPU Info logged to cpu.txt");

    }
}

void vProducerTask(void *pvParameters)
{

    Light_Sensor light_sensor = Light_Sensor::getInstance();
    light_sensor.init();
    uint8_t counter = 0;
    uint32_t sum = 0;
    light_entry_t entry = { 0, 0 };
    uint64_t start_time = xTaskGetMsCount();

    while (1) {

        if (counter == 99) {
            entry.avg = (uint16_t) sum / 100;
            entry.time = (uint64_t) xTaskGetMsCount() - start_time;
            //printf("%lu, %lu\n", entry.time, entry.avg);
            //puts("producer checking in");
            xQueueSend(sensor_queue, &entry, portMAX_DELAY);
            sum = counter = 0;
        }
        else {
            sum += light_sensor.getRawValue();
            counter++;
            vTaskDelay(1);
        }
        xEventGroupSetBits(event_group, PRODUCER_BIT);

    }

}

void vConsumerTask(void *pvParameters)
{
    light_entry_t entry = { 0, 0 };
    uint8_t counter = 0;
    std::string buffer = "";

    char string[BUFSIZ + 1] = { 0 };

    while (1) {
        xQueueReceive(sensor_queue, &entry, portMAX_DELAY);
        snprintf(&string[0], BUFSIZ, "%lu, %u\n", (unsigned long) entry.time, entry.avg);
        //printf("%lu, %u\n", (unsigned long)entry.time, entry.avg);
        buffer += string;
        counter++;
        if (counter == 10) {
            counter = 0;
            Storage::append("1:test.txt", buffer.c_str(), buffer.size());
            buffer.clear();
        }
        xEventGroupSetBits(event_group, CONSUMER_BIT);

    }
}

void vWatchDogTask(void *pvParameters)
{
    std::string buffer = "";
    char string[BUFSIZ + 1] = { 0 };
    while (1) {
        xEventGroupClearBits(event_group, (PRODUCER_BIT | CONSUMER_BIT));
        EventBits_t bits = xEventGroupWaitBits(event_group, (PRODUCER_BIT | CONSUMER_BIT),
        pdTRUE,
        pdTRUE, 1000);
        //printf("bits: %X\n", bits);
        if (!(bits & (PRODUCER_BIT))) {
            if ((!(bits & (CONSUMER_BIT)))) {
                //both bits unset
                snprintf(&string[0], BUFSIZ, "time: %lu: Both Tasks are Stuck.\n", xTaskGetMsCount());
                buffer += string;
                Storage::append("1:stuck.txt", buffer.c_str(), buffer.size());
                memset(string, 0, sizeof(string));
                buffer.clear();
                //puts("Producer and Consumer Stuck");
            }
            else {
                //just producer unset
                snprintf(&string[0], BUFSIZ, "time: %lu: Producer Task is Stuck.\n", xTaskGetMsCount());
                buffer += string;
                Storage::append("1:stuck.txt", buffer.c_str(), buffer.size());
                memset(string, 0, sizeof(string));
                buffer.clear();
                //puts("Producer Stuck");
            }
        }
        else if (!(bits & (CONSUMER_BIT))) {
            //just consumer unset
            snprintf(&string[0], BUFSIZ, "time: %lu: Consumer Task is Stuck.\n", xTaskGetMsCount());
            buffer += string;
            Storage::append("1:stuck.txt", buffer.c_str(), buffer.size());
            memset(string, 0, sizeof(string));
            buffer.clear();
            //puts("Consumer Stuck");
        }

    }
}

int main(void)
{

    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    f_unlink("1:test.txt");
    f_unlink("1:stuck.txt");
    f_unlink("1:cpu.txt");

    xTaskCreate(vProducerTask, "producer", 1024, NULL, PRIORITY_MEDIUM, &producer_handle);
    xTaskCreate(vConsumerTask, "consumer", 1024, NULL, PRIORITY_MEDIUM, &consumer_handle);
    xTaskCreate(vWatchDogTask, "watchdog", 1024, NULL, PRIORITY_HIGH, &watchdog_handle);
    xTaskCreate(vCPUUsageTask, "CPU Usage", 1024, NULL, PRIORITY_MEDIUM, &cpu_info_handle);
    //xTaskCreate(vTokenizer, "Tokenizer", 1024, NULL, PRIORITY_LOW, NULL);
    //xTaskCreate(vTaskGetHandle, "get_handle", 1024, NULL, PRIORITY_LOW, NULL);

    scheduler_start();
    return -1;
}
