/************************************************************************************
 * configs/nucleo-144/src/nucleo-144.h
 *
 *   Copyright (C) 2016 Gregory Nutt. All rights reserved.
 *   Authors: Gregory Nutt <gnutt@nuttx.org>
 *            Mark Olsson <post@markolsson.se>
 *            David Sidrane <david_s5@nscdg.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ************************************************************************************/

#ifndef __CONFIGS_NUCLEO_144_SRC_NUCLEO_144_H
#define __CONFIGS_NUCLEO_144_SRC_NUCLEO_144_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* Configuration ********************************************************************/

/* procfs File System */

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define STM32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define STM32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/* Nucleo-144 GPIO Pin Definitions **************************************************/

/* LED
 *
 * The Nucleo-144 board has numerous LEDs but only three, LD1 a Green LED, LD2 a
 * Blue LED and LD3 a Red LED, that can be controlled by software. The following
 * definitions assume the default Solder Bridges are installed.
 */

#define GPIO_LD1       (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                        GPIO_PORTB | GPIO_PIN0)
#define GPIO_LD2       (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                        GPIO_PORTB | GPIO_PIN7)
#define GPIO_LD3       (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                        GPIO_PORTB | GPIO_PIN14)

#define GPIO_LED_GREEN GPIO_LD1
#define GPIO_LED_BLUE  GPIO_LD2
#define GPIO_LED_RED   GPIO_LD3

#define LED_DRIVER_PATH "/dev/userleds"

/* BUTTONS
 *
 * The Blue pushbutton B1, labeled "User", is connected to GPIO PC13.  A high value
 * will be sensed when the button is depressed.
 * Note:
 *    1) That the EXTI is included in the definition to enable an interrupt on this
 *       IO.
 *    2) The following definitions assume the default Solder Bridges are installed.
 */

#define GPIO_BTN_USER  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | GPIO_PORTC | GPIO_PIN13)

/* SPI ***************************************************************************/

#define GPIO_SPI_CS    (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_SET)

#define GPIO_SPI1_CS0   (GPIO_SPI_CS | GPIO_PORTA | GPIO_PIN15)
#define GPIO_SPI1_CS1   (GPIO_SPI_CS | GPIO_PORTC | GPIO_PIN15)
#define GPIO_SPI1_CS2   (GPIO_SPI_CS | GPIO_PORTC | GPIO_PIN14)
#define GPIO_SPI1_CS3   (GPIO_SPI_CS | GPIO_PORTC | GPIO_PIN2)
#define GPIO_SPI2_CS0   (GPIO_SPI_CS | GPIO_PORTD | GPIO_PIN7)
#define GPIO_SPI2_CS1   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN1)
#define GPIO_SPI2_CS2   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN2)
#define GPIO_SPI2_CS3   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN3)
#define GPIO_SPI3_CS0   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN4)
#define GPIO_SPI3_CS1   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN5)
#define GPIO_SPI3_CS2   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN6)
#define GPIO_SPI3_CS3   (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN7)

/* Logical SPI Chip Selects used to index */

#define NUCLEO_SPI_BUS1_CS0  0
#define NUCLEO_SPI_BUS1_CS1  1
#define NUCLEO_SPI_BUS1_CS2  2
#define NUCLEO_SPI_BUS1_CS3  3
#define NUCLEO_SPI_BUS2_CS0  4
#define NUCLEO_SPI_BUS2_CS1  5
#define NUCLEO_SPI_BUS2_CS2  6
#define NUCLEO_SPI_BUS2_CS3  7
#define NUCLEO_SPI_BUS3_CS0  8
#define NUCLEO_SPI_BUS3_CS1  9
#define NUCLEO_SPI_BUS3_CS2  10
#define NUCLEO_SPI_BUS3_CS3  11

#if defined(CONFIG_STM32F7_SDMMC1) || defined(CONFIG_STM32F7_SDMMC2)
# define HAVE_SDIO
#endif

#if defined(CONFIG_DISABLE_MOUNTPOINT) || !defined(CONFIG_MMCSD_SDIO)
#  undef HAVE_SDIO
#endif

#define SDIO_SLOTNO 0  /* Only one slot */

#ifdef HAVE_SDIO
#  if defined(CONFIG_STM32F7_SDMMC1)
#    define GPIO_SDMMC1_NCD (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI | GPIO_PORTC | GPIO_PIN6)
#  endif

