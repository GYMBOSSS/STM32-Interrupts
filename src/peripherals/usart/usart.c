#include "usart.h"
#include "gpio.h"
#include "ring_buffer.h"
#include "stm32f1xx.h"

#define USART_TX_PORT GPIOA
#define USART_TX_PIN  2u
#define USART_RX_PORT GPIOA
#define USART_RX_PIN  3u

#define USART_TX_TIMEOUT_LOOPS 100000u

static ring_buffer_t rx_buffer;

/* APB1 clock, derived from the actual prescaler setting. */
static uint32_t apb1_clock(void)
{
    static const uint8_t shift_table[8] = {0, 0, 0, 0, 1, 2, 3, 4};
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    return SystemCoreClock >> shift_table[ppre1];   /* assumes AHB prescaler = 1 */
}

void usart_init(uint32_t baudrate)
{
    gpio_clock_enable(USART_TX_PORT);
    gpio_configure(USART_TX_PORT, USART_TX_PIN, GPIO_CFG_AF_PP_50MHZ);
    gpio_configure(USART_RX_PORT, USART_RX_PIN, GPIO_CFG_INPUT_FLOATING);

    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);
    (void)READ_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);

    /* Rounded division: BRR = fck / baud (oversampling by 16). */
    USART2->BRR = (apb1_clock() + baudrate / 2u) / baudrate;

    /* 8 data bits, no parity, 1 stop bit are reset defaults. */
    WRITE_REG(USART2->CR1,
              USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE);

    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void)   /* not static: name comes from the vector table */
{
    uint32_t sr = USART2->SR;

    if (sr & (USART_SR_ORE | USART_SR_RXNE)) {
        /* Reading DR after SR also clears ORE/FE/NE. */
        uint8_t byte = (uint8_t)USART2->DR;

        if ((sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0u) {
            (void)rb_push(&rx_buffer, byte);   /* drop byte if buffer is full */
        }
    }
}

bool usart_read_byte(uint8_t *byte)
{
    return rb_pop(&rx_buffer, byte);
}

void usart_write_byte(uint8_t byte)
{
    uint32_t timeout = USART_TX_TIMEOUT_LOOPS;

    while (!(USART2->SR & USART_SR_TXE)) {
        if (timeout-- == 0u) {
            return;
        }
    }
    USART2->DR = byte;
}

void usart_write_str(const char *str)
{
    while (*str != '\0') {
        usart_write_byte((uint8_t)*str++);
    }
}