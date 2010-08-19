/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Device Info Extension Data Container Plugin header file.
*
*/

#ifndef __NSMLDEVINFEXTDATACONTAINERPLUGIN_H__
#define __NSMLDEVINFEXTDATACONTAINERPLUGIN_H__


//  EXTERNAL INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <ecom/ecomresolverparams.h>

// CONSTANTS
const TUid KExtDataContainerInterfaceUid = { 0x2002DC7C };

//  CLASS DEFINITION
/**
 * Interface definition for Device Info Extension Data Container plugin
 */
class CNSmlDevInfExtDataContainerPlugin : public CBase
    {
    
    public:  // Constructor & Destructor

        /**
         * Destructor
         */
        virtual ~CNSmlDevInfExtDataContainerPlugin();

        /**
         * Constructor for plugin instantiation
         */
        static CNSmlDevInfExtDataContainerPlugin* NewL();

    public:  //  Pure virtual methods to be implemented by the actual plugins
        /**
         * Resolve number of device info extensions <Ext>
         *
         * @return Number of extensions
         */
        virtual TInt GetExtensionCountL() = 0;

        /**
         * Resolve name <XNam> of the extension in the given extension index. 
         * The number of extensions supported can be deduced using 
         * GetExtensionCountL() method. If the method is called with an index
         * out of range (i.e. negative value or value greater than/equal to 
         * the value returned by GetExtensionCountL()), the method should
         * leave with KErrArgument.
         * 
         * Note that any extension has one <XNam> field.
         *
         * @param aExtIndex Index of the extension to be returned.
         * @return Name of the extension
         */
        virtual const TDesC8& GetExtNameL( TInt aExtIndex ) = 0;

        /**
         * Resolve number of values related to particular extension. If the 
         * method is called with an index out of range (i.e. negative value 
         * or value greater than/equal to the value returned by 
         * GetExtensionCountL()), the method should leave with KErrArgument.
         *
         * Note that any extension can have 0...n number of values
         *
         * @param aExtIndex Index of an extension
         * @return Number of available values
         */
        virtual TInt GetExtValueCountL( TInt aExtIndex ) = 0;

        /**
         * Resolve <XVal> field of an extension. If the method is called 
         * with illegal index values (i.e. negative value or with non-existent
         * index), the method should leave with KErrArgument.
         *
         * @param aExtIndex Index of an extension
         * @param aValueIndex Index of a value related to particular extension
         * @return Value on textual format
         */
        virtual const TDesC8& GetExtValueL( 
            TInt aExtIndex, TInt aValueIndex ) = 0;

    private:
        TUid iDtor_ID_Key;  // Instance identifier key
    };

#include "nsmldevinfextdatacontainerplugin.inl" // Base implementations for ECOM

#endif // __NSMLDEVINFEXTDATACONTAINERPLUGIN_H__
