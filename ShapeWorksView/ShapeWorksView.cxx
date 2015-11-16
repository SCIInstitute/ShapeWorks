/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksView.cxx,v $
  Date:      $Date: 2011/03/24 01:17:39 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include <iostream>
#include "ShapeWorksViewApp.h"
#include "CS6350.h"
#include <cstdlib>


void InitializeCallback(void *gui)
{
  // add a new callback to check updates
  // static_cast<ShapeWorksViewApp *>(gui)->InitCheck();

  static_cast<ShapeWorksViewApp *>(gui)->HideGroups();
  static_cast<ShapeWorksViewApp *>(gui)->ViewMeanGroup->show();  
  static_cast<ShapeWorksViewApp *>(gui)->DisplayMean(0);
}

void InitializeCheckUpdatesCallback(void *gui)
{
	// add a new callback to check updates
	static_cast<ShapeWorksViewApp *>(gui)->InitCheck();
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

    ShapeWorksViewApp gui(argv[1]);

    //    Fl::check();

    gui.Show();

	// The default setting is viewer not checking 
	if ( argc == 3 )
	{
		std::string option = argv[2];
		if ( option == "enable_check_update" )
		{
			Fl::add_timeout(0.1, &InitializeCheckUpdatesCallback, &gui);  
		}

	}

    Fl::add_timeout(0.1, &InitializeCallback, &gui);   
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
