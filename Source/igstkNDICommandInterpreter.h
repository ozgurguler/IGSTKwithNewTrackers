/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICommandInterpreter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __NDICommandInterpreter_h__
#define __NDICommandInterpreter_h__

#include "itkObject.h"
#include "igstkSerialCommunication.h"

namespace igstk
{

/** Maximum number of handles that will ever be in use simultaneously */
const int NDI_MAX_HANDLES = 24;


class NDICommandInterpreter : public itk::Object
{
/** \class NDICommandInterpreter
    \brief Mediate between a Tracker and its Communication object.

    The NDICommandInterpreter serves to purposes: it formats and
    builds a CRC value for commands that are sent to the Polaris or
    Aurora, and it decomponses the replies from the Polaris or Aurora
    into numerical values.

    The AuroraTracker and PolarisTracker classes should create a
    private instance of NDICommandInterpreter, and then pass
    their Communication objects to the NDICommandInterpreter.

    \ingroup Tracking
*/

public:

  /** Device and host error codes

  The error code is set only by Command() or by 
  macros and functions that call Command().

  Error codes that equal to or less than 0xff are error codes reported
  by the device itself.  Error codes greater than 0xff are
  errors that are reported by the host computer.

  The error code is returned by NDICommandInterpreter::GetError() and
  the corresponding text is available by passing the code to
  NDICommandInterpreter::ErrorString().
  */
  typedef enum
  {
    NDI_OKAY            = 0x00,  /**< No error */     
    NDI_INVALID         = 0x01,  /**< Invalid command */
    NDI_TOO_LONG        = 0x02,  /**< Command too long */
    NDI_TOO_SHORT       = 0x03,  /**< Command too short */
    NDI_BAD_COMMAND_CRC = 0x04,  /**< Bad CRC calculated for command */
    NDI_INTERN_TIMEOUT  = 0x05,  /**< Timeout on command execution */
    NDI_COMM_FAIL       = 0x06,  /**< New communication parameters failed */
    NDI_PARAMETERS      = 0x07,  /**< Incorrect number of command parameters */
    NDI_INVALID_PORT    = 0x08,  /**< Invalid port selected */
    NDI_INVALID_MODE    = 0x09,  /**< Invalid mode selected */
    NDI_INVALID_LED     = 0x0a,  /**< Invalid LED selected */
    NDI_LED_STATE       = 0x0b,  /**< Invalid LED state selected */
    NDI_BAD_MODE        = 0x0c,  /**< Command invalid for current mode */
    NDI_NO_TOOL         = 0x0d,  /**< No tool plugged in selected port */
    NDI_PORT_NOT_INIT   = 0x0e,  /**< Selected port not initialized */
    NDI_PORT_DISABLED   = 0x0f,  /**< Selected port not enabled */
    NDI_INITIALIZATION  = 0x10,  /**< System not initialized */
    NDI_TSTOP_FAIL      = 0x11,  /**< Failure to stop tracking */
    NDI_TSTART_FAIL     = 0x12,  /**< Failure to start tracking */
    NDI_PINIT_FAIL      = 0x13,  /**< Failure to initialize tool in port */
    NDI_CAMERA          = 0x14,  /**< Invalid camera parameters */
    NDI_INIT_FAIL       = 0x15,  /**< Failure to initialize */
    NDI_DSTART_FAIL     = 0x16,  /**< Failure to start diagnostic mode */
    NDI_DSTOP_FAIL      = 0x17,  /**< Failure to stop diagnostic mode */
    NDI_IRCHK_FAIL      = 0x18,  /**< Failure to determine environmental IR */
    NDI_FIRMWARE        = 0x19,  /**< Failure to read firmware version */
    NDI_INTERNAL        = 0x1a,  /**< Internal device error */
    NDI_IRINIT_FAIL     = 0x1b,  /**< Failure to initialize for IR diagnostics*/
    NDI_IRED_FAIL       = 0x1c,  /**< Failure to set marker firing signature */
    NDI_SROM_FAIL       = 0x1d,  /**< Failure to search for SROM IDs */
    NDI_SROM_READ       = 0x1e,  /**< Failure to read SROM data */
    NDI_SROM_WRITE      = 0x1f,  /**< Failure to write SROM data */
    NDI_SROM_SELECT     = 0x20,  /**< Failure to select SROM */
    NDI_PORT_CURRENT    = 0x21,  /**< Failure to perform tool current test */ 
    NDI_WAVELENGTH      = 0x22,  /**< No camera parameters for this wavelength*/
    NDI_PARAMETER_RANGE = 0x23,  /**< Command parameter out of range */
    NDI_VOLUME          = 0x24,  /**< No camera parameters for this volume */
    NDI_FEATURES        = 0x25,  /**< Failure to determine supported features*/

    NDI_ENVIRONMENT     = 0xf1,  /**< Too much environmental infrared */

    NDI_EPROM_READ      = 0xf6,  /**< Failure to read Flash EPROM */
    NDI_EPROM_WRITE     = 0xf5,  /**< Failure to write Flash EPROM */
    NDI_EPROM_ERASE     = 0xf4,  /**< Failure to erase Flash EPROM */

    /* error codes generated by the host, rather than by the device */

    NDI_BAD_CRC         = 0x0100,  /**< Bad CRC received from device */
    NDI_OPEN_ERROR      = 0x0200,  /**< Error opening serial device */
    NDI_BAD_COMM        = 0x0300,  /**< Bad communication parameters for host*/
    NDI_TIMEOUT         = 0x0400,  /**< device took >5 secs to reply */
    NDI_WRITE_ERROR     = 0x0500,  /**< Device write error */
    NDI_READ_ERROR      = 0x0600,  /**< Device read error */
    NDI_RESET_FAIL      = 0x0700,  /**< device failed to reset on break */
    NDI_PROBE_FAIL      = 0x0800,  /**< device not found on specified port */
  } ErrorCodeType;

  /** COMM() baud rates */
  typedef enum
  {
    NDI_9600     = 0,          
    NDI_14400    = 1, 
    NDI_19200    = 2,
    NDI_38400    = 3,
    NDI_57600    = 4,
    NDI_115200   = 5,
  } COMMBaudType;

