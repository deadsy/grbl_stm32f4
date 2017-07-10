#!/usr/bin/python
# copy selcted sdk files from the unzipped sdk into the grbl_stm32f4 source

import os
import filecmp
import shutil

home_dir = '/home/jasonh/work/jasonh/'

sdk_dir = home_dir + 'hw_docs/st/cube/STM32Cube_FW_F4_V1.16.0/'
grbl_dir = home_dir + 'grbl_stm32f4/board/st/stm32f4/'

hal = sdk_dir + 'Drivers/STM32F4xx_HAL_Driver/'
hal_src = hal + 'Src/'
hal_inc = hal + 'Inc/'

usbd_core = sdk_dir + 'Middlewares/ST/STM32_USB_Device_Library/Core/'
usbd_core_inc = usbd_core + 'Inc/'
usbd_core_src = usbd_core + 'Src/'

usbd_cdc = sdk_dir + 'Middlewares/ST/STM32_USB_Device_Library/Class/CDC/'
usbd_cdc_inc = usbd_cdc + 'Inc/'
usbd_cdc_src = usbd_cdc + 'Src/'

cmsis = sdk_dir + 'Drivers/CMSIS/'
cmsis_inc = cmsis + 'Include/'
cmsis_dev = cmsis + 'Device/ST/STM32F4xx/Include/'

sdk_files = (
  # HAL include files
  (hal_inc + 'Legacy/stm32_hal_legacy.h', 'hal/inc/Legacy'),
  (hal_inc + 'stm32f4xx_hal_flash_ramfunc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_adc_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_adc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_can.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_conf_template.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_cortex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_crc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_cryp_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_cryp.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_dac_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_dac.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_dcmi.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_def.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_dma2d.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_dma_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_dma.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_eth.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_flash_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_flash.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_gpio_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_gpio.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_hash_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_hash.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_hcd.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_i2c_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_i2c.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_i2s_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_i2s.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_irda.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_iwdg.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_ltdc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_nand.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_nor.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_pccard.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_pcd.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_pcd_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_pwr_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_pwr.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_rcc_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_rcc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_rng.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_rtc_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_rtc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_sai.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_sd.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_sdram.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_smartcard.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_spi.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_sram.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_tim_ex.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_tim.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_uart.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_usart.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_hal_wwdg.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_ll_fmc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_ll_fsmc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_ll_sdmmc.h', 'hal/inc'),
  (hal_inc + 'stm32f4xx_ll_usb.h', 'hal/inc'),


  # HAL source files
  (hal_src + 'stm32f4xx_hal_adc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_adc_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_can.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_cortex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_crc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_cryp.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_cryp_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_dac.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_dac_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_dcmi.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_dma2d.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_dma.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_dma_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_eth.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_flash.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_flash_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_gpio.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_hash.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_hash_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_hcd.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_i2c.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_i2c_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_i2s.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_i2s_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_irda.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_iwdg.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_ltdc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_msp_template.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_nand.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_nor.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_pccard.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_pcd.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_pcd_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_pwr.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_pwr_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_rcc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_rcc_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_rng.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_rtc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_rtc_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_sai.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_sd.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_sdram.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_smartcard.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_spi.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_sram.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_tim.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_tim_ex.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_uart.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_usart.c', 'hal/src'),
  (hal_src + 'stm32f4xx_hal_wwdg.c', 'hal/src'),
  (hal_src + 'stm32f4xx_ll_fmc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_ll_fsmc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_ll_sdmmc.c', 'hal/src'),
  (hal_src + 'stm32f4xx_ll_usb.c', 'hal/src'),
  # USB Device core
  (usbd_core_src + 'usbd_core.c', 'usb/core'),
  (usbd_core_src + 'usbd_ctlreq.c', 'usb/core'),
  (usbd_core_src + 'usbd_ioreq.c', 'usb/core'),
  (usbd_core_inc + 'usbd_core.h', 'usb/core'),
  (usbd_core_inc + 'usbd_ctlreq.h', 'usb/core'),
  (usbd_core_inc + 'usbd_def.h', 'usb/core'),
  (usbd_core_inc + 'usbd_ioreq.h', 'usb/core'),
  # USB Device CDC
  (usbd_cdc_src + 'usbd_cdc.c', 'usb/cdc'),
  (usbd_cdc_inc + 'usbd_cdc.h', 'usb/cdc'),
  # CMSIS
  (cmsis_inc + 'core_cm4.h', 'cmsis'),
  (cmsis_inc + 'cmsis_gcc.h', 'cmsis'),
  (cmsis_inc + 'core_cmSimd.h', 'cmsis'),
  (cmsis_inc + 'core_cmFunc.h', 'cmsis'),
  (cmsis_inc + 'core_cmInstr.h', 'cmsis'),
  # CMSIS Device
  (cmsis_dev + 'stm32f407xx.h', 'cmsis'),
  (cmsis_dev + 'stm32f4xx.h', 'cmsis'),
  (cmsis_dev + 'system_stm32f4xx.h', 'cmsis'),
)

def main():
  for (src, dst) in sdk_files:

    dst = grbl_dir + dst + '/' + src.split('/')[-1]

    #if not os.path.exists(src):
    #  print('src %s does not exist' % src)
    #  continue

    #if not os.path.exists(dst):
    #  print('dst %s does not exist' % dst)
    #  continue

    #if not filecmp.cmp(src, dst, shallow = False):
    #  print('%s and %s do not match' % (src, dst))

    #print src, dst

    shutil.copyfile(src, dst)

main()
