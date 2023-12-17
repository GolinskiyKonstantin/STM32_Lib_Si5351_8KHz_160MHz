/*
	
  ******************************************************************************
  * @file 			( фаил ):   Si5351.h
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

#ifndef _SI5351_H
#define _SI5351_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------*/

#include "main.h"

//----------------------------------------------------------------------
// адрес устройства
#define SISYNTH_DEV_ADDR 							(0x60<<1)
#define RDP_I2C												hi2c1

#define XTAL_FREQ       							25000000 		// Частота кварца 25MHz
//#define XTAL_FREQ       						27000000 		// Частота кварца 27MHz
//----------------------------------------------------------------------


extern I2C_HandleTypeDef RDP_I2C;


// See http://www.silabs.com/Support%20Documents/TechnicalDocs/AN619.pdf
enum {
    SI5351_REGISTER_0_DEVICE_STATUS                       = 0,
    SI5351_REGISTER_1_INTERRUPT_STATUS_STICKY             = 1,
    SI5351_REGISTER_2_INTERRUPT_STATUS_MASK               = 2,
    SI5351_REGISTER_3_OUTPUT_ENABLE_CONTROL               = 3,
    SI5351_REGISTER_9_OEB_PIN_ENABLE_CONTROL              = 9,
    SI5351_REGISTER_15_PLL_INPUT_SOURCE                   = 15,
    SI5351_REGISTER_16_CLK0_CONTROL                       = 16,
    SI5351_REGISTER_17_CLK1_CONTROL                       = 17,
    SI5351_REGISTER_18_CLK2_CONTROL                       = 18,
    SI5351_REGISTER_19_CLK3_CONTROL                       = 19,
    SI5351_REGISTER_20_CLK4_CONTROL                       = 20,
    SI5351_REGISTER_21_CLK5_CONTROL                       = 21,
    SI5351_REGISTER_22_CLK6_CONTROL                       = 22,
    SI5351_REGISTER_23_CLK7_CONTROL                       = 23,
    SI5351_REGISTER_24_CLK3_0_DISABLE_STATE               = 24,
    SI5351_REGISTER_25_CLK7_4_DISABLE_STATE               = 25,
    SI5351_REGISTER_42_MULTISYNTH0_PARAMETERS_1           = 42,
    SI5351_REGISTER_43_MULTISYNTH0_PARAMETERS_2           = 43,
    SI5351_REGISTER_44_MULTISYNTH0_PARAMETERS_3           = 44,
    SI5351_REGISTER_45_MULTISYNTH0_PARAMETERS_4           = 45,
    SI5351_REGISTER_46_MULTISYNTH0_PARAMETERS_5           = 46,
    SI5351_REGISTER_47_MULTISYNTH0_PARAMETERS_6           = 47,
    SI5351_REGISTER_48_MULTISYNTH0_PARAMETERS_7           = 48,
    SI5351_REGISTER_49_MULTISYNTH0_PARAMETERS_8           = 49,
    SI5351_REGISTER_50_MULTISYNTH1_PARAMETERS_1           = 50,
    SI5351_REGISTER_51_MULTISYNTH1_PARAMETERS_2           = 51,
    SI5351_REGISTER_52_MULTISYNTH1_PARAMETERS_3           = 52,
    SI5351_REGISTER_53_MULTISYNTH1_PARAMETERS_4           = 53,
    SI5351_REGISTER_54_MULTISYNTH1_PARAMETERS_5           = 54,
    SI5351_REGISTER_55_MULTISYNTH1_PARAMETERS_6           = 55,
    SI5351_REGISTER_56_MULTISYNTH1_PARAMETERS_7           = 56,
    SI5351_REGISTER_57_MULTISYNTH1_PARAMETERS_8           = 57,
    SI5351_REGISTER_58_MULTISYNTH2_PARAMETERS_1           = 58,
    SI5351_REGISTER_59_MULTISYNTH2_PARAMETERS_2           = 59,
    SI5351_REGISTER_60_MULTISYNTH2_PARAMETERS_3           = 60,
    SI5351_REGISTER_61_MULTISYNTH2_PARAMETERS_4           = 61,
    SI5351_REGISTER_62_MULTISYNTH2_PARAMETERS_5           = 62,
    SI5351_REGISTER_63_MULTISYNTH2_PARAMETERS_6           = 63,
    SI5351_REGISTER_64_MULTISYNTH2_PARAMETERS_7           = 64,
    SI5351_REGISTER_65_MULTISYNTH2_PARAMETERS_8           = 65,
    SI5351_REGISTER_66_MULTISYNTH3_PARAMETERS_1           = 66,
    SI5351_REGISTER_67_MULTISYNTH3_PARAMETERS_2           = 67,
    SI5351_REGISTER_68_MULTISYNTH3_PARAMETERS_3           = 68,
    SI5351_REGISTER_69_MULTISYNTH3_PARAMETERS_4           = 69,
    SI5351_REGISTER_70_MULTISYNTH3_PARAMETERS_5           = 70,
    SI5351_REGISTER_71_MULTISYNTH3_PARAMETERS_6           = 71,
    SI5351_REGISTER_72_MULTISYNTH3_PARAMETERS_7           = 72,
    SI5351_REGISTER_73_MULTISYNTH3_PARAMETERS_8           = 73,
    SI5351_REGISTER_74_MULTISYNTH4_PARAMETERS_1           = 74,
    SI5351_REGISTER_75_MULTISYNTH4_PARAMETERS_2           = 75,
    SI5351_REGISTER_76_MULTISYNTH4_PARAMETERS_3           = 76,
    SI5351_REGISTER_77_MULTISYNTH4_PARAMETERS_4           = 77,
    SI5351_REGISTER_78_MULTISYNTH4_PARAMETERS_5           = 78,
    SI5351_REGISTER_79_MULTISYNTH4_PARAMETERS_6           = 79,
    SI5351_REGISTER_80_MULTISYNTH4_PARAMETERS_7           = 80,
    SI5351_REGISTER_81_MULTISYNTH4_PARAMETERS_8           = 81,
    SI5351_REGISTER_82_MULTISYNTH5_PARAMETERS_1           = 82,
    SI5351_REGISTER_83_MULTISYNTH5_PARAMETERS_2           = 83,
    SI5351_REGISTER_84_MULTISYNTH5_PARAMETERS_3           = 84,
    SI5351_REGISTER_85_MULTISYNTH5_PARAMETERS_4           = 85,
    SI5351_REGISTER_86_MULTISYNTH5_PARAMETERS_5           = 86,
    SI5351_REGISTER_87_MULTISYNTH5_PARAMETERS_6           = 87,
    SI5351_REGISTER_88_MULTISYNTH5_PARAMETERS_7           = 88,
    SI5351_REGISTER_89_MULTISYNTH5_PARAMETERS_8           = 89,
    SI5351_REGISTER_90_MULTISYNTH6_PARAMETERS             = 90,
    SI5351_REGISTER_91_MULTISYNTH7_PARAMETERS             = 91,
    SI5351_REGISTER_92_CLOCK_6_7_OUTPUT_DIVIDER           = 92,
    SI5351_REGISTER_165_CLK0_INITIAL_PHASE_OFFSET         = 165,
    SI5351_REGISTER_166_CLK1_INITIAL_PHASE_OFFSET         = 166,
    SI5351_REGISTER_167_CLK2_INITIAL_PHASE_OFFSET         = 167,
    SI5351_REGISTER_168_CLK3_INITIAL_PHASE_OFFSET         = 168,
    SI5351_REGISTER_169_CLK4_INITIAL_PHASE_OFFSET         = 169,
    SI5351_REGISTER_170_CLK5_INITIAL_PHASE_OFFSET         = 170,
    SI5351_REGISTER_177_PLL_RESET                         = 177,
    SI5351_REGISTER_183_CRYSTAL_INTERNAL_LOAD_CAPACITANCE = 183
};

