/**
 *
 *
 * First Attempt at my Capstone Firmware Driver
 *
 * Created by Andrew Geyer
 * Last updated 11/17/20
 */


#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int i;

//User selected RGB values
int uRed;
int uGreen;
int uBlue;

//Target RGB values
int tRed;
int tGreen;
int tBlue;

//Actual output RGB values
int aRed;
int aGreen;
int aBlue;

//An array of simulated sensor values
int sensor1[4][3];
int sensor2[4];

int sens1 = 0xBB0E13;
int sens2 = 0x00;

int receiveByte;

int power = 1;

int testPackage = 0xBB0E13;
int fullPackage;

int bytesReceived = 7;      // Keeps track of # UART bytes received
int ack = 0;                // Track acknowledgement sent to GUI
char start[5];
int temp;

const eUSCI_UART_ConfigV1 uartConfig =
{
         EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
         78,                                     // BRDIV = 78
         2,                                       // UCxBRF = 2
         0,                                       // UCxBRS = 0
         EUSCI_A_UART_NO_PARITY,                  // No Parity
         EUSCI_A_UART_LSB_FIRST,                  // LSB First
         EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
         EUSCI_A_UART_MODE,                       // UART mode
         EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};


void packageHandler(int pack)
{
    uBlue = pack % 256;
    pack = pack / 256;
    uGreen = pack % 256;
    pack = pack / 256;
    uRed = pack % 256;
    pack = pack / 256;

}

void writeSensor(void)
{
    int w=0;
    int v=0;

    for(w=0;w<4;w++)
    {
        sensor1[w][2] = sens1 % 256;
        v = sens1 / 256;
        sensor1[w][1] = v % 256;
        v = v / 256;
        sensor1[w][0] = v;

        sensor2[w] = sens2;
    }
}

double test;
void userTarget(void)
{
    double brightness;
    double factor;
    int w;

    for(w = 0; w<4; w++)
    {
        brightness = brightness + (sensor2[w]/10);
    }
    brightness = brightness/4;
    if(brightness > 25)
    {
        brightness = 25;
    }

    factor = 100+brightness;
    factor = factor/100;

    test = factor;


    if(uRed > uGreen && uRed > uBlue)
    {
        if((uRed * factor)<256)
        {
            tRed = uRed * factor;
            tGreen = uGreen * factor;
            tBlue = uBlue * factor;
        }
        else
        {
            factor = 255/uRed;
            tRed = 255;
            tGreen = uGreen * factor;
            tBlue = uBlue * factor;
        }
    }
    else if(uGreen > uBlue)
    {
        if((uGreen * factor)<256)
        {
            tRed = uRed * factor;
            tGreen = uGreen * factor;
            tBlue = uBlue * factor;
        }
        else
        {
            factor = 255/uGreen;
            tGreen = 255;
            tRed = uRed * factor;
            tBlue = uBlue * factor;
        }
    }
    else
    {
        if((uBlue * factor)<256)
        {
            tRed = uRed * factor;
            tGreen = uGreen * factor;
            tBlue = uBlue * factor;
        }
        else
        {
            factor = 255/uBlue;
            tBlue = 255;
            tGreen = uGreen * factor;
            tRed = uRed * factor;
        }
    }

}


void targetActual(void)
{
    //modifiers for RGB values
    int mRed;
    int mGreen;
    int mBlue;
    double vRed;
    double vGreen;
    double vBlue;

    int w;

    for(w = 0; w<4; w++)
    {
        vRed += sensor1[w][0];
        vGreen += sensor1[w][1];
        vBlue += sensor1[w][2];
    }
    mRed = vRed/4;
    mGreen = vGreen/4;
    mBlue = vBlue/4;

    if(tRed > mRed)
    {
        mRed = tRed-mRed;
        if(mRed > 25)
        {
            mRed = 25;
        }
    }
    else if(tRed < mRed)
    {
        mRed = tRed-mRed;
        if(mRed < -25)
        {
            mRed = -25;
        }
    }
    else if(tRed = mRed)
    {
        mRed = 0;
    }

    if(tGreen > mGreen)
    {
        mGreen = tGreen-mGreen;
        if(mGreen > 25)
        {
            mGreen = 25;
        }
    }
    else if(tGreen < mGreen)
    {
        mGreen = tGreen-mGreen;
        if(mGreen < -25)
        {
            mGreen = -25;
        }
    }
    else if(tGreen = mGreen)
    {
        mGreen = 0;
    }

    if(tBlue > mBlue)
    {
        mBlue = tBlue-mBlue;
        if(mBlue > 25)
        {
            mBlue = 25;
        }
    }
    else if(tBlue < mBlue)
    {
        mBlue = tBlue-mBlue;
        if(mBlue < -25)
        {
            mBlue = -25;
        }
    }
    else if(tBlue = mBlue)
    {
        mBlue = 0;
    }


    if(power == 1)
    {
        aRed = tRed + mRed;
        aGreen = tGreen + mGreen;
        aBlue = tBlue + mBlue;

        if(aRed < 0)
        {
            aRed = 0;
        }
        if(aGreen < 0)
        {
            aGreen = 0;
        }
        if(aBlue < 0)
        {
            aBlue = 0;
        }

        if(aRed > 255)
        {
            aRed = 255;
        }
        if(aGreen > 255)
        {
            aGreen = 255;
        }
        if(aBlue > 255)
        {
            aBlue = 255;
        }

    }
    else
    {
        aRed = 0;
        aGreen = 0;
        aBlue = 0;
    }

    //These lines actually drive the pins
    P3->OUT = aRed;
    P4->OUT = aGreen;
    P5->OUT = aBlue;
}

