#include <ml_port.h>
#include <ml_clocks.h>
#include <ml_adc.h>
#include <ml_dmac.h>
#include <ml_tcc.h>
#include <ml_ac.h>
#include <ml_eic.h>
#include <ml_spi_common.h>
#include <ml_tc0.h>
#include <ml_tc1.h>
#include <ml_tc_common.h>
#include <ml_sercom_1.h>
#include <stdbool.h>
#include <ml_dac0.h>

#include <vector>
#include <string>

#ifdef BUILD_SERCOM_MASTER

void evsys_init(void)
{
    MCLK->APBBMASK.bit.EVSYS_ = 0x01;
    ML_SET_GCLK0_PCHCTRL(EVSYS_GCLK_ID_0);

    EVSYS->Channel[0].CHANNEL.bit.EDGSEL = EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val;
    EVSYS->Channel[0].CHANNEL.bit.PATH = EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val;
    EVSYS->Channel[0].CHANNEL.bit.EVGEN = 0x49;

    EVSYS->USER[45].bit.CHANNEL = 0x00 + 0x01;
}

static DmacDescriptor base_descriptor[3] __attribute__((aligned(16)));
static volatile DmacDescriptor wb_descriptor[3] __attribute__((aligned(16)));

#define SPI_RX_BUFFER_LEN 6
volatile uint32_t spi_rx_buffer[SPI_RX_BUFFER_LEN];

#define SPI_TX_BUFFER_LEN 6
volatile uint32_t spi_tx_buffer[SPI_TX_BUFFER_LEN] =
    {
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,

};

ml_spi_s spi_s = sercom1_spi_dmac_master_prototype;

const uint8_t rx_dmac_chnum = spi_s.rx_dmac_s.ex_chnum;
const uint8_t tx_dmac_chnum = spi_s.tx_dmac_s.ex_chnum;

void sercom_spi_timer_init(void)
{
    TC0_init();
    TC_set_oneshot(TC0);
    TC0->COUNT16.EVCTRL.bit.OVFEO = 0x01;

    TC_enable(TC0);
    TC_force_stop(TC0);
    TC0_CC0_pinout();
}

void led_timer_init(void)
{
    TC1_init();
    TC1->COUNT16.EVCTRL.bit.TCEI = 0x01;
    TC1->COUNT16.EVCTRL.bit.EVACT = 0x01;
    TC_enable(TC1);
    TC_force_stop(TC1);
    TC1_CC0_pinout();
}

ml_pin_settings pad1 = {PORT_GRP_A, 1, PF_D, PP_ODD, OUTPUT_PULL_DOWN, DRIVE_OFF};
ml_pin_settings pad2 = {PORT_GRP_B, 22, PF_B, PP_EVEN, OUTPUT_PULL_DOWN, DRIVE_OFF};
ml_pin_settings pad3 = {PORT_GRP_B, 23, PF_C, PP_ODD, INPUT_STANDARD, DRIVE_OFF};
ml_pin_settings pad0 = {PORT_GRP_A, 0, PF_D, PP_EVEN, OUTPUT_PULL_DOWN, DRIVE_OFF};

void setup(void)
{
    Serial.begin(9600);
    while (!Serial)
    {
    }
    Serial.println("Master Starting");

    MCLK_init();
    GCLK_init();
    OSCULP32K_init();

    eic_init();
    hardware_int_init();
    eic_enable();

    evsys_init();

    DMAC_init(&base_descriptor[0], &wb_descriptor[0]);

    sercom1_spi_init(OPMODE_MASTER);

    spi_s.rx_dmac_s.ex_ptr = &spi_rx_buffer[0];
    spi_s.rx_dmac_s.ex_len = SPI_RX_BUFFER_LEN;

    spi_dmac_rx_init(&spi_s.rx_dmac_s, SERCOM1, &base_descriptor[rx_dmac_chnum]);

    spi_s.tx_dmac_s.ex_ptr = &spi_tx_buffer[0];
    spi_s.tx_dmac_s.ex_len = SPI_TX_BUFFER_LEN;

    spi_dmac_tx_init(&spi_s.tx_dmac_s, SERCOM1, &base_descriptor[tx_dmac_chnum]);

    ML_DMAC_ENABLE();

    ML_DMAC_CHANNEL_ENABLE(rx_dmac_chnum);
    ML_DMAC_CHANNEL_SUSPEND(rx_dmac_chnum);

    ML_DMAC_CHANNEL_ENABLE(tx_dmac_chnum);
    ML_DMAC_CHANNEL_SUSPEND(tx_dmac_chnum);

    led_timer_init();
    sercom_spi_timer_init();
    spi_enable(SERCOM1);
    spi_reciever_enable(SERCOM1);
}

_Bool hardware_intflag = false;
void EIC_2_Handler(void)
{
    EIC_CLR_INTFLAG(2);
    hardware_intflag = true;
}

_Bool ss_timer_intflag = false;
void TC0_Handler(void)
{
    if (ML_TC_OVF_INTFLAG(TC0))
    {
        ML_TC_OVF_CLR_INTFLAG(TC0);
        ss_timer_intflag = true;
    }
}

_Bool dmac_rx_intflag = false;
void DMAC_0_Handler(void)
{
    if (ML_DMAC_CHANNEL_TCMPL_INTFLAG(rx_dmac_chnum))
    {
        ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(rx_dmac_chnum);
        dmac_rx_intflag = true;
    }

    // TC0->COUNT16.INTFLAG.bit.OVF = 0x01;
}

