#include <ml_port.h>
#include <ml_clocks.h>
#include <ml_adc.h>
#include <ml_dmac.h>
#include <ml_tcc.h>
#include <ml_ac.h>
#include <ml_eic.h>
#include <ml_dmac.h>
#include <ml_spi_common.h>
#include <ml_sercom_1.h>
#include <stdbool.h>
#include <SPI.h>

#ifndef BUILD_SERCOM_MASTER

static DmacDescriptor base_descriptor[3] __attribute__((aligned(16)));
static volatile DmacDescriptor wb_descriptor[3] __attribute__((aligned(16)));

#define SPI_RX_BUFFER_LEN 6
volatile uint32_t spi_rx_buffer[SPI_RX_BUFFER_LEN];

#define SPI_TX_BUFFER_LEN 6
volatile uint32_t spi_tx_buffer[SPI_TX_BUFFER_LEN] = 
{
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

ml_spi_s spi_s = sercom1_spi_dmac_slave_prototype;

const uint8_t rx_dmac_chnum = spi_s.rx_dmac_s.ex_chnum;
const uint8_t tx_dmac_chnum = spi_s.tx_dmac_s.ex_chnum;


void setup(void)
{
    Serial.begin(9600);
    while(!Serial){

    }

    Serial.println("Starting slave");

    MCLK_init();
    GCLK_init();

    DMAC_init(&base_descriptor[0], &wb_descriptor[0]);

    sercom1_spi_init(OPMODE_SLAVE);

    spi_s.rx_dmac_s.ex_ptr = &spi_rx_buffer[0];
    spi_s.rx_dmac_s.ex_len = SPI_RX_BUFFER_LEN;

    spi_dmac_rx_init(&spi_s.rx_dmac_s, SERCOM1, &base_descriptor[rx_dmac_chnum]);

    spi_s.tx_dmac_s.ex_ptr = &spi_tx_buffer[0];
    spi_s.tx_dmac_s.ex_len = SPI_TX_BUFFER_LEN;

    spi_dmac_tx_init(&spi_s.tx_dmac_s, SERCOM1, &base_descriptor[tx_dmac_chnum]);

    ML_DMAC_ENABLE();
    ML_DMAC_CHANNEL_ENABLE(rx_dmac_chnum);
    ML_DMAC_CHANNEL_ENABLE(tx_dmac_chnum);

    spi_enable(SERCOM1);
    spi_reciever_enable(SERCOM1);
}

_Bool ssl_intflag = false;
void SERCOM1_3_Handler(void)
{
    ssl_intflag = true;
    ML_SERCOM_SPI_SSL_CLR_INTFLAG(SERCOM1);
}

_Bool dmac_rx_intflag = false;
void DMAC_0_Handler(void)
{
    if(ML_DMAC_CHANNEL_TCMPL_INTFLAG(rx_dmac_chnum))
    {
        ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(rx_dmac_chnum);
        dmac_rx_intflag = true;
    }
}

_Bool dmac_tx_intflag = false;
void DMAC_1_Handler(void)
{
    if(ML_DMAC_CHANNEL_TCMPL_INTFLAG(tx_dmac_chnum))
    {
        ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(tx_dmac_chnum);
        dmac_tx_intflag = true;
    }
}
void loop(void)
{
    if(ssl_intflag)
    {
        ssl_intflag = false;
        Serial.print(millis()); Serial.print(" ");
        Serial.println("ssl pulled");
    }

    if(dmac_rx_intflag & dmac_tx_intflag)
    {
        for(uint8_t i=0; i < SPI_RX_BUFFER_LEN; i++)
        {
            Serial.printf("%x, ", spi_rx_buffer[i]);
        }
        dmac_rx_intflag = dmac_tx_intflag = false;
    }

}


#endif