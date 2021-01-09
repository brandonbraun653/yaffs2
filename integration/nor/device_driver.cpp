/********************************************************************************
 *  File Name:
 *    device_driver.cpp
 *
 *  Description:
 *    NOR flash device driver implementation
 *
 *  2021 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* Aurora Includes */
#include <Aurora/memory>

/* Chimera Includes */
#include <Chimera/assert>

/* Yaffs Includes */
#include "yaffs2_config_prj.hpp"
#include <yaffs2/integration/nor/device_driver.hpp>

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
  Constants
  -------------------------------------------------------------------------------*/
  static constexpr uint32_t FORMAT_VALUE          = 0x1234;
  static constexpr uint32_t SPARE_BYTES_PER_CHUNK = 16;

  /*-------------------------------------------------------------------------------
  Static Data
  -------------------------------------------------------------------------------*/
  /**
   *  NOR flash driver instance. Usually there is only a single Flash device
   *  per system, so this static declaration is likely ok. If more than one
   *  is needed in the future, expand this into an array indexed by the yaffs_dev
   *  object pointer.
   */
  static Aurora::Flash::NOR::Driver sFlashDriver;
  static yaffs_dev sYaffsDevice;

  static uint32_t FormatOffset    = 0;
  static uint32_t BytesPerChunk   = 0;
  static uint32_t BytesPerBlock   = 0;
  static uint32_t ChunksPerBlock  = 0;
  static uint32_t BlocksInDevice  = 0;
  static uint32_t SpareAreaOffset = 0;

