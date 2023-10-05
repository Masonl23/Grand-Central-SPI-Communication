// #include <ml_port.h>
// #include <ml_clocks.h>
// #include <ml_adc.h>
// #include <ml_dmac.h>
// #include <ml_tcc.h>
// #include <ml_ac.h>
// #include <ml_eic.h>

// #ifdef BUILD_SERCOM_SLAVE_OLD

// static DmacDescriptor base_descriptor[3] __attribute__((aligned(16)));
// static volatile DmacDescriptor wb_descriptor[3] __attribute__((aligned(16)));

// #define SPI_RX_DMAC_CHANNEL 0
// #define SPI_RX_BUFFER_LEN 8

// volatile uint32_t spi_rx_buffer[SPI_RX_BUFFER_LEN];

// const uint32_t spi_rx_dmac_channel_settings = 
// (
//     DMAC_CHCTRLA_BURSTLEN_SINGLE |
//     DMAC_CHCTRLA_TRIGSRC(SERCOM1_DMAC_ID_RX) |
//     DMAC_CHCTRLA_TRIGACT_BURST
// );

// const uint16_t spi_rx_dmac_descriptor_settings = 
// (
//     DMAC_BTCTRL_BEATSIZE_WORD |
//     DMAC_BTCTRL_DSTINC |
//     DMAC_BTCTRL_BLOCKACT_INT |
//     DMAC_BTCTRL_VALID
// );

// #define SPI_TX_DMAC_CHANNEL 1
// #define SPI_TX_BUFFER_LEN 8

// volatile uint32_t spi_tx_buffer[SPI_TX_BUFFER_LEN] = 
// {
//     0xffffffff, 0x00000000,
//     0xffffffff, 0x00000000,
//     0xffffffff, 0x00000000,
//     0xffffffff, 0x00000000
// };

// const uint32_t spi_tx_dmac_channel_settings = 
// (
//     DMAC_CHCTRLA_BURSTLEN_SINGLE |
//     DMAC_CHCTRLA_TRIGACT_BURST |
//     DMAC_CHCTRLA_TRIGSRC(SERCOM1_DMAC_ID_TX)
// );

// const uint16_t spi_tx_dmac_descriptor_settings = 
// (
//     DMAC_BTCTRL_VALID |
//     DMAC_BTCTRL_BLOCKACT_INT |
//     DMAC_BTCTRL_BEATSIZE_WORD |
//     DMAC_BTCTRL_SRCINC
// );

// void spi_dma_slave_init(void)
// {
//     DMAC_init(&base_descriptor[0], &wb_descriptor[0]);

//     DMAC_channel_init
//     (
//         SPI_RX_DMAC_CHANNEL,
//         spi_rx_dmac_channel_settings,
//         0
//     );
    
//     DMAC_descriptor_init
//     (
//         spi_rx_dmac_descriptor_settings,
//         SPI_RX_BUFFER_LEN,
//         (uint32_t)&SERCOM1->SPI.DATA.reg,
//         (uint32_t)spi_rx_buffer + sizeof(uint32_t) * SPI_RX_BUFFER_LEN,
//         (uint32_t)&base_descriptor[SPI_RX_DMAC_CHANNEL],
//         &base_descriptor[SPI_RX_DMAC_CHANNEL]
//     );

//     DMAC_channel_intenset
//     (
//         SPI_RX_DMAC_CHANNEL,
//         DMAC_0_IRQn,
//         DMAC_CHINTENSET_TCMPL,
//         0
//     );

//     DMAC_channel_init
//     (
//         SPI_TX_DMAC_CHANNEL,
//         spi_tx_dmac_channel_settings,
//         0
//     );

//     DMAC_descriptor_init
//     (
//         spi_tx_dmac_descriptor_settings,
//         SPI_TX_BUFFER_LEN,
//         (uint32_t)spi_tx_buffer + sizeof(uint32_t) * SPI_TX_BUFFER_LEN,
//         (uint32_t)&SERCOM1->SPI.DATA.reg,
//         (uint32_t)&base_descriptor[SPI_TX_DMAC_CHANNEL],
//         &base_descriptor[SPI_TX_DMAC_CHANNEL]
//     );

