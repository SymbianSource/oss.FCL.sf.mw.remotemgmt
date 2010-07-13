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

/*
 * Dictionary = omawapprovisioning
 * Public id = 0x0B
 * Doc Type = -//WAPFORUM//DTD PROV 1.0//EN
 */

#include "OMAProvisioningDictionary.h"
#include <xml/cxml/nw_wbxml_dictionary.h>


static const NW_Ucs2 NW_omawapprovisioning_ElementTag_wap_provisioningdoc[] = {'w','a','p','-','p','r','o','v','i','s','i','o','n','i','n','g','d','o','c','\0'};
static const NW_Ucs2 NW_omawapprovisioning_ElementTag_characteristic[] = {'c','h','a','r','a','c','t','e','r','i','s','t','i','c','\0'};
static const NW_Ucs2 NW_omawapprovisioning_ElementTag_parm[] = {'p','a','r','m','\0'};

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

/*
 * tag entries - sorted by token
 */
static const
NW_WBXML_DictEntry_t NW_omawapprovisioning_tag_token_0[3] = {
	{0x05, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_ElementTag_wap_provisioningdoc},
	{0x06, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_ElementTag_characteristic},
	{0x07, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_ElementTag_parm}
};

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

/*
 * tag entries - sorted by name
 */
static const
NW_Byte NW_omawapprovisioning_tag_name_0[3] = {
	1,
	2,
	0,
};