  /** COMM() data bits, parity and stop bits */
  typedef enum
  {
    NDI_8N1  =   0,              /* 8 data bits, no parity, 1 stop bit */
    NDI_8N2  =   1,              /* etc. */
    NDI_8O1  =  10,
    NDI_8O2  =  11,
    NDI_8E1  =  20,
    NDI_8E2  =  21,
    NDI_7N1  = 100,
    NDI_7N2  = 101,
    NDI_7O1  = 110,
    NDI_7O2  = 111,
    NDI_7E1  = 120,
    NDI_7E2  = 121 ,
  } COMMDataType;

  /** COMM() hardware handshaking */
  typedef enum
  {
    NDI_NOHANDSHAKE  = 0,
    NDI_HANDSHAKE    = 1,
  } COMMHandshakeType;

  /** VER() reply mode types */
  typedef enum
  {
    NDI_CONTROL_FIRMWARE          = 0, /**< control firmware */
    NDI_LEFT_SENSOR_FIRMWARE      = 1, /**< left sensor firmware */
    NDI_RIGHT_SENSOR_FIRMWARE     = 2, /**< right sensor firmware */
    NDI_TIU_FIRMWARE              = 3, /**< TIU firmware */
    NDI_CONTROL_FIRMWARE_ENHANCED = 4, /**< control firmware with enhanced versioning */
  } VERModeType;

  /** PHSR() handle types */
  typedef enum
  {
    NDI_ALL_HANDLES            = 0x00,
    NDI_STALE_HANDLES          = 0x01,
    NDI_UNINITIALIZED_HANDLES  = 0x02,
    NDI_UNENABLED_HANDLES      = 0x03,
    NDI_ENABLED_HANDLES        = 0x04,
  } PHSRModeType;

  /** PENA() tracking modes */
  typedef enum
  {
    NDI_STATIC      = 'S',    /**< relatively immobile tool (reference) */ 
    NDI_DYNAMIC     = 'D',    /**< dynamic tool (most tools)*/
    NDI_BUTTON_BOX  = 'B',    /**< tool with no IREDs (foot pedal) */
  } PENATrackingModeType;

  /** TX() reply mode bit definitions */
  typedef enum
  {
    NDI_XFORMS_AND_STATUS  = 0x0001,  /**< transforms and status */
    NDI_ADDITIONAL_INFO    = 0x0002,  /**< additional tool transform info */
    NDI_SINGLE_STRAY       = 0x0004,  /**< stray active marker reporting */
    NDI_PASSIVE_STRAY      = 0x1000,  /**< stray passive marker reporting */
  } TXModeType;

  /** GetTXTransform() return values */
  typedef enum
  {
    NDI_TRANSFORM_OKAY =  0, /**< transform has been returned */
    NDI_DISABLED       =  1, /**< port is disabled */
    NDI_MISSING        =  2, /**< tool is out of view or otherwise missing */
    NDI_UNOCCUPIED     =  3, /**< port is unoccupied */
  } TXTransformType;

  /** GetTXPortStatus() return value bits */
  typedef enum
  {
    NDI_TOOL_IN_PORT        = 0x01,
    NDI_SWITCH_1_ON         = 0x02,
    NDI_SWITCH_2_ON         = 0x04,
    NDI_SWITCH_3_ON         = 0x08,
    NDI_INITIALIZED         = 0x10,
    NDI_ENABLED             = 0x20,
    NDI_OUT_OF_VOLUME       = 0x40,
    NDI_PARTIALLY_IN_VOLUME = 0x80,
  } TXPortStatusType;

  /** GetTXSystemStatus() return value bits */
  typedef enum
  {
    NDI_COMM_SYNC_ERROR            = 0x0001,
    NDI_TOO_MUCH_EXTERNAL_INFRARED = 0x0002,
    NDI_COMM_CRC_ERROR             = 0x0004,
    NDI_COMM_RECOVERABLE           = 0x0008,
    NDI_HARDWARE_FAILURE           = 0x0010,
    NDI_HARDWARE_CHANGE            = 0x0020,
    NDI_PORT_OCCUPIED              = 0x0040,
    NDI_PORT_UNOCCUPIED            = 0x0080,
  } TXSystemStatusType;

  /** GetTXToolInfo() return value bits */
  typedef enum
  {
    NDI_BAD_TRANSFORM_FIT   = 0x01,
    NDI_NOT_ENOUGH_MARKERS  = 0x02,
    NDI_TOOL_FACE_USED      = 0x70,
  } TXToolInfoType;

  /** GetTXMarkerInfo() return value bits */
  typedef enum
  {
    NDI_MARKER_MISSING             = 0,
    NDI_MARKER_EXCEEDED_MAX_ANGLE  = 1,
    NDI_MARKER_EXCEEDED_MAX_ERROR  = 2,
    NDI_MARKER_USED                = 3,
  } TXMarkerInfoType;

  /** LED() states */
  typedef enum
  {
    NDI_BLANK = 'B',    /**< off */
    NDI_FLASH = 'F',    /**< flashing */
    NDI_SOLID = 'S',    /**< on */
  } LEDStateType;

  /** PSOUT() states for General Purpose Input Output (GPIO)*/
  typedef enum
  {
    NDI_GPIO_OFF       = 'O',    /**< off */
    NDI_GPIO_SOLID     = 'S',    /**< solid */
    NDI_GPIO_PULSE     = 'P',    /**< pulse */
    NDI_GPIO_NO_CHANGE = 'N',    /**< no change */
  } PSOUTStateType;

  /** PHINF() reply mode bits */
  typedef enum
  {
    NDI_BASIC           = 0x0001,
    NDI_TESTING         = 0x0002,
    NDI_PART_NUMBER     = 0x0004,
    NDI_ACCESSORIES     = 0x0008,
    NDI_MARKER_TYPE     = 0x0010,
    NDI_PORT_LOCATION   = 0x0020,
    NDI_GPIO_STATUS     = 0x0040,
  } PHINFModeType;

