include("${COMMON_TOOL_ROOT}/cmake/utility/embedded.cmake")

# ====================================================
# Common
# ====================================================
set(LINK_LIBS
  aurora_intf_inc        # Aurora public headers
  Boost::boost      # Boost public headers
  chimera_intf_inc       # Chimera public headers
  yaffs_inc         # Yaffs public headers
  project_inc
)

# ====================================================
# Interface Library
# ====================================================
function(build_library variant)
  set(DRIVER yaffs2_integration${variant})
  add_library(${DRIVER} STATIC
    hooks.cpp
    rtos/os_glue.cpp
    nor/device_driver.cpp
  )
  target_link_libraries(${DRIVER} PRIVATE ${LINK_LIBS} prj_device_target prj_build_target${variant})
  export(TARGETS ${DRIVER} FILE "${PROJECT_BINARY_DIR}/yaffs/${DRIVER}.cmake")
endfunction()

add_target_variants(build_library)
