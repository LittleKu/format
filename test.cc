#include <iostream>
#include <string>

#include "format.hpp"

class Test {
 public:
  // const char* ToString() const { return "Test"; } // good
  const wchar_t* ToString() const { return L"Test"; } // good
  // std::string ToString() const { return "Test"; } // error
  // std::wstring ToString() const { return L"Test"; } // error
  // char *ToString() const { return const_cast<char*>("Test"); } // good
  // wchar_t *ToString() const { return const_cast<wchar_t*>(L"Test"); } // good
};

int main(int argc, char** argv) {
  std::string buffer;
  const std::string hello = "hello";
  const std::wstring world = L"world";

  Test test;
  int len = format::Format(buffer, "%s %ls test:%ls", hello, world, test);

  std::cout << "buffer length:" << len << "\n";
  std::cout << "buffer:" << buffer << "\n";

  return EXIT_SUCCESS;
}