#define YNOR_PREMARKER ( 0xF6 )
#define YNOR_POSTMARKER ( 0xF0 )

  /*-------------------------------------------------------------------------------
  Static Functions
  -------------------------------------------------------------------------------*/
  static size_t Block2FormatOffset( int blockNumber )
  {
    return 0;    // is this the right offset?????
  }


  static size_t Block2FormatChunk( int blockNumber )
  {
    // This might need to be +1
    return ( blockNumber * BytesPerBlock ) / BytesPerChunk;
  }


  /*-------------------------------------------------------------------------------
  Public Functions
  -------------------------------------------------------------------------------*/
  yaffs_dev *install_driver( const char *name )
  {
    using namespace Aurora::Flash;

    /*-------------------------------------------------
    Reset
    -------------------------------------------------*/
    memset( &sYaffsDevice, 0, sizeof( sYaffsDevice ) );

    /*-------------------------------------------------
    Figure out a few device properties
    -------------------------------------------------*/
    const auto props = Aurora::Flash::NOR::getProperties( YAFFS_DEVICE );

    switch ( props->writeChunk )
    {
      case Aurora::Memory::Chunk::PAGE:
        BytesPerChunk = props->pageSize;
        break;

      case Aurora::Memory::Chunk::BLOCK:
        BytesPerChunk = props->blockSize;
        break;

      case Aurora::Memory::Chunk::SECTOR:
        BytesPerChunk = props->sectorSize;
        break;

      default:
        return nullptr;
        break;
    };

    switch ( props->eraseChunk )
    {
      case Aurora::Memory::Chunk::PAGE:
        BytesPerBlock = props->pageSize;
        break;

      case Aurora::Memory::Chunk::BLOCK:
        BytesPerBlock = props->blockSize;
        break;

      case Aurora::Memory::Chunk::SECTOR:
        BytesPerBlock = props->sectorSize;
        break;

      default:
        return nullptr;
        break;
    };

    size_t address_range = ( props->endAddress - props->startAddress );

    RT_HARD_ASSERT( BytesPerChunk != 0 );
    RT_HARD_ASSERT( BytesPerBlock != 0 );
    RT_HARD_ASSERT( BytesPerBlock >= BytesPerChunk );
    RT_HARD_ASSERT( address_range >= BytesPerBlock );

    ChunksPerBlock  = BytesPerBlock / BytesPerChunk;
    BlocksInDevice  = address_range / BytesPerBlock;
    SpareAreaOffset = BytesPerChunk - SPARE_BYTES_PER_CHUNK;

    /*-------------------------------------------------
    Configure device properties
    -------------------------------------------------*/
    sYaffsDevice.param.name                  = name;
    sYaffsDevice.param.inband_tags           = 1;
    sYaffsDevice.param.total_bytes_per_chunk = BytesPerChunk;
    sYaffsDevice.param.spare_bytes_per_chunk = SPARE_BYTES_PER_CHUNK;
    sYaffsDevice.param.chunks_per_block      = ChunksPerBlock;
    sYaffsDevice.param.n_reserved_blocks     = 2;
    sYaffsDevice.param.start_block           = 0;                     // Can use block 0
    sYaffsDevice.param.end_block             = BlocksInDevice - 1;    // Last block
    sYaffsDevice.param.use_nand_ecc          = 0;                     // use YAFFS's ECC
    sYaffsDevice.param.disable_soft_del      = 1;
    sYaffsDevice.param.n_caches              = 10;
    sYaffsDevice.param.is_yaffs2             = 0;    // Example is in yaffs 1 mode. Get that working first.

    /*-------------------------------------------------
    Register interface functions
    -------------------------------------------------*/
    sYaffsDevice.drv.drv_write_chunk_fn  = WriteChunk;
    sYaffsDevice.drv.drv_read_chunk_fn   = ReadChunk;
    sYaffsDevice.drv.drv_erase_fn        = EraseBlock;
    sYaffsDevice.drv.drv_initialise_fn   = Initialise;
    sYaffsDevice.drv.drv_deinitialise_fn = Deinitialise;

    /*-------------------------------------------------
    Assign user context if needed (multiple devices)
    -------------------------------------------------*/
    sYaffsDevice.driver_context = nullptr;

    /*-------------------------------------------------
    Register the device
    -------------------------------------------------*/
    yaffs_add_device( &sYaffsDevice );
    return &sYaffsDevice;
  }


  void AndBytes( u8 *target, const u8 *src, int nbytes )
  {
    while ( nbytes > 0 )
    {
      *target &= *src;
      target++;
      src++;
      nbytes--;
    }
  }


  int WriteChunk( yaffs_dev *dev, int nand_chunk, const u8 *data, int data_len, const u8 *oob, int oob_len )
  {
    yaffs_spare *spare = ( yaffs_spare * )oob;
    yaffs_spare tmpSpare;

    if ( data && oob )
    {
      if ( spare->page_status != 0xff )
      {
        Chimera::insert_debug_breakpoint();
      }

      /* Write a pre-marker */
      memset( &tmpSpare, 0xff, sizeof( tmpSpare ) );
      tmpSpare.page_status = YNOR_PREMARKER;
      sFlashDriver.write( nand_chunk, SpareAreaOffset, &tmpSpare, sizeof( tmpSpare ) );

      /* Write the data */
      sFlashDriver.write( nand_chunk, 0, data, data_len );

      /* Write the real tags, but override the premarker */
      memcpy( &tmpSpare, spare, sizeof( tmpSpare ) );
      tmpSpare.page_status = YNOR_PREMARKER;
      sFlashDriver.write( nand_chunk, SpareAreaOffset, &tmpSpare, sizeof( tmpSpare ) );

      /* Write the postmarker */
      tmpSpare.page_status = YNOR_POSTMARKER;
      sFlashDriver.write( nand_chunk, SpareAreaOffset, &tmpSpare, sizeof( tmpSpare ) );
    }
    else if ( spare )
    {
      /* This has to be RMW to handle NOR-ness */
      sFlashDriver.read( nand_chunk, SpareAreaOffset, &tmpSpare, sizeof( tmpSpare ) );

      AndBytes( ( u8 * )&tmpSpare, ( u8 * )spare, sizeof( tmpSpare ) );

      sFlashDriver.write( nand_chunk, SpareAreaOffset, &tmpSpare, sizeof( tmpSpare ) );
    }
    else
    {
      Chimera::insert_debug_breakpoint();
    }
  }


  int ReadChunk( yaffs_dev *dev, int nand_chunk, u8 *data, int data_len, u8 *oob, int oob_len, yaffs_ecc_result *ecc_result )
  {
    yaffs_spare *spare = ( yaffs_spare * )oob;
    static_assert( sizeof( yaffs_spare ) == SPARE_BYTES_PER_CHUNK );

    if ( data )
    {
      sFlashDriver.read( nand_chunk, 0, data, dev->param.total_bytes_per_chunk );
    }

    if ( oob )
    {
      sFlashDriver.read( nand_chunk, SpareAreaOffset, spare, oob_len );

      if ( spare->page_status == YNOR_POSTMARKER )
      {
        spare->page_status = 0xff;
      }
      else if ( spare->page_status != 0xff && ( spare->page_status | YNOR_PREMARKER ) != 0xff )
      {
        spare->page_status = YNOR_PREMARKER;
      }
    }

    if ( ecc_result )
    {
      *ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
    }

    return YAFFS_OK;
  }


  int FormatBlock( yaffs_dev *dev, int blockNumber )
  {
    Chimera::insert_debug_breakpoint();

    size_t chunkNumber = Block2FormatChunk( blockNumber );
    size_t offset      = Block2FormatOffset( blockNumber );

    sFlashDriver.erase( blockNumber );
    sFlashDriver.write( chunkNumber, offset, &FORMAT_VALUE, sizeof( FORMAT_VALUE ) );

    return YAFFS_OK;
  }


  int UnformatBlock( yaffs_dev *dev, int blockNumber )
  {
    Chimera::insert_debug_breakpoint();
    size_t chunkNumber = Block2FormatChunk( blockNumber );
    size_t offset      = Block2FormatOffset( blockNumber );

    u32 formatVal = 0;
    sFlashDriver.write( chunkNumber, offset, &formatVal, sizeof( formatVal ) );

    return YAFFS_OK;
  }


  int IsBlockFormatted( yaffs_dev *dev, int blockNumber )
  {
    Chimera::insert_debug_breakpoint();
    size_t chunkNumber = Block2FormatChunk( blockNumber );
    size_t offset      = Block2FormatOffset( blockNumber );

    u32 formatVal = 0;

    sFlashDriver.read( chunkNumber, offset, &formatVal, sizeof( formatVal ) );
    return ( formatVal == FORMAT_VALUE );
  }


  int EraseBlock( yaffs_dev *dev, int blockNumber )
  {
    if ( blockNumber < 0 || blockNumber >= BlocksInDevice )
    {
      return YAFFS_FAIL;
    }
    else
    {
      UnformatBlock( dev, blockNumber );
      FormatBlock( dev, blockNumber );
    }
  }


  int Initialise( yaffs_dev *dev )
  {
    for ( auto i = dev->param.start_block; i <= dev->param.end_block; i++ )
    {
      if ( !IsBlockFormatted( dev, i ) )
      {
        FormatBlock( dev, i );
      }
    }

    return YAFFS_OK;
  }


  int Deinitialise( yaffs_dev *dev )
  {
    return YAFFS_OK;
  }

}    // namespace Yaffs::NOR
