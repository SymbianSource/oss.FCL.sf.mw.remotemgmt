/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef PNP_TO_PAOS_INTERFACE_H
#define PNP_TO_PAOS_INTERFACE_H

#include <e32base.h>
#include <etelmm.h>

const TInt KMaxVersionStringLength(32);
const TInt KNonceLength(8);
const TInt KMaxKeyInfoLength(20);
_LIT( KPnpUtilDllFileName, "Pnp.dll" );
const TUid KPnpUtilDllUid = { 0x1020433A };

class MPnpToPaosInterface
    {
    public:
        virtual void RESERVED_FUNC() = 0;

        /**
        * Returns PnP version string, max length of version string is KMaxVersionStringLength
        * @return KErrNone, KErrArgument
        */
        virtual TInt Version(TDes& aVersion) = 0;

        /**
        * Creates and stores a new nonce. Leaves with KErrArgument if
        * the nonce does ot fit into aNonce.
        * @param aTimeOut A time-out for nonce validity (not in use!)
        * @param aNonce On return contains the new nonce. Minimum length
        * is KNonceLength.
        */
        virtual void CreateNewNonceL( const TUint aTimeOut, TDes8& aNonce ) = 0;

        /**
        * @param aNonce On return contains the nonce. The maximum length should be
        * KNonceLength. Leaves with KErrArgument if the nonce does ot fit into aNonce.
        */
        virtual void GetNonceL( TDes8& aNonce ) = 0;

        /**
        * @param aKeyInfo On return contains the key info. The maximum length is
        * KMaxKeyInfoLength
        */
        virtual void GetKeyInfoL( TDes8& aKeyInfo ) = 0;

        /**
        * Fetch home network info and store it using SetHomeMccL() and SetHomeMncL()
        */
        virtual void FetchHomeNetworkInfoL() = 0;

        /**
        * Fetch network info and store it using SetNetworkMccL() and SetNetworkMncL()
        */
        virtual void FetchNetworkInfoL() = 0;

        /**
        * Returns the home(sim) MCC stored to shared data / central repository.
        * @return Home MCC
        */ 
        virtual const RMobilePhone::TMobilePhoneNetworkCountryCode HomeMccL() const = 0;

        /**
        * Returns the home(sim) MNC stored to shared data / central repository.
        * @return Home MNC
        */ 
        virtual const RMobilePhone::TMobilePhoneNetworkIdentity HomeMncL() const  = 0;
        
        /**
        * Returns the network MCC stored to shared data / central repository.
        * @return Network MCC
        */ 
        virtual const RMobilePhone::TMobilePhoneNetworkCountryCode NetworkMccL() const  = 0;
        
        /**
        * Stores network MCC to shared data / central repository.
        * @param Network MCC
        */ 
        virtual void SetNetworkMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc ) = 0;
        
        /**
        * Returns the network MNC stored to shared data / central repository.
        * @return Network MNC
        */ 
        virtual const RMobilePhone::TMobilePhoneNetworkIdentity NetworkMncL() const  = 0;
        
        /**
        * Stores network MNC to shared data / central repository.
        * @param Network MNC
        */ 
        virtual void SetNetworkMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc ) = 0;
    };

    
    

#endif // PNP_TO_PAOS_INTERFACE_H
