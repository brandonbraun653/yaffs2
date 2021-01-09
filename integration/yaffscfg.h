/********************************************************************************
 *  File Name:
 *    yaffscfg.h
 *
 *  Description:
 *    Yaffs2 direct configuration settings. Controls the behavior of the entire
 *    filesystem. The file yaffs2/direct/yportenv.h must be modified to include
 *    this config file first, otherwise the drivers won't set themselves up.
 *
 *  2021 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

#pragma once
#ifndef YAFFS_CONFIG_HPP
#define YAFFS_CONFIG_HPP

#ifdef __cplusplus
extern "C"
{
#endif

#define CONFIG_YAFFS_DIRECT               1
#define CONFIG_YAFFS_SHORT_NAMES_IN_RAM   1
#define CONFIG_YAFFS_YAFFS2               1
#define CONFIG_YAFFS_PROVIDE_DEFS         1
#define CONFIG_YAFFSFS_PROVIDE_VALUES     1
#define CONFIG_YAFFS_DEFINES_TYPES        1
#define LOFF_T_32_BIT                     1
#define NO_Y_INLINE                       1
#define loff_t off_t

#define YAFFSFS_N_HANDLES   100   /**< Max number of file handles */
#define YAFFSFS_N_DSC       20    /**< Directory search contexts */


struct yaffsfs_DeviceConfiguration
{
  const char *prefix;
  struct yaffs_dev *dev;
};


#ifdef __cplusplus
}
#endif

#endif  /* !YAFFS_CONFIG_HPP */
