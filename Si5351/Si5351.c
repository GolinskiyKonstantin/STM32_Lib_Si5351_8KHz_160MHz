/*

  ******************************************************************************
  * @file 			( фаил ):   Si5351.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

/* Includes ----------------------------------------------------------*/
#include "Si5351.h"


static int32_t si5351Correction;

static void si5351_write(uint8_t reg, uint8_t value) ;
static void si5351_writeBulk(uint8_t baseaddr, int32_t P1, int32_t P2, int32_t P3, uint8_t divBy4, si5351RDiv_t rdiv);

//------------------------------------------------------------------------------------------------------

/*
 * Initializes Si5351. Call this function before doing anything else.
 * `Correction` is the difference of actual frequency an desired frequency @ 100 MHz.
 * It can be measured at lower frequencies and scaled linearly.
 * E.g. if you get 10_000_097 Hz instead of 10_000_000 Hz, `correction` is 97*10 = 970
 */
void si5351_Init(int32_t correction, si5351CrystalLoad_t crystalLoad)
{
    si5351Correction = correction;

    // Disable all outputs by setting CLKx_DIS high
    si5351_write(SI5351_REGISTER_3_OUTPUT_ENABLE_CONTROL, 0xFF);

    // Power down all output drivers
    si5351_write(SI5351_REGISTER_16_CLK0_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_17_CLK1_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_18_CLK2_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_19_CLK3_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_20_CLK4_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_21_CLK5_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_22_CLK6_CONTROL, 0x80);
    si5351_write(SI5351_REGISTER_23_CLK7_CONTROL, 0x80);

    // Set the load capacitance for the XTAL
    si5351_write(SI5351_REGISTER_183_CRYSTAL_INTERNAL_LOAD_CAPACITANCE, crystalLoad);
}
//------------------------------------------------------------------------------------------------------


// Sets the multiplier for given PLL
void si5351_SetupPLL(si5351PLL_t pll, si5351PLLConfig_t* conf)
{
    int32_t P1, P2, P3;
    int32_t mult = conf->mult;
    int32_t num = conf->num;
    int32_t denom = conf->denom;

    P1 = 128 * mult + (128 * num)/denom - 512;
    // P2 = 128 * num - denom * ((128 * num)/denom);
    P2 = (128 * num) % denom;
    P3 = denom;

    // Get the appropriate base address for the PLL registers
    uint8_t baseaddr = (pll == SI5351_PLL_A ? 26 : 34);
    si5351_writeBulk(baseaddr, P1, P2, P3, 0, 0);

    // Reset both PLLs
    si5351_write(SI5351_REGISTER_177_PLL_RESET, (1<<7) | (1<<5) );
}
//------------------------------------------------------------------------------------------------------


// Configures PLL source, drive strength, multisynth divider, Rdivider and phaseOffset.
// Returns 0 on success, != 0 otherwise.
int si5351_SetupOutput(uint8_t output, si5351PLL_t pllSource, si5351DriveStrength_t driveStrength, si5351OutputConfig_t* conf, uint8_t phaseOffset)
{
    int32_t div = conf->div;
    int32_t num = conf->num;
    int32_t denom = conf->denom;
    uint8_t divBy4 = 0;
    int32_t P1, P2, P3;

    if(output > 2) {
        return 1;
    }

    if((!conf->allowIntegerMode) && ((div < 8) || ((div == 8) && (num == 0)))) {
        // div in { 4, 6, 8 } is possible only in integer mode
        return 2;
    }

    if(div == 4) {
        // special DIVBY4 case, see AN619 4.1.3
        P1 = 0;
        P2 = 0;
        P3 = 1;
        divBy4 = 0x3;
    }
		else {
        P1 = 128 * div + ((128 * num)/denom) - 512;
        // P2 = 128 * num - denom * (128 * num)/denom;
        P2 = (128 * num) % denom;
        P3 = denom;
    }

    // Get the register addresses for given channel
    uint8_t baseaddr = 0;
    uint8_t phaseOffsetRegister = 0;
    uint8_t clkControlRegister = 0;
		
    switch (output) {
    case 0:
        baseaddr = SI5351_REGISTER_42_MULTISYNTH0_PARAMETERS_1;
        phaseOffsetRegister = SI5351_REGISTER_165_CLK0_INITIAL_PHASE_OFFSET;
        clkControlRegister = SI5351_REGISTER_16_CLK0_CONTROL;
        break;
    case 1:
        baseaddr = SI5351_REGISTER_50_MULTISYNTH1_PARAMETERS_1;
        phaseOffsetRegister = SI5351_REGISTER_166_CLK1_INITIAL_PHASE_OFFSET;
        clkControlRegister = SI5351_REGISTER_17_CLK1_CONTROL;
        break;
    case 2:
        baseaddr = SI5351_REGISTER_58_MULTISYNTH2_PARAMETERS_1;
        phaseOffsetRegister = SI5351_REGISTER_167_CLK2_INITIAL_PHASE_OFFSET;
        clkControlRegister = SI5351_REGISTER_18_CLK2_CONTROL;
        break;
    }

    uint8_t clkControl = 0x0C | driveStrength; // clock not inverted, powered up
		
    if(pllSource == SI5351_PLL_B) {
        clkControl |= (1 << 5); // Uses PLLB
    }

    if((conf->allowIntegerMode) && ((num == 0)||(div == 4))) {
        // use integer mode
        clkControl |= (1 << 6);
    }

    si5351_write(clkControlRegister, clkControl);
    si5351_writeBulk(baseaddr, P1, P2, P3, divBy4, conf->rdiv);
    si5351_write(phaseOffsetRegister, (phaseOffset & 0x7F));

    return 0;
}
//------------------------------------------------------------------------------------------------------


