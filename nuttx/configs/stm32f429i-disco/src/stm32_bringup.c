/****************************************************************************
 * config/stm32f429i-disco/src/stm32_bringup.c
 *
 *   Copyright (C) 2012, 2015-2018 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/mount.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <errno.h>

#include <nuttx/board.h>
#include <nuttx/kmalloc.h>

#ifdef CONFIG_STM32_SPI4
#  include <nuttx/mmcsd.h>
#endif

#ifdef CONFIG_MTD_SST25XX
#  include <nuttx/mtd/mtd.h>
#endif

#ifdef CONFIG_VIDEO_FB
#  include <nuttx/video/fb.h>
#endif

#ifdef CONFIG_USBMONITOR
#  include <nuttx/usb/usbmonitor.h>
#endif

#ifndef CONFIG_STM32F429I_DISCO_FLASH_MINOR
#define CONFIG_STM32F429I_DISCO_FLASH_MINOR 0
#endif

#ifdef CONFIG_STM32F429I_DISCO_FLASH_CONFIG_PART
#ifdef CONFIG_PLATFORM_CONFIGDATA
#  include <nuttx/mtd/configdata.h>
#endif
#endif

#ifdef CONFIG_STM32_OTGHS
#  include "stm32_usbhost.h"
#endif

#include "stm32.h"
#include "stm32f429i-disco.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y && CONFIG_LIB_BOARDCTL=y :
 *     Called from the NSH library
 *
 ****************************************************************************/

int stm32_bringup(void)
{

  stm32_gpio_initialize();

#if defined(CONFIG_STM32_SPI4)
  FAR struct spi_dev_s *spi;
#endif
#if defined(CONFIG_MTD)
  FAR struct mtd_dev_s *mtd;
  FAR struct mtd_geometry_s geo;
#endif
#if defined(CONFIG_MTD_PARTITION_NAMES)
  FAR const char *partname = CONFIG_STM32F429I_DISCO_FLASH_PART_NAMES;
#endif
  int ret;

#ifdef HAVE_PROC
  /* mount the proc filesystem */

  ret = mount(NULL, CONFIG_NSH_PROC_MOUNTPOINT, "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR,
             "ERROR: Failed to mount the PROC filesystem: %d (%d)\n",
             ret, errno);
      return ret;
    }
#endif

  /* Configure SPI-based devices */

#ifdef CONFIG_STM32_SPI4
  /* Get the SPI port */

  syslog(LOG_INFO, "Initializing SPI port 4\n");

  spi = stm32_spibus_initialize(4);
  if (!spi)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize SPI port 4\n");
      return -ENODEV;
    }

  syslog(LOG_INFO, "Successfully initialized SPI port 4\n");

  /* Now bind the SPI interface to the SST25F064 SPI FLASH driver.  This
   * is a FLASH device that has been added external to the board (i.e.
   * the board does not ship from STM with any on-board FLASH.
   */

