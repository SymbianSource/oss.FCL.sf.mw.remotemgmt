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
* Description:  XML device information DTD generator
*
*/


#ifndef __XMLDEVINFGENERATOR_H__
#define __XMLDEVINFGENERATOR_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "XMLGenerator.h"

// ------------------------------------------------------------------------------------------------
// Toolkit includes
// ------------------------------------------------------------------------------------------------
#include "smldef.h"
#include "smldtd.h"
#include "smldevinfdtd.h"

// ------------------------------------------------------------------------------------------------
// CNSmlSyncMLGenerator
// ------------------------------------------------------------------------------------------------
class CXMLDevInfGenerator : public CXMLGenerator
	{
public:
	IMPORT_C void CreateWorkspaceL();
	IMPORT_C Ret_t smlDeviceInfo( SmlDevInfDevInfPtr_t aContent );
	IMPORT_C void InitializeL();

protected:
		void AppendPCDataL( TUint8 aElement, SmlPcdataPtr_t aContent );
	void AppendPCDataListL( TUint8 aElement, SmlPcdataListPtr_t aList );
	void AppendXmitL( TUint8 aTag, SmlDevInfXmitPtr_t aContent );
	void AppendXmitListL( TUint8 aTag, SmlDevInfXmitListPtr_t aContent );
	void AppendExtL( SmlDevInfExtPtr_t aContent );
	void AppendExtListL( SmlDevInfExtListPtr_t aContent );
	void AppendFilterCapListL( SmlDevInfFilterCapListPtr_t aContent );
	void AppendFilterCapL( SmlDevInfFilterCapPtr_t aContent );
	void AppendSyncCapL( SmlDevInfSyncCapPtr_t aContent );
	void AppendDSMemL( SmlDevInfDSMemPtr_t aContent );
	void AppendPropParamL( SmlDevInfPropParamPtr_t aContent);
	void AppendPropParamListL( SmlDevInfPropParamListPtr_t aContent);
	void AppendPropertyL( SmlDevInfPropertyPtr_t aContent );
	void AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent );
	void AppendCTDataL( TUint8 aTag, SmlDevInfCTDataPtr_t aContent );
	void AppendCTDataListL( SmlDevInfCTDataListPtr_t aContent );
	void AppendCTDataPropL( SmlDevInfCTDataPropPtr_t aContent );
	void AppendCTDataPropListL( SmlDevInfCTDataPropListPtr_t aContent );
	void AppendCtCapL( SmlDevInfCtCapPtr_t aContent );
	void AppendCtCapListL( SmlDevInfCtCapListPtr_t aContent );
	void AppendDatastoreL( SmlDevInfDatastorePtr_t aontent );
	void AppendDatastoreListL( SmlDevInfDatastoreListPtr_t aContent );
	void AppendDeviceInfoL( SmlDevInfDevInfPtr_t aContent );

	TInt iVerDtd;

private:
	};

#endif // __XMLDEVINFGENERATOR_H__
