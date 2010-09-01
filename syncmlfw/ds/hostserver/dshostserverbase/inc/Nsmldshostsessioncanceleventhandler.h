/*
* ==============================================================================
*  Name        : Nsmldshostsessioncanceleventhandler.h
*  Part of     : Nokia SyncML / Host Server
*  Description : Header file for MNSmlDsHostSessionCancelEventHandler
*  Version     : 
*
*  Copyright © 2002-2005 Nokia Corporation.
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

#ifndef NSMLDSHOSTSESSIONCANCELEVENTHANDLER_H
#define NSMLDSHOSTSESSIONCANCELEVENTHANDLER_H

#include <e32std.h>
#
// CLASS DECLARATION


/**
* Interface class to handle database events
* 
*/
class MNSmlDsHostSessionCancelEventHandler
	{

	public:
		/**
        * CNSmlHostSessionCancel calls this to inform that host session that user has
        * canceled the opeartion.
        * @return None. 
        */
		virtual void HandleCancelEventL() = 0;
	};
	


#endif // NSMLDSHOSTSESSIONCANCELEVENTHANDLER_H

// End of File
