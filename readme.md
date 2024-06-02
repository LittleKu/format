## `C Style Formatter for C++`

### features
* support std::string and std::wstring
* support custom object 

### usage
```cpp

#include "format.hpp"

const std::string str = "string";
const std::wstring wstr = "wstring";
int value = 0;
float fvalue = 1.0f;

class Test{
  public:
  const char * ToString() const { return "Test string";}
  //const wchar_t * ToString() const { return L"Test Wide string";}
}

Test test;

std::string string_buffer;
int length = format::Format(string_buffer, "string:%s, wide string:%ls, int:%d, float:%f, my custom object:%s", str, wstr, value, fvalue, test);

```