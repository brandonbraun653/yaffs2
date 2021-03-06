/********************************************************************************
 *  File Name:
 *    hooks.cpp
 *
 *  Description:
 *    Hooks needed to implement Yaffs
 *
 *  2021 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* STL Includes */
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif
#include "SEGGER_SYSVIEW.h"
#include "yaffs_trace.h"

unsigned int yaffs_trace_mask = YAFFS_TRACE_ALWAYS;

void yaffs_bug_fn(const char *file_name, int line_no)
{
  ( void )file_name;
  ( void )line_no;

  SEGGER_SYSVIEW_PrintfTarget( "YAFFS bug in %s line %d\n", file_name, line_no );
}


#ifdef __cplusplus
}
#endif