//For actual I2C reading from the sensors, will use https://www.vishay.com/docs/84331/designingveml6040.pdf

int main(void)
{


    uRed=0;
    uGreen=0;
    uBlue=0;
    i=0;

    packageHandler(testPackage);

    /* Confinguring P1.1 & P1.4 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);


    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);



    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //configue outputs
    P3->DIR |= 0xFF; // Port 1 will be the Red pins, i=0
    P4->DIR |= 0xFF; // Port 3 will be the Green pins, i=1
    P5->DIR |= 0xFF; // Port 5 will be the Blue pins, i=2



    //Central While Loop
    while(1)
    {
        MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
        MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
        MAP_Interrupt_enableInterrupt(INT_PORT1);
        MAP_Interrupt_enableMaster();

        //Write what the sensor values are (TEST ONLY)
        writeSensor();
        //Take whats been given and turn into real output
        userTarget();
        targetActual();

        i++;
    }
}


// Port 1 handler for when switches are pressed. Mainly for testing
// NOW OBSOLETE
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    /* Handles S1 button press */
    if (status & GPIO_PIN1)
    {
        switch(i)
        {
        case 0: //Red
            if(uRed == 255)
            {
                uRed=0;
                P3->OUT = 0x00;
            }
            else
            {
                uRed++;
                P3->OUT++;
            }
            break;
        case 1: //Green
            if(uGreen == 255)
            {
                uGreen=0;
                P4->OUT = 0x00;
            }
            else
            {
                uGreen++;
                P4->OUT++;
            }
            break;
        case 2:  //Blue
            if(uBlue == 255)
            {
                uBlue=0;
                P5->OUT = 0x00;
            }
            else
            {
                uBlue++;
                P5->OUT++;
            }
            break;
        }

    }
    /* Handles S2 button press */
    if (status & GPIO_PIN4)
    {
        if(i==2)
        {
            i=0;
        }
        else
        {
            i++;
        }
    }
}

void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);



    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);



    receiveByte = UCA0RXBUF;

    /* Wait for 'START' keyword to be received before changing color */


        if (bytesReceived > 6) {
            start[0] = start[1];
            start[1] = start[2];
            start[2] = start[3];
            start[3] = start[4];
            start[4] = receiveByte;

            if (start[0] == 'S' && start[1] == 'T' && start[2] == 'A' && start[3] == 'R' && start[4] == 'T')
            {
                bytesReceived = -1;
                temp = 0;
            }
        }
        else if (bytesReceived==0)
        {
            if(receiveByte==48)
            {
                power = 0;
            }
            else if(receiveByte==49)
            {
                power = 1;
            }
        }
        else
        {
            if (true)
            {
                temp = temp*0x10;

                if(receiveByte==48)
                {
                    temp += 0x0;
                }
                else if(receiveByte==49)
                {
                    temp += 0x1;
                }
                else if(receiveByte==50)
                {
                    temp += 0x2;
                }
                else if(receiveByte==51)
                {
                    temp += 0x3;
                }
                else if(receiveByte==52)
                {
                    temp += 0x4;
                }
                else if(receiveByte==53)
                {
                    temp += 0x5;
                }
                else if(receiveByte==54)
                {
                    temp += 0x6;
                }
                else if(receiveByte==55)
                {
                    temp += 0x7;
                }
                else if(receiveByte==56)
                {
                    temp += 0x8;
                }
                else if(receiveByte==57)
                {
                    temp += 0x9;
                }
                else if(receiveByte== 65 || receiveByte== 97)
                {
                    temp += 0xA;
                }
                else if(receiveByte== 66 || receiveByte== 98)
                {
                    temp += 0xB;
                }
                else if(receiveByte== 67 || receiveByte== 99)
                {
                    temp += 0xC;
                }
                else if(receiveByte== 68 || receiveByte== 100)
                {
                    temp += 0xD;
                }
                else if(receiveByte== 69 || receiveByte== 101)
                {
                    temp += 0xE;
                }
                else if(receiveByte== 70 || receiveByte== 102)
                {
                    temp += 0xF;
                }
                else
                {
                    temp += 0x0;
                }


            }


        }



        if (bytesReceived < 7)
        {
            bytesReceived++;
            if(temp != 0 && bytesReceived == 7)
            {
                packageHandler(temp);
                temp = 0;
            }
        }

}


