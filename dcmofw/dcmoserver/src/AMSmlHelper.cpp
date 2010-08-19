/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */

#include "amsmlhelper.h"
#include <SyncMLClientDM.h>
#include <rconnmon.h> 
#include <es_sock.h>
#include <es_enum_internal.h>
#include <es_sock_partner.h>
#include <es_enum.h>
#include "lawmodebug.h"

const TUid KUidNSmlMediumTypeInternet =
    {
    0x101F99F0
    };
const TUid KSosServerUid =
    {
    0x101F99FB
    };

const TInt KBufSize32 = 32;

void SmlHelper::GetDefaultIAPFromDMProfileL(TInt& aDefaultIAP)
    {
    RLDEBUG("ApplicationManagementServer::GetDefaultIAPFromDMProfileL() - start");

    // first try to obtain the current profile id
    RSyncMLSession ses;
    ses.OpenL();
    RLDEBUG("	1/8");
    CleanupClosePushL(ses);

    TSmlJobId jid;
    TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype);
    RLDEBUG("	2/8");
    RSyncMLDevManJob job;
    job.OpenL(ses, jid);
    RLDEBUG("	3/8");
    CleanupClosePushL(job);
    // get profile id
    TSmlProfileId pid(job.Profile() );
    CleanupStack::PopAndDestroy( &job);

    TBuf8<KBufSize32> key;
    TBuf<KBufSize32> value;
    TInt num = KErrNotFound;

    // get connection property name first
    GetConnectionPropertyNameL(ses, key, EPropertyIntenetAccessPoint);
    RLDEBUG("	4/8");
    // we need to open sync profile in ordere to open the RSyncMLConnection
    RSyncMLDevManProfile syncProfile;
    syncProfile.OpenL(ses, pid);
    RLDEBUG("	5/8");
    CleanupClosePushL(syncProfile);

    TSmlTransportId transport;
    RArray<TSmlTransportId> connections;
    _LIT8( KNSmlIAPId, "NSmlIapId" );

    // now open the syncML connection
    RSyncMLConnection syncConnection;
    syncProfile.ListConnectionsL(connections);

    transport = connections[0];
    syncConnection.OpenL(syncProfile, transport);
    RLDEBUG("	6/8");
    CleanupClosePushL(syncConnection);

    const TDesC8& source(syncConnection.GetPropertyL(KNSmlIAPId) );
    RLDEBUG("	7/8");
    value.Copy(source.Left(value.MaxLength() ) );
    User::LeaveIfError(StrToInt(value, num) );
    RLDEBUG("	8/8");

    aDefaultIAP = num;

    // If there is no default Access point in DM profile then use
    // currently used access point by DM session

    if (aDefaultIAP == -1)
        {
        RLDEBUG("adefault iap -1");
        TInt sockIapid = -1;

        RSocketServ serv;
        CleanupClosePushL(serv);
        User::LeaveIfError(serv.Connect() );

        RConnection conn;
        CleanupClosePushL(conn);
        User::LeaveIfError(conn.Open(serv) );
        RLDEBUG("RConnection opened");
        
        TUint count( 0);
        User::LeaveIfError(conn.EnumerateConnections(count) );
        RLDEBUG_2("RConnection count %d",count);
        // enumerate connections
        for (TUint idx=1; idx<=count; ++idx)
            {
            TConnectionInfo connectionInfo;
            TConnectionInfoBuf connInfo(connectionInfo);
            RLDEBUG("RConnection loop");
            TInt err = conn.GetConnectionInfo(idx, connInfo); // iapid
            RLDEBUG("RConnection get conn info");
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy( 2); // conn, serv
                User::Leave(err);
                }
            // enumerate connectionclients
            TConnectionEnumArg conArg;
            conArg.iIndex = idx;
            TConnEnumArgBuf conArgBuf(conArg);
            RLDEBUG("RConnection control");
            err=conn.Control(KCOLConnection, KCoEnumerateConnectionClients,
                    conArgBuf);
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy( 2); // conn, serv
                User::Leave(err);
                }
            TInt cliCount = conArgBuf().iCount;
            RLDEBUG("RConnection second loop");
            for (TUint j=1; j<=cliCount; ++j)
                {
                TConnectionGetClientInfoArg conCliInfo;
                conCliInfo.iIndex = j;
                TConnGetClientInfoArgBuf conCliInfoBuf(conCliInfo);
                RLDEBUG("RConnection another control");
                err=conn.Control(KCOLConnection, KCoGetConnectionClientInfo,
                        conCliInfoBuf);

                if (err != KErrNone)
                    {
                    CleanupStack::PopAndDestroy( 2); // conn, serv
                    User::Leave(err);
                    }
                TConnectionClientInfo conCliInf = conCliInfoBuf().iClientInfo;
                TUid uid = conCliInf.iUid;
                RLDEBUG_2("RConnection check uid %d", uid.iUid);
                if (uid == KSosServerUid)
                    {
                    sockIapid = connInfo().iIapId;
                    RLDEBUG("RConnection uid matched");
                    }

                }
            }
        CleanupStack::PopAndDestroy( 2); // conn, serv		

        aDefaultIAP = sockIapid;
        RLDEBUG("RConnection out");
        }
    // get profile's server id
    //aServerId = syncProfile.ServerId().AllocL();
    RLDEBUG("RConnection total out");

    CleanupStack::PopAndDestroy( &syncConnection);
    CleanupStack::PopAndDestroy( &syncProfile);

    CleanupStack::PopAndDestroy( &ses);
    connections.Close();
    RLDEBUG("ApplicationManagementServer::GetDefaultIAPFromDMProfileL() - end");
    }

void SmlHelper::GetConnectionPropertyNameL(RSyncMLSession& aSyncSession,
        TDes8& aText, TInt aPropertyPos)
    {
    RLDEBUG("ApplicationManagementServer::GetConnectionPropertyNameL() - start");

    // at the moment RSyncMLTransport is only needed
    // for internet connection settings
    RSyncMLTransport transport;
    CleanupClosePushL(transport);
    transport.OpenL(aSyncSession, KUidNSmlMediumTypeInternet.iUid);

    const CSyncMLTransportPropertiesArray& arr = transport.Properties();
    RLDEBUG("GetConnectionPropertyNameL interior");
    __ASSERT_DEBUG( arr.Count()> aPropertyPos, User::Panic(_L("invalid count"), KErrGeneral) );

    const TSyncMLTransportPropertyInfo& info = arr.At(aPropertyPos);
    aText = info.iName;
    CleanupStack::PopAndDestroy( &transport);

    RLDEBUG("ApplicationManagementServer::GetConnectionPropertyNameL() - end");
    }

TInt SmlHelper::StrToInt(const TDesC& aText, TInt& aNum)
    {
    TLex lex(aText);
    TInt err = lex.Val(aNum);
    return err;
    }

// End of File
