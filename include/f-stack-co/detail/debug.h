#pragma once

#include <cstdio>

#ifndef SOURCE_PATH_LENGTH
#define SOURCE_PATH_LENGTH 0
#endif

#define __FSTACKCO_FILENAME__ (&__FILE__[SOURCE_PATH_LENGTH])

namespace fstackco {

enum class LogLevel {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
};
}

constexpr static inline fstackco::LogLevel fstackco_log_level =
    fstackco::LogLevel::DEBUG;

#define FSTACKCO_LOG_TRACE(msg, ...)                                            \
  do {                                                                         \
    if (uringpp_log_level > uringpp::LogLevel::TRACE)                          \
      break;                                                                   \
    printf("[TRACE] [%s:%d] " msg "\n", __FSTACKCO_FILENAME__,                  \
           __LINE__ __VA_OPT__(, ) __VA_ARGS__);                               \
  } while (0)

#define FSTACKCO_LOG_DEBUG(msg, ...)                                            \
  do {                                                                         \
    if (uringpp_log_level > uringpp::LogLevel::DEBUG)                          \
      break;                                                                   \
    printf("[DEBUG] [%s:%d] " msg "\n", __FSTACKCO_FILENAME__,                  \
           __LINE__ __VA_OPT__(, ) __VA_ARGS__);                               \
  } while (0)

#define FSTACKCO_LOG_INFO(msg, ...)                                             \
  do {                                                                         \
    if (uringpp_log_level > uringpp::LogLevel::INFO)                           \
      break;                                                                   \
    printf("[INFO ] [%s:%d] " msg "\n", __FSTACKCO_FILENAME__,                  \
           __LINE__ __VA_OPT__(, ) __VA_ARGS__);                               \
  } while (0)

#define FSTACKCO_LOG_ERROR(msg, ...)                                            \
  do {                                                                         \
    printf("[ERROR] [%s:%d] " msg "\n", __FSTACKCO_FILENAME__,                  \
           __LINE__ __VA_OPT__(, ) __VA_ARGS__);                               \
  } while (0)
