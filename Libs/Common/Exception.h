#pragma once

namespace shapeworks {

class Exception
{
public:
  Exception(const std::string &message) : errorMessage(message) {};
  ~Exception() {}

  std::string message() { return errorMessage; }

private:
  std::string errorMessage;

};

} // shapeworks
