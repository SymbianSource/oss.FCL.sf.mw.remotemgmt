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
* Description:  Debug library
*
*/


#define __DEBUG_THIS__
#include <nsmldebug.h>
#undef __DEBUG_THIS__

#include <f32file.h>
#include <s32file.h>
#include <utf.h>
#include <e32std.h>

#ifdef __WINS__
_LIT( KNSmlDebugOutputName, "C:\\NSmlDebug.txt" );
_LIT( KNSmlDebugDumpName, "C:\\NSmlDebugDump.txt" );
#else
// In emulator logs are written into D: (or E:) drive (easier to read them using PC)
_LIT( KNSmlDebugOutputName, ":\\NSmlDebug.txt" );
_LIT( KNSmlDebugDumpName, ":\\NSmlDebugDump.txt" );
_LIT( KNSmlDebugDriveLetters, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

#endif

_LIT( KNSmlDebugMutexName, "NSmlDebugMutex" );
_LIT( KNSmlDebugMutexNameDump, "NSmlDebugMutexDump" );

static const TInt KBufferSize = 512;

#pragma warning (push)
#pragma warning (disable : 4127)


// --------------------------------------------------------------------------------
// void NSmlGrabMutexLC( RMutex& aMutex, const TDesC& aMutexName )
// --------------------------------------------------------------------------------
LOCAL_C void NSmlGrabMutexLC( RMutex& aMutex, const TDesC& aMutexName )
	{
	while( ETrue )
		{
		TInt ret = aMutex.CreateGlobal(aMutexName);
		if( ret == KErrNone ) // We created the mutex -> Issue wait()
			{
			aMutex.Wait();
			break;
			}
		if( ret == KErrAlreadyExists ) // Mutex already existed -> Open it
			{
			ret = aMutex.OpenGlobal(aMutexName);
			if( ret == KErrNone ) // We got handle to the mutex -> Issue wait()
				{
				aMutex.Wait();
				break;
				}
			}
		}
	CleanupClosePushL(aMutex);
	}

#pragma warning (pop)

// --------------------------------------------------------------------------------
// void DumpToFileL( TPtrC8 aBuffer )
// --------------------------------------------------------------------------------
void DumpToFileL( TPtrC8 aBuffer )
	{
	RFs fileSession;
	TInt pushed(0);
	if( fileSession.Connect() == KErrNone )
		{
		CleanupClosePushL(fileSession);
		pushed++;
		RFile file;
#ifdef __WINS__
		if( file.Open(fileSession, KNSmlDebugOutputName(), EFileShareAny|EFileWrite) == KErrNone )
#else
		HBufC* outputName = HBufC::NewLC( KNSmlDebugOutputName().Length()+1 );
		pushed++;
		TPtr namePtr = outputName->Des();
		TDriveInfo driveInfo;
		for ( TUint driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ ) 
			{
			fileSession.Drive( driveInfo, driveNumber );
			if ( KDriveAttRemovable & driveInfo.iDriveAtt ) 
				{
				// this is MMC
				namePtr.Insert( 0, KNSmlDebugDriveLetters().Mid( driveNumber, 1 ));
				namePtr.Insert( 1, KNSmlDebugOutputName() );
				break;
				}
			}
		if( file.Open(fileSession, *outputName, EFileShareAny|EFileWrite) == KErrNone )
#endif

			{
			CleanupClosePushL(file);
			TInt pos(0);
			file.Seek(ESeekEnd, pos);
			RMutex mutex;
			NSmlGrabMutexLC(mutex, KNSmlDebugMutexName());
			TInt result = file.Write(aBuffer);
			TInt result2 = file.Flush();
			mutex.Signal();
			CleanupStack::PopAndDestroy(2); // file, mutex
			User::LeaveIfError(result);
			User::LeaveIfError(result2);
			}
		CleanupStack::PopAndDestroy( pushed ); // fileSession and (for target) outputName
		}
	}

// --------------------------------------------------------------------------------
// void NSmlGetDateAndTimeL( TDes8& aDateBuffer, TDes8& aTimeBuffer )
// --------------------------------------------------------------------------------
LOCAL_C void NSmlGetDateAndTimeL( TDes8& aDateBuffer, TDes8& aTimeBuffer )
	{
	TTime time;
	time.HomeTime();
	HBufC* dateBuffer = HBufC::NewLC(64);
	TPtr ptrDateBuffer = dateBuffer->Des();
	HBufC* timeBuffer = HBufC::NewLC(64);
	TPtr ptrTimeBuffer = timeBuffer->Des();
	time.FormatL(ptrDateBuffer, _L("%D%M%Y%/0%1%/1%2%/2%3%/3"));
	time.FormatL(ptrTimeBuffer, _L("%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B"));
	CnvUtfConverter::ConvertFromUnicodeToUtf8(aDateBuffer, ptrDateBuffer);
	CnvUtfConverter::ConvertFromUnicodeToUtf8(aTimeBuffer, ptrTimeBuffer);
	CleanupStack::PopAndDestroy(2); // dateBuffer, timeBuffer
	}

// --------------------------------------------------------------------------------
// void doNSmlDebugInitL() - Windows compilation
// --------------------------------------------------------------------------------

#ifdef __WINS__

void doNSmlDebugInitL()
	{
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	fileSession.Delete(KNSmlDebugOutputName());
	fileSession.Delete(KNSmlDebugDumpName());	
	RFile file;

#if !defined(__SML_DEVELOPER_DEBUG__)
		{
		User::LeaveIfError(file.Create(fileSession, KNSmlDebugOutputName(), EFileShareAny));
		file.Close();
		}
#endif
		{
		User::LeaveIfError(file.Create(fileSession, KNSmlDebugDumpName(), EFileShareAny));
		file.Close();
		}
	CleanupStack::PopAndDestroy(); // fileSession
	}

// --------------------------------------------------------------------------------
// void doNSmlDebugInitL() - Target compilation
// --------------------------------------------------------------------------------

#else  // #ifdef __WINS__

void doNSmlDebugInitL()
	{
	TInt pushed(0);
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	pushed++;

	TDriveInfo driveInfo;
	for ( TUint driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ ) 
		{
		fileSession.Drive( driveInfo, driveNumber );
		if ( KDriveAttRemovable & driveInfo.iDriveAtt ) 
			{
			RFile file;
			HBufC* outputName = HBufC::NewLC( KNSmlDebugOutputName().Length()+1 );
			pushed++;
			TPtr namePtr = outputName->Des();
			namePtr.Insert( 0, KNSmlDebugDriveLetters().Mid( driveNumber, 1 ));
			namePtr.Insert( 1, KNSmlDebugOutputName() );
			
			HBufC* outputDumpName = HBufC::NewLC( KNSmlDebugDumpName().Length()+1 );
			pushed++;
			TPtr nameDumpPtr = outputDumpName->Des();
			nameDumpPtr.Insert( 0, KNSmlDebugDriveLetters().Mid( driveNumber, 1 ));
			nameDumpPtr.Insert( 1, KNSmlDebugDumpName() );
			
			fileSession.Delete(*outputName);
			fileSession.Delete(*outputDumpName);	

#if !defined(__SML_DEVELOPER_DEBUG__)
				{
				User::LeaveIfError(file.Create(fileSession, *outputName, EFileShareAny));
				file.Close();
				}
#endif
				{
				User::LeaveIfError(file.Create(fileSession, *outputDumpName, EFileShareAny));
				file.Close();
				break;
				}
			}
		}
	CleanupStack::PopAndDestroy( pushed ); // fileSession
	}

#endif


// --------------------------------------------------------------------------------
// void NSmlDebugInitL()
// --------------------------------------------------------------------------------
EXPORT_C void NSmlDebugInitL()
	{
	TInt err(KErrNone);
	TRAP(err, doNSmlDebugInitL());
	}

// --------------------------------------------------------------------------------
// void doNSmlDebugPrintL( TInt aCode, const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine )
// --------------------------------------------------------------------------------
void doNSmlDebugPrintL( TInt aCode, const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine )
	{
	HBufC8* dateBuffer8 = HBufC8::NewLC(64);
	TPtr8 ptrDateBuffer8 = dateBuffer8->Des();
	HBufC8* timeBuffer8 = HBufC8::NewLC(64);
	TPtr8 ptrTimeBuffer8 = timeBuffer8->Des();

	NSmlGetDateAndTimeL(ptrDateBuffer8, ptrTimeBuffer8);
	TPtrC8 fileName(aFile);
	HBufC8* buffer8 = HBufC8::NewLC(KBufferSize);
	TPtr8 ptrBuffer8 = buffer8->Des();
	TPtrC8 msg(aMsg);

#if !defined(__SML_DEVELOPER_DEBUG__)
	ptrBuffer8.Format(_L8("[%S File: %S Line: %d] %S [Code: %d]\r\n"), &ptrTimeBuffer8, &fileName, aLine, &msg, aCode);
#else
	ptrBuffer8.Format(_L8("[%S] %S\r\n"), &ptrTimeBuffer8, &msg);
#endif

	if( aPrintToFile )
		{
		DumpToFileL(ptrBuffer8);
		}
	else
		{
		HBufC* buffer = HBufC::NewLC(KBufferSize);
		TPtr ptrBuffer = buffer->Des();
		CnvUtfConverter::ConvertToUnicodeFromUtf8(ptrBuffer, ptrBuffer8);
		User::InfoPrint(ptrBuffer);
		CleanupStack::PopAndDestroy(); // buffer
		}
	CleanupStack::PopAndDestroy(3); // buffer8, dateBuffer8, timeBuffer8
	}

// --------------------------------------------------------------------------------
// void NSmlDebugPrintL( const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine )
// --------------------------------------------------------------------------------
EXPORT_C void NSmlDebugPrintL( const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine )
	{
	TInt err(KErrNone);
	TRAP(err, doNSmlDebugPrintL( 0, aMsg, aPrintToFile, aFile, aLine));
	}

// --------------------------------------------------------------------------------
// void NSmlDebugPrint_CodeL( TInt aCode, const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine )
// --------------------------------------------------------------------------------
EXPORT_C void NSmlDebugPrint_CodeL( TInt aCode, const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine )
	{
	TInt err(KErrNone);
	TRAP(err, doNSmlDebugPrintL(aCode, aMsg, aPrintToFile, aFile, aLine));
	}

// --------------------------------------------------------------------------------
// void doNSmlDebugDumpL( void* aData, TInt aLength, TText8* aFile, TInt aLine, const TText8* aMsg )
// --------------------------------------------------------------------------------
void doNSmlDebugDumpL( void* aData, TInt aLength, TText8* aFile, TInt aLine, const TText8* aMsg )
	{
	TInt pushed(0);
	HBufC8* pDateBuffer8 = HBufC8::NewLC(64);
	pushed++;
	TPtr8 dateBuffer8 = pDateBuffer8->Des();
	HBufC8* pTimeBuffer8 = HBufC8::NewLC(64);
	pushed++;
	TPtr8 timeBuffer8 = pTimeBuffer8->Des();
	NSmlGetDateAndTimeL(dateBuffer8, timeBuffer8);
	TPtrC8 data(REINTERPRET_CAST(TUint8*, aData), aLength);
	TPtrC8 fileName(aFile);
	HBufC8* buffer8 = HBufC8::NewLC(KBufferSize);
	pushed++;
	TPtr8 ptrBuffer8 = buffer8->Des();
	TPtrC8 msg(aMsg);
	if( !msg.Length() )
		{
#if !defined(__SML_DEVELOPER_DEBUG__)
		ptrBuffer8.Format(_L8("[%S %S File: %S Line: %d Length: %d]\r\n"), &dateBuffer8, &timeBuffer8, &fileName, aLine, aLength);
#else
		ptrBuffer8.Format(_L8("[%S %S Length: %d]\r\n"), &dateBuffer8, &timeBuffer8, aLength);
#endif
		}
	else
		{
#if !defined(__SML_DEVELOPER_DEBUG__)
		ptrBuffer8.Format(_L8("[%S %S File: %S Line: %d Length: %d] %S\r\n"), &dateBuffer8, &timeBuffer8, &fileName, aLine, aLength, &msg);
#else
		ptrBuffer8.Format(_L8("[%S %S Length: %d] %S\r\n"), &dateBuffer8, &timeBuffer8, aLength, &msg);
#endif
		}
	// Now we're ready to write into file
	RFs fileSession;
	if( fileSession.Connect() == KErrNone )
		{
		CleanupClosePushL(fileSession);
		pushed++;
		RFile file;
#ifdef __WINS__
		if( file.Open(fileSession, KNSmlDebugDumpName(), EFileShareAny|EFileWrite) == KErrNone )
#else
		HBufC* outputDumpName = HBufC::NewLC( KNSmlDebugDumpName().Length()+1 );
		pushed++;
		TPtr nameDumpPtr = outputDumpName->Des();
		TDriveInfo driveInfo;
		for ( TUint driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ ) 
			{
			fileSession.Drive( driveInfo, driveNumber );
			if ( KDriveAttRemovable & driveInfo.iDriveAtt ) 
				{
				// this is MMC
				nameDumpPtr.Insert( 0, KNSmlDebugDriveLetters().Mid( driveNumber, 1 ));
				nameDumpPtr.Insert( 1, KNSmlDebugDumpName() );
				break;
				}
			}
		if( file.Open(fileSession, *outputDumpName, EFileShareAny|EFileWrite) == KErrNone )
#endif

			{
			CleanupClosePushL(file);
			TInt pos(0);
			file.Seek(ESeekEnd, pos);
			RMutex mutex;
			NSmlGrabMutexLC(mutex, KNSmlDebugMutexNameDump());
			TInt result1 = file.Write(ptrBuffer8);
			TInt result2 = file.Write(data);
			TInt result3 = file.Write(_L8("\r\n\r\n"));
			TInt result4 = file.Flush();
			mutex.Signal();
			CleanupStack::PopAndDestroy(2); // file, mutex
			User::LeaveIfError(result1);
			User::LeaveIfError(result2);
			User::LeaveIfError(result3);
			User::LeaveIfError(result4);
			}
		}
	CleanupStack::PopAndDestroy( pushed ); // buffer8, pDateBuffer8, pTimeBuffer8
	}

// --------------------------------------------------------------------------------
// void NSmlDebugDumpL( void* aData, TInt aLength, TText8* aFile, TInt aLine, const TText8* aMsg )
// --------------------------------------------------------------------------------
EXPORT_C void NSmlDebugDumpL( void* aData, TInt aLength, TText8* aFile, TInt aLine, const TText8* aMsg )
	{
	TInt err(KErrNone);
	TRAP(err, doNSmlDebugDumpL(aData, aLength, aFile, aLine, aMsg));
	}

// --------------------------------------------------------------------------------
// void NSmlDebugPrintHeaderL( TText8* aFile, TInt aLine )
// --------------------------------------------------------------------------------
void _NSmlDebugPrintHeaderL( TText8* aFile, TInt aLine )
	{
	HBufC8* pDateBuffer8 = HBufC8::NewLC(64);
	TPtr8 dateBuffer8 = pDateBuffer8->Des();
	HBufC8* pTimeBuffer8 = HBufC8::NewLC(64);
	TPtr8 timeBuffer8 = pTimeBuffer8->Des();
	NSmlGetDateAndTimeL(dateBuffer8, timeBuffer8);
	TPtrC8 fileName(aFile);
	HBufC8* buffer8 = HBufC8::NewLC(KBufferSize);
	TPtr8 ptrBuffer8 = buffer8->Des();
#if !defined(__SML_DEVELOPER_DEBUG__)
	ptrBuffer8.Format(_L8("[%S File: %S Line: %d]"), &timeBuffer8, &fileName, aLine);
#else
	ptrBuffer8.Format(_L8("[%S]"), &timeBuffer8);
#endif
	ptrBuffer8.Append(_L8(" "));
	DumpToFileL(ptrBuffer8);
	CleanupStack::PopAndDestroy(3); // buffer8, pDateBuffer, pTimeBuffer
	}

EXPORT_C void NSmlDebugPrintHeaderL( TText8* aFile, TInt aLine )
	{
	TInt err(KErrNone);
	TRAP(err, _NSmlDebugPrintHeaderL(aFile, aLine));
	}

// --------------------------------------------------------------------------------
// void NSmlDebugPrintArgs8L( TText8* aFmt, ... )
// --------------------------------------------------------------------------------
void _NSmlDebugPrintArgs8L( const TDesC8& aFmt, VA_LIST aList )
	{
	HBufC8* pBuf = HBufC8::NewLC(1024*2);
	TPtr8 buf = pBuf->Des();
	buf.FormatList(aFmt, aList);
	buf.Append(_L8("\r\n"));
	DumpToFileL(buf);
	CleanupStack::PopAndDestroy(); // pBuf
	}

EXPORT_C void NSmlDebugPrintArgs8L( const TText8* aFmt, ... )
	{
	VA_LIST args;
	VA_START (args, aFmt);
	TInt err(KErrNone);
	TRAP(err, _NSmlDebugPrintArgs8L(TPtrC8(aFmt), args));
	VA_END(args);
	}

// --------------------------------------------------------------------------------
// void NSmlDebugPrintArgs16L( TText8* aFmt, ... )
// --------------------------------------------------------------------------------
void _NSmlDebugPrintArgs16L( const TDesC16& aFmt, VA_LIST aList )
	{
	HBufC16* buf = HBufC16::NewLC(1024*2);
	buf->Des().FormatList(aFmt, aList);
	HBufC8* pBuf8 = HBufC8::NewLC(1024*2);
	TPtr8 buf8 = pBuf8->Des();
	buf8.Copy(*buf);
	buf8.Append(_L8("\r\n"));
	DumpToFileL(buf8);
	CleanupStack::PopAndDestroy(2); // buf, bBuf8
	}

EXPORT_C void NSmlDebugPrintArgs16L( const TText16* aFmt, ... )
	{
	VA_LIST args;
	VA_START (args, aFmt);
	TInt err(KErrNone);
	TRAP(err, _NSmlDebugPrintArgs16L(TPtrC16(aFmt), args));
	VA_END(args);
	}

//End of File