typedef enum {
    SI5351_CRYSTAL_LOAD_6PF  = (1<<6),
    SI5351_CRYSTAL_LOAD_8PF  = (2<<6),
    SI5351_CRYSTAL_LOAD_10PF = (3<<6)
} si5351CrystalLoad_t;

typedef enum {
    SI5351_PLL_A = 0,
    SI5351_PLL_B,
} si5351PLL_t;

typedef enum {
    SI5351_R_DIV_1   = 0,
    SI5351_R_DIV_2   = 1,
    SI5351_R_DIV_4   = 2,
    SI5351_R_DIV_8   = 3,
    SI5351_R_DIV_16  = 4,
    SI5351_R_DIV_32  = 5,
    SI5351_R_DIV_64  = 6,
    SI5351_R_DIV_128 = 7,
} si5351RDiv_t;

typedef enum {
    SI5351_DRIVE_STRENGTH_2MA = 0x00, //  ~ 2.2 dBm
    SI5351_DRIVE_STRENGTH_4MA = 0x01, //  ~ 7.5 dBm
    SI5351_DRIVE_STRENGTH_6MA = 0x02, //  ~ 9.5 dBm
    SI5351_DRIVE_STRENGTH_8MA = 0x03, // ~ 10.7 dBm
} si5351DriveStrength_t;

