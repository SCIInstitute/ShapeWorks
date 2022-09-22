#include "StudioEnums.h"

//-----------------------------------------------------------------------------
std::string shapeworks::display_mode_to_string(DisplayMode display_mode) {
  std::string str;
  switch (display_mode) {
    case DisplayMode::Original:
      return "Original";
      break;
    case DisplayMode::Groomed:
      return "Groomed";
      break;
    case DisplayMode::Reconstructed:
      return "Reconstructed";
      break;
  }
  return "Original";
}

//-----------------------------------------------------------------------------
shapeworks::DisplayMode shapeworks::string_to_display_mode(std::string str) {
  if (str == "Groomed") {
    return DisplayMode::Groomed;
  } else if (str == "Reconstructed") {
    return DisplayMode::Reconstructed;
  }
  return DisplayMode::Original;
}
