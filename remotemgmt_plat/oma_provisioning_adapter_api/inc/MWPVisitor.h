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
* Description:  Declares visitor interface for Provisioning data.
*
*/


#ifndef MWPVISITOR_H
#define MWPVISITOR_H

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CWPParameter;

// CLASS DECLARATION

/**
 * MWPVisitor has to be implemented by visitors.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class MWPVisitor 
    {
    public:
        /**
        * Called for each characteristic found.
        * @param aCharacteristic The characteristic found
        */
        virtual void VisitL(CWPCharacteristic& aCharacteristic) = 0;
        
        /**
        * Called for each parameter found.
        * @param aParameter The parameter found
        */
        virtual void VisitL(CWPParameter& aParameter) = 0;

        /**
        * Called for each link to a logical proxy or access point.
        * @param aCharacteristic The characteristic found by following the link.
        */
        virtual void VisitLinkL(CWPCharacteristic& aCharacteristic ) = 0;
    };

#endif /* MWPVISITOR_H */
