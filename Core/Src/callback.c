
#include <string.h>
#include "main.h"
#include "usart.h"

extern uint8_t rx_msg[10];
uint8_t tx_msg[10];

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart1 && HAL_UARTEx_GetRxEventType(huart) != HAL_UART_RXEVENT_IDLE)
  {
    if (rx_msg[0] == 'R')
      HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
    else if (rx_msg[0] == 'M')
      HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
    memcpy(tx_msg, rx_msg, Size);
    HAL_UART_Transmit(&huart1, tx_msg, Size, 1000);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_msg, sizeof(rx_msg));
  }
}