  /** GetPHINFAccessories() return value bits */
  typedef enum
  {
    NDI_TOOL_IN_PORT_SWITCH   = 0x01,  /**< tool has tool-in-port switch */
    NDI_SWITCH_1              = 0x02,  /**< tool has button #1 */
    NDI_SWITCH_2              = 0x04,  /**< tool has button #2 */
    NDI_SWITCH_3              = 0x08,  /**< tool has button #3 */
    NDI_TOOL_TRACKING_LED     = 0x10,  /**< tool has tracking LED */
    NDI_LED_1                 = 0x20,  /**< tool has LED #1 */
    NDI_LED_2                 = 0x40,  /**< tool has LED #2 */
    NDI_LED_3                 = 0x80,  /**< tool has LED #3 */
  } PHINFAccessoriesType;
  
  /** GetPHINFMarkerType() return value */
  typedef enum
  {
    NDI_950NM            = 0x00,
    NDI_850NM            = 0x01,
    NDI_NDI_ACTIVE       = 0x08,
    NDI_NDI_CERAMIC      = 0x10,
    NDI_PASSIVE_ANY      = 0x20,
    NDI_PASSIVE_SPHERE   = 0x28,
    NDI_PASSIVE_DISC     = 0x30,
  } PHINFMarkerType;

  /** Tool type specifiers stored in the second byte returned by
      GetPHINFToolInfo() */
  typedef enum
  {
    NDI_TYPE_REFERENCE   = 0x01,
    NDI_TYPE_POINTER     = 0x02,
    NDI_TYPE_BUTTON      = 0x03,
    NDI_TYPE_SOFTWARE    = 0x04,
    NDI_TYPE_MICROSCOPE  = 0x05,
    NDI_TYPE_DOCK        = 0x08,
    NDI_TYPE_CARM        = 0x0A,
    NDI_TYPE_CATHETER    = 0x0B,
  } PHINFToolInfoType;

  /** SSTAT() reply format bits */
  typedef enum
  {
    NDI_CONTROL         = 0x0001,  /**< control processor information */
    NDI_SENSORS         = 0x0002,  /**< sensor processors */
    NDI_TIU             = 0x0004,  /**< TIU processor */
  } SSTATModeType;

  /** GetSSTATControl() return value bits */
  typedef enum
  {
    NDI_EPROM_CODE_CHECKSUM     = 0x01,
    NDI_EPROM_SYSTEM_CHECKSUM   = 0x02,
  } SSTATControlType;

 /** GetSSTATSensor() return value bits */
  typedef enum
  {
    NDI_LEFT_ROM_CHECKSUM   = 0x01,
    NDI_LEFT_SYNC_TYPE_1    = 0x02,
    NDI_LEFT_SYNC_TYPE_2    = 0x04,
    NDI_RIGHT_ROM_CHECKSUM  = 0x10,
    NDI_RIGHT_SYNC_TYPE_1   = 0x20,
    NDI_RIGHT_SYNC_TYPE_2   = 0x40,
  } STATSensorType;

  /** GetSSTATTIU() return value bits */
  typedef enum
  {
    NDI_ROM_CHECKSUM        = 0x01,
    NDI_OPERATING_VOLTAGES  = 0x02,
    NDI_MARKER_SEQUENCING   = 0x04,
    NDI_SYNC                = 0x08,
    NDI_COOLING_FAN         = 0x10,
    NDI_INTERNAL_ERROR      = 0x20,
  } STATTIUType;

  /** IRCHK() reply mode bits */
  typedef enum
  {
    NDI_DETECTED   = 0x0001,   /**< simple yes/no whether IR detected */
    NDI_SOURCES    = 0x0002,   /**< locations of up to 20 sources per camera */
  } IRCHKModeType;

  /** GetIRCHKNumberOfSources(), GetIRCHKSourceXY() sensor arguments */
  typedef enum
  {
    NDI_LEFT   = 0,            /**< left sensor */
    NDI_RIGHT  = 1,            /**< right sensor */
  } IRCHKSensorType;

  /** SFLIST() reply mode */
  typedef enum
  {
    NDI_FEATURE_SUMMARY = 0x00, /**< feature summary */
    NDI_ACTIVE_PORTS    = 0x01, /**< number of active tool ports */
    NDI_PASSIVE_PORTS   = 0x02, /**< number of passive tool ports  */
    NDI_VOLUMES         = 0x03, /**< list of volumes available (see NDI docs)*/
    NDI_TIP_PORTS       = 0x04, /**< number of ports with current sensing */
  } SFLISTModeType;

  /** SFLIST() summary bits */
  typedef enum
  {
    NDI_SUPPORTS_ACTIVE  = 0x0001, /**< active tool ports are available */
    NDI_SUPPORTS_PASSIVE = 0x0002, /**< passive tool ports are available */
    NDI_SUPPORTS_VOLUMES = 0x0004, /**< multiple volumes are available */
    NDI_SUPPORTS_SENSING = 0x0008, /**< tool-in-port sensing is available */
  } SFLISTSummaryType;

  /** Some required typedefs for itk::Object. */

