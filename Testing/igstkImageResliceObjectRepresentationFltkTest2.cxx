/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceObjectRepresentationFltkTest2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkFLTKWidget.h"

#include "igstkConfigure.h"
#include "igstkImageResliceObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkView2D.h"

namespace ImageResliceObjectRepresentationFltkTest2
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}

/** This test demonstrates how to perform orthogonal reslicing in the manual reslicing mode. 
* The reslicing position and orientation are set by means of RequestSetCursorPosition() and 
* RequestSetOrientationType(). Then we span the reslicing plane along each orthogonal 
* direction (i.e. axial, sagittal and coronal).
*/
int igstkImageResliceObjectRepresentationFltkTest2( int argc , char * argv [] )
{
  std::cout << " igstkImageResliceObjectRepresentationQtTest2: application started " << std::endl;

  igstk::RealTimeClock::Initialize();

  if( argc < 3 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " \
              << "Output image file for a screenshot" << std::endl; 
    return EXIT_FAILURE;
    }

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  // the reslicer plane we want to test.
  typedef igstk::ReslicerPlaneSpatialObject            ReslicerPlaneType;

  // we will use an ImageSpatialObject as the bounding box provider for the ReslicerPlane
  typedef igstk::ImageSpatialObject< PixelType, Dimension > 
                                                       ImageSpatialObjectType;

  typedef ImageSpatialObjectType::IndexType::IndexValueType IndexValueType;

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                            igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);// redirect messages from VTK 
                                     // OutputWindow -> logger

  // Create Axes object to act as a reference coordinate system
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer worldReference = AxesObjectType::New();
  worldReference->SetSize(3.0, 3.0, 3.0);

  // Define identity transform
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

  //set up CT image observer
  typedef ImageResliceObjectRepresentationFltkTest2::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New(); 
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();
  reader->RequestGetImage();
 
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  // Set input image spatial object
  ImageSpatialObjectType::Pointer imageSpatialObject = ImageSpatialObjectType::New(); 
  imageSpatialObject = ctImageObserver->GetCTImage();

  //Determine the image parameters
  //first access the VTK image data
  typedef ImageResliceObjectRepresentationFltkTest2::VTKImageObserver 
                                                        VTKImageObserverType;
  
  VTKImageObserverType::Pointer vtkImageObserver = VTKImageObserverType::New();

  imageSpatialObject->AddObserver( ::igstk::VTKImageModifiedEvent(), 
                                     vtkImageObserver );
  vtkImageObserver->Reset();
  imageSpatialObject->RequestGetVTKImage();

  if( !vtkImageObserver->GotVTKImage() ) 
    {
    std::cout << "No VTKImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  vtkImageData * imageData = vtkImageObserver->GetVTKImage();

  int imageExtent[6];
  imageData->GetWholeExtent( imageExtent );
  std::cout << "Image extent: " << "(" << imageExtent[0] << "," 
                                << imageExtent[1] << ","
                                << imageExtent[2] << ","
                                << imageExtent[3] << ","
                                << imageExtent[4] << ","
                                << imageExtent[5] << ")" << std::endl;
  
  //Connect the image spatial object to the reference coordinate system
  imageSpatialObject->RequestSetTransformAndParent( identity, worldReference );

  // Set a 2D View
  typedef igstk::View2D  View2DType;
  View2DType::Pointer view2D = View2DType::New();
//  view2D->SetLogger( logger );
    
  view2D->RequestResetCamera();

  Fl_Window * form = new Fl_Window(512,512,"igstkImageResliceObjectRepresentationFltkTest2");

  typedef igstk::FLTKWidget      FLTKWidgetType;

   // instantiate FLTK widget 
  FLTKWidgetType * fltkWidget2D = 
                      new FLTKWidgetType(0,0,512,512,"2D View");

  fltkWidget2D->RequestSetView( view2D );
//  fltkWidget2D->SetLogger( logger );

  view2D->RequestSetTransformAndParent( identity, worldReference );
  view2D->SetRefreshRate( 40 );

  form->end();
  form->show();

typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageResliceRepresentationType; 

  ImageResliceRepresentationType::Pointer  imageResliceRepresentation = 
    ImageResliceRepresentationType::New(); 

  //imageResliceRepresentation->SetLogger( logger );
  imageResliceRepresentation->SetWindowLevel( 1559, -244 );
  imageResliceRepresentation->RequestSetImageSpatialObject( imageSpatialObject );
  
  // a variable to hold image index
  ImageSpatialObjectType::IndexType index;

  // a variable to hold world point coords
  ImageSpatialObjectType::PointType point;

//  initialize the tool transform in the middle of the image
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  // auxiliar
  const double *data = NULL;

  data = point.GetVnlVector().data_block();

  const double transformUncertainty = 1.0;

  // Instantiate and use a reslicer plane spatial object
  typedef igstk::ReslicerPlaneSpatialObject            ReslicerPlaneType;

  ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
//  reslicerPlaneSpatialObject->SetLogger( logger );

  // select Orthogonal reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );

  // set Axial orientation type
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  // set bounding box provider spatial object to the reslicer plane object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  // set transform and parent to the reslicer plane
  reslicerPlaneSpatialObject->RequestSetTransformAndParent( identity, worldReference );

  // set the reslicer plane spatial object to the image representation
  imageResliceRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  view2D->RequestDetachFromParent();
  view2D->RequestSetTransformAndParent( identity, reslicerPlaneSpatialObject );
  view2D->SetCameraParallelProjection(true);

  // add the image representation to the view
  view2D->RequestAddObject( imageResliceRepresentation );

  // Start the view
  view2D->RequestStart();

  std::cout << "Axial view: " << std::endl;
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
//  view2D->RequestSetOrientation( View2DType::Axial ); 
  view2D->RequestResetCamera();

  // Iteratively change the tool transform to reslice through the image
  for(unsigned int i=(unsigned int)(imageExtent[5]/2); i<=(unsigned int)(3*imageExtent[5]/4); i++)
  {
  index[2] = static_cast<IndexValueType>(i);

  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  data = point.GetVnlVector().data_block();
  std::cout << data[0] << " " << data[1] << " " << data[2] << " axial slice # " << i << std::endl;

  reslicerPlaneSpatialObject->RequestSetCursorPosition( data ); 

  Fl::wait( 0.01 );
  igstk::PulseGenerator::CheckTimeouts();
  }

  // Change slice orientation to sagittal
  std::cout << "Sagittal view: " << std::endl;
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );
//  view2D->RequestSetOrientation( View2DType::Sagittal ); 
  view2D->RequestResetCamera();

  // Iteratively change the tool transform to reslice through the image
  for(unsigned int i=(unsigned int)(imageExtent[1]/2); i<(unsigned int)(3*imageExtent[1]/4); i++)
  {
  index[0] = static_cast<IndexValueType>(i);
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  const double *data = point.GetVnlVector().data_block();
  std::cout << data[0] << " " << data[1] << " " << data[2] << " sagittal slice # " << i << std::endl;

  reslicerPlaneSpatialObject->RequestSetCursorPosition( data ); 

  Fl::wait( 0.01 );
  igstk::PulseGenerator::CheckTimeouts();
  }

  /* Change slice orientation to coronal */
  std::cout << "Coronal view: " << std::endl;
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );
//  view2D->RequestSetOrientation( View2DType::Coronal ); 
  view2D->RequestResetCamera();

  // Iteratively change the tool transform to reslice through the image
  for(unsigned int i=(unsigned int)(imageExtent[3]/2); i<(unsigned int)(3*imageExtent[3]/4); i++)
  {
  index[1] = static_cast<IndexValueType>(i);
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  const double *data = point.GetVnlVector().data_block();
  std::cout << data[0] << " " << data[1] << " " << data[2] << " coronal slice # " << i << std::endl;

  reslicerPlaneSpatialObject->RequestSetCursorPosition( data ); 

  Fl::wait( 0.01 );
  igstk::PulseGenerator::CheckTimeouts();
  }

  //Reslice to the center axial slice and take a screenshot.
  //
  view2D->RequestStop();

