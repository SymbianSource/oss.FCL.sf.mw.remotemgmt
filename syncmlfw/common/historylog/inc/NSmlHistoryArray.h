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
* Description: 
*		Declaration file for history array.
*
*/


#ifndef __NSMLHISTORYARRAY_H__
#define __NSMLHISTORYARRAY_H__

#include <e32cmn.h>
#include "nsmlconstants.h"
#include <SyncMLHistory.h>


class CSyncMLHistoryEntry;

class CNSmlHistoryArray : public CBase
	{
	public:
	
		/**
        * Two-phased constructor.
        */
		static IMPORT_C CNSmlHistoryArray* NewL();
		
		/**
        * Destructor
        */
		~CNSmlHistoryArray();
		
	public:
		
		/**
        * Returns count of history entries
        * @since Series ?XX ?SeriesXX_version
        * @param -
        * @return Count of history entries in the array
        */
		IMPORT_C TInt Count();
		
		/**
        * Adds new entry to history entry array
        * @since Series ?XX ?SeriesXX_version
        * @param aEntry pointer to entry to be added
        * @return -
        */
		IMPORT_C void AppendEntryL( CSyncMLHistoryEntry* aEntry );
		
		/**
        * Removes entry from the specified index
        * @since Series ?XX ?SeriesXX_version
        * @param aIndex Index of the entry to be removed
        * @return Pointer to the removed entry
        */
		IMPORT_C CSyncMLHistoryEntry* RemoveEntry( TInt aIndex );
		
		/**
        * Removes all history entries from the array
        * @since Series ?XX ?SeriesXX_version
        * @param -
        * @return -
        */
		IMPORT_C void DeleteAllEntries();
		
		/**
        * Retrieves an entry from the array
        * @since Series ?XX ?SeriesXX_version
        * @param aIndex Index of the entry to be retrieved 
        * @return Reference to the retrieved entry
        */
		IMPORT_C CSyncMLHistoryEntry& Entry( TInt aIndex );
		
		/**
        * Sorts the history array by specified key
        * @since Series ?XX ?SeriesXX_version
        * @param aSortOrder Specifies the way sorting is performed
        * @return -
        */
		IMPORT_C void SortEntries( CSyncMLHistoryEntry::TSortOrder aSortOrder );
		
		/**
        * Transfers the ownership of the entries in the array
        * @since Series ?XX ?SeriesXX_version
        * @param aOwner ETrue transfers ownership to history array, EFalse sets ownership to caller
        * @return -
        */
		IMPORT_C void SetOwnerShip( TBool aOwner );
		
		/**
        * Reads data from the given stream
        * @since Series ?XX ?SeriesXX_version
        * @param aStream Reference to stream data is read from
        * @return -
        */
		IMPORT_C void InternalizeL( RReadStream& aStream );
		
		/**
        * Writes data to the given stream
        * @since Series ?XX ?SeriesXX_version
        * @param aStream Reference to stream data is written to
        * @return -
        */
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		
		/**
        * Implements sort by entry type
        * @since Series ?XX ?SeriesXX_version
        * @param aLeft Reference to entry
        * @param aRight Reference to entry aLeft is compared to
        * @return Positive, if entry is greater than the specified entry. Negative, if this entry is less than the specified entry. Zero, if the content of both entries match
        */
		static TInt SortByType( const CSyncMLHistoryEntry& aLeft, const CSyncMLHistoryEntry& aRight );
		
		/**
        * Implements sort by entry time
        * @since Series ?XX ?SeriesXX_version
        * @param aLeft Reference to entry
        * @param aRight Reference to entry aLeft is compared to
        * @return Positive, if entry's timestamp is greater than the specified entry's. Negative, if this entry's timestamp is less than the specified entry's. Zero, if the content of both entries' timestamps match
        */
        static TInt SortByTime( const CSyncMLHistoryEntry& aLeft, const CSyncMLHistoryEntry& aRight );
        
	private:
	    void UpdateTaskListL(const CSyncMLHistoryJob* aHistoryJob, RArray<TInt>& aTaskList);
	    void InitializeTaskListL(RArray<TInt>& aTaskList);
	    void RemoveHistoryJobL();
	    void RemoveHistoryJob();
	    TBool RemoveHistoryJobL(RArray<TInt>& aTaskList);
	    TBool CanRemove(const CSyncMLHistoryJob* aHistoryJob, RArray<TInt>& aTaskList);
	    void ResetCorruptedHistroy () ;
	    
#ifdef _DEBUG	    
	    void LogHistoryJobL(const CSyncMLHistoryJob* aHistoryJob);
	    void LogHistoryArrayL();
	    void GetDateTimeTextL(TDes& aText, TTime aDateTime);
	    void CheckHistoryJobCount();
#endif	        
	    
	private:
		
		/**
        * C++ default constructor.
        */
		CNSmlHistoryArray();	
		
		/**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		/**
        * Checks the count of job entries is within the limit
        * @since Series ?XX ?SeriesXX_version
        * @param -
        * @return -
        */
		void DoJobLimitCheck();
		
		/**
        * Checks the count of push message entries is within the limit
        * @since Series ?XX ?SeriesXX_version
        * @param -
        * @return -
        */
		void DoPushMsgLimitCheck();
		
	private:
		RPointerArray<CSyncMLHistoryEntry> iHistory;
		TBool iHistoryOwned;
		CSyncMLHistoryEntry::TSortOrder iSortOrder;
	};

#endif