//     DMAC_channel_intenset
//     (
//         SPI_TX_DMAC_CHANNEL,
//         DMAC_1_IRQn,
//         DMAC_CHINTENSET_TCMPL,
//         0
//     );

//     ML_DMAC_ENABLE();

//     ML_DMAC_CHANNEL_ENABLE(SPI_RX_DMAC_CHANNEL);
//     //ML_DMAC_CHANNEL_SUSPEND(SPI_RX_DMAC_CHANNEL);

//     ML_DMAC_CHANNEL_ENABLE(SPI_TX_DMAC_CHANNEL);
//     //ML_DMAC_CHANNEL_SUSPEND(SPI_TX_DMAC_CHANNEL);

// }


// volatile uint32_t data = 0;

// void spi_slave_init(void)
// {

//     MCLK->APBAMASK.bit.SERCOM1_ = 0x1;
//     GCLK->PCHCTRL[SERCOM1_GCLK_ID_CORE].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;

//     /*
//      * PFC
//      *
//      * PA16 --> SERCOM1/PAD0
//      * PA17 --> SERCOM1/PAD1
//      * PA18 --> SERCOM1/PAD2
//      * PA19 --> SERCOM1/PAD3
//      * 
//      * PA12 --> SERCOM2/PAD0
//      * PA13 --> SERCOM2/PAD1
//      * PA14 --> SERCOM2/PAD2
//      * PA15 --> SERCOM2/PAD3
//      * 
//      * PA01 --> SERCOM1/PAD1 (SCK) ("SCK")
//      * PA00 --> SERCOM1/PAD0 (DI) ("MOSI")
//      * PB23 --> SERCOM1/PAD3 (DO) ("MISO")
//      * PB22 --> SERCOM1/PAD2 (SS) (D3)
//      *
//      */
//     ml_pin_settings pad1 = {PORT_GRP_A, 1, PF_D, PP_ODD, INPUT_STANDARD, DRIVE_OFF};
//     ml_pin_settings pad2 = {PORT_GRP_B, 22, PF_C, PP_EVEN, INPUT_STANDARD, DRIVE_OFF};
//     ml_pin_settings pad3 = {PORT_GRP_B, 23, PF_C, PP_ODD, OUTPUT_PULL_DOWN, DRIVE_OFF};
//     ml_pin_settings pad0 = {PORT_GRP_A, 0, PF_D, PP_EVEN, INPUT_STANDARD, DRIVE_OFF};

//     peripheral_port_init(&pad1);
//     peripheral_port_init(&pad0);
//     peripheral_port_init(&pad2);
//     peripheral_port_init(&pad3);

//     SERCOM1->SPI.CTRLA.bit.ENABLE = 0x00;
//     while(SERCOM1->SPI.CTRLA.bit.ENABLE);

//     SERCOM1->SPI.CTRLA.bit.SWRST = 0x01;
//     while(SERCOM1->SPI.CTRLA.bit.SWRST);

//     SERCOM1->SPI.CTRLA.bit.MODE = 0x2;
//     SERCOM1->SPI.CTRLA.bit.CPOL = 0x0;
//     SERCOM1->SPI.CTRLA.bit.CPHA = 0x0;
//     SERCOM1->SPI.CTRLA.bit.FORM = 0x0;
//     SERCOM1->SPI.CTRLA.bit.DIPO = 0x0;
//     SERCOM1->SPI.CTRLA.bit.DOPO = 0x2;
//     SERCOM1->SPI.CTRLA.bit.DORD = 0x0;
//     SERCOM1->SPI.CTRLA.bit.IBON = 0x1;

//     SERCOM1->SPI.CTRLB.bit.CHSIZE = 0x0;
//     while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);
//     //SERCOM1->SPI.CTRLB.bit.MSSEN = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);
//     SERCOM1->SPI.CTRLB.bit.SSDE = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);
//     SERCOM1->SPI.CTRLB.bit.PLOADEN = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);


//     SERCOM1->SPI.CTRLC.bit.DATA32B = 0x1;

//     /*
//      * 
//      * f_baud = f_ref/(2*(baud + 1))
//      * 
//      * ==> baud = f_ref/(2*f_baud) - 1
//      * 
//      * f_ref = f_gclk0 = 100 MHz
//      * 
//      * Let f_baud = 10 MHz ==> baud = 100Meg/(2*10Meg) - 1 = 4
//      *  
//      */
//     SERCOM1->SPI.BAUD.bit.BAUD = 50;

