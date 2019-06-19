#ifndef COLOR_SCHEME_H
#define COLOR_SCHEME_H

#include <vector>

class ColorScheme
{
public:
  struct rgb {
    rgb() : r( 1.0 ), g( 1.0 ), b( 1.0 ) {}
    rgb( float l, float m, float n )
    {
      float q = 255.0; //sqrt(l*l + m*m + n*n) + 1.0e-6;
      r = l / q; g = m / q; b = n / q;
    }
    float r;
    float g;
    float b;
  };

  rgb foreground;
  rgb background;
  rgb alt;

  ColorScheme() {}
  ~ColorScheme() {}
};

class ColorSchemes : public std::vector<ColorScheme>
{
public:

  ColorSchemes()
  {
    ColorScheme a;

    // orange on grey
    a.foreground = ColorScheme::rgb( 255, 191, 0 );
    a.background = ColorScheme::rgb( 25, 25, 25 );
    a.alt = ColorScheme::rgb( 245, 245, 220 );
    this->push_back( a );


    // purple on grey
    a.foreground = ColorScheme::rgb( 128, 0, 32.0 );
    a.background = ColorScheme::rgb( 25, 25, 25 );
    a.alt = ColorScheme::rgb( 245, 245, 220 );
    this->push_back( a );

    // blue on white
    a.foreground = ColorScheme::rgb( 0, 71, 171 );
    a.background = ColorScheme::rgb( 245, 245, 245 );
    a.alt = ColorScheme::rgb( 175, 64, 53 );
    this->push_back( a );

    // orange on white
    a.foreground = ColorScheme::rgb( 255, 191, 0 );
    a.background = ColorScheme::rgb( 255, 255, 255 );
    a.alt = ColorScheme::rgb( 0, 223, 22 );
    this->push_back( a );

    // grey on white
    // For Black and White figures
    a.foreground = ColorScheme::rgb( 200, 200, 200 );
    a.background = ColorScheme::rgb( 255, 255, 255 );
    a.alt = ColorScheme::rgb( 80, 80, 80 );
    this->push_back( a );

    // orange on blue
    a.foreground = ColorScheme::rgb( 255, 219, 88 );
    a.background = ColorScheme::rgb( 0, 0, 128 );
    a.alt = ColorScheme::rgb( 173, 255, 47 );
    this->push_back( a );

    // yellow on grey
    a.foreground = ColorScheme::rgb( 240, 220, 130 );
    a.background = ColorScheme::rgb( 25, 25, 25 );
    a.alt = ColorScheme::rgb( 0, 223, 22 );
    this->push_back( a );

    // grey on black
    a.foreground = ColorScheme::rgb( 200, 200, 200 );
    a.background = ColorScheme::rgb( 0, 0, 0 );
    a.alt = ColorScheme::rgb( 100, 100, 100 );
    this->push_back( a );

    // blue on black
    a.foreground = ColorScheme::rgb( 0, 191, 255 );
    a.background = ColorScheme::rgb( 0, 0, 0 );
    a.alt = ColorScheme::rgb( 175, 64, 53 );
    this->push_back( a );

    // green on black
    //a.foreground = ColorScheme::rgb( 71, 171, 71 );
    //a.foreground = ColorScheme::rgb( 152, 251, 152 );
    //a.foreground = ColorScheme::rgb( 60, 184, 120 );
    a.foreground = ColorScheme::rgb( 137, 232, 148 );
    a.background = ColorScheme::rgb( 0, 0, 0 );
    a.alt = ColorScheme::rgb( 175, 64, 53 );
    this->push_back( a );

  }
  ~ColorSchemes() {}
};

#endif // ifndef COLOR_SCHEME_H
