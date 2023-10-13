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

// build flag so we can select which program to upload
#ifndef BUILD_SERCOM_MASTER

static DmacDescriptor base_descriptor[3] __attribute__((aligned(16)));
static volatile DmacDescriptor wb_descriptor[3] __attribute__((aligned(16)));


// allocated space for RX and TX buffers
#define SPI_RX_BUFFER_LEN 6
volatile uint32_t spi_rx_buffer[SPI_RX_BUFFER_LEN];

#define SPI_TX_BUFFER_LEN 6
volatile uint32_t spi_tx_buffer[SPI_TX_BUFFER_LEN] = 
{
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};


// create SPI object
ml_spi_s spi_s = sercom1_spi_dmac_slave_prototype;

// get DMAC channel numbers for rx and tx
const uint8_t rx_dmac_chnum = spi_s.rx_dmac_s.ex_chnum;
const uint8_t tx_dmac_chnum = spi_s.tx_dmac_s.ex_chnum;

void setup(void)
{
    // create serial connection and wait til a device connects
    Serial.begin(9600);
    while(!Serial){
        // this will run indef until you create a serial connection w computer
    }

    Serial.println("Starting servant");

    // starting the clocks
    MCLK_init();
    GCLK_init();


    // start the DMAC
    DMAC_init(&base_descriptor[0], &wb_descriptor[0]);

    // enable the SERCOM1 pad for SPI mode 
    sercom1_spi_init(OPMODE_SLAVE);

    // setup DMAC for receiving data, pointing where data should be stored
    spi_s.rx_dmac_s.ex_ptr = &spi_rx_buffer[0];
    spi_s.rx_dmac_s.ex_len = SPI_RX_BUFFER_LEN;
    spi_dmac_rx_init(&spi_s.rx_dmac_s, SERCOM1, &base_descriptor[rx_dmac_chnum]);

    // setup DMAC for transmitting data, pointing where data should be sent from
    spi_s.tx_dmac_s.ex_ptr = &spi_tx_buffer[0];
    spi_s.tx_dmac_s.ex_len = SPI_TX_BUFFER_LEN;
    spi_dmac_tx_init(&spi_s.tx_dmac_s, SERCOM1, &base_descriptor[tx_dmac_chnum]);

    // enable DMAC and turn respective channels on
    ML_DMAC_ENABLE();
    ML_DMAC_CHANNEL_ENABLE(rx_dmac_chnum);
    ML_DMAC_CHANNEL_ENABLE(tx_dmac_chnum);

    // enable spi on SERCOM1 pad
    spi_enable(SERCOM1);
    spi_reciever_enable(SERCOM1);
}

// when select pin has been pulled low this means the master wants to communicat
_Bool ssl_intflag = false;
void SERCOM1_3_Handler(void)
{
    ssl_intflag = true;
    ML_SERCOM_SPI_SSL_CLR_INTFLAG(SERCOM1);
}

// interrupt for reciever DMAC 
// when transfer is complete this is called
_Bool dmac_rx_intflag = false;
void DMAC_0_Handler(void)
{
    if(ML_DMAC_CHANNEL_TCMPL_INTFLAG(rx_dmac_chnum))
    {
        ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(rx_dmac_chnum);
        dmac_rx_intflag = true;
    }
}

// iterrupt for transmitter DMAC
// when transfer is complete this is called
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

    // if rx and tx are complete transferring data when read out new data in RX from master
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