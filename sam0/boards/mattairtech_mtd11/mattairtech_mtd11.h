/**
 * \file
 *
 * \brief Mattairtech MT-D11 board definition
 *
 */

#ifndef MATTAIRTECH_MTD11_H_
#define MATTAIRTECH_MTD11_H_

#include <conf_board.h>
#include <compiler.h>


// =========================== MattairTech MT-D11 (ATsamd11D14AM) =======================================
// Other    INT       PWM  Digital  Analog                       Digital   PWM        INT       Other
// ======================================================================================================
//                                            -------------------
// DAC                        2     2(ADC0)  | A2   | USB |  Gnd |
// REF                        3     3(ADC1)  | A3   |     |  Vcc |
//        4(INT4)   4(TCC00)  4     4(ADC2)  | A4    -----   A31 | 31     31(TC21)  31(INT3)  RX/SWDIO
//        5(INT5)   5(TCC01)  5     5(ADC3)  | A5            A30 | 30     30(TC20)            TX/SWDCLK
//                  6(TCC02)  6     6(ADC4)  | A6            A27 | 27               27(INT7)
//                  7(TCC03)  7     7(ADC5)  | A7            A23 | 23                         SCL
// MOSI    10(INT2)           10    10(ADC8) | A10           A22 | 22               22(INT6)  SDA
// SCK                        11    11(ADC9) | A11           A17 | 17     17(TC11)
// MISO    14(INTNMI)         14    14(ADC6) | A14           A16 | 16     16(TC10)  16(INT0)  LED
// BTN/SS  15(INT1)           15    15(ADC7) | A15           RST |                            Reset
//                                            -------------------

// ======================================================================================================


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_custom_boards
 * \defgroup mattairtech_mtd11_group MattairTech MT-D11 development board
 *
 * @{
 */

void system_board_init(void);

/**
 * \defgroup mattairtech_mtd11_features_group Features
 *
 * Symbols that describe features and capabilities of the board.
 *
 * @{
 */

/** Name string macro */
#define BOARD_NAME                "MATTAIRTECH_MTD11"

/** \name Resonator definitions
 *  @{ */
#define BOARD_FREQ_SLCK_XTAL      (32768U)
#define BOARD_FREQ_SLCK_BYPASS    (32768U)
#define BOARD_FREQ_MAINCK_XTAL    0 /* Not Mounted */
#define BOARD_FREQ_MAINCK_BYPASS  0 /* Not Mounted */
#define BOARD_MCK                 CHIP_FREQ_CPU_MAX
#define BOARD_OSC_STARTUP_US      15625
/** @} */

/** \name LED0 definitions
 *  @{ */
#define LED0_PIN                  PIN_PA16
#define LED0_ACTIVE               false
#define LED0_INACTIVE             !LED0_ACTIVE
#define LED0_GPIO                 LED0_PIN
/** @} */

/** \name SW0 definitions
 *  @{ */
#define SW0_PIN                   PIN_PA15
#define SW0_ACTIVE                false
#define SW0_INACTIVE              !SW0_ACTIVE
#define SW0_EIC_PIN               PIN_PA15A_EIC_NMI
#define SW0_EIC_MUX               MUX_PA15A_EIC_NMI
#define SW0_EIC_PINMUX            PINMUX_PA15A_EIC_NMI
#define SW0_EIC_LINE              0
/** @} */

/**
 * \name LED #0 definitions
 *
 * Wrapper macros for the on-board LED
 *
 *  @{ */
#define LED_0_NAME                "LED0 (blue)"
#define LED_0_PIN                 LED0_PIN
#define LED_0_ACTIVE              LED0_ACTIVE
#define LED_0_INACTIVE            LED0_INACTIVE
/** @} */

/** Number of on-board LEDs */
#define LED_COUNT                 1

/**
 * \name Button #0 definitions
 *
 * Wrapper macros for SW0, to ensure common naming across all Xplained Pro
 * boards.
 *
 *  @{ */
#define BUTTON_0_NAME             "SW0"
#define BUTTON_0_PIN              SW0_PIN
#define BUTTON_0_ACTIVE           SW0_ACTIVE
#define BUTTON_0_INACTIVE         SW0_INACTIVE
#define BUTTON_0_EIC_PIN          SW0_EIC_PIN
#define BUTTON_0_EIC_MUX          SW0_EIC_MUX
#define BUTTON_0_EIC_PINMUX       SW0_EIC_PINMUX
#define BUTTON_0_EIC_LINE         SW0_EIC_LINE
/** @} */

/** Number of on-board buttons */
#define BUTTON_COUNT 1


/** \name ADC pin definitions
 *  @{
 */
#define EXT_PIN_ADC_0            PIN_PA02
#define EXT_PIN_ADC_1            PIN_PA03
#define EXT_PIN_ADC_2            PIN_PA04
#define EXT_PIN_ADC_3            PIN_PA05
#define EXT_PIN_ADC_4            PIN_PA06
#define EXT_PIN_ADC_5            PIN_PA07
#define EXT_PIN_ADC_6            PIN_PA14
#define EXT_PIN_ADC_7            PIN_PA15
#define EXT_PIN_ADC_8            PIN_PA10
#define EXT_PIN_ADC_9            PIN_PA11
/** @} */

/** \name Miscellaneous function pin definitions
 *  @{
 */
#define EXT_PIN_DAC              PIN_PA02
#define EXT_PIN_REF              PIN_PA03
/** @} */


/** \name PWM pin definitions
 *  @{
 */
#define EXT_PIN_PWM_TCC0             PIN_PA04
#define EXT_PIN_PWM_TCC1             PIN_PA05
#define EXT_PIN_PWM_TCC2             PIN_PA06
#define EXT_PIN_PWM_TCC3             PIN_PA07
#define EXT_PIN_PWM_TC10             PIN_PA16
#define EXT_PIN_PWM_TC11             PIN_PA17
#define EXT_PIN_PWM_TC20             PIN_PA30
#define EXT_PIN_PWM_TC21             PIN_PA31
/** @} */


/** \name I2C pin definitions
 *  @{
 */
#define EXT_PIN_I2C_SDA          PIN_PA22
#define EXT_PIN_I2C_SCL          PIN_PA23
/** @} */

/** \name Single Wire Debug pin definitions
 *  @{
 */
#define EXT_PIN_RX_SWDIO          PIN_PA31
#define EXT_PIN_TX_SWDCLK         PIN_PA30
/** @} */

/** \name SPI pin definitions
 *  @{
 */
#define EXT_PIN_SPI_MOSI         PIN_PA10
#define EXT_PIN_SPI_SCK          PIN_PA11
#define EXT_PIN_SPI_MISO         PIN_PA14
/** @} */


/**
 * \brief Turns off the specified LEDs.
 *
 * \param led_gpio LED to turn off (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_Off(led_gpio)     port_pin_set_output_level(led_gpio,true)

/**
 * \brief Turns on the specified LEDs.
 *
 * \param led_gpio LED to turn on (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_On(led_gpio)      port_pin_set_output_level(led_gpio,false)

/**
 * \brief Toggles the specified LEDs.
 *
 * \param led_gpio LED to toggle (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_Toggle(led_gpio)  port_pin_toggle_output_level(led_gpio)


#ifdef __cplusplus
}
#endif

#endif  /* MATTAIRTECH_MTD11_H_ */
