#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"

struct MAX2871_reg {
    unsigned int R0;
    unsigned int R1;
    unsigned int R2;
    unsigned int R3;
    unsigned int R4;
    unsigned int R5;
    unsigned int R6;
    unsigned char R0_adr;
    unsigned char R1_adr;
    unsigned char R2_adr;
    unsigned char R3_adr;
    unsigned char R4_adr;
    unsigned char R5_adr;
    unsigned char R6_adr;
} MAX2871_reg;

struct MAX2871_reg MAX2871_reg;

void send_MAX2871(unsigned char addr, unsigned int data){
    data &= 0xFFFFFFF8;
    data |= (addr&0x00000007); // prepare data
    int i = 32;
    LE_LOW;
    DATA_LOW;
    //send data to MAX2871
    while(i){
        CLK_LOW;
        if(data & 0x80000000){
            DATA_HIGH;
        }
        else{
            DATA_LOW;
        }
        data = data<<1;
        delay_us(10);
        CLK_HIGH;
        delay_us(10);
        i--;
    }
    CLK_LOW;
    delay_us(10);
    LE_HIGH;
    delay_us(10);
    LE_LOW;
    delay_us(10);
}

void send_MAX2871_all(struct MAX2871_reg data){
    send_MAX2871(data.R5_adr,data.R5);
    send_MAX2871(data.R4_adr,data.R4);
    send_MAX2871(data.R3_adr,data.R3);
    send_MAX2871(data.R2_adr,data.R2);
    send_MAX2871(data.R1_adr,data.R1);
    send_MAX2871(data.R0_adr,data.R0);
}

void MAX2871_init(void){
    // Register address
    MAX2871_reg.R0_adr = 0x00;
    MAX2871_reg.R1_adr = 0x01;
    MAX2871_reg.R2_adr = 0x02;
    MAX2871_reg.R3_adr = 0x03;
    MAX2871_reg.R4_adr = 0x04;
    MAX2871_reg.R5_adr = 0x05;
    MAX2871_reg.R6_adr = 0x06;

    // Default values
    MAX2871_reg.R0 = 0x004B0000; // 4,8 GHz
    MAX2871_reg.R1 = 0x2000FD01; // my: 0x2000FFF9 - CPL=01,CPT=00,P=000000000001,M=4000=0xFA0=111110100000
    MAX2871_reg.R2 = 0x78007842; // my: 0x00004042 - LDS=0,SDN=11,MUX=110,DBR=0,RDIV2=0,R=0000000001,REG4DB=1,CP=1100,LDF=0,LDP=0,PDP=1,SHDN=0,TRI=0,RST=0
    MAX2871_reg.R3 = 0x00000803; // my: 0x0000000B - VCO=000000,VASSHDN=0,VASTEMP=0,CSM=0,MUTEDEL=0,CDM=00,CDIV=000100000000
    MAX2871_reg.R4 = 0x6288003C; // my: 0x6180B23C - 011,SDLDO=0,SDDIV=0,SDREF=0,BS=0x280,FB=1,SDVCO=0,MTLD=0,BDIV=0,RFBEN=0,BPWR=0,RFAEN=1,APWR=11
    MAX2871_reg.R5 = 0x01400005; // my: 0x00400005 - VAS_DLY=00,SDPLL=0,F01=1,LD=01,MUX3=0,ADCS=0,ADCM=000
    MAX2871_reg.R6 = 0; // read register

    send_MAX2871(MAX2871_reg.R5_adr,MAX2871_reg.R5);
    delay_ms(20);

    send_MAX2871(MAX2871_reg.R4_adr,MAX2871_reg.R4);
    send_MAX2871(MAX2871_reg.R3_adr,MAX2871_reg.R3);
    send_MAX2871(MAX2871_reg.R2_adr,MAX2871_reg.R2);
    send_MAX2871(MAX2871_reg.R1_adr,MAX2871_reg.R1);
    send_MAX2871(MAX2871_reg.R0_adr,MAX2871_reg.R0);

    send_MAX2871(MAX2871_reg.R5_adr,MAX2871_reg.R5);
    send_MAX2871(MAX2871_reg.R4_adr,MAX2871_reg.R4);
    send_MAX2871(MAX2871_reg.R3_adr,MAX2871_reg.R3);
    send_MAX2871(MAX2871_reg.R2_adr,MAX2871_reg.R2);
    send_MAX2871(MAX2871_reg.R1_adr,MAX2871_reg.R1);
    send_MAX2871(MAX2871_reg.R0_adr,MAX2871_reg.R0);
}

unsigned int read_MAX2871(void){
    MAX2871_reg.R2 = 0x70007842;
    MAX2871_reg.R5 = 0x01440005;

    send_MAX2871_all(MAX2871_reg);
    send_MAX2871_all(MAX2871_reg);
    delay_ms(1);

    unsigned int data = 0x00000006; // Only R6
    int i = 32;
    DATA_LOW;
    LE_LOW;
    // Shift R6 address to MAX2871
    while(i){
        CLK_LOW;
        if(data & 0x80000000){
            DATA_HIGH;
        }
        else{
            DATA_LOW;
        }
        data = data<<1;
        delay_us(1);
        CLK_HIGH;
        delay_us(1);
        i--;
    }
    CLK_LOW;
    delay_us(1);
    LE_HIGH;
    delay_us(1);
    CLK_HIGH;
    delay_us(1);
    CLK_LOW;
    // Read the data
    i = 32;
    unsigned data_out = 0;
    while(i){
        CLK_HIGH;
        delay_us(1);
        if(MUX){
            data_out |= 1;
        }
        data_out = data_out<<1;
        CLK_LOW;
        delay_us(1);
        i--;
    }
    LE_LOW;
    MAX2871_reg.R2 = 0x78007842;
    MAX2871_reg.R5 = 0x01400005;
    //MAX2871_reg.R2 &= 0xE3FFFFFF; // clear MUX 2-0
    //MAX2871_reg.R5 &= 0xFFFBFFFF; // clear MUX 3
    //MAX2871_reg.R2 |= 0x18000000; // MUX 0110
    send_MAX2871_all(MAX2871_reg);

    //read MAX2871 R6 register
    MAX2871_reg.R6 = data_out; // save register value to struct
    return data_out;
}

int MAX2871_set_frequency(int frequency){
    int REG0 = 0; // all 0
    int N = 0;
    int FRAC = 0;
    int DIV = 0;
    int freq_out = 0;

    while(DIV <= 7){
        if(frequency > (3000000>>DIV)) break; // divide frequency with 2
        DIV++;
    }

    N = (frequency<<DIV)/32000;
    FRAC = (((frequency<<DIV)%32000)*4000)/32000;

    freq_out = ((32000*N)+((FRAC*32000)/4000))>>DIV;

    FRAC = FRAC<<3;
    N = N<<15;
    REG0 |= (FRAC & 0x00007FF8);
    REG0 |= (N & 0x7FFF8000);

    MAX2871_reg.R0 = REG0;
    MAX2871_reg.R4 &= 0b11111111100011111111111111111111; // reset DIVA bite
    MAX2871_reg.R4 |= DIV<<20; // Add DIV

    send_MAX2871_all(MAX2871_reg);

    return freq_out;
}
