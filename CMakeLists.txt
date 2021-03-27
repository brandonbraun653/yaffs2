include("${COMMON_TOOL_ROOT}/cmake/utility/embedded.cmake")

add_subdirectory(direct)
add_subdirectory(integration)


gen_intf_lib(
  TARGET
    yaffs_intf_inc
  INTF_INCLUDES
    ./../
    ./include
    ./direct
    ./integration
  INTF_LIBRARIES
    segger_sys_view_intf
  EXPORT_DIR
    "${PROJECT_BINARY_DIR}/Yaffs"
)

gen_intf_lib_variants(
  TARGET
    yaffs_core
  LIBRARIES
    yaffs2_direct
    yaffs2_integration
  EXPORT_DIR
    "${PROJECT_BINARY_DIR}/Yaffs"
)