_Bool dmac_tx_intflag = false;
void DMAC_1_Handler(void)
{
    if (ML_DMAC_CHANNEL_TCMPL_INTFLAG(tx_dmac_chnum))
    {
        ML_DMAC_CHANNEL_CLR_TCMPL_INTFLAG(tx_dmac_chnum);
        dmac_tx_intflag = true;
    }
}

ml_spi_state dstate = IDLE;

// ml_pin_settings pad2 = {PORT_GRP_B, 22, PF_B, PP_EVEN, OUTPUT_PULL_DOWN, DRIVE_OFF};

// stack overflowage
size_t split(const std::string txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos)
    {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

bool isFloat(std::string line)
{
    char *p;
    strtof(line.c_str(), &p);
    return *p == 0;
}

std::string userInput;
void spi_loop()
{
    switch (dstate)
    {
    case IDLE:
    {
        if (Serial.available())
        {
            char input = Serial.read();
            Serial.print(input);
            if (input == '\r')
            {
                // new line feed
                Serial.print('\n');
                // parse strings
                std::vector<std::string> parsedData;
                int len = split(userInput, parsedData, ' ');

                if (parsedData.at(0) == "all")
                {
                    if (parsedData.size() > 1)
                    {
                        if (isFloat(parsedData.at(1).c_str()))
                        {
                            Serial.print("All motors position: ");
                            Serial.println(strtof(parsedData.at(1).c_str(), NULL));
                        }
                    }
                }
                else
                {

                    // check lengths
                    for (size_t i = 0; i < parsedData.size(); i++)
                    {
                        Serial.print("Motor ");
                        Serial.print(i);
                        if (isFloat(parsedData.at(i).c_str()))
                        {
                            float angle = strtof(parsedData.at(i).c_str(), NULL);
                            Serial.print(" Angle: ");
                            Serial.println(angle);
                            spi_tx_buffer[i] = angle;
                        }
                        else
                        {
                            Serial.println(" not float");
                        }
                    }
                }
                // reset input
                userInput.clear();

                // send spi
                dstate = SS_LOW_WAIT;
                TC_force_retrigger(TC0);
                GC_SERCOM1_SPI_SS0_PULL();
                hardware_intflag = false;
            }
            else
            {
                userInput.push_back(input);
            }
        }
        // if (Serial.available())
        // {
        //     // userInput = Serial.readStringUntil('\n');
        //     // Serial.println(userInput);
        //     Serial.read();

        //     dstate = SS_LOW_WAIT;
        //     TC_force_retrigger(TC0);
        //     GC_SERCOM1_SPI_SS0_PULL();
        //     hardware_intflag = false;
        // }

        break;
    }
    case SS_LOW_WAIT:
    {
        if (ss_timer_intflag)
        {
            dstate = XFER_START;

            // Serial.println("ss timer intflag (low)");
            ss_timer_intflag = false;
        }
        break;
    }
    case XFER_START:
    {
        ML_DMAC_CHANNEL_RESUME(rx_dmac_chnum);
        ML_DMAC_CHANNEL_RESUME(tx_dmac_chnum);

        Serial.print(millis());
        Serial.print(" ");
        Serial.println("XFER");
        dstate = XFER_DMAC;

        break;
    }
    case XFER_DMAC:
    {
        if (dmac_tx_intflag & dmac_rx_intflag)
        {
            TC_force_retrigger(TC0);

            dstate = SS_HIGH_WAIT;

            dmac_tx_intflag = dmac_rx_intflag = false;
        }
        break;
    }
    case SS_HIGH_WAIT:
    {
        if (ss_timer_intflag)
        {
            dstate = IDLE;
            GC_SERCOM1_SPI_SS0_PUSH();
            Serial.print("Data: ");
            for (uint8_t i = 0; i < SPI_RX_BUFFER_LEN; i++)
            {
                Serial.printf("%x, ", spi_rx_buffer[i]);
            }
            ss_timer_intflag = false;
        }
        break;
    }
    }
}

unsigned long lastTime = 0;
void loop(void)
{

    spi_loop();

    // if (Serial.available())
    // {
    //     char input = Serial.read();
    //     Serial.print(input);
    //     if (input == '\r')
    //     {
    //         // new line feed
    //         Serial.print('\n');
    //         // parse strings
    //         std::vector<std::string> parsedData;
    //         int len = split(userInput, parsedData, ' ');
    //         // Serial.println(userInput.c_str());
    //         // Serial.print(" Length: ");
    //         // Serial.println(len);

    //         if (parsedData.at(0) == "all")
    //         {
    //             if(parsedData.size() > 1){
    //                 if(isFloat(parsedData.at(1).c_str())){
    //                     Serial.print("All motors position: "); Serial.println(strtof(parsedData.at(1).c_str(),NULL));
    //                 }

    //             }
    //         }
    //         else
    //         {

    //             // check lengths
    //             for (size_t i = 0; i < parsedData.size(); i++)
    //             {
    //                 Serial.print("Motor ");
    //                 Serial.print(i);
    //                 if (isFloat(parsedData.at(i).c_str()))
    //                 {
    //                     float angle = strtof(parsedData.at(i).c_str(), NULL);
    //                     Serial.print(" Angle: ");
    //                     Serial.println(angle);
    //                     spi_tx_buffer[i] = (uint32_t) angle;
    //                 }
    //                 else
    //                 {
    //                     Serial.println(" not float");
    //                 }
    //             }
    //         }
    //         // reset input
    //         userInput.clear();
    //     }
    //     else
    //     {
    //         userInput.push_back(input);
    //     }
    // }
}
#endif