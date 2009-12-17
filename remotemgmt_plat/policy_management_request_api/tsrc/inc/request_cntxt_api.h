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



#ifndef __request_cntxt_api_H__
#define __request_cntxt_api_H__

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <ssl.h>


_LIT( Krequest_cntxt_apiLogPath, "\\logs\\testframework\\request_cntxt_api\\" ); 

_LIT( Krequest_cntxt_apiLogFile, "request_cntxt_api.txt" ); 


class Crequest_cntxt_api;


NONSHARABLE_CLASS(Crequest_cntxt_api) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Crequest_cntxt_api* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Crequest_cntxt_api();

    public: // New functions

     


        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions



    protected:  // Functions from base classes



    private:

        /**
        * C++ default constructor.
        */
        Crequest_cntxt_api( CTestModuleIf& aTestModuleIf );

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
		TInt DestructorL();
		TInt AddSub_CertificateL();
		TInt AddSub_SidL();
		TInt AddSubjectAttributeL(CStifItemParser& aItem);
		TInt AddActionAttributeL(CStifItemParser& aItem);
		TInt AddResourceAttributeL(CStifItemParser& aItem);
		TInt AddEnvAttrL(CStifItemParser& aItem);
		TInt SetResponseValueL(CStifItemParser& aItem);
		TInt GetResponseValueL(CStifItemParser& aItem);
		
		void CreateCertificate(TCertInfo& aCertInfo);
        
    };

#endif      // __request_cntxt_api_H__

// End of File
