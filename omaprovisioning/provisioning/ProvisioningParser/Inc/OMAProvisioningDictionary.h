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
*
*/


/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Feb 24 12:02:42 2005
**                        (coordinated universal time)
**
** Command line: dictionary provisioning_v12.txt omaprovisioningdictionary.c omaprovisioningdictionary.h
*/

#ifndef HEADER_GUARD_omaprovisioningdictionary_h
#define HEADER_GUARD_omaprovisioningdictionary_h

#ifdef __cplusplus
extern "C"
{
#endif


#define NW_omawapprovisioning_PublicId 0x0B

typedef enum NW_omawapprovisioning_ElementToken_0_e{
	NW_omawapprovisioning_ElementToken_wap_provisioningdoc = 0x0005,
	NW_omawapprovisioning_ElementToken_characteristic = 0x0006,
	NW_omawapprovisioning_ElementToken_parm = 0x0007
}NW_omawapprovisioning_ElementToken_0_t;


typedef enum NW_omawapprovisioning_AttributeToken_0_e{
	NW_omawapprovisioning_AttributeToken_name = 0x0005,
	NW_omawapprovisioning_AttributeToken_value = 0x0006,
	NW_omawapprovisioning_AttributeToken_name_NAME = 0x0007,
	NW_omawapprovisioning_AttributeToken_name_NAP_ADDRESS = 0x0008,
	NW_omawapprovisioning_AttributeToken_name_NAP_ADDRTYPE = 0x0009,
	NW_omawapprovisioning_AttributeToken_name_CALLTYPE = 0x000a,
	NW_omawapprovisioning_AttributeToken_name_VALIDUNTIL = 0x000b,
	NW_omawapprovisioning_AttributeToken_name_AUTHTYPE = 0x000c,
	NW_omawapprovisioning_AttributeToken_name_AUTHNAME = 0x000d,
	NW_omawapprovisioning_AttributeToken_name_AUTHSECRET = 0x000e,
	NW_omawapprovisioning_AttributeToken_name_LINGER = 0x000f,
	NW_omawapprovisioning_AttributeToken_name_BEARER = 0x0010,
	NW_omawapprovisioning_AttributeToken_name_NAPID = 0x0011,
	NW_omawapprovisioning_AttributeToken_name_COUNTRY = 0x0012,
	NW_omawapprovisioning_AttributeToken_name_NETWORK = 0x0013,
	NW_omawapprovisioning_AttributeToken_name_INTERNET = 0x0014,
	NW_omawapprovisioning_AttributeToken_name_PROXY_ID = 0x0015,
	NW_omawapprovisioning_AttributeToken_name_PROXY_PROVIDER_ID = 0x0016,
	NW_omawapprovisioning_AttributeToken_name_DOMAIN = 0x0017,
	NW_omawapprovisioning_AttributeToken_name_PROVURL = 0x0018,
	NW_omawapprovisioning_AttributeToken_name_PXAUTH_TYPE = 0x0019,
	NW_omawapprovisioning_AttributeToken_name_PXAUTH_ID = 0x001a,
	NW_omawapprovisioning_AttributeToken_name_PXAUTH_PW = 0x001b,
	NW_omawapprovisioning_AttributeToken_name_STARTPAGE = 0x001c,
	NW_omawapprovisioning_AttributeToken_name_BASAUTH_ID = 0x001d,
	NW_omawapprovisioning_AttributeToken_name_BASAUTH_PW = 0x001e,
	NW_omawapprovisioning_AttributeToken_name_PUSHENABLED = 0x001f,
	NW_omawapprovisioning_AttributeToken_name_PXADDR = 0x0020,
	NW_omawapprovisioning_AttributeToken_name_PXADDRTYPE = 0x0021,
	NW_omawapprovisioning_AttributeToken_name_TO_NAPID = 0x0022,
	NW_omawapprovisioning_AttributeToken_name_PORTNBR = 0x0023,
	NW_omawapprovisioning_AttributeToken_name_SERVICE = 0x0024,
	NW_omawapprovisioning_AttributeToken_name_LINKSPEED = 0x0025,
	NW_omawapprovisioning_AttributeToken_name_DNLINKSPEED = 0x0026,
	NW_omawapprovisioning_AttributeToken_name_LOCAL_ADDR = 0x0027,
	NW_omawapprovisioning_AttributeToken_name_LOCAL_ADDRTYPE = 0x0028,
	NW_omawapprovisioning_AttributeToken_name_CONTEXT_ALLOW = 0x0029,
	NW_omawapprovisioning_AttributeToken_name_TRUST = 0x002a,
	NW_omawapprovisioning_AttributeToken_name_MASTER = 0x002b,
	NW_omawapprovisioning_AttributeToken_name_SID = 0x002c,
	NW_omawapprovisioning_AttributeToken_name_SOC = 0x002d,
	NW_omawapprovisioning_AttributeToken_name_WSP_VERSION = 0x002e,
	NW_omawapprovisioning_AttributeToken_name_PHYSICAL_PROXY_ID = 0x002f,
	NW_omawapprovisioning_AttributeToken_name_CLIENT_ID = 0x0030,
	NW_omawapprovisioning_AttributeToken_name_DELIVERY_ERR_SDU = 0x0031,
	NW_omawapprovisioning_AttributeToken_name_DELIVERY_ORDER = 0x0032,
	NW_omawapprovisioning_AttributeToken_name_TRAFFIC_CLASS = 0x0033,
	NW_omawapprovisioning_AttributeToken_name_MAX_SDU_SIZE = 0x0034,
	NW_omawapprovisioning_AttributeToken_name_MAX_BITRATE_UPLINK = 0x0035,
	NW_omawapprovisioning_AttributeToken_name_MAX_BITRATE_DNLINK = 0x0036,
	NW_omawapprovisioning_AttributeToken_name_RESIDUAL_BER = 0x0037,
	NW_omawapprovisioning_AttributeToken_name_SDU_ERROR_RATIO = 0x0038,
	NW_omawapprovisioning_AttributeToken_name_TRAFFIC_HANDL_PRIO = 0x0039,
	NW_omawapprovisioning_AttributeToken_name_TRANSFER_DELAY = 0x003a,
	NW_omawapprovisioning_AttributeToken_name_GUARANTEED_BITRATE_UPLINK = 0x003b,
	NW_omawapprovisioning_AttributeToken_name_GUARANTEED_BITRATE_DNLINK = 0x003c,
	NW_omawapprovisioning_AttributeToken_name_PXADDR_FQDN = 0x003d,
	NW_omawapprovisioning_AttributeToken_name_PROXY_PW = 0x003e,
	NW_omawapprovisioning_AttributeToken_name_PPGAUTH_TYPE = 0x003f,
	NW_omawapprovisioning_AttributeToken_version = 0x0045,
	NW_omawapprovisioning_AttributeToken_version_1_0 = 0x0046,
	NW_omawapprovisioning_AttributeToken_name_PULLENABLED = 0x0047,
	NW_omawapprovisioning_AttributeToken_name_DNS_ADDR = 0x0048,
	NW_omawapprovisioning_AttributeToken_name_MAX_NUM_RETRY = 0x0049,
	NW_omawapprovisioning_AttributeToken_name_FIRST_RETRY_TIMEOUT = 0x004a,
	NW_omawapprovisioning_AttributeToken_name_REREG_THRESHOLD = 0x004b,
	NW_omawapprovisioning_AttributeToken_name_T_BIT = 0x004c,
	NW_omawapprovisioning_AttributeToken_name_AUTH_ENTITY = 0x004e,
	NW_omawapprovisioning_AttributeToken_name_SPI = 0x004f,
	NW_omawapprovisioning_AttributeToken_type = 0x0050,
	NW_omawapprovisioning_AttributeToken_type_PXLOGICAL = 0x0051,
	NW_omawapprovisioning_AttributeToken_type_PXPHYSICAL = 0x0052,
	NW_omawapprovisioning_AttributeToken_type_PORT = 0x0053,
	NW_omawapprovisioning_AttributeToken_type_VALIDITY = 0x0054,
	NW_omawapprovisioning_AttributeToken_type_NAPDEF = 0x0055,
	NW_omawapprovisioning_AttributeToken_type_BOOTSTRAP = 0x0056,
	NW_omawapprovisioning_AttributeToken_type_VENDORCONFIG = 0x0057,
	NW_omawapprovisioning_AttributeToken_type_CLIENTIDENTITY = 0x0058,
	NW_omawapprovisioning_AttributeToken_type_PXAUTHINFO = 0x0059,
	NW_omawapprovisioning_AttributeToken_type_NAPAUTHINFO = 0x005a,
	NW_omawapprovisioning_AttributeToken_type_ACCESS = 0x005b,
	NW_omawapprovisioning_AttributeToken_type_BEARERINFO = 0x005c,
	NW_omawapprovisioning_AttributeToken_type_DNS_ADDRINFO = 0x005d,
	NW_omawapprovisioning_AttributeToken_type_WLAN = 0x005e,
	NW_omawapprovisioning_AttributeToken_type_WEPKEY = 0x005f,
	NW_omawapprovisioning_AttributeToken_name_DIRECTION = 0x0060,
	NW_omawapprovisioning_AttributeToken_name_DNS_ADDRTYPE = 0x0061,
	NW_omawapprovisioning_AttributeToken_name_DNS_PRIORITY = 0x0062,
	NW_omawapprovisioning_AttributeToken_name_SOURCE_STATISTICS_DESCRIPTOR = 0x0063,
	NW_omawapprovisioning_AttributeToken_name_SIGNALLING_INDICATION = 0x0064,
	NW_omawapprovisioning_AttributeToken_name_DEFGW = 0x0065,
	NW_omawapprovisioning_AttributeToken_name_NETWORKMASK = 0x0066,
	NW_omawapprovisioning_AttributeToken_name_USECB = 0x0067,
	NW_omawapprovisioning_AttributeToken_name_CBNBR = 0x0068,
	NW_omawapprovisioning_AttributeToken_name_PPPCOMP = 0x0069,
	NW_omawapprovisioning_AttributeToken_name_TO_LOGINSCRIPTID = 0x006a,
	NW_omawapprovisioning_AttributeToken_name_USEPTXTLOG = 0x006b,
	NW_omawapprovisioning_AttributeToken_name_GPRSPDP = 0x006c,
	NW_omawapprovisioning_AttributeToken_name_MODEMINIT = 0x006d,
	NW_omawapprovisioning_AttributeToken_name_IPADDRFROMSERVER = 0x006e,
	NW_omawapprovisioning_AttributeToken_name_IPDNSADDRFROMSERVER = 0x006f,
	NW_omawapprovisioning_AttributeToken_name_IPV6DNSADDRFROMSERVER = 0x0070,
	NW_omawapprovisioning_AttributeToken_name_IFNETWORKS = 0x0071,
	NW_omawapprovisioning_AttributeToken_name_IAPSERVICE = 0x0072,
	NW_omawapprovisioning_AttributeToken_name_SSID = 0x0073,
	NW_omawapprovisioning_AttributeToken_name_NETWORKMODE = 0x0074,
	NW_omawapprovisioning_AttributeToken_name_SECURITYMODE = 0x0075,
	NW_omawapprovisioning_AttributeToken_name_WPAPRESHAREDKEY = 0x0076,
	NW_omawapprovisioning_AttributeToken_name_LENGTH = 0x0077,
	NW_omawapprovisioning_AttributeToken_name_DATA = 0x0078,
	NW_omawapprovisioning_AttributeToken_IPV4 = 0x0085,
	NW_omawapprovisioning_AttributeToken_IPV6 = 0x0086,
	NW_omawapprovisioning_AttributeToken_E164 = 0x0087,
	NW_omawapprovisioning_AttributeToken_ALPHA = 0x0088,
	NW_omawapprovisioning_AttributeToken_APN = 0x0089,
	NW_omawapprovisioning_AttributeToken_SCODE = 0x008a,
	NW_omawapprovisioning_AttributeToken_TETRA_ITSI = 0x008b,
	NW_omawapprovisioning_AttributeToken_MAN = 0x008c,
	NW_omawapprovisioning_AttributeToken_PPP = 0x008f,
	NW_omawapprovisioning_AttributeToken_ANALOG_MODEM = 0x0090,
	NW_omawapprovisioning_AttributeToken_V_120 = 0x0091,
	NW_omawapprovisioning_AttributeToken_V_110 = 0x0092,
	NW_omawapprovisioning_AttributeToken_X_31 = 0x0093,
	NW_omawapprovisioning_AttributeToken_BIT_TRANSPARENT = 0x0094,
	NW_omawapprovisioning_AttributeToken_DIRECT_ASYNCHRONOUS_DATA_SERVICE = 0x0095,
	NW_omawapprovisioning_AttributeToken_PAP = 0x009a,
	NW_omawapprovisioning_AttributeToken_CHAP = 0x009b,
	NW_omawapprovisioning_AttributeToken_HTTP_BASIC = 0x009c,
	NW_omawapprovisioning_AttributeToken_HTTP_DIGEST = 0x009d,
	NW_omawapprovisioning_AttributeToken_WTLS_SS = 0x009e,
	NW_omawapprovisioning_AttributeToken_MD5 = 0x009f,
	NW_omawapprovisioning_AttributeToken_GSM_USSD = 0x00a2,
	NW_omawapprovisioning_AttributeToken_GSM_SMS = 0x00a3,
	NW_omawapprovisioning_AttributeToken_ANSI_136_GUTS = 0x00a4,
	NW_omawapprovisioning_AttributeToken_IS_95_CDMA_SMS = 0x00a5,
	NW_omawapprovisioning_AttributeToken_IS_95_CDMA_CSD = 0x00a6,
	NW_omawapprovisioning_AttributeToken_IS_95_CDMA_PACKET = 0x00a7,
	NW_omawapprovisioning_AttributeToken_ANSI_136_CSD = 0x00a8,
	NW_omawapprovisioning_AttributeToken_ANSI_136_GPRS = 0x00a9,
	NW_omawapprovisioning_AttributeToken_GSM_CSD = 0x00aa,
	NW_omawapprovisioning_AttributeToken_GSM_GPRS = 0x00ab,
	NW_omawapprovisioning_AttributeToken_AMPS_CDPD = 0x00ac,
	NW_omawapprovisioning_AttributeToken_PDC_CSD = 0x00ad,
	NW_omawapprovisioning_AttributeToken_PDC_PACKET = 0x00ae,
	NW_omawapprovisioning_AttributeToken_IDEN_SMS = 0x00af,
	NW_omawapprovisioning_AttributeToken_IDEN_CSD = 0x00b0,
	NW_omawapprovisioning_AttributeToken_IDEN_PACKET = 0x00b1,
	NW_omawapprovisioning_AttributeToken_FLEX_REFLEX = 0x00b2,
	NW_omawapprovisioning_AttributeToken_PHS_SMS = 0x00b3,
	NW_omawapprovisioning_AttributeToken_PHS_CSD = 0x00b4,
	NW_omawapprovisioning_AttributeToken_TETRA_SDS = 0x00b5,
	NW_omawapprovisioning_AttributeToken_TETRA_PACKET = 0x00b6,
	NW_omawapprovisioning_AttributeToken_ANSI_136_GHOST = 0x00b7,
	NW_omawapprovisioning_AttributeToken_MOBITEX_MPAK = 0x00b8,
	NW_omawapprovisioning_AttributeToken_CDMA2000_1X_SIMPLE_IP = 0x00b9,
	NW_omawapprovisioning_AttributeToken_CDMA2000_1X_MOBILE_IP = 0x00ba,
	NW_omawapprovisioning_AttributeToken__3G_GSM = 0x00bb,
	NW_omawapprovisioning_AttributeToken_WLAN = 0x00bc,
	NW_omawapprovisioning_AttributeToken_AUTOBAUDING = 0x00c5,
	NW_omawapprovisioning_AttributeToken_CL_WSP = 0x00ca,
	NW_omawapprovisioning_AttributeToken_CO_WSP = 0x00cb,
	NW_omawapprovisioning_AttributeToken_CL_SEC_WSP = 0x00cc,
	NW_omawapprovisioning_AttributeToken_CO_SEC_WSP = 0x00cd,
	NW_omawapprovisioning_AttributeToken_CL_SEC_WTA = 0x00ce,
	NW_omawapprovisioning_AttributeToken_CO_SEC_WTA = 0x00cf,
	NW_omawapprovisioning_AttributeToken_OTA_HTTP_TO = 0x00d0,
	NW_omawapprovisioning_AttributeToken_OTA_HTTP_TLS_TO = 0x00d1,
	NW_omawapprovisioning_AttributeToken_OTA_HTTP_PO = 0x00d2,
	NW_omawapprovisioning_AttributeToken_OTA_HTTP_TLS_PO = 0x00d3,
	NW_omawapprovisioning_AttributeToken_AAA = 0x00e0,
	NW_omawapprovisioning_AttributeToken_HA = 0x00e1,
	NW_omawapprovisioning_AttributeToken_INFRASTRUCTURE = 0x00e2,
	NW_omawapprovisioning_AttributeToken_ADHOC = 0x00e3,
	NW_omawapprovisioning_AttributeToken_WEP = 0x00e4,
	NW_omawapprovisioning_AttributeToken__8021X = 0x00e5,
	NW_omawapprovisioning_AttributeToken_WPA = 0x00e6,
	NW_omawapprovisioning_AttributeToken_WPA_PRESHARED_KEY = 0x00e7,
	NW_omawapprovisioning_AttributeToken_INCOMING = 0x00e8,
	NW_omawapprovisioning_AttributeToken_OUTGOING = 0x00e9
}NW_omawapprovisioning_AttributeToken_0_t;


typedef enum NW_omawapprovisioning_ElementToken_1_e{
	NW_omawapprovisioning_ElementToken_characteristic_1 = 0x0106,
	NW_omawapprovisioning_ElementToken_parm_1 = 0x0107
}NW_omawapprovisioning_ElementToken_1_t;


typedef enum NW_omawapprovisioning_AttributeToken_1_e{
	NW_omawapprovisioning_AttributeToken_name_1 = 0x0105,
	NW_omawapprovisioning_AttributeToken_value_1 = 0x0106,
	NW_omawapprovisioning_AttributeToken_name_NAME_1 = 0x0107,
	NW_omawapprovisioning_AttributeToken_name_INTERNET_1 = 0x0114,
	NW_omawapprovisioning_AttributeToken_name_STARTPAGE_1 = 0x011c,
	NW_omawapprovisioning_AttributeToken_name_TO_NAPID_1 = 0x0122,
	NW_omawapprovisioning_AttributeToken_name_PORTNBR_1 = 0x0123,
	NW_omawapprovisioning_AttributeToken_name_SERVICE_1 = 0x0124,
	NW_omawapprovisioning_AttributeToken_name_AACCEPT = 0x012e,
	NW_omawapprovisioning_AttributeToken_name_AAUTHDATA = 0x012f,
	NW_omawapprovisioning_AttributeToken_name_AAUTHLEVEL = 0x0130,
	NW_omawapprovisioning_AttributeToken_name_AAUTHNAME = 0x0131,
	NW_omawapprovisioning_AttributeToken_name_AAUTHSECRET = 0x0132,
	NW_omawapprovisioning_AttributeToken_name_AAUTHTYPE = 0x0133,
	NW_omawapprovisioning_AttributeToken_name_ADDR = 0x0134,
	NW_omawapprovisioning_AttributeToken_name_ADDRTYPE = 0x0135,
	NW_omawapprovisioning_AttributeToken_name_APPID = 0x0136,
	NW_omawapprovisioning_AttributeToken_name_APROTOCOL = 0x0137,
	NW_omawapprovisioning_AttributeToken_name_PROVIDER_ID = 0x0138,
	NW_omawapprovisioning_AttributeToken_name_TO_PROXY = 0x0139,
	NW_omawapprovisioning_AttributeToken_name_URI = 0x013a,
	NW_omawapprovisioning_AttributeToken_name_RULE = 0x013b,
	NW_omawapprovisioning_AttributeToken_name_APPREF = 0x013c,
	NW_omawapprovisioning_AttributeToken_name_TO_APPREF = 0x013d,
	NW_omawapprovisioning_AttributeToken_name_LOGINTW = 0x013e,
	NW_omawapprovisioning_AttributeToken_name_LOGINSCRIPTTYPE = 0x013f,
	NW_omawapprovisioning_AttributeToken_name_LOGINSCRIPTDATA = 0x0140,
	NW_omawapprovisioning_AttributeToken_name_LOGINSCRIPTID = 0x0141,
	NW_omawapprovisioning_AttributeToken_type_1 = 0x0150,
	NW_omawapprovisioning_AttributeToken_type_PORT_1 = 0x0153,
	NW_omawapprovisioning_AttributeToken_type_APPLICATION = 0x0155,
	NW_omawapprovisioning_AttributeToken_type_APPADDR = 0x0156,
	NW_omawapprovisioning_AttributeToken_type_APPAUTH = 0x0157,
	NW_omawapprovisioning_AttributeToken_type_CLIENTIDENTITY_1 = 0x0158,
	NW_omawapprovisioning_AttributeToken_type_RESOURCE = 0x0159,
	NW_omawapprovisioning_AttributeToken_type_LOGINSCRIPT = 0x015a,
	NW_omawapprovisioning_AttributeToken_IPV6_1 = 0x0186,
	NW_omawapprovisioning_AttributeToken_E164_1 = 0x0187,
	NW_omawapprovisioning_AttributeToken_ALPHA_1 = 0x0188,
	NW_omawapprovisioning_AttributeToken_APPSRV = 0x018d,
	NW_omawapprovisioning_AttributeToken_OBEX = 0x018e,
	NW_omawapprovisioning_AttributeToken__ = 0x0190,
	NW_omawapprovisioning_AttributeToken_HTTP_ = 0x0191,
	NW_omawapprovisioning_AttributeToken_BASIC = 0x0192,
	NW_omawapprovisioning_AttributeToken_DIGEST = 0x0193
}NW_omawapprovisioning_AttributeToken_1_t;

// extern const NW_WBXML_Dictionary_t NW_omawapprovisioning_WBXMLDictionary

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Feb 24 12:02:42 2005
**                        (coordinated universal time)
**
** Command line: dictionary provisioning_v12.txt omaprovisioningdictionary.c omaprovisioningdictionary.h
*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
