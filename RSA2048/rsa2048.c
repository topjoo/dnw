/* 
RSA2048 - RSA 2048 encryption using CryptoAPI and C 
Copyright (C) 2016  @maldevel 
 
This program is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version. 
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details. 
 
You should have received a copy of the GNU General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/ 


#include "..\resource.h"

#if CIPHER_RSA2048 // 2018.02.20


#include <Windows.h> 
#include <Wincrypt.h> 
#include <stdio.h> 
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING) 
#include "common.h" 
#include "base64.h" 
#include "rsa2048.h" 
#define RSA2048BIT_KEY 0x8000000 


void EB_Printf(const TCHAR *fmt,...); //engine.h



static BOOL generateKeys(HCRYPTKEY *key, HCRYPTPROV provider, unsigned char **publicKey, unsigned char **privateKey) 
{ 
	unsigned long publicKeyLen = 0; 
	unsigned long privateKeyLen = 0; 


	if (!provider) 
		return false; 


	if (!CryptGenKey(provider, AT_KEYEXCHANGE, RSA2048BIT_KEY | CRYPT_EXPORTABLE, key)) 
	{ 
		EB_Printf(TEXT("[dnw] Error during CryptGenKey. Error(%d) \r\n"), GetLastError() );
		return false; 
	} 


	if (!CryptExportKey(*key, 0, PUBLICKEYBLOB, 0, NULL, &publicKeyLen)) 
	{ 
		if (*key) CryptDestroyKey(*key); 
		return false; 
	} 


	*publicKey = (unsigned char *)malloc(publicKeyLen * sizeof(unsigned char)); 
	if (*publicKey == NULL) 
	{ 
		if (*key) CryptDestroyKey(*key); 
		return false; 
	} 
	SecureZeroMemory(*publicKey, publicKeyLen * sizeof(unsigned char)); 


    // Export the public key. Here the public key is exported to a PUBLICKEYBLOB.
    // This BLOB can be written to a file and sent to another user.
	if (!CryptExportKey(*key, 0, PUBLICKEYBLOB, 0, *publicKey, &publicKeyLen)) 
	{ 
        //printf("Error computing BLOB length.\n");
		EB_Printf(TEXT("[dnw] Error computing BLOB length. Error(%d) \r\n"), GetLastError() );

		SAFE_FREE(*publicKey); 
		if (*key) CryptDestroyKey(*key); 
		return false; 
	} 

	EB_Printf(TEXT("[dnw] publicKeyLen : %ld \r\n"), publicKeyLen );

	#if 1
	{
	unsigned int idx=0, icount=0;
	unsigned char tmpPublicKey[2048];

	memset(tmpPublicKey, 0x00, sizeof(tmpPublicKey) );
	EB_Printf(TEXT("[dnw] RSA2048 Public Key as \r\n") );
	EB_Printf(TEXT("[dnw] ------------------------------------------") );
	for(idx=0; idx<publicKeyLen; idx++)
	{
		tmpPublicKey[idx] = (unsigned char)publicKey[0][idx];

		if(0==icount%16) EB_Printf(TEXT("\n")  );
		EB_Printf(TEXT("%02x:"), tmpPublicKey[idx]  );
		icount++;
	}
	EB_Printf(TEXT("\r\n") );
	//EB_Printf(TEXT("[%s]"), tmpPublicKey);
	}
	#endif


	
	if (!CryptExportKey(*key, 0, PRIVATEKEYBLOB, 0, NULL, &privateKeyLen)) 
	{ 
		SAFE_FREE(*publicKey); 
		if (*key) CryptDestroyKey(*key); 
		return false; 
	} 

	// Allocate memory for the privateKey.
	*privateKey = (unsigned char *)malloc(privateKeyLen * sizeof(unsigned char)); 
	if (*privateKey == NULL) 
	{ 
		SAFE_FREE(*publicKey); 
		if (*key) CryptDestroyKey(*key); 
		return false; 
	} 
	SecureZeroMemory(*privateKey, privateKeyLen * sizeof(unsigned char)); 


	if (!CryptExportKey(*key, 0, PRIVATEKEYBLOB, 0, *privateKey, &privateKeyLen)) 
	{ 
		EB_Printf(TEXT("[dnw] Error computing BLOB2 length. Error(%d) \r\n"), GetLastError() );

		SAFE_FREE(*publicKey); 
		SAFE_FREE(*privateKey); 
		if (*key) CryptDestroyKey(*key); 
		return false; 
	} 

	EB_Printf(TEXT("[dnw] privateKeyLen: %ld \r\n"), privateKeyLen );

	#if 1
	{
		unsigned int idx=0, icount=0;
		unsigned char tmpPrivateKey[1024];
	
		memset(tmpPrivateKey, 0x00, sizeof(tmpPrivateKey) );
		EB_Printf(TEXT("[dnw] RSA2048 Private Key as \r\n") );
		EB_Printf(TEXT("[dnw] ------------------------------------------") );
		for(idx=0; idx<privateKeyLen; idx++)
		{
			tmpPrivateKey[idx] = (unsigned char)privateKey[0][idx];

			if(0==icount%16) EB_Printf(TEXT("\n")  );
			EB_Printf(TEXT("%02x:"), tmpPrivateKey[idx]  );
			icount++;
		}
		EB_Printf(TEXT(" \r\n\r\n") );
		//EB_Printf(TEXT("[%s]"), tmpPrivateKey);
	}
	#endif


	return true; 
} 


BOOL CryptoInit(HCRYPTKEY *key, HCRYPTPROV *provider, unsigned char **publicKey, unsigned char **privateKey) 
{ 
	if (!CryptAcquireContextW(provider, NULL, NULL, PROV_RSA_FULL, 0)) 
	{ 
		if (GetLastError() == NTE_BAD_KEYSET) 
		{ 
			if (!CryptAcquireContextW(provider, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) 
			{ 
				return false; 
			} 
		} 
		else 
		{ 
			return false; 
		} 
	} 


	if (!generateKeys(key, *provider, publicKey, privateKey)) 
	{ 
		if (*provider) CryptReleaseContext(*provider, 0); 
		return false; 
	} 


	return true; 
} 


BOOL Encrypt(HCRYPTKEY key, char **cipherText, unsigned long *cLen, unsigned char *plainText, unsigned long pLen) 
{ 
	unsigned long len = 0; 
	unsigned char *encrypted = 0; 
	unsigned long enLen = 0; 


	len = pLen + 1; 


	if (!CryptEncrypt(key, 0, TRUE, 0, NULL, &len, 0)) 
	{ 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 


	enLen = len; 


	encrypted = (unsigned char *)malloc(len * sizeof(unsigned char)); 
	if (encrypted == NULL) 
	{ 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 
	SecureZeroMemory(encrypted, len * sizeof(unsigned char)); 


	memcpy_s(encrypted, len, plainText, pLen + 1); 


	len = pLen + 1; 
	if (!CryptEncrypt(key, 0, TRUE, 0, encrypted, &len, enLen)) 
	{ 
		SAFE_FREE(encrypted); 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 


	if (!Base64EncodeA(cipherText, cLen, encrypted, enLen)) 
	{ 
		SAFE_FREE(encrypted); 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 


	SAFE_FREE(encrypted); 


	return true; 
} 


BOOL Decrypt(HCRYPTKEY key, unsigned char **plainText, char *cipherText, unsigned long cLen) 
{ 
	unsigned long len = 0; 
	unsigned long decodedLen = 0; 
	char *decoded = 0; 


	if (!Base64DecodeA(&decoded, &decodedLen, cipherText, cLen)) 
	{ 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 


	*plainText = (unsigned char *)malloc(decodedLen * sizeof(unsigned char)); 
	if (*plainText == NULL) 
	{ 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 
	SecureZeroMemory(*plainText, decodedLen * sizeof(unsigned char)); 


	memcpy_s(*plainText, decodedLen, decoded, decodedLen); 


	SAFE_FREE(decoded); 


	len = decodedLen; 
	if (!CryptDecrypt(key, 0, TRUE, 0, *plainText, &len)) 
	{ 
		SAFE_FREE(*plainText); 
		if (key) CryptDestroyKey(key); 
		return false; 
	} 


	return true; 
} 



void CryptoUninit(HCRYPTKEY key, HCRYPTPROV provider) 
{ 
	if (key)  
		if (!CryptDestroyKey(key))  
		{
			//printf("Error: %d\n", GetLastError()); 
			EB_Printf(TEXT("[dnw] CryptoUninit:1: Error(%d) \r\n"), GetLastError() );
		}

	if (provider)  
		if (!CryptReleaseContext(provider, 0))  
		{
			//printf("Error: %d\n", GetLastError()); 
			EB_Printf(TEXT("[dnw] CryptoUninit:2: Error(%d) \r\n"), GetLastError() );
		}

	if (!CryptAcquireContextW(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_DELETEKEYSET)) 
	{
		//printf("Error: %d\n", GetLastError()); 
		EB_Printf(TEXT("[dnw] CryptoUninit:3: Error(%d) \r\n"), GetLastError() );
	}
} 

#endif

