// this file defines the igstkTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkTestMain.h" 


void RegisterTests()
{
REGISTER_TEST(DicomImageReaderTest);
REGISTER_TEST(DicomImageReaderTestDirectory);
REGISTER_TEST(RegistrationCTTest);
REGISTER_TEST(ImageSliceViewerTest);
REGISTER_TEST(LiverTumorSegmentationBaseTest);
REGISTER_TEST(MotionViewerTest);
REGISTER_TEST(igstkStateMachineTest);
REGISTER_TEST(ICPRegistrationTest);
}
