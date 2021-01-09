/********************************************************************************
 *  File Name:
 *    os_glue.cpp
 *
 *  Description:
 *    Implements the RTOS aware hooks for YAFFS2
 *
 *  2021 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* STL Includes */
#include <string_view>

/* Chimera Includes */
#include <Chimera/assert>
#include <Chimera/allocator>
#include <Chimera/common>
#include <Chimera/thread>

/* Yaffs Includes */
#ifdef __cplusplus
extern "C"
{
#endif

#include "yaffs_guts.h"
#include "yaffs_osglue.h"
#include "yaffs_trace.h"
#include "yaffscfg.h"
#include "yaffsfs.h"

#ifdef __cplusplus
}
#endif

/*-------------------------------------------------------------------------------
Static Constants
-------------------------------------------------------------------------------*/
static constexpr size_t THREAD_STACK = STACK_BYTES( 512 );
static constexpr std::string_view THREAD_NAME = "yaffs_gc";

/*-------------------------------------------------------------------------------
Static Data
-------------------------------------------------------------------------------*/
static int yaffsfs_lastError;
static Chimera::Threading::RecursiveMutex yaffs_mutex;


/*-------------------------------------------------------------------------------
Static Functions
-------------------------------------------------------------------------------*/
/**
 *  Background thread that performs garbage collection on the file system.
 *  This is pretty low priority and shouldn't be consuming a large amount
 *  of resources.
 */
static void background_garbage_collect( void *arg )
{
  yaffs_dev *dev;
  int urgent = 0;
  int result;
  int next_urgent;

  /* Sleep for a bit to allow start up */
  Chimera::delayMilliseconds( 2000 );

  while ( 1 )
  {
    /* Iterate through devices, do bg gc updating ungency */
    yaffs_dev_rewind();
    next_urgent = 0;

    while ( ( dev = yaffs_next_dev() ) != NULL )
    {
      result = yaffs_do_background_gc_reldev( dev, urgent );
      if ( result > 0 )
      {
        next_urgent = 1;
      }
    }

    urgent = next_urgent;

    if ( next_urgent )
    {
      Chimera::delayMilliseconds( 1 );
    }
    else
    {
      Chimera::delayMilliseconds( 5 );
    }
  }
}

/*-------------------------------------------------------------------------------
Public Functions
-------------------------------------------------------------------------------*/
void yaffsfs_OSInitialisation( void )
{
  using namespace Chimera::Threading;

  Thread garbageCollector;
  garbageCollector.initialize( background_garbage_collect, nullptr, Priority::LEVEL_1, THREAD_STACK, THREAD_NAME.cbegin() );
  ThreadId id = garbageCollector.start();

  RT_HARD_ASSERT( id != THREAD_ID_INVALID );
}


u32 yaffsfs_CurrentTime( void )
{
  return static_cast<u32>( Chimera::millis() );
}


void yaffsfs_SetError( int err )
{
  yaffsfs_lastError = err;
}


int yaffsfs_GetLastError( void )
{
  return yaffsfs_lastError;
}


void yaffsfs_Lock( void )
{
  yaffs_mutex.lock();
}


void yaffsfs_Unlock( void )
{
  yaffs_mutex.unlock();
}


void *yaffsfs_malloc( size_t size )
{
  return Chimera::malloc( size );
}


void yaffsfs_free( void *ptr )
{
  Chimera::free( ptr );
}


void yaffsfs_get_malloc_values( unsigned *current, unsigned *high_water )
{
  ( void )current;
  ( void )high_water;
}


int yaffsfs_CheckMemRegion( const void *addr, size_t size, int write_request )
{
  ( void )size;
  ( void )write_request;

  return !addr ? -1 : 0;
}
