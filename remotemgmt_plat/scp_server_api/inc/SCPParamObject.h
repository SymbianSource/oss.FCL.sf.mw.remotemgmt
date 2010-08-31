/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Defines a class that can be used to store a set of parameters and values
*       and trasfer them in a packed format.
*
*
*/


#ifndef SCPPARAMOBJECT_H
#define SCPPARAMOBJECT_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>
#include <bldvariant.hrh>

class RFs;

/**
*  The definition for the parameter object class
*/
class CSCPParamObject: public CBase
    {
	public:  // Methods

        // Constructors and destructor                
        
        /**
        * Static constructor.
        */
   	    IMPORT_C static CSCPParamObject* NewL();
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        IMPORT_C static CSCPParamObject* NewLC();         

        
        /**
        * Destructor
        */        
        IMPORT_C virtual ~CSCPParamObject();  
        
        // New methods 
        
        /**
        * Get the value of an identified parameter stored in the object
        * @param aParamID The ID of the requested parameter.
        * @return A positive integer indicating the lenght, or an error code
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> The length or an error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt GetParamLength( TInt aParamID );                       
        
        /**
        * Get the value of an identified parameter stored in the object
        * @param aParamID The ID of the requested parameter.
        * @param aValue Will contain a pointer to a descriptor containing the value
        * after a successful call. The client owns this pointer after the call.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt Get( TInt aParamID, TDes& aValue ); 
        
        /**
        * Get the value of an identified parameter stored in the object
        * @param aParamID The ID of the requested parameter.
        * @param aValue Will contain the requested value converted to an integer
        * after a successful call. If the value cannot be converted, an error will
        * be returned.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt Get( TInt aParamID, TInt& aValue ); 
        
        /**
        * Set the value of a parameter. Will replace an existing value, or add a
        * new one.
        * @param aParamID The ID of the parameter.
        * @param aValue The value to set.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt Set( TInt aParamID, TDes& aValue ); 
        
        /**
        * Set the value of a parameter. Will replace an existing value, or add a
        * new one.
        * @param aParamID The ID of the parameter.
        * @param aValue The value to set, will be converted to a descriptor.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt Set( TInt aParamID, TInt aValue ); 
        
        /**
        * Remove the identified parameter from the storage.
        * @param aParamID The ID of the parameter to remove.         
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.
        */        
        IMPORT_C TInt Unset( TInt aParamID );         
        
        /**
        * Reset the object and remove all parameters.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.
        */        
        IMPORT_C TInt Reset();                 
        
        /**
        * Parse the given buffer, and add its contents to the object.
        * @param aBuffer The buffer that will be parsed.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt Parse( TDes8& aBuffer ); 
        
        /**
        * Pack the stored parameters into a buffer.        
        * @param aValue Will contain a pointer to the packed buffer after
        * a successful call. The client owns the pointer after the call.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt GetBuffer( HBufC8*& aBuffer ); 
        
        /**
        * Write the content to disk in the format returned by GetBuffer.
        * Leaves with a generic status code on error.
        * @param aFilename The name of the file to be written. An existing file will
        * be overwritten, or a new file will be created.
        * @param aRfs An optional pointer to a connected file server session. If this
        * pointer is not supplied a new RFs will be connected.
        */        
        IMPORT_C void WriteToFileL( TDesC& aFilename, RFs* aRfs = NULL ); 
        
        /**
        * Reset the content of this object and load new content from a file. 
        * Leaves with a generic status code on error.       
        * @param aFilename The file name to read the content from. This filename must
        * contain a buffer returned by GetBuffer.
        * @param aRfs An optional pointer to a connected file server session. If this
        * pointer is not supplied a new RFs will be connected.
        */        
        IMPORT_C void ReadFromFileL( TDesC& aFilename, RFs* aRfs = NULL );                 

		/*
		* Would store details of the policy for which the current context is incompatible
		*
		* NOTE: The context could be Lock Code verification, Storing a New Lock Code etc...
		*/
        IMPORT_C TInt AddtoFailedPolices( TInt aFailedpolicyID );

        /*
        * Returns a reference to the array that contains the policies for which the current
        * context is incompatible
        *
        * NOTE: The context could be Lock Code verification, Storing a New Lock Code etc...
        */
        IMPORT_C const RArray<TInt>& GetFailedPolices( );
		
    protected:  // Methods
                        
        // Methods from base classes       
                
        
    private: //Methods 
        
        // New Methods   
                           
        /**
        * C++ default constructor.
        */        
        CSCPParamObject();
        
        /**
        * Symbian second-phase constructor
        */
        void ConstructL();                                     
        
        
    private:   // Data 
        /** An array containing the IDs for the stored parameters */
        RArray<TInt> iParamIDs;
        /** An array containing the pointers to the parameter values */
        RPointerArray<HBufC> iParamValues;
        RArray<TInt> iFailedPolicyIDs;
    };

#endif      // SCPPARAMOBJECT_H   
            
// End of File
