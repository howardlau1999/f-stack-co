find_path(MLX5_INCLUDE_DIRS
  NAMES infiniband/mlx5dv.h
  HINTS
  ${MLX5_INCLUDE_DIR}
  ${MLX5_ROOT_DIR}
  ${MLX5_ROOT_DIR}/include)

find_library(MLX5_LIBRARIES
  NAMES mlx5
  HINTS
  ${MLX5_LIB_DIR}
  ${MLX5_ROOT_DIR}
  ${MLX5_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(mlx5 DEFAULT_MSG MLX5_INCLUDE_DIRS MLX5_LIBRARIES)
mark_as_advanced(MLX5_INCLUDE_DIR MLX5_LIBRARIES)
