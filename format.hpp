/**
 * @file      : format.hpp
 * @author    : LittleKu<kklvzl@gmail.com>
 * @date      : 2024-06-02 11:27:09
 * @brief     : A C style formatter support std::string and std::wstring.
 *              And of course custom types are supported.require c++20.
 */
#ifndef FORMAT_HPP_
#define FORMAT_HPP_

#include <stdarg.h>
#include <stdio.h>

#include <concepts>
#include <string>
#include <type_traits>

namespace format {

namespace internal {

template <typename T>
concept IsChar = std::is_same_v<std::decay_t<T>, char*> ||
                 std::is_same_v<std::decay_t<T>, const char*>;

template <typename T>
concept IsWchar = std::is_same_v<std::decay_t<T>, wchar_t*> ||
                  std::is_same_v<std::decay_t<T>, const wchar_t*>;

template <typename T>
concept IsRegular = std::is_integral_v<T> || std::is_floating_point_v<T> ||
                    IsChar<T> || IsWchar<T>;

template <typename T>
concept IsXChar = IsChar<T> || IsWchar<T>;

template <typename T>
concept HasToString = requires(T& t) {
  { t.ToString() } -> IsXChar;
};

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
  const int len = GetFormattedLength(format, args);
  if (len <= 0) {
    return -1;
  }
  buffer.resize(static_cast<size_t>(len));
  int n = FormatV(&buffer[0], len + 1, format, args);
  va_end(args);
  return n;
}

template <typename T>
  requires IsRegular<T>
T Argument(T value) noexcept {
  return value;
}

template <typename CharT>
CharT const* Argument(std::basic_string<CharT> const& value) noexcept {
  return value.c_str();
}

template <typename T>
  requires HasToString<T>
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