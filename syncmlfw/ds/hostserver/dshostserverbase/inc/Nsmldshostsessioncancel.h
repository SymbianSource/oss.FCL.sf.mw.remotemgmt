/*
* ==============================================================================
*  Name        : nsmldshostsessioncancel.h
*  Part of     : Nokia SyncML / Host Server
*  Interface   : None
*  Description : 
*  Version     : $Revision: 54 $ $Modtime: 2.11.05 8:53 $
*
*  Copyright © 2002-2005 Nokia. All rights reserved.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#ifndef __NSMLDSHOSTSESSIONCANCEL_H__
#define __NSMLDSHOSTSESSIONCANCEL_H__


// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------
#include <e32base.h>
#include <s32mem.h> 
#include <badesca.h>
#include <e32property.h>


// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
class MNSmlDsHostSessionCancelEventHandler;

/**
* An active class to handle user initiated cancel.
*  This class informs session to cancel the outgoing session.
*
*  @since 
*/
class CNSmlHostSessionCancel : public CActive
	{
public:
    static CNSmlHostSessionCancel* NewL( MNSmlDsHostSessionCancelEventHandler* aHandler );
    //Destructure
	~CNSmlHostSessionCancel();
	//Subscribe for the property "Cancel Operation".
    void Subscribe();

private:    
    //Contructure
    CNSmlHostSessionCancel( MNSmlDsHostSessionCancelEventHandler* aHandler );
    //Second phase construction.
    void ConstructL();
    //From CActive class
    void RunL();
    void DoCancel();
    TInt RunError ( TInt aError );

private:
    MNSmlDsHostSessionCancelEventHandler* iHandler;
    RProperty iProperty;
	};


#endif // __NSMLDSHOSTSESSIONCANCEL_H__