  typedef NDICommandInterpreter          Self;
  typedef itk::Object                    Superclass; 
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef SerialCommunication            CommunicationType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(NDICommandInterpreter, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** Set the communication object that commands will be sent to */
  void SetCommunication(CommunicationType* communication);

  /**
  Send a text command to the device and receive a text reply.

  \param command the command to send, without the trailing CRC

  \return       the text reply from the device with the 
                CRC chopped off

  The standard format of an NDI API command is, for example, "INIT:" or
  "PENA:AD".  A CRC value and a carriage return will be appended to the
  command before it is sent to the device.
  
  This function will automatically recogize certain commands and behave
  accordingly:
  - 0 - A serial break will be sent to the device if the command is a
        null string
  - "COMM:" - After the COMM() is sent, the host computer serial port is
           adjusted to match the device.
  - "PHSR:" - The information returned by the PHSR() command is stored and can
           be retrieved though the GetPHSR() functions.
  - "PHINF:" - The information returned by the PHINF() command is stored and
           can be retrieved though the GetPHINF() functions.
  - "TX:"   - The information returned by the TX() command is stored and can
           be retrieved though the GetTX() functions.
  - "SSTAT:" - The information returned by the SSTAT() command is stored and
           can be retrieved through one of the GetSSTAT() functions.    
  - "IRCHK:" - The information returned by the IRCHK() command is stored and
           can be retrieved through the GetIRCHK() functions.

  <p>The GetError() function can be used to check whether an error
  occured.
  */
  const char *Command(const char *command);


  /**
  Cause the device to beep.

  \param n   the number of times to beep, an integer between 1 and 9

  This command can be used in tracking mode.
  */
  void BEEP(int n) {
    this->Command("BEEP:%i", n % 10); }

  /**
  Change the device communication parameters.  The host parameters
  will automatically be adjusted to match.  If the specified baud rate is
  not supported by the serial port, then the error code will be set to
  NDI_BAD_COMM and the device will have to be reset before
  communication can continue.  Most modern UNIX systems accept all baud
  rates except 14400, and Windows systems support all baud rates.

  \param baud        one of NDI_9600, NDI_14400, NDI_19200, NDI_38400,
                     NDI_57600, NDI_115200
  \param dps         should usually be NDI_8N1, the most common mode
  \param handshake   one of NDI_HANDSHAKE or NDI_NOHANDSHAKE
  */
  void COMM(COMMBaudType baud, COMMDataType dps, COMMHandshakeType handshake) {
    this->Command("COMM:%d%03d%d", baud, dps, handshake); }

  /**
  Put the device into diagnostic mode.  This must be done prior
  to executing the IRCHK() command.  Diagnostic mode is only useful on the POLARIS.
  */
  void DSTART() {
    this->Command("DSTART:"); }

  /**
  Take the device out of diagnostic mode.
  */
  void DSTOP() {
    this->Command("DSTOP:"); }

  /**
  Initialize the device.  The device must be
  initialized before any other commands are sent.
  */
  void INIT() {
    this->Command("INIT:"); }

  /**
  Check for sources of environmental infrared.

  This command is only valid in diagnostic mode after an IRINIT command.

  \param mode  reply mode bits:
  - NDI_DETECTED   0x0001 - return '1' if IR detected, else '0'
  - NDI_SOURCES    0x0002 - locations of up to 20 sources per camera

  <p>The IRCHK command is used to update the information returned by the
  GetIRCHKDetected() and GetIRCHKSourceXY() functions.
  */
  void IRCHK(int mode) {
    this->Command("IRCHK:%04X", mode); }

  /**
  Initialize the diagnostic environmental infrared checking system.
  This command must be called prior to using the IRCHK command.
  */
  void IRINIT() {
    this->Command("IRINIT:"); }

  /**
  Set a tool LED to a particular state.

  \param ph     valid port handle in the range 0x01 to 0xFF
  \param led    an integer between 1 and 3
  \param state  desired state: NDI_BLANK 'B', NDI_FLASH 'F' or NDI_SOLID 'S'

  This command can be used in tracking mode.
  */
  void LED(int ph, int led, LEDStateType state) {
    this->Command("LED:%02X%d%c", ph, led, state); }

  /**
  Disable transform reporting on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF
  */
  void PDIS(int ph) {
    this->Command("PDIS:%02X", ph); }

  /**
  Enable transform reporting on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF
  \param mode one of NDI_STATIC 'S', NDI_DYNAMIC 'D' or NDI_BUTTON_BOX 'B'
  */
  void PENA(int ph, int mode) {
    this->Command("PENA:%02X%c", ph, mode); }

  /**
  Free the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF 
  */
  void PHF(int ph) {
    this->Command("PHF:%02X", ph); }

  /**
  Ask the device for information about a tool handle.

  \param ph valid port handle in the range 0x01 to 0xFF 

  \param mode  a reply format mode composed of the following bits:
  - NDI_BASIC           0x0001 - get port status and basic tool information
  - NDI_TESTING         0x0002 - get current test for active tools
  - NDI_PART_NUMBER     0x0004 - get a 20 character part number
  - NDI_ACCESSORIES     0x0008 - get a summary of the tool accessories
  - NDI_MARKER_TYPE     0x0010 - get the tool marker type
  - NDI_PORT_LOCATION   0x0020 - get the physical port location
  - NDI_GPIO_STATUS     0x0040 - get AURORA GPIO status

  <p>The use of the PHINF command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref GetPHINFPortStatus()
  - int \ref GetPHINFToolInfo(char information[30])
  - unsigned int \ref GetPHINFCurrentTest()
  - int \ref GetPHINFAccessories()
  - int \ref GetPHINFMarkerType()

  <p>This command is not available during tracking mode.
  */
  void PHINF(int ph, int mode) {
    this->Command("PHINF:%02X%04X", ph, mode); }

  /**
  Request a port handle given specific tool criteria.

  \param  num    8-digit device number or wildcard "********"
  \param  sys    system type TIU "0" or AURORA SCU "1" or wildcard "*"
  \param  tool   wired "0" or wireless "1" or wildcard "*" 
  \param  port   wired "01" to "10", wireless "0A" to "0I" or wildcard "**"
  \param  chan   AURORA tool channel "00" or "01" or wildcard "**"

  <p>The use of the PHRQ command updates the information returned by the
  following commands:
  - int \ref GetPHRQHandle()
  */
  void PHRQ(const char* num, const char* sys, const char* tool,
            const char* port, const char* chan) {
    this->Command("PHRQ:%-8.8s%1.1s%1.1s%2.2s%2.2s", num, sys, tool, port, chan); }

  /**
  List the port handles.

  \param mode   the reply mode:
  - NDI_ALL_HANDLES            0x00 - return all handles
  - NDI_STALE_HANDLES          0x01 - only handles waiting to be freed
  - NDI_UNINITIALIZED_HANDLES  0x02 - handles needing initialization
  - NDI_UNENABLED_HANDLES      0x03 - handles needing enabling
  - NDI_ENABLED_HANDLES        0x04 - handles that are enabled

  <p>The use of the PHSR command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref GetPHSRNumberOfHandles()
  - int \ref GetPHSRHandle(int i)
  - int \ref GetPHSRInformation(int i)

  <p>This command is not available during tracking mode.
  */
  void PHSR(PHSRModeType mode) {
    this->Command("PHSR:%02X", mode); }

  /**
  Initialize the tool on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF 
  */
  void PINIT(int ph) {
    this->Command("PINIT:%02X", ph); }

  /**
  Set the three GPIO wire states for an AURORA tool.
  The states available are NDI_GPIO_NO_CHANGE, NDI_GPIO_SOLID,
  NDI_GPIO_PULSE, and NDI_GPIO_OFF.

  \param ph  valid port handle in the range 0x01 to 0xFF
  \param a   GPIO 1 state
  \param b   GPIO 2 state
  \param c   GPIO 3 state
  */
  void PSOUT(int ph, int a, int b, int c) {
    this->Command("PSOUT:%02X%c%c%c", ph, a, b, c); };

  /**
  Clear the virtual SROM for the specified port.  For a passive tool,
  this is equivalent to unplugging the tool.  This command has been
  deprecated in favor of the PHF() command.

  \param port one of '1', '2', '3' or 'A' to 'I'
  */
  void PVCLR(int port) {
    this->Command("PVCLR:%c", port); }

  /**
  Write to a virtual SROM address on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF 
  \param a an address between 0x0000 and 0x07C0
  \param x 64-byte data array encoded as a 128-character hexadecimal string
  */
  void PVWR(int ph, int a, const char* x) {
    this->Command("PVWR:%02X%04X%.128s", ph, a, x); }

  /**
  Send a serial break to reset the device.  If the reset was not
  successful, the error code will be set to NDI_RESET_FAIL.
  */
  void RESET() {
    this->Command(0); }

  /**
  Get a feature list for this device.

  \param mode  the desired reply mode
  - NDI_FEATURE_SUMMARY  0x00 - 32-bit feature summary as 8 hexadecimal digits
  - NDI_ACTIVE_PORTS     0x01 - number of active tool ports as a single digit
  - NDI_PASSIVE_PORTS    0x02 - number of passive tool ports as a single digit
  - NDI_VOLUMES          0x03 - list of volumes available (see NDI docs)
  - NDI_TIP_PORTS        0x04 - number of ports supporting tool-in-port sensing

  <p>The use of the SFLIST command with the appropriate reply format updates
  the information returned by the following commands:
  */
  void SFLIST(SFLISTModeType mode) {
    this->Command("SFLIST:%02X", mode); }

  /**
  Request status information from the device.

  \param mode  a reply format mode composed of the following bits:
  - NDI_CONTROL  0x0001 - control processor information
  - NDI_SENSORS  0x0002 - sensor processors
  - NDI_TIU      0x0004 - TIU processor

  <p>The use of the SSTAT command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref GetSSTATControl()
  - int \ref GetSSTATSensors()
  - int \ref GetSSTATTIU()

  <p>This command is not available during tracking mode.
  */
  void SSTAT(int mode) {
    this->Command("SSTAT:%04X", mode); }

  /**
  Put the device into tracking mode.
  */
  void TSTART() {
    this->Command("TSTART:"); }

  /**
  Take the device out of tracking mode.
  */
  void TSTOP() {
    this->Command("TSTOP:"); }

  /**
  Request tracking information from the device.  This command is
  only available in tracking mode.

  \param mode a reply mode containing the following bits:
  - NDI_XFORMS_AND_STATUS  0x0001 - transforms and status
  - NDI_ADDITIONAL_INFO    0x0002 - additional tool transform info
  - NDI_SINGLE_STRAY       0x0004 - stray active marker reporting
  - NDI_PASSIVE_STRAY      0x1000 - stray passive marker reporting

  <p>The TX command with the appropriate reply mode is used to update the
  data that is returned by the following functions:
  - int \ref GetTXTransform(int ph, double transform[8])
  - int \ref GetTXPortStatus(int ph)
  - unsigned int \ref GetTXFrame(int ph)
  - int \ref GetTXToolInfo(int ph)
  - int \ref GetTXMarkerInfo(int ph, int marker)
  - int \ref GetTXSingleStray(int ph, double coord[3])
  - int \ref GetTXNumberOfPassiveStrays()
  - int \ref GetTXPassiveStray(int i, double coord[3])
  - int \ref GetTXSystemStatus()
  */
  void TX(int mode) {
    this->Command("TX:%04X", mode); }

  /**
  Get a string that describes the device firmware version.

  \param n   the processor to get the firmware revision of:
  - NDI_CONTROL_FIRMWARE          0 - control firmware
  - NDI_LEFT_SENSOR_FIRMWARE      1 - left sensor firmware
  - NDI_RIGHT_SENSOR_FIRMWARE     2 - right sensor firmware
  - NDI_TIU_CIRMWARE              3 - TIU firmware
  - NDI_CONTROL_FIRMWARE_ENHANCED 4 - control firmware with enhanced versioning
  */
  const char* VER(VERModeType n) {
    return this->Command("VER:%d", n); }

  /**
  Get error code from the last command.  An error code of NDI_OKAY signals
  that no error occurred.  The error codes are listed in \ref ErrorCodeType.
  */
  int GetError() const;

  /**
  Get the port handle returned by a PHRQ() command.

  \return  a port handle between 0x01 and 0xFF

  <p>An SROM can be written to the port handle with the PVWR() command.
  */
  int GetPHRQHandle() const;

  /**
  Get the number of port handles as returned by a PHSR() command.

  \return  an integer, the maximum possible value is 255
  */
  int GetPHSRNumberOfHandles() const;

  /**
  Get one of the port handles returned by a PHSR() command.

  \param i         a value between 0 and \em n where \em n is the
                   value returned by GetPHSRNumberOfHandles().

  \return  a port handle between 0x01 and 0xFF

  <p>The PHINF() command can be used to get detailed information about the
   port handle.
  */
  int GetPHSRHandle(int i) const;

  /**
  Get the information for a port handle returned by a PHSR() command.

  \param i         a value between 0 and \em n where \em n is the
                   value returned by GetPHSRNumberOfHandles().

  \return  a 12-bit bitfield where the following bits are defined:
  - NDI_TOOL_IN_PORT        0x01 - there is a tool in the port
  - NDI_SWITCH_1_ON         0x02 - button 1 is pressed
  - NDI_SWITCH_2_ON         0x04 - button 2 is pressed
  - NDI_SWITCH_3_ON         0x08 - button 3 is pressed
  - NDI_INITIALIZED         0x10 - tool port has been initialized
  - NDI_ENABLED             0x20 - tool port is enabled for tracking
  - NDI_CURRENT_DETECT      0x80 - tool sensed through current detection

  <p>The PHINF() command can be used to get detailed information about the
   port handle.
  */
  int GetPHSRInformation(int i) const;

  /**
  Get the 8-bit status value for the port handle.

   \return an integer composed of the following bits:
  - NDI_TOOL_IN_PORT        0x01 - there is a tool in the port
  - NDI_SWITCH_1_ON         0x02 - button 1 is pressed
  - NDI_SWITCH_2_ON         0x04 - button 2 is pressed
  - NDI_SWITCH_3_ON         0x08 - button 3 is pressed
  - NDI_INITIALIZED         0x10 - tool port has been initialized
  - NDI_ENABLED             0x20 - tool port is enabled for tracking
  - NDI_CURRENT_DETECT      0x80 - tool sensed through current detection

  <p>The return value is updated only when a PHINF() command is sent with
  the NDI_BASIC (0x0001) bit set in the reply mode.
  */
  int GetPHINFPortStatus() const;

  /**
  Get a 31-byte string describing the tool.

  \param information array that information is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - NDI_OKAY - information was returned
  - NDI_UNOCCUPIED - port is unoccupied or no information is available

  <p>The returned string will not be null-terminated by default.  You
  must set information[31] to 0 in order to terminate the string.
  If the port is unoccupied then the contents of the \em information
  string are undefined.
  
  The information is updated only when a PHINF() command is sent with
  the NDI_BASIC (0x0001) bit set in the reply mode.
  */
  int GetPHINFToolInfo(char information[31]) const;

  /**
  Return the results of a current test on the IREDS on an active 
  POLARIS tool.

  \return 32-bit integer (see NDI documentation)

  The information is updated only when a PHINF() command is sent with
  the NDI_TESTING (0x0002) bit set in the reply mode.
  */
  unsigned int GetPHINFCurrentTest() const;

  /**
  Get a 20-byte string that contains the part number of the tool.

  \param part        array that part number is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - NDI_OKAY - information was returned
  - NDI_UNOCCUPIED - port is unoccupied or no information is available

  <p>If a terminated string is required, then set part[20] to 0
  before calling this function.

  The information is updated only when a PHINF() command is sent with
  the NDI_PART_NUMBER (0x0004) bit set in the reply mode.
  */
  int GetPHINFPartNumber(char part[20]) const;

  /**
  Get the 8-bit value specifying the tool accessories.

  \return an integer composed of the following bits:
  - NDI_TOOL_IN_PORT_SWITCH   0x01  - tool has tool-in-port switch
  - NDI_SWITCH_1              0x02  - tool has button 1
  - NDI_SWITCH_2              0x04  - tool has button 2
  - NDI_SWITCH_3              0x08  - tool has button 3
  - NDI_TOOL_TRACKING_LED     0x10  - tool has tracking LED
  - NDI_LED_1                 0x20  - tool has LED 1
  - NDI_LED_2                 0x40  - tool has LED 2
  - NDI_LED_3                 0x80  - tool has LED 3

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.

  The return value is updated only when a PHINF() command is sent with
  the NDI_ACCESSORIES (0x0008) bit set in the reply mode.
  */
  int GetPHINFAccessories() const;

  /**
  Get an 8-bit value describing the marker type for the tool.
  The low three bits descibe the wavelength, and the high three
  bits are the marker type code.

  \return  see NDI documentation for more information:
  - low bits:
    - NDI_950NM            0x00
    - NDI_850NM            0x01
  - high bits:
    - NDI_NDI_ACTIVE       0x08
    - NDI_NDI_CERAMIC      0x10
    - NDI_PASSIVE_ANY      0x20
    - NDI_PASSIVE_SPHERE   0x28
    - NDI_PASSIVE_DISC     0x30

  <p>The return value is updated only when a PHINF() command is sent with
  the NDI_MARKER_TYPE (0x0010) bit set in the reply mode.
  */
  int GetPHINFMarkerType() const;

  /**
  Get a 14-byte description of the physical location of the tool
  on the system.

  \return  see NDI documentation for more information:
  - 8 chars: device number
  - 1 char: 0 = POLARIS, 1 = AURORA
  - 1 char: 0 = wired, 1 = wireless
  - 2 chars: port number 01-12 or 0A-0I
  - 2 chars: hexadecimal channel number

  <p>The return value is updated only when a PHINF() command is sent with
  the NDI_PORT_LOCATION (0x0020) bit set in the reply mode.
  */
  int GetPHINFPortLocation(char location[14]) const;

  /**
  Get the 8-bit GPIO status for this tool.

  \return  an 8-bit integer, see NDI documentation for more information.

  <p>The return value is updated only when a PHINF() command is sent with
  the NDI_GPIO_STATUS (0x0040) bit set in the reply mode.
  */
  int GetPHINFGPIOStatus() const;

  /**
  Get the transformation for the specified port.
  The first four numbers are a quaternion, the next three numbers are
  the coodinates in millimetres, and the final number
  is a unitless error estimate.

  \param ph        valid port handle in range 0x01 to 0xFF
  \param transform space for the 8 numbers in the transformation
  
  \return one of the following: 
  - NDI_TRANSFORM_OKAY if successful
  - NDI_DISABLED if tool port is nonexistent or disabled
  - NDI_MISSING if tool transform cannot be computed (tool is out of range)
  - NDI_UNOCCUPIED if the tool port is not occupied

  <p>If NDI_DISABLED, NDI_MISSING or NDI_UNOCCUPIED is returned, then
  the values in the supplied transform array will be left unchanged.

  The transformations for each of the port handles remain the same
  until the next TX() command is sent to the device.
  */ 
  int GetTXTransform(int ph, double transform[8]) const;

  /**
  Get the 16-bit status value for the specified port handle.

  \param ph        valid port handle in range 0x01 to 0xFF

  \return status bits or zero if there is no information:
  - NDI_TOOL_IN_PORT        0x0001
  - NDI_SWITCH_1_ON         0x0002
  - NDI_SWITCH_2_ON         0x0004
  - NDI_SWITCH_3_ON         0x0008
  - NDI_INITIALIZED         0x0010
  - NDI_ENABLED             0x0020
  - NDI_OUT_OF_VOLUME       0x0040
  - NDI_PARTIALLY_IN_VOLUME 0x0080

  This information is updated each time that the TX() command
  is sent to the device.
  */
  int GetTXPortStatus(int ph) const;

  /**
  Get the camera frame number for the latest transform.

  \param ph        valid port handle in range 0x01 to 0xFF

  \return a 32-bit frame number, or zero if no information was available

  This information is updated each time that the TX() command
  is sent to the device.
  */
  unsigned int GetTXFrame(int ph) const;

  /**
  Get additional information about the tool transformation.

  \param ph        valid port handle in range 0x01 to 0xFF

  \return status bits, or zero if there is no information available
  - NDI_BAD_TRANSFORM_FIT   0x01
  - NDI_NOT_ENOUGH_MARKERS  0x02
  - NDI_TOOL_FACE_USED      0x70 - 3 bits give 8 possible faces

  <p>The tool information is only updated when the TX() command is called with
  the NDI_ADDITIONAL_INFO (0x0002) mode bit.
  */
  int GetTXToolInfo(int ph) const;

  /**
  Get additional information about the tool markers.

  \param ph        valid port handle in range 0x01 to 0xFF
  \param marker    one of 'A' through 'T' for the 20 markers

  \return status bits, or zero if there is no information available
  - NDI_MARKER_MISSING             0
  - NDI_MARKER_EXCEEDED_MAX_ANGLE  1
  - NDI_MARKER_EXCEEDED_MAX_ERROR  2
  - NDI_MARKER_USED                3  

  <p>The tool marker information is only updated when the TX() command is
  called with the NDI_ADDITIONAL_INFO (0x0002) mode bit set.
  */
  int GetTXMarkerInfo(int ph, int marker) const;

  /**
  Get the coordinates of a stray marker on a wired POLARIS tool.
  This command is only meaningful for tools that have a stray
  marker.

  \param ph        valid port handle in range 0x01 to 0xFF
  \param coord     array to hold the three coordinates

  \return the return value will be one of
  - NDI_OKAY - values returned in coord
  - NDI_DISABLED - port disabled or illegal port specified
  - NDI_MISSING - stray marker is not visible to the device

  <p>The stray marker position is only updated when the TX() command is
  called with the NDI_SINGLE_STRAY (0x0004) bit set.
  */
  int GetTXSingleStray(int ph, double coord[3]) const;

  /**
  Get the number of passive stray markers detected.

  \return          a number between 0 and 20
  
  The passive stray marker coordinates are updated when a TX() command
  is sent with the NDI_PASSIVE_STRAY (0x1000) bit set in the reply mode.
  */
  int GetTXNumberOfPassiveStrays() const;

  /**
  Copy the coordinates of the specified stray marker into the
  supplied array.

  \param i         a number between 0 and 19
  \param coord     array to hold the coordinates
  \return          one of:
  - NDI_OKAY - information was returned in coord
  - NDI_DISABLED - no stray marker reporting is available
  - NDI_MISSING - marker number i is not visible

  <p>Use GetTXNumberOfPassiveStrays() to get the number of stray
  markers that are visible.
  
  The passive stray marker coordinates are updated when a TX() command
  is sent with the NDI_PASSIVE_STRAY (0x1000) bit set in the reply mode.
  */
  int GetTXPassiveStray(int i, double coord[3]) const;

  /**
  Get an 16-bit status bitfield for the system.

  \return status bits or zero if there is no information:
  - NDI_COMM_SYNC_ERROR            0x0001
  - NDI_TOO_MUCH_EXTERNAL_INFRARED 0x0002
  - NDI_COMM_CRC_ERROR             0x0004
  - NDI_COMM_RECOVERABLE           0x0008
  - NDI_HARDWARE_FAILURE           0x0010
  - NDI_HARDWARE_CHANGE            0x0020
  - NDI_PORT_OCCUPIED              0x0040
  - NDI_PORT_UNOCCUPIED            0x0080

  <p>The system stutus information is updated whenever the TX() command is
  called with the NDI_XFORMS_AND_STATUS (0x0001) bit set in the reply mode.
  */
  int GetTXSystemStatus() const;

  /**
  Get the status of the control processor.

  \return an int with the following bit definitions for errors:
  - NDI_EPROM_CODE_CHECKSUM     0x01
  - NDI_EPROM_SYSTEM_CHECKSUM   0x02

  <p>This information is updated only when the SSTAT() command is sent
  with the NDI_CONTROL (0x0001) bit set in the reply mode.
  */
  int GetSSTATControl() const;

  /**
  Get the status of the sensor processors.

  \return an int with the following bit definitions for errors:
  - NDI_LEFT_ROM_CHECKSUM   0x01
  - NDI_LEFT_SYNC_TYPE_1    0x02
  - NDI_LEFT_SYNC_TYPE_2    0x04
  - NDI_RIGHT_ROM_CHECKSUM  0x10
  - NDI_RIGHT_SYNC_TYPE_1   0x20
  - NDI_RIGHT_SYNC_TYPE_2   0x40

  <p>This information is updated only when the SSTAT() command is sent
  with the NDI_SENSORS (0x0002) bit set in the reply mode.
  */
  int GetSSTATSensors() const;

  /**
  Get the status of the sensor processors.

  \return an int with the following bit definitions for errors:
  - NDI_ROM_CHECKSUM        0x01
  - NDI_OPERATING_VOLTAGES  0x02
  - NDI_MARKER_SEQUENCING   0x04
  - NDI_SYNC                0x08
  - NDI_COOLING_FAN         0x10
  - NDI_INTERNAL_ERROR      0x20

  <p>This information is updated only when the SSTAT() command is sent
  with the NDI_TIU (0x0004) bit set in the reply mode.
  */
  int GetSSTATTIU() const;

  /**
  Check to see whether environmental infrared was detected.
 
  \return       1 if infrared was detected and 0 otherwise.

  This information is only updated if the IRCHK() command is called
  with the NDI_DETECTED (0x0001) format bit set.
  */
  int GetIRCHKDetected() const;

  /**
  Get the number of infrared sources seen by one of the two sensors.

  \param side   one of NDI_LEFT or NDI_RIGHT

  return  the number of infrared sources seen by the specified sensor

  This information is valid only immediately after the IRCHK() command
  has been called with the NDI_SOURCES (0x0002) format bit set.  Otherwise,
  the return value will be zero.
  */
  int GetIRCHKNumberOfSources(int side) const;

  /**
  Get the coordinates of one of the infrared sources seen by one of
  the two sensors. 

  \param side   one of NDI_LEFT or NDI_RIGHT
  \param i      the source to get the coordinates for
  \param xy     space to store the returned coordinates

  return  NDI_OKAY or NDI_MISSING

  If there is no available information, then the xy array will be left
  unchanged and the value NDI_MISSING will be returned.  Otherwise,
  the return value will be NDI_OKAY.

  This information is valid only immediately after the IRCHK() command
  has been called with the NDI_SOURCES (0x0002) format bit set.
  */
  int GetIRCHKSourceXY(int side, int i, double xy[2]) const;

  /**
  Convert an error code returned by GetError() into a string that
  describes the error.

  An unrecognized error code will return "Unrecognized error code".
  */
  static const char* ErrorString(int errnum);

  /**
  This function is used to convert raw binary data into a stream of
  hexadecimal digits that can be sent to the device.
  The length of the output string will be twice the number of bytes
  in the input data, since each byte will be represented by two
  hexadecimal digits.

  As a convenience, the return value is a pointer to the hexadecimal
  string.  If the string must be terminated, then set cp[2*n] to 0
  before calling this function, otherwise the string will be left
  unterminated.
  */
  static char* HexEncode(char* cp, const void* data, int n);

  /**
  This function converts a hex-encoded string into binary data.
  This can be used to decode the SROM data sent from the device.
  The length of the input string must be twice the expected number
  of bytes in the output data, since each binary byte is ecoded by
  two hexadecimal digits.

  As a convenience, the return value is a pointer to the decoded data.
  */
  static void* HexDecode(void* data, const char* cp, int n);
  
protected:
  /** Constructor */
  NDICommandInterpreter();

  /** Destructor */
  virtual ~NDICommandInterpreter();

  /** Get the communication object */
  CommunicationType* GetCommunication();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** the communication object */
  CommunicationType::Pointer m_Communication;

  /** command reply -- this is the return value from Command() */

  char *m_CommandReply;                    /* reply without CRC and <CR> */
  char *m_SerialCommand;                   /* raw text to send to device */
  char *m_SerialReply;                     /* raw reply from device */

  /** this is set to 1 during tracking mode */

  bool m_Tracking;                           /* 'is tracking' flag */

  /** error handling information */

  int m_ErrorCode;                         /* error code (zero if no error) */

  /** SSTAT command reply data */

  char m_SSTATControl[2];                  /* control processor status */
  char m_SSTATSensor[2];                   /* sensor processors status */
  char m_SSTATTIU[2];                      /* tiu processor status */

  /** IRCHK command reply data */

  int m_IRCHKDetected;                     /* irchk detected infrared */
  char m_IRCHKSources[128];                /* coordinates of sources */

  /** PHRQ comand reply data */

  char m_PHRQReply[2];

  /** PHSR comand reply data */

  char m_PHSRReply[1284];

  /** PHINF command reply data */

  int m_PHINFUnoccupied;
  char m_PHINFBasic[34];
  char m_PHINFTesting[8];
  char m_PHINFPartNumber[20];
  char m_PHINFAccessories[2];
  char m_PHINFMarkerType[2];
  char m_PHINFPortLocation[14];
  char m_PHINFGPIOStatus[2];

  /** TX command reply data */

  int m_TXNumberOfHandles;
  unsigned char m_TXHandles[NDI_MAX_HANDLES];
  char m_TXTransforms[NDI_MAX_HANDLES][52];
  char m_TXStatus[NDI_MAX_HANDLES][8];
  char m_TXFrame[NDI_MAX_HANDLES][8];
  char m_TXInformation[NDI_MAX_HANDLES][12];
  char m_TXSingleStray[NDI_MAX_HANDLES][24];
  char m_TXSystemStatus[4];

  int m_TXNumberOfPassiveStrays;
  char m_TXPassiveStrayOutOfVolume[14];
  char m_TXPassiveStray[1052];

  /**
  Send a command to the device using printf-style format string.
  */
  const char* Command(const char* format, int a);
  /**
  Send a command to the device using printf-style format string.
  */
  const char* Command(const char* format, int a, int b);
  /**
  Send a command to the device using printf-style format string.
  */
  const char* Command(const char* format, int a, int b, int c);
  /**
  Send a command to the device using printf-style format string.
  */
  const char* Command(const char* format, int a, int b, int c, int d);
  /**
  Send a command to the device using printf-style format string.
  */
  const char* Command(const char* format, int a, int b, const char* c);
  /**
  Send a command to the device using printf-style format string.
  */
  const char* Command(const char* format, const char* a, const char* b,
                      const char* c, const char* d, const char* e);

  /** set error indicator */
  int SetErrorCode(int errnum);

  /** helper functions that set the appropriate ivar as a result of
     information that is received from the AURORA or POLARIS */
  
  void HelperForCOMM(const char* cp, const char* crp);
  void HelperForPHINF(const char* cp, const char* crp);
  void HelperForPHSR(const char* cp, const char* crp);
  void HelperForTX(const char* cp, const char* crp);
  void HelperForIRCHK(const char* cp, const char* crp);
  void HelperForSSTAT(const char* cp, const char* crp);
  void HelperForPHRQ(const char* cp, const char* crp);

  /**
  Convert \em n characters of a hexadecimal string into an unsigned int.
  The conversion halts if a non-hexadecimal digit is found.

  The primary use of this function is decoding replies from the
  device.
  */
  static unsigned int HexadecimalStringToUnsignedInt(const char* cp, int n);

  /**
  Convert \em n characters of a hexadecimal string into an integer..
  The conversion halts if a non-hexadecimal digit is found.

  The primary use of this function is decoding replies from the
  device.
  */
  static int HexadecimalStringToInt(const char* cp, int n);

  /**
  Convert \em n characters of a signed decimal string to a long.
  The first character in the string must be '+' or '-', otherwise
  the result will be zero.

  The primary use of this function is decoding replies from the
  device.
  */
  static int SignedStringToInt(const char* cp, int n);

  NDICommandInterpreter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

}
#endif