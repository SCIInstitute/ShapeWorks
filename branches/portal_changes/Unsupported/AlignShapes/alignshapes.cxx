/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: alignshapes.cxx,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include <iostream>
#include "alignshapesApp.h"
#include "CS6350.h"

int main(int argc, char *argv[])
{
  try
    {
    if (argc < 3)
      {
      std::cerr << "Use: " << argv[0] << " reference moving <reference_points moving_points>"
                << std::endl;
      return 1;
      }

    char *ptsRef = 0;
    char *ptsMov = 0;
    bool use_points = false;
    if (argc > 4)
      {
      ptsRef = argv[3];
      ptsMov = argv[4];
      use_points = true;
      }

    alignshapesApp gui(argv[1], argv[2], ptsRef, ptsMov, use_points);

    //    Fl::check();

    gui.Show();

    Fl::run();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    return 1;
    }
  catch( CS6350::exception & eg )
    {
    std::cerr << eg << std::endl;
    }
  catch( std::exception & ex )
    {
    std::cerr << ex.what() << std::endl;
    }
  catch( ... )
    {
    std::cerr << "Unknown exception" << std::endl;
    return 2;
    }  
  
  return 0;
}
