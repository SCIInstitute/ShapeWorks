#pragma once

#define DIMENSION 3

namespace shapeworks {
  enum class ConstraintType : char {
    CuttingPlane = 'C',
    FreeForm = 'F'
  };
}
