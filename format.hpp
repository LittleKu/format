/**
 * @file      : format.hpp
 * @author    : LittleKu<kklvzl@gmail.com>
 * @date      : 2024-06-02 11:27:09
 * @brief     : A C style formatter support std::string and std::wstring.
 *              And of course custom types are supported.require C++14
 */
#ifndef FORMAT_HPP_
#define FORMAT_HPP_

#include <stdarg.h>
#include <stdio.h>

#include <string>
#include <type_traits>

namespace format {

namespace internal {
template <typename T>
struct is_char
    : std::integral_constant<
          bool,
          std::is_same_v<typename std::decay_t<T>, char*> ||
              std::is_same_v<typename std::decay_t<T>, const char*>> {};

template <typename T>
struct is_wchar
    : std::integral_constant<
          bool,
          std::is_same_v<typename std::decay_t<T>, wchar_t*> ||
              std::is_same_v<typename std::decay_t<T>, const wchar_t*>> {};

inline int GetFormattedLength(const char* format, va_list args) {
  return _vscprintf(format, args);
}

inline int GetFormattedLength(const wchar_t* const fmt, va_list args) {
  int const result = __stdio_common_vswprintf(
      _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |
          _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR |
          _CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS,
      nullptr, 0, fmt, nullptr, args);

  return result < 0 ? -1 : result;
}

inline int FormatV(char* const buffer,
                   size_t const count,
                   const char* const format,
                   va_list args) {
  return vsprintf_s(buffer, count, format, args);
}

inline int FormatV(wchar_t* const buffer,
                   size_t const count,
                   const wchar_t* const format,
                   va_list args) {
  int const result = __stdio_common_vswprintf_s(
      _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |
          _CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS,
      buffer, count, format, nullptr, args);

  return result < 0 ? -1 : result;
}

template <typename T>
int Format(std::basic_string<T>& buffer, T const* const format, ...) {
  va_list args;
  va_start(args, format);
  const size_t len = GetFormattedLength(format, args);
  if (len <= 0) {
    return -1;
  }
  buffer.resize(len);
  int n = FormatV(&buffer[0], len + 1, format, args);
  va_end(args);
  return n;
}

template <typename T,
          typename = std::enable_if_t<std::is_integral_v<T> ||
                                      std::is_floating_point_v<T> ||
                                      is_char<T>::value || is_wchar<T>::value>>
T Argument(T value) noexcept {
  return value;
}

template <typename CharT>
CharT const* Argument(std::basic_string<CharT> const& value) noexcept {
  return value.c_str();
}

template <typename T, typename = void>
struct HasStringType : std::false_type {};

template <typename T>
struct HasStringType<T, std::void_t<decltype(std::declval<T*>()->ToString())>>
    : std::true_type {};

template <typename T,
          typename = std::enable_if_t<
              HasStringType<T>::value &&
              (is_char<decltype(std::declval<T>().ToString())>::value ||
               is_wchar<decltype(std::declval<T>().ToString())>::value)>>
auto Argument(T const& value) noexcept {
  return value.ToString();
}
}  // namespace internal

template <typename CharT, typename... Args>
int Format(std::basic_string<CharT>& buffer,
           CharT const* const format,
           Args const&... args) {
  return internal::Format(buffer, format, internal::Argument(args)...);
}

}  // namespace format

#endif  // FORMAT_HPP_