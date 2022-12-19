#pragma once


namespace shapeworks {

//! Update Checker
/*!
 * This class checks for new versions of ShapeWorks to inform the user
 */
class UpdateChecker {
 public:

  UpdateChecker();
  ~UpdateChecker();

  void runAutoUpdateCheck();
  void runManualUpdateCheck();

 private:

};

}  // namespace shapeworks