/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_Communication_h_
#define __igstk_Communication_h_

namespace igstk
{
/** \class Communication
    \brief Class Communication is the base class for communication between
    the tracker class and the hardware tracking device. This communication
    may be established via serial port, parallel port, ethernet, or an 
    offline communication (through a file or class emulating tracker response).
    Classes specific to each such communication would be derived from this 
    base Communication class.
*/

class Communication
{
public:
    Communication(void);
    virtual ~Communication(void);

    /** The method OpenCommunication sets up communication as per the data
    provided. */
    void OpenCommunication( const void *data );

    /** The method CloseCommunication closes the communication. */
    void CloseCommunication( void );

private:

    /** Number of tries before quitting */
    unsigned int  m_CommunicationTimeOut;

};

}
#endif //__igstk_Communication_h_