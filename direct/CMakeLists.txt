include("${COMMON_TOOL_ROOT}/cmake/utility/embedded.cmake")

# ====================================================
# Common
# ====================================================
set(LINK_LIBS
  yaffs_inc
)

# ====================================================
# Interface Library
# ====================================================
function(build_library variant)
  set(DRIVER yaffs2_direct${variant})
  add_library(${DRIVER} STATIC
    optional_sort/qsort.c
    yaffs_allocator.c
    yaffs_attribs.c
    yaffs_bitmap.c
    yaffs_checkptrw.c
    yaffs_ecc.c
    yaffs_endian.c
    yaffs_error.c
    yaffs_guts.c
    yaffs_hweight.c
    yaffs_nameval.c
    yaffs_nand.c
    yaffs_packedtags1.c
    yaffs_packedtags2.c
    yaffs_summary.c
    yaffs_tagscompat.c
    yaffs_tagsmarshall.c
    yaffs_verify.c
    yaffs_yaffs1.c
    yaffs_yaffs2.c
    yaffsfs.c
  )
  target_link_libraries(${DRIVER} PRIVATE ${LINK_LIBS} prj_device_target prj_build_target${variant})
  export(TARGETS ${DRIVER} FILE "${PROJECT_BINARY_DIR}/yaffs/${DRIVER}.cmake")
endfunction()

add_target_variants(build_library)
