/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkTrackerTool.h"

int igstkTrackerToolTest( int, char * [] )
{
    typedef igstk::TrackerTool  TrackerToolType;
    typedef TrackerToolType::PositionType      PositionType;
    typedef TrackerToolType::OrientationType   OrientationType;
    typedef TrackerToolType::ErrorType         ErrorType;
    
    TrackerToolType trackerTool;

    PositionType    position    = trackerTool.GetPosition();
    OrientationType orientation = trackerTool.GetOrientation();
    ErrorType       errorvalue  = trackerTool.GetError();

    trackerTool.SetPosition( position );
    trackerTool.SetOrientation( orientation );
    trackerTool.SetError( errorvalue );


    return EXIT_SUCCESS;
}