static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name[] = {'n','a','m','e','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_value[] = {'v','a','l','u','e','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NAME[] = {'n','a','m','e','=','N','A','M','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NAP_ADDRESS[] = {'n','a','m','e','=','N','A','P','-','A','D','D','R','E','S','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NAP_ADDRTYPE[] = {'n','a','m','e','=','N','A','P','-','A','D','D','R','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_CALLTYPE[] = {'n','a','m','e','=','C','A','L','L','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_VALIDUNTIL[] = {'n','a','m','e','=','V','A','L','I','D','U','N','T','I','L','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AUTHTYPE[] = {'n','a','m','e','=','A','U','T','H','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AUTHNAME[] = {'n','a','m','e','=','A','U','T','H','N','A','M','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AUTHSECRET[] = {'n','a','m','e','=','A','U','T','H','S','E','C','R','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LINGER[] = {'n','a','m','e','=','L','I','N','G','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_BEARER[] = {'n','a','m','e','=','B','E','A','R','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NAPID[] = {'n','a','m','e','=','N','A','P','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_COUNTRY[] = {'n','a','m','e','=','C','O','U','N','T','R','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NETWORK[] = {'n','a','m','e','=','N','E','T','W','O','R','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_INTERNET[] = {'n','a','m','e','=','I','N','T','E','R','N','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PROXY_ID[] = {'n','a','m','e','=','P','R','O','X','Y','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PROXY_PROVIDER_ID[] = {'n','a','m','e','=','P','R','O','X','Y','-','P','R','O','V','I','D','E','R','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DOMAIN[] = {'n','a','m','e','=','D','O','M','A','I','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PROVURL[] = {'n','a','m','e','=','P','R','O','V','U','R','L','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PXAUTH_TYPE[] = {'n','a','m','e','=','P','X','A','U','T','H','-','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PXAUTH_ID[] = {'n','a','m','e','=','P','X','A','U','T','H','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PXAUTH_PW[] = {'n','a','m','e','=','P','X','A','U','T','H','-','P','W','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_STARTPAGE[] = {'n','a','m','e','=','S','T','A','R','T','P','A','G','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_BASAUTH_ID[] = {'n','a','m','e','=','B','A','S','A','U','T','H','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_BASAUTH_PW[] = {'n','a','m','e','=','B','A','S','A','U','T','H','-','P','W','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PUSHENABLED[] = {'n','a','m','e','=','P','U','S','H','E','N','A','B','L','E','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PXADDR[] = {'n','a','m','e','=','P','X','A','D','D','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PXADDRTYPE[] = {'n','a','m','e','=','P','X','A','D','D','R','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TO_NAPID[] = {'n','a','m','e','=','T','O','-','N','A','P','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PORTNBR[] = {'n','a','m','e','=','P','O','R','T','N','B','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SERVICE[] = {'n','a','m','e','=','S','E','R','V','I','C','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LINKSPEED[] = {'n','a','m','e','=','L','I','N','K','S','P','E','E','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DNLINKSPEED[] = {'n','a','m','e','=','D','N','L','I','N','K','S','P','E','E','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LOCAL_ADDR[] = {'n','a','m','e','=','L','O','C','A','L','-','A','D','D','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LOCAL_ADDRTYPE[] = {'n','a','m','e','=','L','O','C','A','L','-','A','D','D','R','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_CONTEXT_ALLOW[] = {'n','a','m','e','=','C','O','N','T','E','X','T','-','A','L','L','O','W','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TRUST[] = {'n','a','m','e','=','T','R','U','S','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_MASTER[] = {'n','a','m','e','=','M','A','S','T','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SID[] = {'n','a','m','e','=','S','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SOC[] = {'n','a','m','e','=','S','O','C','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_WSP_VERSION[] = {'n','a','m','e','=','W','S','P','-','V','E','R','S','I','O','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PHYSICAL_PROXY_ID[] = {'n','a','m','e','=','P','H','Y','S','I','C','A','L','-','P','R','O','X','Y','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_CLIENT_ID[] = {'n','a','m','e','=','C','L','I','E','N','T','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DELIVERY_ERR_SDU[] = {'n','a','m','e','=','D','E','L','I','V','E','R','Y','-','E','R','R','-','S','D','U','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DELIVERY_ORDER[] = {'n','a','m','e','=','D','E','L','I','V','E','R','Y','-','O','R','D','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TRAFFIC_CLASS[] = {'n','a','m','e','=','T','R','A','F','F','I','C','-','C','L','A','S','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_MAX_SDU_SIZE[] = {'n','a','m','e','=','M','A','X','-','S','D','U','-','S','I','Z','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_MAX_BITRATE_UPLINK[] = {'n','a','m','e','=','M','A','X','-','B','I','T','R','A','T','E','-','U','P','L','I','N','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_MAX_BITRATE_DNLINK[] = {'n','a','m','e','=','M','A','X','-','B','I','T','R','A','T','E','-','D','N','L','I','N','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_RESIDUAL_BER[] = {'n','a','m','e','=','R','E','S','I','D','U','A','L','-','B','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SDU_ERROR_RATIO[] = {'n','a','m','e','=','S','D','U','-','E','R','R','O','R','-','R','A','T','I','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TRAFFIC_HANDL_PRIO[] = {'n','a','m','e','=','T','R','A','F','F','I','C','-','H','A','N','D','L','-','P','R','I','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TRANSFER_DELAY[] = {'n','a','m','e','=','T','R','A','N','S','F','E','R','-','D','E','L','A','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_GUARANTEED_BITRATE_UPLINK[] = {'n','a','m','e','=','G','U','A','R','A','N','T','E','E','D','-','B','I','T','R','A','T','E','-','U','P','L','I','N','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_GUARANTEED_BITRATE_DNLINK[] = {'n','a','m','e','=','G','U','A','R','A','N','T','E','E','D','-','B','I','T','R','A','T','E','-','D','N','L','I','N','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PXADDR_FQDN[] = {'n','a','m','e','=','P','X','A','D','D','R','-','F','Q','D','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PROXY_PW[] = {'n','a','m','e','=','P','R','O','X','Y','-','P','W','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PPGAUTH_TYPE[] = {'n','a','m','e','=','P','P','G','A','U','T','H','-','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_version[] = {'v','e','r','s','i','o','n','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_version_1_0[] = {'v','e','r','s','i','o','n','=','1','.','0','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PULLENABLED[] = {'n','a','m','e','=','P','U','L','L','E','N','A','B','L','E','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DNS_ADDR[] = {'n','a','m','e','=','D','N','S','-','A','D','D','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_MAX_NUM_RETRY[] = {'n','a','m','e','=','M','A','X','-','N','U','M','-','R','E','T','R','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_FIRST_RETRY_TIMEOUT[] = {'n','a','m','e','=','F','I','R','S','T','-','R','E','T','R','Y','-','T','I','M','E','O','U','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_REREG_THRESHOLD[] = {'n','a','m','e','=','R','E','R','E','G','-','T','H','R','E','S','H','O','L','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_T_BIT[] = {'n','a','m','e','=','T','-','B','I','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AUTH_ENTITY[] = {'n','a','m','e','=','A','U','T','H','-','E','N','T','I','T','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SPI[] = {'n','a','m','e','=','S','P','I','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type[] = {'t','y','p','e','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_PXLOGICAL[] = {'t','y','p','e','=','P','X','L','O','G','I','C','A','L','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_PXPHYSICAL[] = {'t','y','p','e','=','P','X','P','H','Y','S','I','C','A','L','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_PORT[] = {'t','y','p','e','=','P','O','R','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_VALIDITY[] = {'t','y','p','e','=','V','A','L','I','D','I','T','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_NAPDEF[] = {'t','y','p','e','=','N','A','P','D','E','F','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_BOOTSTRAP[] = {'t','y','p','e','=','B','O','O','T','S','T','R','A','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_VENDORCONFIG[] = {'t','y','p','e','=','V','E','N','D','O','R','C','O','N','F','I','G','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_CLIENTIDENTITY[] = {'t','y','p','e','=','C','L','I','E','N','T','I','D','E','N','T','I','T','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_PXAUTHINFO[] = {'t','y','p','e','=','P','X','A','U','T','H','I','N','F','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_NAPAUTHINFO[] = {'t','y','p','e','=','N','A','P','A','U','T','H','I','N','F','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_ACCESS[] = {'t','y','p','e','=','A','C','C','E','S','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_BEARERINFO[] = {'t','y','p','e','=','B','E','A','R','E','R','I','N','F','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_DNS_ADDRINFO[] = {'t','y','p','e','=','D','N','S','-','A','D','D','R','I','N','F','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_WLAN[] = {'t','y','p','e','=','W','L','A','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_WEPKEY[] = {'t','y','p','e','=','W','E','P','K','E','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DIRECTION[] = {'n','a','m','e','=','D','I','R','E','C','T','I','O','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DNS_ADDRTYPE[] = {'n','a','m','e','=','D','N','S','-','A','D','D','R','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DNS_PRIORITY[] = {'n','a','m','e','=','D','N','S','-','P','R','I','O','R','I','T','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SOURCE_STATISTICS_DESCRIPTOR[] = {'n','a','m','e','=','S','O','U','R','C','E','-','S','T','A','T','I','S','T','I','C','S','-','D','E','S','C','R','I','P','T','O','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SIGNALLING_INDICATION[] = {'n','a','m','e','=','S','I','G','N','A','L','L','I','N','G','-','I','N','D','I','C','A','T','I','O','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DEFGW[] = {'n','a','m','e','=','D','E','F','G','W','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NETWORKMASK[] = {'n','a','m','e','=','N','E','T','W','O','R','K','M','A','S','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_USECB[] = {'n','a','m','e','=','U','S','E','C','B','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_CBNBR[] = {'n','a','m','e','=','C','B','N','B','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PPPCOMP[] = {'n','a','m','e','=','P','P','P','C','O','M','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TO_LOGINSCRIPTID[] = {'n','a','m','e','=','T','O','-','L','O','G','I','N','S','C','R','I','P','T','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_USEPTXTLOG[] = {'n','a','m','e','=','U','S','E','P','T','X','T','L','O','G','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_GPRSPDP[] = {'n','a','m','e','=','G','P','R','S','P','D','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_MODEMINIT[] = {'n','a','m','e','=','M','O','D','E','M','I','N','I','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_IPADDRFROMSERVER[] = {'n','a','m','e','=','I','P','A','D','D','R','F','R','O','M','S','E','R','V','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_IPDNSADDRFROMSERVER[] = {'n','a','m','e','=','I','P','D','N','S','A','D','D','R','F','R','O','M','S','E','R','V','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_IPV6DNSADDRFROMSERVER[] = {'n','a','m','e','=','I','P','V','6','D','N','S','A','D','D','R','F','R','O','M','S','E','R','V','E','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_IFNETWORKS[] = {'n','a','m','e','=','I','F','N','E','T','W','O','R','K','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_IAPSERVICE[] = {'n','a','m','e','=','I','A','P','S','E','R','V','I','C','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SSID[] = {'n','a','m','e','=','S','S','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NETWORKMODE[] = {'n','a','m','e','=','N','E','T','W','O','R','K','M','O','D','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SECURITYMODE[] = {'n','a','m','e','=','S','E','C','U','R','I','T','Y','M','O','D','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_WPAPRESHAREDKEY[] = {'n','a','m','e','=','W','P','A','P','R','E','S','H','A','R','E','D','K','E','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LENGTH[] = {'n','a','m','e','=','L','E','N','G','T','H','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_DATA[] = {'n','a','m','e','=','D','A','T','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IPV4[] = {'I','P','V','4','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IPV6[] = {'I','P','V','6','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_E164[] = {'E','1','6','4','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ALPHA[] = {'A','L','P','H','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_APN[] = {'A','P','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_SCODE[] = {'S','C','O','D','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_TETRA_ITSI[] = {'T','E','T','R','A','-','I','T','S','I','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_MAN[] = {'M','A','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_PPP[] = {'P','P','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ANALOG_MODEM[] = {'A','N','A','L','O','G','-','M','O','D','E','M','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_V_120[] = {'V','.','1','2','0','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_V_110[] = {'V','.','1','1','0','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_X_31[] = {'X','.','3','1','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_BIT_TRANSPARENT[] = {'B','I','T','-','T','R','A','N','S','P','A','R','E','N','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_DIRECT_ASYNCHRONOUS_DATA_SERVICE[] = {'D','I','R','E','C','T','-','A','S','Y','N','C','H','R','O','N','O','U','S','-','D','A','T','A','-','S','E','R','V','I','C','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_PAP[] = {'P','A','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CHAP[] = {'C','H','A','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_HTTP_BASIC[] = {'H','T','T','P','-','B','A','S','I','C','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_HTTP_DIGEST[] = {'H','T','T','P','-','D','I','G','E','S','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_WTLS_SS[] = {'W','T','L','S','-','S','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_MD5[] = {'M','D','5','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_GSM_USSD[] = {'G','S','M','-','U','S','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_GSM_SMS[] = {'G','S','M','-','S','M','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ANSI_136_GUTS[] = {'A','N','S','I','-','1','3','6','-','G','U','T','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IS_95_CDMA_SMS[] = {'I','S','-','9','5','-','C','D','M','A','-','S','M','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IS_95_CDMA_CSD[] = {'I','S','-','9','5','-','C','D','M','A','-','C','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IS_95_CDMA_PACKET[] = {'I','S','-','9','5','-','C','D','M','A','-','P','A','C','K','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ANSI_136_CSD[] = {'A','N','S','I','-','1','3','6','-','C','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ANSI_136_GPRS[] = {'A','N','S','I','-','1','3','6','-','G','P','R','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_GSM_CSD[] = {'G','S','M','-','C','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_GSM_GPRS[] = {'G','S','M','-','G','P','R','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_AMPS_CDPD[] = {'A','M','P','S','-','C','D','P','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_PDC_CSD[] = {'P','D','C','-','C','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_PDC_PACKET[] = {'P','D','C','-','P','A','C','K','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IDEN_SMS[] = {'I','D','E','N','-','S','M','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IDEN_CSD[] = {'I','D','E','N','-','C','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IDEN_PACKET[] = {'I','D','E','N','-','P','A','C','K','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_FLEX_REFLEX[] = {'F','L','E','X','/','R','E','F','L','E','X','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_PHS_SMS[] = {'P','H','S','-','S','M','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_PHS_CSD[] = {'P','H','S','-','C','S','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_TETRA_SDS[] = {'T','E','T','R','A','-','S','D','S','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_TETRA_PACKET[] = {'T','E','T','R','A','-','P','A','C','K','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ANSI_136_GHOST[] = {'A','N','S','I','-','1','3','6','-','G','H','O','S','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_MOBITEX_MPAK[] = {'M','O','B','I','T','E','X','-','M','P','A','K','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CDMA2000_1X_SIMPLE_IP[] = {'C','D','M','A','2','0','0','0','-','1','X','-','S','I','M','P','L','E','-','I','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CDMA2000_1X_MOBILE_IP[] = {'C','D','M','A','2','0','0','0','-','1','X','-','M','O','B','I','L','E','-','I','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag__3G_GSM[] = {'3','G','-','G','S','M','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_WLAN[] = {'W','L','A','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_AUTOBAUDING[] = {'A','U','T','O','B','A','U','D','I','N','G','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CL_WSP[] = {'C','L','-','W','S','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CO_WSP[] = {'C','O','-','W','S','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CL_SEC_WSP[] = {'C','L','-','S','E','C','-','W','S','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CO_SEC_WSP[] = {'C','O','-','S','E','C','-','W','S','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CL_SEC_WTA[] = {'C','L','-','S','E','C','-','W','T','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_CO_SEC_WTA[] = {'C','O','-','S','E','C','-','W','T','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_OTA_HTTP_TO[] = {'O','T','A','-','H','T','T','P','-','T','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_OTA_HTTP_TLS_TO[] = {'O','T','A','-','H','T','T','P','-','T','L','S','-','T','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_OTA_HTTP_PO[] = {'O','T','A','-','H','T','T','P','-','P','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_OTA_HTTP_TLS_PO[] = {'O','T','A','-','H','T','T','P','-','T','L','S','-','P','O','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_AAA[] = {'A','A','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_HA[] = {'H','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_INFRASTRUCTURE[] = {'I','N','F','R','A','S','T','R','U','C','T','U','R','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ADHOC[] = {'A','D','H','O','C','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_WEP[] = {'W','E','P','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag__8021X[] = {'8','0','2','1','X','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_WPA[] = {'W','P','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_WPA_PRESHARED_KEY[] = {'W','P','A','-','P','R','E','S','H','A','R','E','D','-','K','E','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_INCOMING[] = {'I','N','C','O','M','I','N','G','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_OUTGOING[] = {'O','U','T','G','O','I','N','G','\0'};

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

/*
 * attribute entries - sorted by token
 */
static const
NW_WBXML_DictEntry_t NW_omawapprovisioning_attribute_token_0[179] = {
	{0x05, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name},
	{0x06, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_value},
	{0x07, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NAME},
	{0x08, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NAP_ADDRESS},
	{0x09, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NAP_ADDRTYPE},
	{0x0a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_CALLTYPE},
	{0x0b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_VALIDUNTIL},
	{0x0c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AUTHTYPE},
	{0x0d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AUTHNAME},
	{0x0e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AUTHSECRET},
	{0x0f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LINGER},
	{0x10, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_BEARER},
	{0x11, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NAPID},
	{0x12, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_COUNTRY},
	{0x13, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NETWORK},
	{0x14, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_INTERNET},
	{0x15, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PROXY_ID},
	{0x16, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PROXY_PROVIDER_ID},
	{0x17, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DOMAIN},
	{0x18, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PROVURL},
	{0x19, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PXAUTH_TYPE},
	{0x1a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PXAUTH_ID},
	{0x1b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PXAUTH_PW},
	{0x1c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_STARTPAGE},
	{0x1d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_BASAUTH_ID},
	{0x1e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_BASAUTH_PW},
	{0x1f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PUSHENABLED},
	{0x20, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PXADDR},
	{0x21, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PXADDRTYPE},
	{0x22, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TO_NAPID},
	{0x23, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PORTNBR},
	{0x24, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SERVICE},
	{0x25, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LINKSPEED},
	{0x26, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DNLINKSPEED},
	{0x27, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LOCAL_ADDR},
	{0x28, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LOCAL_ADDRTYPE},
	{0x29, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_CONTEXT_ALLOW},
	{0x2a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TRUST},
	{0x2b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_MASTER},
	{0x2c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SID},
	{0x2d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SOC},
	{0x2e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_WSP_VERSION},
	{0x2f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PHYSICAL_PROXY_ID},
	{0x30, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_CLIENT_ID},
	{0x31, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DELIVERY_ERR_SDU},
	{0x32, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DELIVERY_ORDER},
	{0x33, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TRAFFIC_CLASS},
	{0x34, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_MAX_SDU_SIZE},
	{0x35, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_MAX_BITRATE_UPLINK},
	{0x36, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_MAX_BITRATE_DNLINK},
	{0x37, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_RESIDUAL_BER},
	{0x38, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SDU_ERROR_RATIO},
	{0x39, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TRAFFIC_HANDL_PRIO},
	{0x3a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TRANSFER_DELAY},
	{0x3b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_GUARANTEED_BITRATE_UPLINK},
	{0x3c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_GUARANTEED_BITRATE_DNLINK},
	{0x3d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PXADDR_FQDN},
	{0x3e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PROXY_PW},
	{0x3f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PPGAUTH_TYPE},
	{0x45, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_version},
	{0x46, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_version_1_0},
	{0x47, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PULLENABLED},
	{0x48, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DNS_ADDR},
	{0x49, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_MAX_NUM_RETRY},
	{0x4a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_FIRST_RETRY_TIMEOUT},
	{0x4b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_REREG_THRESHOLD},
	{0x4c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_T_BIT},
	{0x4e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AUTH_ENTITY},
	{0x4f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SPI},
	{0x50, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type},
	{0x51, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_PXLOGICAL},
	{0x52, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_PXPHYSICAL},
	{0x53, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_PORT},
	{0x54, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_VALIDITY},
	{0x55, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_NAPDEF},
	{0x56, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_BOOTSTRAP},
	{0x57, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_VENDORCONFIG},
	{0x58, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_CLIENTIDENTITY},
	{0x59, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_PXAUTHINFO},
	{0x5a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_NAPAUTHINFO},
	{0x5b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_ACCESS},
	{0x5c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_BEARERINFO},
	{0x5d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_DNS_ADDRINFO},
	{0x5e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_WLAN},
	{0x5f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_WEPKEY},
	{0x60, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DIRECTION},
	{0x61, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DNS_ADDRTYPE},
	{0x62, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DNS_PRIORITY},
	{0x63, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SOURCE_STATISTICS_DESCRIPTOR},
	{0x64, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SIGNALLING_INDICATION},
	{0x65, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DEFGW},
	{0x66, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NETWORKMASK},
	{0x67, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_USECB},
	{0x68, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_CBNBR},
	{0x69, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PPPCOMP},
	{0x6a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TO_LOGINSCRIPTID},
	{0x6b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_USEPTXTLOG},
	{0x6c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_GPRSPDP},
	{0x6d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_MODEMINIT},
	{0x6e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_IPADDRFROMSERVER},
	{0x6f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_IPDNSADDRFROMSERVER},
	{0x70, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_IPV6DNSADDRFROMSERVER},
	{0x71, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_IFNETWORKS},
	{0x72, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_IAPSERVICE},
	{0x73, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SSID},
	{0x74, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NETWORKMODE},
	{0x75, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SECURITYMODE},
	{0x76, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_WPAPRESHAREDKEY},
	{0x77, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LENGTH},
	{0x78, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_DATA},
	{0x85, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IPV4},
	{0x86, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IPV6},
	{0x87, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_E164},
	{0x88, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ALPHA},
	{0x89, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_APN},
	{0x8a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_SCODE},
	{0x8b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_TETRA_ITSI},
	{0x8c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_MAN},
	{0x8f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_PPP},
	{0x90, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ANALOG_MODEM},
	{0x91, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_V_120},
	{0x92, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_V_110},
	{0x93, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_X_31},
	{0x94, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_BIT_TRANSPARENT},
	{0x95, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_DIRECT_ASYNCHRONOUS_DATA_SERVICE},
	{0x9a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_PAP},
	{0x9b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CHAP},
	{0x9c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_HTTP_BASIC},
	{0x9d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_HTTP_DIGEST},
	{0x9e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_WTLS_SS},
	{0x9f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_MD5},
	{0xa2, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_GSM_USSD},
	{0xa3, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_GSM_SMS},
	{0xa4, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ANSI_136_GUTS},
	{0xa5, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IS_95_CDMA_SMS},
	{0xa6, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IS_95_CDMA_CSD},
	{0xa7, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IS_95_CDMA_PACKET},
	{0xa8, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ANSI_136_CSD},
	{0xa9, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ANSI_136_GPRS},
	{0xaa, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_GSM_CSD},
	{0xab, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_GSM_GPRS},
	{0xac, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_AMPS_CDPD},
	{0xad, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_PDC_CSD},
	{0xae, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_PDC_PACKET},
	{0xaf, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IDEN_SMS},
	{0xb0, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IDEN_CSD},
	{0xb1, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IDEN_PACKET},
	{0xb2, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_FLEX_REFLEX},
	{0xb3, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_PHS_SMS},
	{0xb4, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_PHS_CSD},
	{0xb5, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_TETRA_SDS},
	{0xb6, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_TETRA_PACKET},
	{0xb7, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ANSI_136_GHOST},
	{0xb8, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_MOBITEX_MPAK},
	{0xb9, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CDMA2000_1X_SIMPLE_IP},
	{0xba, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CDMA2000_1X_MOBILE_IP},
	{0xbb, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag__3G_GSM},
	{0xbc, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_WLAN},
	{0xc5, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_AUTOBAUDING},
	{0xca, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CL_WSP},
	{0xcb, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CO_WSP},
	{0xcc, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CL_SEC_WSP},
	{0xcd, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CO_SEC_WSP},
	{0xce, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CL_SEC_WTA},
	{0xcf, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_CO_SEC_WTA},
	{0xd0, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_OTA_HTTP_TO},
	{0xd1, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_OTA_HTTP_TLS_TO},
	{0xd2, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_OTA_HTTP_PO},
	{0xd3, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_OTA_HTTP_TLS_PO},
	{0xe0, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_AAA},
	{0xe1, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_HA},
	{0xe2, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_INFRASTRUCTURE},
	{0xe3, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ADHOC},
	{0xe4, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_WEP},
	{0xe5, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag__8021X},
	{0xe6, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_WPA},
	{0xe7, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_WPA_PRESHARED_KEY},
	{0xe8, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_INCOMING},
	{0xe9, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_OUTGOING}
};

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

/*
 * attribute entries - sorted by name
 */
static const
NW_Byte NW_omawapprovisioning_attribute_name_0[179] = {
	156,
	174,
	169,
	172,
	113,
	141,
	119,
	137,
	152,
	138,
	133,
	114,
	158,
	123,
	155,
	154,
	126,
	161,
	163,
	159,
	162,
	164,
	160,
	124,
	112,
	147,
	139,
	140,
	132,
	131,
	170,
	127,
	128,
	145,
	146,
	144,
	177,
	171,
	110,
	111,
	135,
	136,
	134,
	117,
	130,
	153,
	167,
	168,
	166,
	165,
	178,
	125,
	142,
	143,
	149,
	148,
	118,
	115,
	116,
	151,
	150,
	121,
	120,
	173,
	157,
	175,
	176,
	129,
	122,
	0,
	67,
	8,
	9,
	7,
	24,
	25,
	11,
	5,
	93,
	43,
	36,
	13,
	109,
	90,
	44,
	45,
	85,
	33,
	62,
	86,
	87,
	18,
	64,
	97,
	55,
	54,
	103,
	102,
	15,
	99,
	100,
	101,
	108,
	10,
	32,
	34,
	35,
	38,
	49,
	48,
	63,
	47,
	98,
	2,
	3,
	4,
	12,
	14,
	91,
	105,
	42,
	30,
	58,
	94,
	19,
	16,
	17,
	57,
	61,
	26,
	27,
	56,
	28,
	21,
	22,
	20,
	65,
	50,
	51,
	106,
	31,
	39,
	89,
	40,
	88,
	68,
	104,
	23,
	66,
	95,
	29,
	46,
	52,
	53,
	37,
	92,
	96,
	6,
	107,
	41,
	69,
	80,
	81,
	75,
	77,
	82,
	79,
	74,
	72,
	78,
	70,
	71,
	73,
	76,
	84,
	83,
	1,
	59,
	60,
};

//static const NW_Ucs2 NW_omawapprovisioning_ElementTag_characteristic_1[] = {'c','h','a','r','a','c','t','e','r','i','s','t','i','c','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_ElementTag_parm_1[] = {'p','a','r','m','\0'}; // Commented to remove warning in armv5

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

/*
 * tag entries - sorted by token
 */
static const
NW_WBXML_DictEntry_t NW_omawapprovisioning_tag_token_1[2] = {
	{0x06, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_ElementTag_characteristic},
	{0x07, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_ElementTag_parm}
};

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

/*
 * tag entries - sorted by name
 */
static const
NW_Byte NW_omawapprovisioning_tag_name_1[2] = {
	0,
	1,
};

//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_1[] = {'n','a','m','e','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_value_1[] = {'v','a','l','u','e','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_NAME_1[] = {'n','a','m','e','=','N','A','M','E','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_INTERNET_1[] = {'n','a','m','e','=','I','N','T','E','R','N','E','T','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_STARTPAGE_1[] = {'n','a','m','e','=','S','T','A','R','T','P','A','G','E','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TO_NAPID_1[] = {'n','a','m','e','=','T','O','-','N','A','P','I','D','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PORTNBR_1[] = {'n','a','m','e','=','P','O','R','T','N','B','R','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_SERVICE_1[] = {'n','a','m','e','=','S','E','R','V','I','C','E','\0'}; // Commented to remove warning in armv5
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AACCEPT[] = {'n','a','m','e','=','A','A','C','C','E','P','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AAUTHDATA[] = {'n','a','m','e','=','A','A','U','T','H','D','A','T','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AAUTHLEVEL[] = {'n','a','m','e','=','A','A','U','T','H','L','E','V','E','L','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AAUTHNAME[] = {'n','a','m','e','=','A','A','U','T','H','N','A','M','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AAUTHSECRET[] = {'n','a','m','e','=','A','A','U','T','H','S','E','C','R','E','T','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_AAUTHTYPE[] = {'n','a','m','e','=','A','A','U','T','H','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_ADDR[] = {'n','a','m','e','=','A','D','D','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_ADDRTYPE[] = {'n','a','m','e','=','A','D','D','R','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_APPID[] = {'n','a','m','e','=','A','P','P','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_APROTOCOL[] = {'n','a','m','e','=','A','P','R','O','T','O','C','O','L','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_PROVIDER_ID[] = {'n','a','m','e','=','P','R','O','V','I','D','E','R','-','I','D','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TO_PROXY[] = {'n','a','m','e','=','T','O','-','P','R','O','X','Y','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_URI[] = {'n','a','m','e','=','U','R','I','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_RULE[] = {'n','a','m','e','=','R','U','L','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_APPREF[] = {'n','a','m','e','=','A','P','P','R','E','F','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_TO_APPREF[] = {'n','a','m','e','=','T','O','-','A','P','P','R','E','F','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LOGINTW[] = {'n','a','m','e','=','L','O','G','I','N','T','W','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LOGINSCRIPTTYPE[] = {'n','a','m','e','=','L','O','G','I','N','S','C','R','I','P','T','T','Y','P','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LOGINSCRIPTDATA[] = {'n','a','m','e','=','L','O','G','I','N','S','C','R','I','P','T','D','A','T','A','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_name_LOGINSCRIPTID[] = {'n','a','m','e','=','L','O','G','I','N','S','C','R','I','P','T','I','D','\0'};
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_1[] = {'t','y','p','e','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_PORT_1[] = {'t','y','p','e','=','P','O','R','T','\0'}; // Commented to remove warning in armv5
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_APPLICATION[] = {'t','y','p','e','=','A','P','P','L','I','C','A','T','I','O','N','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_APPADDR[] = {'t','y','p','e','=','A','P','P','A','D','D','R','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_APPAUTH[] = {'t','y','p','e','=','A','P','P','A','U','T','H','\0'};
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_CLIENTIDENTITY_1[] = {'t','y','p','e','=','C','L','I','E','N','T','I','D','E','N','T','I','T','Y','\0'}; // Commented to remove warning in armv5
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_RESOURCE[] = {'t','y','p','e','=','R','E','S','O','U','R','C','E','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_type_LOGINSCRIPT[] = {'t','y','p','e','=','L','O','G','I','N','S','C','R','I','P','T','\0'};
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_IPV6_1[] = {'I','P','V','6','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_E164_1[] = {'E','1','6','4','\0'}; // Commented to remove warning in armv5
//static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_ALPHA_1[] = {'A','L','P','H','A','\0'}; // Commented to remove warning in armv5
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_APPSRV[] = {'A','P','P','S','R','V','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_OBEX[] = {'O','B','E','X','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag__[] = {',','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_HTTP_[] = {'H','T','T','P','-','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_BASIC[] = {'B','A','S','I','C','\0'};
static const NW_Ucs2 NW_omawapprovisioning_AttributeTag_DIGEST[] = {'D','I','G','E','S','T','\0'};

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

/*
 * attribute entries - sorted by token
 */
static const
NW_WBXML_DictEntry_t NW_omawapprovisioning_attribute_token_1[45] = {
	{0x05, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name},
	{0x06, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_value},
	{0x07, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_NAME},
	{0x14, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_INTERNET},
	{0x1c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_STARTPAGE},
	{0x22, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TO_NAPID},
	{0x23, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PORTNBR},
	{0x24, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_SERVICE},
	{0x2e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AACCEPT},
	{0x2f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AAUTHDATA},
	{0x30, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AAUTHLEVEL},
	{0x31, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AAUTHNAME},
	{0x32, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AAUTHSECRET},
	{0x33, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_AAUTHTYPE},
	{0x34, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_ADDR},
	{0x35, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_ADDRTYPE},
	{0x36, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_APPID},
	{0x37, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_APROTOCOL},
	{0x38, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_PROVIDER_ID},
	{0x39, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TO_PROXY},
	{0x3a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_URI},
	{0x3b, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_RULE},
	{0x3c, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_APPREF},
	{0x3d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_TO_APPREF},
	{0x3e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LOGINTW},
	{0x3f, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LOGINSCRIPTTYPE},
	{0x40, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LOGINSCRIPTDATA},
	{0x41, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_name_LOGINSCRIPTID},
	{0x50, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type},
	{0x53, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_PORT},
	{0x55, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_APPLICATION},
	{0x56, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_APPADDR},
	{0x57, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_APPAUTH},
	{0x58, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_CLIENTIDENTITY},
	{0x59, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_RESOURCE},
	{0x5a, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_type_LOGINSCRIPT},
	{0x86, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_IPV6},
	{0x87, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_E164},
	{0x88, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_ALPHA},
	{0x8d, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_APPSRV},
	{0x8e, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_OBEX},
	{0x90, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag__},
	{0x91, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_HTTP_},
	{0x92, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_BASIC},
	{0x93, (NW_String_UCS2Buff_t *) NW_omawapprovisioning_AttributeTag_DIGEST}
};

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

/*
 * attribute entries - sorted by name
 */
static const
NW_Byte NW_omawapprovisioning_attribute_name_1[45] = {
	41,
	38,
	39,
	43,
	44,
	37,
	42,
	36,
	40,
	0,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	22,
	17,
	3,
	26,
	27,
	25,
	24,
	2,
	6,
	18,
	21,
	7,
	4,
	23,
	5,
	19,
	20,
	28,
	31,
	32,
	30,
	33,
	35,
	29,
	34,
	1,
};

/*
 * Tag codepage table
 */
static const
NW_WBXML_Codepage_t NW_omawapprovisioning_tag_codepages[2] = {
	{3, (NW_WBXML_DictEntry_t*)&NW_omawapprovisioning_tag_token_0[0], (NW_Byte *)&NW_omawapprovisioning_tag_name_0[0]},
	{2, (NW_WBXML_DictEntry_t*)&NW_omawapprovisioning_tag_token_1[0], (NW_Byte *)&NW_omawapprovisioning_tag_name_1[0]},
};

/*
 * Attribute codepage table
 */
static const
NW_WBXML_Codepage_t NW_omawapprovisioning_attribute_codepages[2] = {
	{179, (NW_WBXML_DictEntry_t*)&NW_omawapprovisioning_attribute_token_0[0], (NW_Byte *)&NW_omawapprovisioning_attribute_name_0[0]},
	{45, (NW_WBXML_DictEntry_t*)&NW_omawapprovisioning_attribute_token_1[0], (NW_Byte *)&NW_omawapprovisioning_attribute_name_1[0]},
};

static const NW_Ucs2 NW_omawapprovisioning_docType[] = {'-','/','/','W','A','P','F','O','R','U','M','/','/','D','T','D',' ','P','R','O','V',' ','1','.','0','/','/','E','N','\0'};

/*
 * Dictionary
 */
const NW_WBXML_Dictionary_t NW_omawapprovisioning_WBXMLDictionary = {
	NW_omawapprovisioning_PublicId,
	(NW_Ucs2 *)NW_omawapprovisioning_docType,
	2, (NW_WBXML_Codepage_t*)&NW_omawapprovisioning_tag_codepages[0],
	2, (NW_WBXML_Codepage_t*)&NW_omawapprovisioning_attribute_codepages[0],
};

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
