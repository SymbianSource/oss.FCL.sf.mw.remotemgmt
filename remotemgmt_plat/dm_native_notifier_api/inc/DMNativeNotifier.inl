
inline CDMNativeNotifier* CDMNativeNotifier::NewL(TUid aOperationId)
    {  
    
    TAny* interface = REComSession::CreateImplementationL( aOperationId
                                        ,_FOFF (CDMNativeNotifier, iDtor_ID_Key)); 
    return reinterpret_cast <CDMNativeNotifier*> (interface);
    
    }

inline CDMNativeNotifier::~CDMNativeNotifier()
    {
    REComSession::DestroyedImplementation (iDtor_ID_Key);    
    }

inline void CDMNativeNotifier::ListImplementationsL(RImplInfoPtrArray& aImplInfoArray)
    {
    _LIT8(KOperationName,"DialogEcom");
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(KOperationName());
    resolverParams.SetWildcardMatch(ETrue);     // Allow wildcard matching

    REComSession::ListImplementationsL(KNotifierInterfaceUid,resolverParams,KRomOnlyResolverUid, aImplInfoArray);
    }
