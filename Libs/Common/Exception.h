#pragma once

namespace shapeworks {

class Exception
{
public:
  Exception(const std::string &message);
  ~Exception() {}

private:
  std::string errorMessage;  

};

} // shapeworks
