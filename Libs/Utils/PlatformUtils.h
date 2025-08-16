
namespace shapeworks {

//! Platform utility functions
class PlatformUtils {
 public:
  static bool is_windows() {
#ifdef _WIN32
    return true;
#endif
    return false;
  }

  static bool is_linux() {
#ifdef __linux__
    return true;
#endif
    return false;
  }

  static bool is_macos() {
#ifdef __APPLE__
    return true;
#endif
    return false;
  }
};

}  // namespace shapeworks
