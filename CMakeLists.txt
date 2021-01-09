include("${COMMON_TOOL_ROOT}/cmake/utility/embedded.cmake")

add_subdirectory(direct)
add_subdirectory(integration)

# ====================================================
# All Drivers
# ====================================================
set(all_libraries
  yaffs2_direct
  yaffs2_integration
)

# ====================================================
# This function generates the following targets:
#   yaffs_core
#   yaffs_core_rel
#   yaffs_core_rel_dbg
#   yaffs_core_dbg
#
# Each target builds the appropriate variant of the
# libraries listed above in "all_libraries".
# ====================================================
function(build_yaffs_core variant)

  # Add the variant flag to each library target
  set(all_libraries_tmp${variant} "")
  foreach(lib ${all_libraries})
    list(APPEND all_libraries_tmp${variant} ${lib}${variant})
  endforeach()

  # Create interface library for the appropriate variant
  set(YAFFS${variant} yaffs_core${variant})
  add_library("${YAFFS${variant}}" INTERFACE)
  target_link_libraries("${YAFFS${variant}}" INTERFACE "${all_libraries_tmp${variant}}")
  export(TARGETS "${YAFFS${variant}}" FILE "${PROJECT_BINARY_DIR}/Ripple/${YAFFS${variant}}.cmake")

endfunction()
add_target_variants(build_yaffs_core)


# ====================================================
# Various exports
# ====================================================
# Public include location
add_library(yaffs_inc INTERFACE)
target_include_directories(yaffs_inc INTERFACE ./../ ./include ./direct ./integration)
target_link_libraries(yaffs_inc INTERFACE ctre_inc)
export(TARGETS yaffs_inc FILE "${PROJECT_BINARY_DIR}/yaffs/yaffs_inc.cmake")
