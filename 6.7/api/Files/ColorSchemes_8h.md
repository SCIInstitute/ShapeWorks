---
title: Studio/Visualization/ColorSchemes.h

---

# Studio/Visualization/ColorSchemes.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[ColorScheme](../Classes/classColorScheme.md)**  |
| struct | **[ColorScheme::rgb](../Classes/structColorScheme_1_1rgb.md)**  |
| class | **[ColorSchemes](../Classes/classColorSchemes.md)**  |




## Source code

```cpp
#pragma once

#include <QColor>
#include <vector>

class ColorScheme {
 public:
  struct rgb {
    rgb() : r(1.0), g(1.0), b(1.0) {}
    rgb(float l, float m, float n) {
      float q = 255.0;  // sqrt(l*l + m*m + n*n) + 1.0e-6;
      r = l / q;
      g = m / q;
      b = n / q;
    }

    float r;
    float g;
    float b;
  };

  rgb foreground;
  rgb background;
  rgb alt;

  //-----------------------------------------------------------------------------
  QColor background_qcolor(int alpha) { return QColor(background.r * 255, background.g * 255, background.b * 255, alpha); }

  //-----------------------------------------------------------------------------
  QColor foreground_qcolor() { return QColor(foreground.r * 255, foreground.g * 255, foreground.b * 255, 255); }

  //-----------------------------------------------------------------------------
  QColor get_text_color() {
    double average = (background.r + background.g + background.b) / 3.0;

    double color = 255;
    if (average > 0.5) {
      color = 0;
    }
    return QColor(color, color, color);
  }

  double get_text_intensity() {
    auto color = get_text_color();
    if (color.red() == 0) {
      return 0;
    }
    return 1;
  }

  ColorScheme() {}
  ~ColorScheme() {}
};

class ColorSchemes : public std::vector<ColorScheme> {
 public:
  ColorSchemes() {
    ColorScheme a;

    // orange on grey
    a.foreground = ColorScheme::rgb(255, 191, 0);
    a.background = ColorScheme::rgb(25, 25, 25);
    a.alt = ColorScheme::rgb(245, 245, 220);
    this->push_back(a);

    // purple on grey
    a.foreground = ColorScheme::rgb(128, 0, 32.0);
    a.background = ColorScheme::rgb(25, 25, 25);
    a.alt = ColorScheme::rgb(245, 245, 220);
    this->push_back(a);

    // blue on white
    a.foreground = ColorScheme::rgb(0, 71, 171);
    a.background = ColorScheme::rgb(245, 245, 245);
    a.alt = ColorScheme::rgb(175, 64, 53);
    this->push_back(a);

    // orange on white
    a.foreground = ColorScheme::rgb(255, 191, 0);
    a.background = ColorScheme::rgb(255, 255, 255);
    a.alt = ColorScheme::rgb(0, 223, 22);
    this->push_back(a);

    // grey on white
    // For Black and White figures
    a.foreground = ColorScheme::rgb(200, 200, 200);
    a.background = ColorScheme::rgb(255, 255, 255);
    a.alt = ColorScheme::rgb(80, 80, 80);
    this->push_back(a);

    // orange on blue
    a.foreground = ColorScheme::rgb(255, 219, 88);
    a.background = ColorScheme::rgb(0, 0, 128);
    a.alt = ColorScheme::rgb(173, 255, 47);
    this->push_back(a);

    // yellow on grey
    a.foreground = ColorScheme::rgb(240, 220, 130);
    a.background = ColorScheme::rgb(25, 25, 25);
    a.alt = ColorScheme::rgb(0, 223, 22);
    this->push_back(a);

    // grey on black
    a.foreground = ColorScheme::rgb(200, 200, 200);
    a.background = ColorScheme::rgb(0, 0, 0);
    a.alt = ColorScheme::rgb(100, 100, 100);
    this->push_back(a);

    // blue on black
    a.foreground = ColorScheme::rgb(0, 191, 255);
    a.background = ColorScheme::rgb(0, 0, 0);
    a.alt = ColorScheme::rgb(175, 64, 53);
    this->push_back(a);

    // green on black
    a.foreground = ColorScheme::rgb(137, 232, 148);
    a.background = ColorScheme::rgb(0, 0, 0);
    a.alt = ColorScheme::rgb(175, 64, 53);
    this->push_back(a);
  }
  ~ColorSchemes() {}
};
```


-------------------------------

Updated on 2025-10-11 at 04:45:09 +0000
