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
* Description:  SyncML Obex client
*
*/


#include <bt_sock.h>
#include "NSmlSdpAttributeParser.h"
#include "NSmlSdpAttributeNotifier.h"


//------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeParser
//------------------------------------------------------------
TSdpAttributeParser::TSdpAttributeParser(
    TSdpAttributeList& aNodeList, 
    MSdpAttributeNotifier& aObserver
) 
:   iObserver(aObserver),
    iNodeList(aNodeList),
    iCurrentNodeIndex(0)
    {
    // no implementation required
    }
//------------------------------------------------------------
// TSdpAttributeParser::HasFinished
//------------------------------------------------------------
TBool TSdpAttributeParser::HasFinished() const
    {
    return CurrentNode().iCommand == EFinished;
    }
//------------------------------------------------------------
// TSdpAttributeParser::VisitAttributeValueL
//------------------------------------------------------------
void TSdpAttributeParser::VisitAttributeValueL(CSdpAttrValue& aValue, TSdpElementType aType)
    {
    switch(CurrentNode().iCommand)
        {
        case ECheckType:
            CheckTypeL(aType);
            break;

        case ECheckValue:
            CheckTypeL(aType);
            CheckValueL(aValue);
            break;

        case ECheckEnd:
            User::Leave(KErrGeneral);   //  list element contains too many items
            break;

        case ESkip:
            break;  // no checking required

        case EReadValue:
            CheckTypeL(aType);
            ReadValueL(aValue);
            break;

        case EFinished:
            User::Leave(KErrGeneral);   // element is after value should have ended
            return;

        default:
            Panic(ESdpAttributeParserInvalidCommand);
        }

    AdvanceL();
    }
//------------------------------------------------------------
// TSdpAttributeParser::StartListL
//------------------------------------------------------------
void TSdpAttributeParser::StartListL(CSdpAttrValueList& /*aList*/)
    {
    // no checks done here
    }
//------------------------------------------------------------
// TSdpAttributeParser::EndListL
//------------------------------------------------------------
void TSdpAttributeParser::EndListL()
    {
    // check we are at the end of a list
    if (CurrentNode().iCommand != ECheckEnd)
        {
        User::Leave(KErrGeneral);
        }

    AdvanceL();
    }
//------------------------------------------------------------
// TSdpAttributeParser::CheckTypeL
//------------------------------------------------------------
void TSdpAttributeParser::CheckTypeL(TSdpElementType aElementType) const
    {
    if (CurrentNode().iType != aElementType)
        {
        User::Leave(KErrGeneral);
        }
    }
//------------------------------------------------------------
// TSdpAttributeParser::CheckValueL
//------------------------------------------------------------
void TSdpAttributeParser::CheckValueL(CSdpAttrValue& aValue) const
    {
    switch(aValue.Type())
        {
        case ETypeNil:
            Panic(ESdpAttributeParserNoValue);
            break;

        case ETypeUint:
            if (aValue.Uint() != (TUint)CurrentNode().iValue)
                {
                User::Leave(KErrArgument);
                }
            break;

        case ETypeInt:
            if (aValue.Int() != CurrentNode().iValue)
                {
                User::Leave(KErrArgument);
                }
            break;

        case ETypeBoolean:
            if (aValue.Bool() != CurrentNode().iValue)
                {
                User::Leave(KErrArgument);
                }
            break;

        case ETypeUUID:
            if (aValue.UUID() != TUUID(CurrentNode().iValue))
                {
                User::Leave(KErrArgument);
                }
            break;

        // these are lists, so have to check contents
        case ETypeDES:
        case ETypeDEA:
            Panic(ESdpAttributeParserValueIsList);
            break;

        // these aren't supported - use EReadValue and leave on error
        //case ETypeString:
        //case ETypeURL:
        //case ETypeEncoded:
        default:
            Panic(ESdpAttributeParserValueTypeUnsupported);
            break;
        }
    }
//------------------------------------------------------------
// TSdpAttributeParser::ReadValueL
//------------------------------------------------------------
void TSdpAttributeParser::ReadValueL(CSdpAttrValue& aValue) const
    {
    iObserver.FoundElementL(CurrentNode().iValue, aValue);
    }
//------------------------------------------------------------
// TSdpAttributeParser::CurrentNode
//------------------------------------------------------------
const TSdpAttributeParser::SSdpAttributeNode& TSdpAttributeParser::CurrentNode() const
    {
    return  iNodeList[iCurrentNodeIndex];
    }
//------------------------------------------------------------
// TSdpAttributeParser::AdvanceL
//------------------------------------------------------------
void TSdpAttributeParser::AdvanceL()
    {
    // check not at end
    if (CurrentNode().iCommand == EFinished)
        {
        User::Leave(KErrEof);
        }

    // move to the next item
    ++iCurrentNodeIndex;
    }


