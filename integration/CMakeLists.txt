include("${COMMON_TOOL_ROOT}/cmake/utility/embedded.cmake")


gen_static_lib_variants(
  TARGET
    yaffs2_integration
  SOURCES
    hooks.cpp
    os_glue.cpp
  PRV_LIBRARIES
    aurora_intf_inc
    chimera_intf_inc
    project_inc
    yaffs_intf_inc
  EXPORT_DIR
    "${PROJECT_BINARY_DIR}/Yaffs"
)