//     SERCOM1->SPI.INTENSET.bit.ERROR = 0x1;
//     SERCOM1->SPI.INTENSET.bit.SSL = 0x1;
//     SERCOM1->SPI.INTENSET.bit.RXC = 0x1;
//     SERCOM1->SPI.INTENSET.bit.TXC = 0x1;
//     SERCOM1->SPI.INTENSET.bit.DRE = 0x1;

//     //NVIC_EnableIRQ(SERCOM1_0_IRQn);
//     //NVIC_SetPriority(SERCOM1_0_IRQn, 2);
//     //NVIC_EnableIRQ(SERCOM1_1_IRQn);
//     //NVIC_SetPriority(SERCOM1_1_IRQn, 2);
//     //NVIC_EnableIRQ(SERCOM1_2_IRQn);
//     //NVIC_SetPriority(SERCOM1_2_IRQn, 2);
//     NVIC_EnableIRQ(SERCOM1_3_IRQn);
//     NVIC_SetPriority(SERCOM1_3_IRQn, 2);

//     SERCOM1->SPI.CTRLA.bit.ENABLE = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.ENABLE);

//     SERCOM1->SPI.CTRLB.bit.RXEN = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);

// }
// boolean rxc_intflag = false;

// boolean ssl_intflag = false;
// uint8_t intflag_reg_cpy = 0;
// void SERCOM1_3_Handler(void)
// {
//     //SERCOM1->SPI.DATA.reg = 0xee;
//     ssl_intflag = true;
//     //intflag_reg_cpy = SERCOM1->SPI.INTFLAG.reg;
//     SERCOM1->SPI.INTFLAG.bit.SSL = 0x1;


// }

// boolean hardware_intflag = false;

// void EIC_0_Handler(void)
// {
//   EIC_CLR_INTFLAG(0);
//   hardware_intflag = true;
// }

// void SERCOM1_2_Handler(void)
// {
//     rxc_intflag = true;
// }

// boolean txc_intflag = false;

// void SERCOM1_1_Handler(void)
// {
//     SERCOM1->SPI.INTFLAG.bit.TXC = 0x1;

//     //data = SERCOM1->SPI.DATA.reg;
//     txc_intflag = true;
// }

// boolean dmac_rx_intflag = false;
// void DMAC_0_Handler(void)
// {
//     if(ML_DMAC_CHANNEL_IS_TCMPL(SPI_RX_DMAC_CHANNEL))
//     {
//         ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(SPI_RX_DMAC_CHANNEL);
//         dmac_rx_intflag = true;
//     }
// }
// boolean dmac_tx_intflag = false;
// void DMAC_1_Handler(void)
// {
//     if(ML_DMAC_CHANNEL_IS_TCMPL(SPI_TX_DMAC_CHANNEL))
//     {
//         ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(SPI_TX_DMAC_CHANNEL);
//         dmac_tx_intflag = true;
//     }
// }


// void setup(void)
// {
//     Serial.begin(9600);

//     MCLK_init();
//     GCLK_init();

//     spi_slave_init();
//     spi_dma_slave_init();

// }

// void loop(void)
// {

//     /*if(SERCOM1->SPI.INTFLAG.bit.DRE != 0)
//     {
//         SERCOM1->SPI.DATA.reg = 0xeeeeeeee;
//     }*/

//     if(ssl_intflag)
//     { 
//         //ML_DMAC_CHANNEL_RESUME(SPI_TX_DMAC_CHANNEL);       
//         //ML_DMAC_CHANNEL_RESUME(SPI_RX_DMAC_CHANNEL);
//         //while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
//         //data = SERCOM1->SPI.DATA.reg;
//         //Serial.println(data);
//         ssl_intflag = false;
//     }

//     if(dmac_rx_intflag & dmac_tx_intflag)
//     {
//         for(uint8_t i=0; i < SPI_RX_BUFFER_LEN; i++)
//         {
//             Serial.printf("%x, ", spi_rx_buffer[i]);
//         }
//         dmac_rx_intflag = dmac_tx_intflag = false;
//     }


// }

// #endif