typedef struct {
    int32_t mult;
    int32_t num;
    int32_t denom;
} si5351PLLConfig_t;

typedef struct {
    uint8_t allowIntegerMode;
    int32_t div;
    int32_t num;
    int32_t denom;
    si5351RDiv_t rdiv;
} si5351OutputConfig_t;


#define SI5351_CLK0 						0
#define SI5351_CLK1 						1
#define SI5351_CLK2 						2

//------------------------------------------------------------------------------------------------------
/*
 * Basic interface allows to use only CLK0 and CLK2.
 * This interface uses separate PLLs for both CLK0 and CLK2 thus the frequencies
 * can be changed independently. If you also need CLK1 one PLL should
 * be shared between two CLKx and things get a little more complicated.
 * CLK0 and CLK2 were chosen because they are distant from each other on a common
 * Si5351 module. This makes using them a little more convenient than CLK0 and CLK1.
 */
void si5351_Init(int32_t correction, si5351CrystalLoad_t crystalLoad);
void si5351_SetupCLK0(int32_t Fclk, si5351DriveStrength_t driveStrength);
void si5351_SetupCLK2(int32_t Fclk, si5351DriveStrength_t driveStrength);
void si5351_EnableOutputs(uint8_t enabled);

/*
 * Advanced interface. Use it if you need:
 *
 * a. CLK0, CLK1 and CLK2 simultaneously;
 * b. A phase shift 90° between two channels;
 *
 * si5351_Calc() always uses 900 MHz PLL for frequencies below 81 MHz.
 * This PLL can safely be shared between all CLKx that work @ <= 81 MHz.
 * You can also modify si5351.c to share one PLL for any frequencies <= 112.5 MHz,
 * however this will increase the worse case calculation error to 13 Hz.
 */
void si5351_Calc(int32_t Fclk, si5351PLLConfig_t* pll_conf, si5351OutputConfig_t* out_conf);

/*
 * si5351_CalcIQ() finds PLL and MS parameters that give phase shift 90° between two channels,
 * if 0 and (uint8_t)out_conf.div are passed as phaseOffset for these channels. Channels should
 * use the same PLL to make it work.
 */
void si5351_CalcIQ(int32_t Fclk, si5351PLLConfig_t* pll_conf, si5351OutputConfig_t* out_conf);

void si5351_SetupPLL(si5351PLL_t pll, si5351PLLConfig_t* conf);
int si5351_SetupOutput(uint8_t output, si5351PLL_t pllSource, si5351DriveStrength_t driveStength, si5351OutputConfig_t* conf, uint8_t phaseOffset);

//------------------------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif	/*	_SI5351_H */

/************************ (C) COPYRIGHT GKP *****END OF FILE****/