// Calculates PLL, MS and RDiv settings for given Fclk in [8_000, 160_000_000] range.
// The actual frequency will differ less than 6 Hz from given Fclk, assuming `correction` is right.
void si5351_Calc(int32_t Fclk, si5351PLLConfig_t* pll_conf, si5351OutputConfig_t* out_conf)
{
    if(Fclk < 8000){ Fclk = 8000; }
    else if(Fclk > 160000000){ Fclk = 160000000; }

    out_conf->allowIntegerMode = 1;

    if(Fclk < 1000000) {
        // For frequencies in [8_000, 500_000] range we can use si5351_Calc(Fclk*64, ...) and SI5351_R_DIV_64.
        // In practice it's worth doing for any frequency below 1 MHz, since it reduces the error.
        Fclk *= 64;
        out_conf->rdiv = SI5351_R_DIV_64;
    } 
		else {
        out_conf->rdiv = SI5351_R_DIV_1;
    }

    // Apply correction, _after_ determining rdiv.
    Fclk = Fclk - ((Fclk/1000000)*si5351Correction)/100;

    // Here we are looking for integer values of a,b,c,x,y,z such as:
    // N = a + b / c    # pll settings
    // M = x + y / z    # ms  settings
    // Fclk = Fxtal * N / M
    // N in [24, 36]
    // M in [8, 1800] or M in {4,6}
    // b < c, y < z
    // b,c,y,z <= 2**20
    // c, z != 0
    // For any Fclk in [500K, 160MHz] this algorithm finds a solution
    // such as abs(Ffound - Fclk) <= 6 Hz

    const int32_t Fxtal = 25000000;
    int32_t a, b, c, x, y, z, t;

    if(Fclk < 81000000) {
        // Valid for Fclk in 0.5..112.5 MHz range
        // However an error is > 6 Hz above 81 MHz
        a = 36; // PLL runs @ 900 MHz
        b = 0;
        c = 1;
        int32_t Fpll = 900000000;
        x = Fpll/Fclk;
        t = (Fclk >> 20) + 1;
        y = (Fpll % Fclk) / t;
        z = Fclk / t;
    } 
		else {
        // Valid for Fclk in 75..160 MHz range
        if(Fclk >= 150000000) {
            x = 4;
        } 
				else if (Fclk >= 100000000) {
            x = 6;
        } 
				else {
            x = 8;
        }
        y = 0;
        z = 1;
        
        int32_t numerator = x*Fclk;
        a = numerator/Fxtal;
        t = (Fxtal >> 20) + 1;
        b = (numerator % Fxtal) / t;
        c = Fxtal / t;
    }

    pll_conf->mult = a;
    pll_conf->num = b;
    pll_conf->denom = c;
    out_conf->div = x;
    out_conf->num = y;
    out_conf->denom = z;
}
//------------------------------------------------------------------------------------------------------


