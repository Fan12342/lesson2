#include "callback.h"
#include <string.h>
#include "main.h"
#include "usart.h"
#include "remote.h"

//extern uint8_t rx_msg[36];

Remote rc(&huart3);

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart1 && HAL_UARTEx_GetRxEventType(huart) != HAL_UART_RXEVENT_IDLE)
  {
    //memcpy(tx_msg, rx_msg, Size);
    //HAL_UART_Transmit(&huart1, tx_msg, Size, 1000);
    //HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_msg, sizeof(rx_msg));
  } else if (rc.rxMsgCheck(huart))
  {
    rc.rxMsgCallback(Size);
  }
}

extern "C" {
void rc_init()
{
  rc.init();
}
}