#if defined(CONFIG_MTD) && defined(CONFIG_MTD_SST25XX)
  syslog(LOG_INFO, "Bind SPI to the SPI flash driver\n");

  mtd = sst25xx_initialize(spi);
  if (!mtd)
    {
      syslog(LOG_ERR, "ERROR: Failed to bind SPI port 4 to the SPI FLASH driver\n");
    }
  else
    {
      syslog(LOG_INFO, "Successfully bound SPI port 4 to the SPI FLASH driver\n");

      /* Get the geometry of the FLASH device */

      ret = mtd->ioctl(mtd, MTDIOC_GEOMETRY, (unsigned long)((uintptr_t)&geo));
      if (ret < 0)
        {
          ferr("ERROR: mtd->ioctl failed: %d\n", ret);
          return ret;
        }

#ifdef CONFIG_STM32F429I_DISCO_FLASH_PART
      {
        int partno;
        int partsize;
        int partoffset;
        int partszbytes;
        int erasesize;
        const char *partstring = CONFIG_STM32F429I_DISCO_FLASH_PART_LIST;
        const char *ptr;
        FAR struct mtd_dev_s *mtd_part;
        char  partref[4];

        /* Now create a partition on the FLASH device */

        partno = 0;
        ptr = partstring;
        partoffset = 0;

        /* Get the Flash erase size */

        erasesize = geo.erasesize;

        while (*ptr != '\0')
          {
            /* Get the partition size */

            partsize = atoi(ptr);
            partszbytes = (partsize << 10); /* partsize is defined in KB */

            /* Check if partition size is bigger then erase block */

            if (partszbytes < erasesize)
              {
                ferr("ERROR: Partition size is lesser than erasesize!\n");
                return -1;
              }

            /* Check if partition size is multiple of erase block */

            if ((partszbytes % erasesize) != 0)
              {
                ferr("ERROR: Partition size is not multiple of erasesize!\n");
                return -1;
              }

            mtd_part    = mtd_partition(mtd, partoffset, partszbytes / erasesize);
            partoffset += partszbytes / erasesize;

#ifdef CONFIG_STM32F429I_DISCO_FLASH_CONFIG_PART
            /* Test if this is the config partition */

            if (CONFIG_STM32F429I_DISCO_FLASH_CONFIG_PART_NUMBER == partno)
              {
                /* Register the partition as the config device */

                mtdconfig_register(mtd_part);
              }
            else
#endif
              {
                /* Now initialize a SMART Flash block device and bind it
                 * to the MTD device.
                 */

#if defined(CONFIG_MTD_SMART) && defined(CONFIG_FS_SMARTFS)
                sprintf(partref, "p%d", partno);
                smart_initialize(CONFIG_STM32F429I_DISCO_FLASH_MINOR, mtd_part, partref);
#endif
              }

#if defined(CONFIG_MTD_PARTITION_NAMES)
            /* Set the partition name */

            if (mtd_part == NULL)
              {
                ferr("ERROR: failed to create partition %s\n", partname);
                return -1;
              }

            mtd_setpartitionname(mtd_part, partname);

            /* Now skip to next name.  We don't need to split the string here
             * because the MTD partition logic will only display names up to
             * the comma, thus allowing us to use a single static name
             * in the code.
             */

            while (*partname != ',' && *partname != '\0')
              {
                /* Skip to next ',' */

                partname++;
              }

            if (*partname == ',')
              {
                partname++;
              }
#endif

            /* Update the pointer to point to the next size in the list */

            while ((*ptr >= '0') && (*ptr <= '9'))
              {
                ptr++;
              }

            if (*ptr == ',')
              {
                ptr++;
              }

            /* Increment the part number */

            partno++;
          }
      }
#else /* CONFIG_STM32F429I_DISCO_FLASH_PART */

      /* Configure the device with no partition support */

      smart_initialize(CONFIG_STM32F429I_DISCO_FLASH_MINOR, mtd, NULL);

#endif /* CONFIG_STM32F429I_DISCO_FLASH_PART */
    }

#endif /* CONFIG_MTD */
#endif /* CONFIG_STM32_SPI4 */

#ifdef CONFIG_VIDEO_FB
  /* Initialize and register the framebuffer driver */

  ret = fb_register(0, 0);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: fb_register() failed: %d\n", ret);
    }
#endif

#if defined(CONFIG_RAMMTD) && defined(CONFIG_STM32F429I_DISCO_RAMMTD)
  /* Create a RAM MTD device if configured */

  {
    uint8_t *start = (uint8_t *) kmm_malloc(CONFIG_STM32F429I_DISCO_RAMMTD_SIZE * 1024);
    mtd = rammtd_initialize(start, CONFIG_STM32F429I_DISCO_RAMMTD_SIZE * 1024);
    mtd->ioctl(mtd, MTDIOC_BULKERASE, 0);

    /* Now initialize a SMART Flash block device and bind it to the MTD device */

#if defined(CONFIG_MTD_SMART) && defined(CONFIG_FS_SMARTFS)
    smart_initialize(CONFIG_STM32F429I_DISCO_RAMMTD_MINOR, mtd, NULL);
#endif
  }

#endif /* CONFIG_RAMMTD && CONFIG_STM32F429I_DISCO_RAMMTD */

#ifdef HAVE_USBHOST
  /* Initialize USB host operation.  stm32_usbhost_initialize() starts a thread
   * will monitor for USB connection and disconnection events.
   */

  ret = stm32_usbhost_initialize();
  if (ret != OK)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize USB host: %d\n", ret);
      return ret;
    }
#endif

#ifdef HAVE_USBMONITOR
  /* Start the USB Monitor */

  ret = usbmonitor_start();
  if (ret != OK)
    {
      syslog(LOG_ERR, "ERROR: Failed to start USB monitor: %d\n", ret);
    }
#endif

#ifdef CONFIG_INPUT_STMPE811
  /* Initialize the touchscreen */

  ret = stm32_tsc_setup(0);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_tsc_setup failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_L3GD20
  ret = stm32_l3gd20initialize("/dev/gyr0");
  if (ret != OK)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize l3gd20 sensor: %d\n", ret);
    }
#endif


#ifdef CONFIG_PWM
  /* Initialize PWM and register the PWM device. */

  ret = stm32_pwm_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_pwm_setup() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_ADC
  /* Initialize ADC and register the ADC device. */

  ret = stm32_adc_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_adc_setup() failed: %d\n", ret);
    }
#endif

  UNUSED(ret);
  return OK;
}