// si5351_CalcIQ() finds PLL and MS parameters that give phase shift 90° between two channels,
// if 0 and (uint8_t)out_conf.div are passed as phaseOffset for these channels. Channels should
// use the same PLL to make it work. Fclk can be from 1.4 MHz to 100 MHz. The actual frequency will
// differ less than 4 Hz from given Fclk, assuming `correction` is right.
void si5351_CalcIQ(int32_t Fclk, si5351PLLConfig_t* pll_conf, si5351OutputConfig_t* out_conf) 
{
    const int32_t Fxtal = XTAL_FREQ;
    int32_t Fpll;

    if(Fclk < 1400000){ Fclk = 1400000; }
    else if(Fclk > 100000000){ Fclk = 100000000; }

    // apply correction
    Fclk = Fclk - ((Fclk/1000000)*si5351Correction)/100;

    // disable integer mode
    out_conf->allowIntegerMode = 0;

    // Using RDivider's changes the phase shift and AN619 doesn't give any
    // guarantees regarding this change.
    out_conf->rdiv = 0;

    if(Fclk < 4900000) {
        // Little hack, run PLL below 600 MHz to cover 1.4 MHz .. 4.725 MHz range.
        // AN619 doesn't literally say that PLL can't run below 600 MHz.
        // Experiments showed that PLL gets unstable when you run it below 177 MHz,
        // which limits Fclk to 177 / 127 = 1.4 MHz.
        out_conf->div = 127;
    }
		else if(Fclk < 8000000) {
        out_conf->div = 625000000 / Fclk;
    } 
		else {
        out_conf->div = 900000000 / Fclk;
    }
    out_conf->num = 0;
    out_conf->denom = 1;

    Fpll = Fclk * out_conf->div;
    pll_conf->mult = Fpll / Fxtal;
    pll_conf->num = (Fpll % Fxtal) / 24;
    pll_conf->denom = Fxtal / 24; // denom can't exceed 0xFFFFF
}
//------------------------------------------------------------------------------------------------------


// Setup CLK0 for given frequency and drive strength. Use PLLA.
void si5351_SetupCLK0(int32_t Fclk, si5351DriveStrength_t driveStrength)
{
	si5351PLLConfig_t pll_conf;
	si5351OutputConfig_t out_conf;

	si5351_Calc(Fclk, &pll_conf, &out_conf);
	si5351_SetupPLL(SI5351_PLL_A, &pll_conf);
	si5351_SetupOutput(0, SI5351_PLL_A, driveStrength, &out_conf, 0);
}
//------------------------------------------------------------------------------------------------------


// Setup CLK2 for given frequency and drive strength. Use PLLB.
void si5351_SetupCLK2(int32_t Fclk, si5351DriveStrength_t driveStrength)
{
	si5351PLLConfig_t pll_conf;
	si5351OutputConfig_t out_conf;

	si5351_Calc(Fclk, &pll_conf, &out_conf);
	si5351_SetupPLL(SI5351_PLL_B, &pll_conf);
	si5351_SetupOutput(2, SI5351_PLL_B, driveStrength, &out_conf, 0);
}
//------------------------------------------------------------------------------------------------------


// Enables or disables outputs depending on provided bitmask.
// Examples:
// si5351_EnableOutputs(1 << 0) enables CLK0 and disables CLK1 and CLK2
// si5351_EnableOutputs((1 << 2) | (1 << 0)) enables CLK0 and CLK2 and disables CLK1
void si5351_EnableOutputs(uint8_t enabled) 
{
    si5351_write(SI5351_REGISTER_3_OUTPUT_ENABLE_CONTROL, ~enabled);
}
//------------------------------------------------------------------------------------------------------


// Writes an 8 bit value of a register over I2C.
static void si5351_write(uint8_t reg, uint8_t value) 
{
   HAL_I2C_Mem_Write(&RDP_I2C, (uint8_t)(SISYNTH_DEV_ADDR), (uint8_t)reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)(&value), 1, 500);
}
//------------------------------------------------------------------------------------------------------

// Common code for _SetupPLL and _SetupOutput
static void si5351_writeBulk(uint8_t baseaddr, int32_t P1, int32_t P2, int32_t P3, uint8_t divBy4, si5351RDiv_t rdiv)
{
    si5351_write(baseaddr,   (P3 >> 8) & 0xFF);
    si5351_write(baseaddr+1, P3 & 0xFF);
    si5351_write(baseaddr+2, ((P1 >> 16) & 0x3) | ((divBy4 & 0x3) << 2) | ((rdiv & 0x7) << 4));
    si5351_write(baseaddr+3, (P1 >> 8) & 0xFF);
    si5351_write(baseaddr+4, P1 & 0xFF);
    si5351_write(baseaddr+5, ((P3 >> 12) & 0xF0) | ((P2 >> 16) & 0xF));
    si5351_write(baseaddr+6, (P2 >> 8) & 0xFF);
    si5351_write(baseaddr+7, P2 & 0xFF);
}
//------------------------------------------------------------------------------------------------------



/************************ (C) COPYRIGHT GKP *****END OF FILE****/