//  view2D->RequestSetOrientation( View2DType::Axial );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));

  view2D->RequestStart();
  view2D->RequestResetCamera();

  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  data = point.GetVnlVector().data_block();

  std::cout << "Saving snapshot to: " << argv[2] << std::endl;
  view2D->RequestSaveScreenShot( argv[2] );
  Fl::wait( 0.01 );
  igstk::PulseGenerator::CheckTimeouts();
  view2D->RequestStop();

  delete fltkWidget2D;
  form->hide();
  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Got VTK Logger errors!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  std::cout << "[SUCCESS]" << std::endl;
  return EXIT_SUCCESS;

}
/*
  typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageRepresentationType;

  ImageRepresentationType::Pointer  representation =  ImageRepresentationType::New(); 
  //representation->SetLogger( logger );
  representation->SetWindowLevel( 1559, -244 );
  representation->RequestSetImageSpatialObject( imageSpatialObject );

  //Instantiate and use a reslicer plane spatial object
  ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
//  reslicerPlaneSpatialObject->SetLogger( logger );

  // Select Orthogonal reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );

  // Select Axial orientation type
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  // Set bounding box provider spatial object to the reslicer plane object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  // Set the reslicer plane spatial object to the representation
  representation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  // add the image representation to the view
  view2D->RequestAddObject( representation );

  // a variable to hold image index
  typedef ImageSpatialObjectType::IndexType IndexType;

  typedef ImageSpatialObjectType::IndexType::IndexValueType IndexValueType;

  IndexType  index;

  // a variable to hold world point coords
  ImageSpatialObjectType::PointType point;

  // auxiliar
  const double *data = NULL;

  // Select axial orientation in the reslicer plane and view
  view2D->RequestSetOrientation( View2DType::Axial );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
  
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(imageExtent[4]);
  
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data ); 

  view2D->RequestStart();
  view2D->RequestResetCamera();
  form->show();

  for(unsigned int i= (unsigned int)(imageExtent[4]); i<(unsigned int)(imageExtent[5]); i++)
      {
      index[2] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      data = point.GetVnlVector().data_block();
      reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
      std::cout << data[0] << " " << data[1] << " " << data[2] << " axial slice # " << i << std::endl;
      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();
      }
  view2D->RequestStop();

  std::cout << "Sagittal view: " << std::endl;
  view2D->RequestSetOrientation( View2DType::Sagittal );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );

  index[0] = static_cast<IndexValueType>(imageExtent[0]);
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  view2D->RequestStart();
  view2D->RequestResetCamera();

  form->end();
  form->show();

  for(unsigned int i=(unsigned int)(imageExtent[0]); i<(unsigned int)(imageExtent[1]); i++)
      {
      index[0] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      const double *data = point.GetVnlVector().data_block();
      reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
      std::cout << data[0] << " " << data[1] << " " << data[2] << " sagittal slice # " << i << std::endl;
      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();      
      }
  view2D->RequestStop();

  std::cout << "Coronal view: " << std::endl;
  view2D->RequestSetOrientation( View2DType::Coronal );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );

  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(imageExtent[2]);
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  form->end();
  form->show();

  for(unsigned int i=(unsigned int)(imageExtent[2]); i<(unsigned int)(imageExtent[3]); i++)
    {
    index[1] = i;
    imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
    const double *data = point.GetVnlVector().data_block();
    reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
    std::cout << data[0] << " " << data[1] << " " << data[2] << " coronal slice # " << i << std::endl;
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    }
  view2D->RequestStop();


  //Reslice the center axial slice and take a screenshot.
  //
  view2D->RequestSetOrientation( View2DType::Axial );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));

  view2D->RequestStart();
  view2D->RequestResetCamera();
  form->end();
  form->show();

  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  view2D->RequestSaveScreenShot( argv[2] );
  view2D->RequestStop();

  delete fltkWidget2D;

  form->hide();

  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Got VTK Logger errors!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  std::cout << "[SUCCESS]" << std::endl;
 
  return EXIT_SUCCESS;
}

*/