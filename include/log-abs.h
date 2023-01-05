#ifndef LOG_ABS_H
#define LOG_ABS_H

#ifndef USE_DOMESTIC_LOG
#include <spdlog/spdlog.h>
#define LOG_ERROR SPDLOG_ERROR
#define LOG_WARN SPDLOG_WARN
#define LOG_INFO SPDLOG_INFO
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_CRITICAL SPDLOG_CRITICAL
#else
#include <string>
#include <regex>
static const char *convert_printf_format(const char* fmt) {
    
    std::string s(fmt);
    s = std::regex_replace(s, std::regex("{}"), "%s"); // replace 'def' -> 'klm'
    return s.c_str();
}
#define LOG_ERROR(fmt , ...) fprintf(stderr, convert_printf_format(fmt) __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(fmt , ...) fprintf(stderr, convert_printf_format(fmt) __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(fmt , ...) fprintf(stderr, convert_printf_format(fmt) __VA_OPT__(,) __VA_ARGS__)
#define LOG_DEBUG(fmt , ...) fprintf(stderr, convert_printf_format(fmt) __VA_OPT__(,) __VA_ARGS__)
#define LOG_CRITICAL(fmt , ...) fprintf(stderr, convert_printf_format(fmt) __VA_OPT__(,) __VA_ARGS__)
#endif

#endif