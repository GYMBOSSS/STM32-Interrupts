#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx.h"

#define HSE_STARTUP_TIMEOUT 0x5000

void SystemClock_Config()
{
    uint32_t Time_Counter = 0;

    RCC -> CR |= RCC_CR_HSEON; // включаем HSE
    while (!(RCC -> CR & RCC_CR_HSERDY)) { // проверяем готовность HSE
        if (Time_Counter++ > HSE_STARTUP_TIMEOUT)
            return;
    }

    FLASH -> ACR |= FLASH_ACR_LATENCY_2; // настраиваем ожидание Flash-памяти
    FLASH -> ACR |= FLASH_ACR_PRFTBE; // настраиваем кеш

    RCC -> CFGR |= RCC_CFGR_HPRE_DIV1; // настраиваем делитель шины AHB
    RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2; // настраиваем делитель шины APB1
    RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1; // настраиваем делитель шины APB2

    RCC -> CFGR |= RCC_CFGR_PLLSRC; //
    RCC -> CFGR &= ~RCC_CFGR_PLLXTPRE;
    RCC -> CFGR |= RCC_CFGR_PLLMULL9;

    RCC -> CR |= RCC_CR_PLLON;
    Time_Counter = 0;
    while (!(RCC -> CR & RCC_CR_PLLRDY)) {
        if (Time_Counter++ > HSE_STARTUP_TIMEOUT)
            return;
    }

    RCC -> CFGR &= ~RCC_CFGR_SW;
    RCC -> CFGR |=  RCC_CFGR_SW_PLL;

    Time_Counter = 0;
    while ((RCC -> CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
        if (Time_Counter++ > HSE_STARTUP_TIMEOUT)
            return;
    }

    SystemCoreClock = 72000000; // Обновляем переменную SystemCoreClock
}

volatile uint32_t tick_ms = 0;

void SysTick_Handler(void)
{
    // Обработчик прерывания SysTick
    tick_ms++;
}

void SysTick_Init(void)
{
    // Настройка SysTick для генерации прерывания каждые 1 мс
    SysTick -> LOAD = (SystemCoreClock / 1000) - 1; // Загрузка значения для 1 мс
    SysTick -> VAL = 0; // Сброс текущего значения
    SysTick -> CTRL |= (1 << 1) | (1 << 2) | (1 << 0); // Включение SysTick с прерыванием
}

void USART2_Config() 
{
    RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;

    USART2 -> CR1 |= USART_CR1_TE; 
    USART2 -> CR1 |= USART_CR1_RE;
    USART2 -> CR1 &= ~USART_CR1_M; // Message length, 8 bit
    
    USART2 -> BRR = 0x139;

    USART2 -> CR1 |= USART_CR1_UE; // UART Enable

    USART2 -> CR1 |= USART_CR1_RXNEIE; // Enable RXNE interrupt
}

void GPIOA_Config()
{
    RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN; //IO Port A Enable
    
    RCC -> APB2ENR |= RCC_APB2ENR_AFIOEN;

    GPIOA -> CRL &= ~(0xF << 8); // Output mode 50 MHz
    GPIOA -> CRL |= (0xB << 8); // Alternate function push-pull

    GPIOA -> CRL &= ~(0xF << 12); // Input mode
    GPIOA -> CRL |= (0x4 << 12); // Floating input
}

volatile char received_char_global = 0;

void USART2_IRQHandler(void)
{
    if (USART2 -> SR & USART_SR_RXNE) // Check if RXNE flag is set
    {
        char received_char = USART2 -> DR; // Read received character
        received_char_global = received_char; // Store it in a global variable
    }
}

int main(void)
{
    SystemClock_Config();
    SysTick_Init();
    USART2_Config();
    GPIOA_Config();

    NVIC_EnableIRQ(USART2_IRQn); // Enable USART2 interrupt in NVIC

    RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA -> CRL &= ~(0xF << 20);
    GPIOA -> CRL |= (0x2 << 20);

    uint32_t last_tick = 0;

    while(1)
    {
        if (tick_ms - last_tick >= 4000)
        {
            last_tick = tick_ms;
            
            char buf[50];
            sprintf(buf, "Core Clock: %lu Hz\r\n", SystemCoreClock);
            char buf2[100];
            sprintf(buf2, "CFGR: 0x%lX\r\n", RCC->CFGR);

            char *p = buf;
            char *p2 = buf2;
            while (*p)
            {
                while (!(USART2 -> SR & USART_SR_TXE));

                USART2 -> DR = *p++;
            }
            while (*p2)
            {
                while (!(USART2 -> SR & USART_SR_TXE));

                USART2 -> DR = *p2++;
            }
        }

        if (received_char_global != 0)
            {
                __disable_irq(); // Disable interrupts to safely access the global variable
                char temp_char = received_char_global; // Store the received character in a temporary variable
                received_char_global = 0; // Reset the global variable
                __enable_irq(); // Re-enable interrupts
            
                if (temp_char == '1') {
                    GPIOA -> BSRR = (1 << 5); // Set PA5 high
                } else if (temp_char == '0') {
                    GPIOA -> BSRR = (1 << (5 + 16)); // Set PA5 low
                } else if (temp_char == 't') {
                    GPIOA -> ODR ^= (1 << 5); // Toggle PA5
                }

                while (!(USART2 -> SR & USART_SR_TXE));
                USART2 -> DR = temp_char; // Echo back the received character
            }
    }
}
