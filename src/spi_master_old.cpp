// #include <ml_port.h>
// #include <ml_clocks.h>
// #include <ml_adc.h>
// #include <ml_dmac.h>
// #include <ml_tcc.h>
// #include <ml_ac.h>
// #include <ml_eic.h>

// #ifdef BUILD_SERCOM_MASTER_OLD

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
//     DMAC_BTCTRL_BLOCKACT_BOTH |
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
//     DMAC_BTCTRL_BLOCKACT_BOTH |
//     DMAC_BTCTRL_BEATSIZE_WORD |
//     DMAC_BTCTRL_SRCINC
// );

// void spi_dma_master_init(void)
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
//         2
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
//         2
//     );

//     ML_DMAC_ENABLE();

//     ML_DMAC_CHANNEL_ENABLE(SPI_RX_DMAC_CHANNEL);
//     ML_DMAC_CHANNEL_SUSPEND(SPI_RX_DMAC_CHANNEL);

//     ML_DMAC_CHANNEL_ENABLE(SPI_TX_DMAC_CHANNEL);
//     ML_DMAC_CHANNEL_SUSPEND(SPI_TX_DMAC_CHANNEL);

// }

// volatile uint32_t data = 0;

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
//      * PA00 --> SERCOM1/PAD0 (DO) ("MOSI")
//      * PB23 --> SERCOM1/PAD3 (DI) ("MISO")
//      * PB22 --> SERCOM1/PAD2 (SS) (D3)
//      *
//      */
//     ml_pin_settings pad1 = {PORT_GRP_A, 1, PF_D, PP_ODD, OUTPUT_PULL_DOWN, DRIVE_OFF};
//     ml_pin_settings pad2 = {PORT_GRP_B, 22, PF_B, PP_EVEN, OUTPUT_PULL_DOWN, DRIVE_OFF};
//     ml_pin_settings pad3 = {PORT_GRP_B, 23, PF_C, PP_ODD, INPUT_STANDARD, DRIVE_OFF};
//     ml_pin_settings pad0 = {PORT_GRP_A, 0, PF_D, PP_EVEN, OUTPUT_PULL_DOWN, DRIVE_OFF};


// void spi_master_init(void)
// {

//     MCLK->APBAMASK.bit.SERCOM1_ = 0x1;
//     GCLK->PCHCTRL[SERCOM1_GCLK_ID_CORE].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;


//     peripheral_port_init(&pad1);
//     peripheral_port_init(&pad0);
//     peripheral_port_init(&pad2);
//     logical_set(&pad2);
//     peripheral_port_init(&pad3);

//     SERCOM1->SPI.CTRLA.bit.ENABLE = 0x00;
//     while(SERCOM1->SPI.CTRLA.bit.ENABLE);

//     SERCOM1->SPI.CTRLA.bit.SWRST = 0x01;
//     while(SERCOM1->SPI.CTRLA.bit.SWRST);

//     SERCOM1->SPI.CTRLA.bit.MODE = 0x3;
//     SERCOM1->SPI.CTRLA.bit.CPOL = 0x0;
//     SERCOM1->SPI.CTRLA.bit.CPHA = 0x0;
//     SERCOM1->SPI.CTRLA.bit.FORM = 0x0;
//     SERCOM1->SPI.CTRLA.bit.DIPO = 0x3;
//     SERCOM1->SPI.CTRLA.bit.DOPO = 0x0;
//     SERCOM1->SPI.CTRLA.bit.DORD = 0x0;

//     SERCOM1->SPI.CTRLB.bit.CHSIZE = 0x0;
//     //SERCOM1->SPI.CTRLB.bit.MSSEN = 0x1;
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
//      * Let f_baud = 1 MHz ==> baud = 100Meg/(2*1Meg) - 1 = 4
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
//     //NVIC_EnableIRQ(SERCOM1_3_IRQn);
//     //NVIC_SetPriority(SERCOM1_3_IRQn, 2);

//     SERCOM1->SPI.CTRLA.bit.ENABLE = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.ENABLE);

//     SERCOM1->SPI.CTRLB.bit.RXEN = 0x1;
//     while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);

// }

// void ss_timer_init(void)
// {
//     ML_SET_GCLK7_PCHCTRL(TC0_GCLK_ID);

//     TC0->COUNT16.CTRLA.bit.ENABLE = 0;
//     while(TC0->COUNT16.SYNCBUSY.bit.ENABLE);

//     TC0->COUNT16.CTRLA.bit.SWRST = 0x1;
//     while(TC0->COUNT16.SYNCBUSY.bit.SWRST);

//     TC0->COUNT16.CTRLA.reg = 
//     (
//         TC_CTRLA_PRESCALER_DIV1 |
//         TC_CTRLA_MODE_COUNT16 |
//         TC_CTRLA_PRESCSYNC_PRESC
//     );

//     TC0->COUNT16.WAVE.reg |= TC_WAVE_WAVEGEN_MFRQ;

