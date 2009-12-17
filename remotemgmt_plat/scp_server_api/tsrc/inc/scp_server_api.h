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



#ifndef __SCP_SERVER_API_H__
#define __SCP_SERVER_API_H__

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


_LIT( Kscp_server_apiLogPath, "\\logs\\testframework\\scp_server_api\\" ); 

_LIT( Kscp_server_apiLogFile, "scp_server_api.txt" ); 


class Cscp_server_api;


NONSHARABLE_CLASS(Cscp_server_api) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cscp_server_api* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cscp_server_api();

    public: // New functions

     


        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions



    protected:  // Functions from base classes



    private:

        /**
        * C++ default constructor.
        */
        Cscp_server_api( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();

        virtual TInt ExampleL( CStifItemParser& aItem );
        
        TInt ConstructorL();
		TInt ConnectL();
		TInt VersionL();
		TInt GetCodeL();
		TInt SetCodeL();
		TInt ChangeCodeL();
		TInt SetPhoneLockL();
		TInt GetLockStateL();
		TInt QueryAdminCmdL();
		TInt SetParamL();
		TInt GetParamL();
		TInt AuthenticateS60L();
		TInt CodeChangeQueryL();
		TInt CheckConfigL();
        TInt PerformCleanUpL(CStifItemParser& aItem );
        
    };

#endif      // __SCP_SERVER_API_H__

// End of File
