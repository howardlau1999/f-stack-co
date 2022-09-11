find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(dpdk QUIET libdpdk)
endif()

if(dpdk_INCLUDE_DIRS)
  # good
elseif(TARGET dpdk::dpdk)
  get_target_property(dpdk_INCLUDE_DIRS
     dpdk::dpdk INTERFACE_INCLUDE_DIRECTORIES)
else()
  find_path(dpdk_config_INCLUDE_DIR rte_config.h
    HINTS
      ENV DPDK_DIR
    PATH_SUFFIXES
      dpdk
      include)
  find_path(dpdk_common_INCLUDE_DIR rte_common.h
    HINTS
      ENV DPDK_DIR
    PATH_SUFFIXES
      dpdk
      include)
  set(dpdk_INCLUDE_DIRS "${dpdk_config_INCLUDE_DIR}")
  if(dpdk_common_INCLUDE_DIR AND NOT dpdk_config_INCLUDE_DIR STREQUAL dpdk_common_INCLUDE_DIR)
    list(APPEND dpdk_INCLUDE_DIRS "${dpdk_common_INCLUDE_DIR}")
  endif()
endif()

set(components
  net_bond
  cfgfile
  cmdline
  ethdev
  mbuf
  eal
  bus_pci
  hash
  kvargs
  mempool
  mempool_ring
  net
  pci
  net_af_packet
  net_bnxt
  net_bond
  net_cxgbe
  net_e1000
  net_ena
  net_enic
  net_i40e
  net_mlx5
  common_mlx5
  net_nfp
  net_qede
  net_ring
  net_sfc_efx
  net_vmxnet3_uio
  net_hns3
  net_hinic
  bus_vdev
  bus_auxiliary
  ring
  timer
  kni  
  rcu
  telemetry
)

# for collecting dpdk library targets, it will be used when defining dpdk::dpdk
set(_dpdk_libs)
# for list of dpdk library archive paths
set(dpdk_LIBRARIES "")
foreach(c ${components})
  set(dpdk_lib dpdk::${c})
  if(TARGET ${dpdk_lib})
    get_target_property(DPDK_rte_${c}_LIBRARY
      ${dpdk_lib} IMPORTED_LOCATION)
  else()
    find_library(DPDK_rte_${c}_LIBRARY rte_${c}
      HINTS
        ENV DPDK_DIR
        ${dpdk_LIBRARY_DIRS}
        PATH_SUFFIXES lib)
  endif()
  if(DPDK_rte_${c}_LIBRARY)
    if (NOT TARGET ${dpdk_lib})
      add_library(${dpdk_lib} UNKNOWN IMPORTED)
      set_target_properties(${dpdk_lib} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${dpdk_INCLUDE_DIRS}"
        IMPORTED_LOCATION "${DPDK_rte_${c}_LIBRARY}")
    endif()
    list(APPEND _dpdk_libs ${dpdk_lib})
    list(APPEND dpdk_LIBRARIES ${DPDK_rte_${c}_LIBRARY})
  endif()
endforeach()

mark_as_advanced(dpdk_INCLUDE_DIRS ${dpdk_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(dpdk DEFAULT_MSG
  dpdk_INCLUDE_DIRS
  dpdk_LIBRARIES)

if(dpdk_FOUND)
  if(NOT TARGET dpdk::cflags)
     if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64|x86_64|AMD64")
      set(rte_cflags "-march=core2")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM")
      set(rte_cflags "-march=armv7-a")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|AARCH64")
      set(rte_cflags "-march=armv8-a+crc")
    endif()
    add_library(dpdk::cflags INTERFACE IMPORTED)
    if (rte_cflags)
      set_target_properties(dpdk::cflags PROPERTIES
        INTERFACE_COMPILE_OPTIONS "${rte_cflags}")
    endif()
  endif()

  if(NOT TARGET dpdk::dpdk)
    add_library(dpdk::dpdk INTERFACE IMPORTED)
    find_package(Threads QUIET)
    list(APPEND _dpdk_libs
      Threads::Threads
      dpdk::cflags
      numa)
    set_target_properties(dpdk::dpdk PROPERTIES
      INTERFACE_LINK_LIBRARIES "${_dpdk_libs}"
      INTERFACE_INCLUDE_DIRECTORIES "${dpdk_INCLUDE_DIRS}")
  endif()
endif()

unset(_dpdk_libs)
