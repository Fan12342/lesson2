/**
******************************************************************************
 * @file    remote.cpp/h
 * @brief   Remote control. 遥控器
 ******************************************************************************
 * Copyright (c) 2026 Team JiaoLong-SJTU
 * All rights reserved.
 ******************************************************************************
 */

#include "remote.h"

#include <string.h>

constexpr uint16_t REMOTE_CONNECT_TIMEOUT = 500u; 
// Constructor 构造函数
Remote::Remote(UART_HandleTypeDef *huart): huart_(huart), connect_(REMOTE_CONNECT_TIMEOUT){
    switch_.l = RCSwitchState_e::DOWN;
    switch_.r = RCSwitchState_e::DOWN;
}

// Start UART(SBUS) receive. 打开UART接收
void Remote::init() {
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_buf, RC_RX_BUF_SIZE);
    connect_.refresh();
}

// Reset RC data. 重置遥控器数据
void Remote::reset() {
    memset(rx_buf, 0, RC_RX_BUF_SIZE);
    memset(rx_data_, 0, RC_FRAME_LEN);
    memset(&channel_, 0, sizeof(channel_));
    rx_len_ = 0;
    switch_.l = RCSwitchState_e::DOWN;
    switch_.r = RCSwitchState_e::DOWN;
}

// Check for uart correspondence. 检查串口是否匹配
bool Remote::rxMsgCheck(UART_HandleTypeDef *huart) const {
    if (huart == &huart3 && HAL_UARTEx_GetRxEventType(&huart3) != HAL_UART_RXEVENT_IDLE)
    {
        return true;
    }
    return false;
}

// Update connect status, restart UART(SBUS) receive.
// 更新连接状态，重新打开UART(SBUS)接收
void Remote::rxMsgCallback(uint16_t Size){
    memcpy(rx_data_, rx_buf, RC_FRAME_LEN);
    connect_.refresh();
    rx_len_ = Size;
    handle();
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_buf, RC_RX_BUF_SIZE);
}

// Unpack data. 数据解包
void Remote::handle() {
    if (connect_.check())
    {
        channel_.l_row = ((uint16_t)rx_data_[0] | ((uint16_t)rx_data_[1] << 8)) & 0x07FF;
        channel_.l_col = (((uint16_t)rx_data_[1] >> 3) | ((uint16_t)rx_data_[2] << 5)) & 0x07FF;
        channel_.r_row = (((uint16_t)rx_data_[2] >> 6) | ((uint16_t)rx_data_[3] << 2) | ((uint16_t)rx_data_[4] << 10)) & 0x07FF;
        channel_.r_col = (((uint16_t)rx_data_[4] >> 1) | ((uint16_t)rx_data_[5] << 7)) & 0x07FF;

        uint8_t s1 = ((rx_data_[5] >> 4) & 0x000C) >> 2;
        uint8_t s2 = (rx_data_[5] >> 4) & 0x0003;

        if (s1 == 1)
            switch_.l = RCSwitchState_e::UP;
        else if (s1 == 2)
            switch_.l = RCSwitchState_e::DOWN;
        else
            switch_.l = RCSwitchState_e::MID;

        if (s2 == 1)
            switch_.r = RCSwitchState_e::UP;
        else if (s2 == 2)
            switch_.r = RCSwitchState_e::DOWN;
        else
            switch_.r = RCSwitchState_e::MID;
    }
}
