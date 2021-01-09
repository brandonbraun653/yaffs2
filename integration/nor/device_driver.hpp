/********************************************************************************
 *  File Name:
 *    device_driver.hpp
 *
 *  Description:
 *    NOR flash device driver interface for Yaffs. The interface was taken from
 *    the example yaffs_nor_drv.c implementation.
 *
 *  2021 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

#pragma once
#ifndef YAFFS_NOR_FLASH_DEVICE_DRIVER_HPP
#define YAFFS_NOR_FLASH_DEVICE_DRIVER_HPP

/* Chimer Includes */
#include <Chimera/common>

/* Yaffs Includes */
#ifdef __cplusplus
extern "C"
{
#endif

#include "yportenv.h"
#include "yaffs_guts.h"

#ifdef __cplusplus
}
#endif

namespace Yaffs::NOR
{
  /*-------------------------------------------------------------------------------
  Public Functions
  -------------------------------------------------------------------------------*/
  yaffs_dev *install_driver( const char *name );
  u32 Block2Addr( yaffs_dev *dev, int blockNumber );
  u32 Block2FormatAddr( yaffs_dev *dev, int blockNumber );
  u32 Chunk2DataAddr( yaffs_dev *dev, int chunk_id );
  u32 Chunk2SpareAddr( yaffs_dev *dev, int chunk_id );
  void AndBytes( u8 *target, const u8 *src, int nbytes );
  int WriteChunk( yaffs_dev *dev, int nand_chunk, const u8 *data, int data_len, const u8 *oob, int oob_len );
  int ReadChunk( yaffs_dev *dev, int nand_chunk, u8 *data, int data_len, u8 *oob, int oob_len, yaffs_ecc_result *ecc_result );
  int FormatBlock( yaffs_dev *dev, int blockNumber );
  int UnformatBlock( yaffs_dev *dev, int blockNumber );
  int IsBlockFormatted( yaffs_dev *dev, int blockNumber );
  int EraseBlock( yaffs_dev *dev, int blockNumber );
  int Initialise( yaffs_dev *dev );
  int Deinitialise( yaffs_dev *dev );

}    // namespace Yaffs::NOR

#endif /* !YAFFS_NOR_FLASH_DEVICE_DRIVER_HPP */