#  if defined(CONFIG_NSH_MMCSDSLOTNO) && (CONFIG_NSH_MMCSDSLOTNO != 0)
#    warning "Only one MMC/SD slot, slot 0"
#    define CONFIG_NSH_MMCSDSLOTNO SDIO_SLOTNO
#  endif

#  if defined(CONFIG_NSH_MMCSDMINOR)
#    define SDIO_MINOR CONFIG_NSH_MMCSDMINOR
#  else
#    define SDIO_MINOR 0
#  endif
#endif

/* USB OTG FS
 *
 * PA9  OTG_FS_VBUS VBUS sensing (also connected to the green LED)
 * PC0  OTG_FS_PowerSwitchOn
 * PD5  OTG_FS_Overcurrent
 */

#define GPIO_OTGFS_VBUS   (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_OPENDRAIN|GPIO_PORTA|GPIO_PIN9)

#define GPIO_OTGFS_PWRON  (GPIO_OUTPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_PUSHPULL|GPIO_PORTG|GPIO_PIN6)

#ifdef CONFIG_USBHOST
#  define GPIO_OTGFS_OVER (GPIO_INPUT|GPIO_EXTI|GPIO_FLOAT|\
                           GPIO_SPEED_100MHz|GPIO_PUSHPULL|\
                           GPIO_PORTG|GPIO_PIN7)

#else
#  define GPIO_OTGFS_OVER (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_PUSHPULL|GPIO_PORTG|GPIO_PIN7)
#endif

/* GPIO pins used by the GPIO Subsystem */

#define BOARD_NGPIOIN     1 /* Amount of GPIO Input pins */
#define BOARD_NGPIOOUT    1 /* Amount of GPIO Output pins */
#define BOARD_NGPIOINT    1 /* Amount of GPIO Input w/ Interruption pins */

#define GPIO_IN1          (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTB | GPIO_PIN0)
#define GPIO_OUT1         (GPIO_OUTPUT | GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                           GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN1)
#define GPIO_INT1         (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTB | GPIO_PIN2)

/************************************************************************************
 * Public data
 ************************************************************************************/

#ifndef __ASSEMBLY__

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: stm32_spidev_initialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the Nucleo-144 board.
 *
 ************************************************************************************/

#if defined(CONFIG_SPI)
void stm32_spidev_initialize(void);
#endif

/************************************************************************************
 * Name: stm32_spidev_bus_test
 *
 * Description:
 *   Called to create the defined SPI buses and test them by initializing them
 *   and sending the NUCLEO_SPI_TEST (no chip select).
 *
 ************************************************************************************/

#if defined(CONFIG_NUCLEO_SPI_TEST)
int stm32_spidev_bus_test(void);
#endif

/************************************************************************************
 * Name: stm32_dma_alloc_init
 *
 * Description:
 *   Called to create a FAT DMA allocator
 *
 * Returned Value:
 *   0 on success or -ENOMEM
 *
 ************************************************************************************/

void stm32_dma_alloc_init(void);

#if defined (CONFIG_FAT_DMAMEMORY)
int stm32_dma_alloc_init(void);
#endif

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Called at application startup time to initialize the SCMMC functionality.
 *
 ****************************************************************************/

#ifdef CONFIG_MMCSD
int stm32_sdio_initialize(void);
#endif

/************************************************************************************
 * Name: stm32_usbinitialize
 *
 * Description:
 *   Called from stm32_usbinitialize very early in inialization to setup USB-related
 *   GPIO pins for the nucleo-144 board.
 *
 ************************************************************************************/

#ifdef CONFIG_STM32F7_OTGFS
void stm32_usbinitialize(void);
#endif

/************************************************************************************
 * Name: stm32_pwm_setup
 *
 * Description:
 *   Initialize PWM and register the PWM device.
 *
 ************************************************************************************/

#ifdef CONFIG_PWM
int stm32_pwm_setup(void);
#endif

/************************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ************************************************************************************/

#ifdef CONFIG_ADC
int stm32_adc_setup(void);
#endif

/************************************************************************************
 * Name: stm32_bbsram_int
 ************************************************************************************/

#ifdef CONFIG_STM32F7_BBSRAM
int stm32_bbsram_int(void);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __CONFIGS_NUCLEO_144_SRC_NUCLEO_144_H */
