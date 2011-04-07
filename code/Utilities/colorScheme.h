/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: colorScheme.h,v $

  Date:      $Date: 2011/03/24 01:17:40 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _colorScheme_h

#define _colorScheme_h



#include <vector>



class colorScheme

{

public:

  struct myrgb {

    myrgb() : r(1.0), g(1.0), b(1.0) {}

    myrgb(float l, float m, float n)

    {

      float q = 255.0;//sqrt(l*l + m*m + n*n) + 1.0e-6;

      r=l/q; g=m/q; b=n/q;

    }

    float r;

    float g;

    float b;

  };



  myrgb foreground;

  myrgb background;

  myrgb alt;

  

  colorScheme()  {}

  ~colorScheme() {}

};



class colorSchemes : public std::vector<colorScheme>

{

public:



  colorSchemes()

  {    

    colorScheme  a;

    a.foreground = colorScheme::myrgb(255, 191, 0);

    a.background = colorScheme::myrgb(25,25,25);

    a.alt        = colorScheme::myrgb(245,245,220);

    this->push_back(a);



    a.foreground = colorScheme::myrgb(255, 191, 0);

    a.background = colorScheme::myrgb(255,255,255);

    a.alt        = colorScheme::myrgb(0,223,22);

    this->push_back(a);



    a.foreground = colorScheme::myrgb(128, 0, 32.0);

    a.background = colorScheme::myrgb(25,25,25);

    a.alt        = colorScheme::myrgb(245,245,220);

    this->push_back(a);



    a.foreground = colorScheme::myrgb(0,71,171);

    a.background = colorScheme::myrgb(245,245,245);

    a.alt        = colorScheme::myrgb(175,64,53);

    this->push_back(a);

   

    // For Black and White figures

    a.foreground = colorScheme::myrgb(200, 200, 200);

    a.background = colorScheme::myrgb(255,255,255);

    a.alt        = colorScheme::myrgb(80,80,80);

    this->push_back(a);



    a.foreground = colorScheme::myrgb(200, 200, 200);

    a.background = colorScheme::myrgb(0,0,0);

    a.alt        = colorScheme::myrgb(100,100,100);

    this->push_back(a);



    a.foreground = colorScheme::myrgb(255,219,88);

    a.background = colorScheme::myrgb(0,0,128);

    a.alt        = colorScheme::myrgb(173,255,47);

    this->push_back(a);

  



    a.foreground = colorScheme::myrgb(240,220,130);

    a.background = colorScheme::myrgb(25,25,25);

    a.alt        = colorScheme::myrgb(0,223,22);

    this->push_back(a);

    

  }

  ~colorSchemes() {}

  

};





#endif