//     TC0->COUNT16.CTRLBSET.reg |= TC_CTRLBSET_ONESHOT;
//     while(TC0->COUNT16.SYNCBUSY.bit.CTRLB);

//     //TC0->COUNT16.PER.reg = TC_COUNT8_PER_PER(31);

//     TC0->COUNT16.CC[0].reg |= TC_COUNT16_CC_CC(50);
//     while(TC0->COUNT16.SYNCBUSY.bit.CC0);

//     TC0->COUNT16.CTRLA.bit.ENABLE = 0x1;
//     while(TC0->COUNT16.SYNCBUSY.bit.ENABLE);

//     TC0->COUNT16.CTRLBSET.reg |= TC_CTRLBSET_CMD_STOP;
//     while(TC0->COUNT16.SYNCBUSY.bit.CTRLB);

//     TC0->COUNT16.INTENSET.bit.OVF = 0x1;
//     NVIC_EnableIRQ(TC0_IRQn);
//     NVIC_SetPriority(TC0_IRQn, 2);

//     // PA04 --> A4 (IB)
//     ml_pin_settings ss_timer_pin = {PORT_GRP_A, 4, PF_E, PP_EVEN, OUTPUT_PULL_DOWN, DRIVE_ON};
//     peripheral_port_init(&ss_timer_pin);
// }

// boolean hardware_intflag = false;
// boolean ss_timer_intflag = false;
// void TC0_Handler(void)
// {
//     ss_timer_intflag = true;
//     TC0->COUNT16.INTFLAG.bit.OVF = 0x1;
// }

// void EIC_2_Handler(void)
// {
//   EIC_CLR_INTFLAG(2);
//   hardware_intflag = true;
// }

// void SERCOM1_1_Handler(void)
// {
//     logical_set(&pad2);
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
//     OSCULP32K_init();

//     eic_init();
//     hardware_int_init();
//     eic_enable();
//     spi_master_init();
//     spi_dma_master_init();
//     ss_timer_init();

// }

// uint32_t spi_send(uint32_t data)
// {
//     while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
//     SERCOM1->SPI.DATA.reg = data;
//     while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
//     return (uint32_t)SERCOM1->SPI.DATA.reg;
// }

// uint32_t response = 0;
// boolean ss_start_flag = false;
// boolean ss_end_flag = false;

// typedef enum _dstate
// {
//     IDLE = 0x00,
//     SS_LOW_WAIT,
//     XFER_START,
//     XFER_DMAC,
//     SS_HIGH_WAIT
// } dstate;

// dstate state = IDLE;

// void loop(void)
// {
//     switch(state)
//     {
//         case IDLE:
//         {
//             if(hardware_intflag)
//             {
//                 state = SS_LOW_WAIT;

//                 TC0->COUNT16.CTRLBSET.reg |= TC_CTRLBSET_CMD_RETRIGGER;
//                 while(TC0->COUNT16.SYNCBUSY.bit.CTRLB);

//                 logical_unset(&pad2);

//                 Serial.println("hardware intflag");



//                 hardware_intflag = false;
//             }
//             break;
//         }
//         case SS_LOW_WAIT:
//         {
//             if(ss_timer_intflag)
//             {
//                 state = XFER_START;

//                 Serial.println("ss timer intflag (low)");
//                 ss_timer_intflag = false;
//             }
//             break;
//         }
//         case XFER_START:
//         {


//             //response = spi_send(0xffffffff);
//             ML_DMAC_CHANNEL_RESUME(SPI_RX_DMAC_CHANNEL);
//             ML_DMAC_CHANNEL_RESUME(SPI_TX_DMAC_CHANNEL);
//             Serial.println("Xfer");

//             state = XFER_DMAC;


//             break;
//         }
//         case XFER_DMAC:
//         {
//             if(dmac_tx_intflag & dmac_rx_intflag)
//             {
//                 TC0->COUNT16.CTRLBSET.reg |= TC_CTRLBSET_CMD_RETRIGGER;
//                 while(TC0->COUNT16.SYNCBUSY.bit.CTRLB);

//                 state = SS_HIGH_WAIT;

//                 dmac_tx_intflag = false;
//                 dmac_rx_intflag = false;
//             }
//             break;
//         }
//         case SS_HIGH_WAIT:
//         {
//             if(ss_timer_intflag)
//             {
//                 state = IDLE;

//                 logical_set(&pad2);

//                 for(uint8_t i = 0; i < SPI_RX_BUFFER_LEN; i++)
//                 {
//                     Serial.printf("%x, ", spi_rx_buffer[i]);
//                 }

//                 ss_timer_intflag = false;
//             }
//             break;
//         }
//     }
// /*
//     if(dmac_tx_intflag)
//     {
//         Serial.println("TX intflag");
//         dmac_tx_intflag = false;
//     }   
//     if(dmac_rx_intflag)
//     {
//         Serial.println("RX intflag");
//         dmac_rx_intflag = false;
//     }
// */
// }



// #endif





