#pragma once

#include <Libs/Project/Project.h>

namespace shapeworks {

class GroomParameters {

public:

  explicit GroomParameters(ProjectHandle project);
  void save_to_project();

  bool get_center_tool();
  void set_center_tool(bool value);

  bool get_isolate_tool();
  void set_isolate_tool(bool value);

  bool get_fill_holes_tool();
  void set_fill_holes_tool(bool value);

  bool get_auto_pad_tool();
  void set_auto_pad_tool(bool value);

  int get_padding_amount();
  void set_padding_amount(int padding_amount);

  bool get_antialias_tool();
  void set_antialias_tool(bool value);

  int get_antialias_iterations();
  void set_antialias_iterations(int iterations);

  bool get_blur_tool();
  void set_blur_tool(bool value);

  double get_blur_amount();
  void set_blur_amount(double blur_amount);

  bool get_fast_marching();
  void set_fast_marching(bool value);
  
private:

  Parameters params_;
  ProjectHandle project_;

};

}