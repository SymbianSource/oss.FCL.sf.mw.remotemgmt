/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



#ifndef __DMCERTAPI_H__
#define __DMCERTAPI_H__

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


_LIT( KdmcertapiLogPath, "\\logs\\testframework\\dmcertapi\\" ); 

_LIT( KdmcertapiLogFile, "dmcertapi.txt" ); 


class Cdmcertapi;


NONSHARABLE_CLASS(Cdmcertapi) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cdmcertapi* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cdmcertapi();

    public: // New functions

     


        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions



    protected:  // Functions from base classes



    private:

        /**
        * C++ default constructor.
        */
        Cdmcertapi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();

        virtual TInt ExampleL( CStifItemParser& aItem );
        
        TInt GetCertificateL();
        
    };

#endif      // __dmcertapi_H__

// End of File
