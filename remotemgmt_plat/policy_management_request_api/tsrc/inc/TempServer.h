/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
 


#ifndef __Temp_SERVER_H__
#define __Temp_SERVER_H__

#include <e32base.h>
#include <e32std.h>



// CLASS DECLARATION     
class CShutdown : public CTimer
	{
	enum 
		{
		KTempServerShutdownDelay = 0x300000//0x1000000 changed to 3 sec
		};	
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};



class CTempServer: public CServer2
{
	friend class CTempSession;
	
	public:
		~CTempServer();
		static CServer2*    NewLC();
		void AddSession();
		void DropSession();
	
	private:
		TInt iSessionCount;
		CShutdown iShutdown;
	private:	
		void ConstructL();
		CTempServer();
		TInt PerformRFS();
	protected:
		CSession2* NewSessionL(const TVersion&,const RMessage2&) const;

};


#endif