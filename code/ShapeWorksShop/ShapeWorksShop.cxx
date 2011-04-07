/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: ShapeWorksShop.cxx,v $

  Date:      $Date: 2011/03/24 01:17:38 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>

#include "ShapeWorksShopApp.h"

#include "CS6350.h"



void UpdateAtIterateCallback(void *gui)

{

  //    m_CheckpointCounter++;

  double rate = static_cast<ShapeWorksShopApp *>(gui)->frame_rate_spinner->value();

  if (rate == 0.0)

    {

    Fl::repeat_timeout(1.0, &UpdateAtIterateCallback, gui);

    }

  else

    {

    static_cast<ShapeWorksShopApp *>(gui)->m_Viewer1->Render();

    //  static_cast<ShapeWorksShopApp *>(gui)->m_Viewer2->Render();

    Fl::repeat_timeout(1.0 / rate, &UpdateAtIterateCallback, gui);

    }

}



int main(int argc, char *argv[])

{

  try

    {

    if (argc < 2)

      {

      std::cerr << "Use: " << argv[0] << " parameterfile" << std::endl;

      return 1;

      }



    ShapeWorksShopApp gui(argv[1]);



    //    Fl::check();



    gui.Show();



    Fl::scheme("plastic");

    //    Fl::add_idle(&UpdateAtIterateCallback, &gui);

    Fl::add_timeout(2.0, &UpdateAtIterateCallback, &gui);

    

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

    return 2;

    }

  catch( std::exception & ex )

    {

    std::cerr << ex.what() << std::endl;

    return 3;

    }

  catch( ... )

    {

    std::cerr << "Unknown exception" << std::endl;

    return 4;

    }  

  

  return 0;

}

