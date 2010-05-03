/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  implementation of MDMEncryptionUtilInterface. 
*
*/

#ifndef DMENCRYPTIONUTIL_H_
#define DMENCRYPTIONUTIL_H_

#include <dmencryptionutilinterface.h>

class CDMEncryptionUtil : public CBase, public MDMEncryptionUtilInterface
    {
public:
    // Instantiates an object of this type  
    /**
   * Create instance of CDMEncryptionUtil, this NewL always loads only the secenv hardware encryption plugin.
   * @param Nil
   * @return Instance of CDMEncryptionUtil
   */
    static CDMEncryptionUtil* NewL();
    /**
    * Create instance of CDMEncryptionUtil, this NewL always loads the plugin which matches with the default_data.
    * @param aData is the value of default_data.
    * @return Instance of CDMEncryptionUtil
    *       Leaves with KErrBadName: if the default_data exists more than 1.                  
    */
    static CDMEncryptionUtil* CDMEncryptionUtil::NewL(const TDesC8& aData);
    
    /**
   * C++ Destructor
   */
    virtual ~CDMEncryptionUtil();    
    
private:
    // Unique instance identifier key
    TUid iDtor_ID_Key;

    };
    
#include "dmencryptionutil.inl"


#endif /* DMENCRYPTIONUTIL_H_ */
