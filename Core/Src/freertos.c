/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
QueueHandle_t queueMotorHandle;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void TaskSensor(void *argument);
void TaskMotor(void *argument);
void TaskHeartbeat(void *argument);
/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

// 1. 初始化函数
void MX_FREERTOS_Init(void)
{
    // 创建队列
    queueMotorHandle = xQueueCreate(5, sizeof(uint8_t));

    // 创建任务
    xTaskCreate(TaskSensor, "Sensor", 128, NULL, 3, NULL);
    xTaskCreate(TaskMotor, "Motor", 128, NULL, 3, NULL);
    xTaskCreate(TaskHeartbeat, "Heartbeat", 64, NULL, 1, NULL);
}

// 2. 传感器任务
void TaskSensor(void *argument)
{
    uint8_t sensor_count = 0;
    uint8_t msg = 1;
    for(;;)
    {
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
        {
            vTaskDelay(20 / portTICK_PERIOD_MS);
            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
            {
                sensor_count++;
                while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET);
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }
        }

        if(sensor_count >= 5)
        {
            xQueueSend(queueMotorHandle, &msg, 0);
            sensor_count = 0;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// 3. 电机任务
void TaskMotor(void *argument)
{
    uint8_t received_msg;
    for(;;)
    {
        if(xQueueReceive(queueMotorHandle, &received_msg, portMAX_DELAY) == pdTRUE)
        {
            for(int i = 0; i < 3; i++)
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
                vTaskDelay(200 / portTICK_PERIOD_MS);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }
        }
    }
}

// 4. 心跳任务
void TaskHeartbeat(void *argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/* USER CODE END Application */
