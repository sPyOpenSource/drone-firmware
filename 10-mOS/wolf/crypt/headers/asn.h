/* asn.h
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/*!
    \file wolfssl/wolfcrypt/asn.h
*/

#ifndef WOLF_CRYPT_ASN_H
#define WOLF_CRYPT_ASN_H

#include <wolf/crypt/headers/types.h>

#ifndef NO_ASN


#if !defined(NO_ASN_TIME) && defined(NO_TIME_H)
    #define NO_ASN_TIME /* backwards compatibility with NO_TIME_H */
#endif

#include <wolf/crypt/headers/integer.h>

/* fips declare of RsaPrivateKeyDecode @wc_fips */
#if defined(HAVE_FIPS) && !defined(NO_RSA) && \
	(!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
    #include <cyassl/ctaocrypt/rsa.h>
#endif

#ifndef NO_DH
    #include <wolf/crypt/headers/dh.h>
#endif
#ifndef NO_DSA
    #include <wolf/crypt/headers/dsa.h>
#endif
#ifndef NO_SHA
    #include <wolf/crypt/headers/sha.h>
#endif
#ifndef NO_MD5
    #include <wolf/crypt/headers/md5.h>
#endif
#include <wolf/crypt/headers/sha256.h>
#include <wolf/crypt/headers/asn_public.h>   /* public interface */


#ifdef __cplusplus
    extern "C" {
#endif

enum {
    ISSUER  = 0,
    SUBJECT = 1,

    EXTERNAL_SERIAL_SIZE = 32,

    BEFORE  = 0,
    AFTER   = 1
};

/* ASN Tags   */
enum ASN_Tags {
    ASN_BOOLEAN           = 0x01,
    ASN_INTEGER           = 0x02,
    ASN_BIT_STRING        = 0x03,
    ASN_OCTET_STRING      = 0x04,
    ASN_TAG_NULL          = 0x05,
    ASN_OBJECT_ID         = 0x06,
    ASN_ENUMERATED        = 0x0a,
    ASN_UTF8STRING        = 0x0c,
    ASN_SEQUENCE          = 0x10,
    ASN_SET               = 0x11,
    ASN_UTC_TIME          = 0x17,
    ASN_OTHER_TYPE        = 0x00,
    ASN_RFC822_TYPE       = 0x01,
    ASN_DNS_TYPE          = 0x02,
    ASN_DIR_TYPE          = 0x04,
    ASN_URI_TYPE          = 0x06, /* the value 6 is from GeneralName OID */
    ASN_GENERALIZED_TIME  = 0x18,
    CRL_EXTENSIONS        = 0xa0,
    ASN_EXTENSIONS        = 0xa3,
    ASN_LONG_LENGTH       = 0x80,
    ASN_INDEF_LENGTH      = 0x80,

    /* ASN_Flags - Bitmask */
    ASN_CONSTRUCTED       = 0x20,
    ASN_CONTEXT_SPECIFIC  = 0x80,
};

#define ASN_UTC_TIME_SIZE 14
#define ASN_GENERALIZED_TIME_SIZE 16

enum DN_Tags {
    ASN_COMMON_NAME   = 0x03,   /* CN */
    ASN_SUR_NAME      = 0x04,   /* SN */
    ASN_SERIAL_NUMBER = 0x05,   /* serialNumber */
    ASN_COUNTRY_NAME  = 0x06,   /* C  */
    ASN_LOCALITY_NAME = 0x07,   /* L  */
    ASN_STATE_NAME    = 0x08,   /* ST */
    ASN_ORG_NAME      = 0x0a,   /* O  */
    ASN_ORGUNIT_NAME  = 0x0b,    /* OU */
    ASN_EMAIL_NAME    = 0x98,    /* not oid number there is 97 in 2.5.4.0-97 */

    /* pilot attribute types
     * OID values of 0.9.2342.19200300.100.1.* */
    ASN_USER_ID          = 0x01, /* UID */
    ASN_DOMAIN_COMPONENT = 0x19  /* DC */
};

/* DN Tag Strings */
#define WOLFSSL_COMMON_NAME      "/CN="
#define WOLFSSL_SUR_NAME         "/SN="
#define WOLFSSL_SERIAL_NUMBER    "/serialNumber="
#define WOLFSSL_COUNTRY_NAME     "/C="
#define WOLFSSL_LOCALITY_NAME    "/L="
#define WOLFSSL_STATE_NAME       "/ST="
#define WOLFSSL_ORG_NAME         "/O="
#define WOLFSSL_ORGUNIT_NAME     "/OU="
#define WOLFSSL_DOMAIN_COMPONENT "/DC="

enum ECC_TYPES {
    ECC_PREFIX_0 = 160,
    ECC_PREFIX_1 = 161
};

enum Misc_ASN {
    ASN_NAME_MAX        = 256,
    MAX_SALT_SIZE       =  64,     /* MAX PKCS Salt length */
    MAX_IV_SIZE         =  64,     /* MAX PKCS Iv length */
    ASN_BOOL_SIZE       =   2,     /* including type */
    ASN_ECC_HEADER_SZ   =   2,     /* String type + 1 byte len */
    ASN_ECC_CONTEXT_SZ  =   2,     /* Content specific type + 1 byte len */
#ifdef NO_SHA
    KEYID_SIZE          = WC_SHA256_DIGEST_SIZE,
#else
    KEYID_SIZE          = WC_SHA_DIGEST_SIZE,
#endif
    RSA_INTS            =   8,     /* RSA ints in private key */
    DSA_INTS            =   5,     /* DSA ints in private key */
    MIN_DATE_SIZE       =  13,
    MAX_DATE_SIZE       =  32,
    ASN_GEN_TIME_SZ     =  15,     /* 7 numbers * 2 + Zulu tag */
    MAX_ENCODED_SIG_SZ  = 512,
    MAX_SIG_SZ          = 256,
    MAX_ALGO_SZ         =  20,
    MAX_SHORT_SZ        =   6,     /* asn int + byte len + 4 byte length */
    MAX_SEQ_SZ          =   5,     /* enum(seq | con) + length(4) */
    MAX_SET_SZ          =   5,     /* enum(set | con) + length(4) */
    MAX_OCTET_STR_SZ    =   5,     /* enum(set | con) + length(4) */
    MAX_EXP_SZ          =   5,     /* enum(contextspec|con|exp) + length(4) */
    MAX_PRSTR_SZ        =   5,     /* enum(prstr) + length(4) */
    MAX_VERSION_SZ      =   5,     /* enum + id + version(byte) + (header(2))*/
    MAX_ENCODED_DIG_SZ  =  73,     /* sha512 + enum(bit or octet) + length(4) */
    MAX_RSA_INT_SZ      = 517,     /* RSA raw sz 4096 for bits + tag + len(4) */
    MAX_NTRU_KEY_SZ     = 610,     /* NTRU 112 bit public key */
    MAX_NTRU_ENC_SZ     = 628,     /* NTRU 112 bit DER public encoding */
    MAX_LENGTH_SZ       =   4,     /* Max length size for DER encoding */
    MAX_RSA_E_SZ        =  16,     /* Max RSA public e size */
    MAX_CA_SZ           =  32,     /* Max encoded CA basic constraint length */
    MAX_SN_SZ           =  35,     /* Max encoded serial number (INT) length */
    MAX_DER_DIGEST_SZ   = MAX_ENCODED_DIG_SZ + MAX_ALGO_SZ + MAX_SEQ_SZ, /* Maximum DER digest size */
#ifdef WOLFSSL_CERT_GEN
    #ifdef WOLFSSL_CERT_REQ
                          /* Max encoded cert req attributes length */
        MAX_ATTRIB_SZ   = MAX_SEQ_SZ * 3 + (11 + MAX_SEQ_SZ) * 2 +
                          MAX_PRSTR_SZ + CTC_NAME_SIZE, /* 11 is the OID size */
    #endif
    #if defined(WOLFSSL_ALT_NAMES) || defined(WOLFSSL_CERT_EXT)
        MAX_EXTENSIONS_SZ   = 1 + MAX_LENGTH_SZ + CTC_MAX_ALT_SIZE,
    #else
        MAX_EXTENSIONS_SZ   = 1 + MAX_LENGTH_SZ + MAX_CA_SZ,
    #endif
                                   /* Max total extensions, id + len + others */
#endif
#if defined(WOLFSSL_CERT_EXT) || defined(OPENSSL_EXTRA)
    MAX_OID_SZ          = 32,      /* Max DER length of OID*/
    MAX_OID_STRING_SZ   = 64,      /* Max string length representation of OID*/
#endif
#ifdef WOLFSSL_CERT_EXT
    MAX_KID_SZ			= 45,	   /* Max encoded KID length (SHA-256 case) */
    MAX_KEYUSAGE_SZ     = 18,      /* Max encoded Key Usage length */
    MAX_EXTKEYUSAGE_SZ  = 12 + (6 * (8 + 2)) +
                          CTC_MAX_EKU_OID_SZ, /* Max encoded ExtKeyUsage
                        (SEQ/LEN + OBJID + OCTSTR/LEN + SEQ + (6 * (SEQ + OID))) */
    MAX_CERTPOL_NB      = CTC_MAX_CERTPOL_NB,/* Max number of Cert Policy */
    MAX_CERTPOL_SZ      = CTC_MAX_CERTPOL_SZ,
#endif
    MAX_NAME_ENTRIES    = 5,       /* extra entries added to x509 name struct */
    OCSP_NONCE_EXT_SZ   = 35,      /* OCSP Nonce Extension size */
    MAX_OCSP_EXT_SZ     = 58,      /* Max OCSP Extension length */
    MAX_OCSP_NONCE_SZ   = 16,      /* OCSP Nonce size           */
    EIGHTK_BUF          = 8192,    /* Tmp buffer size           */
    MAX_PUBLIC_KEY_SZ   = MAX_NTRU_ENC_SZ + MAX_ALGO_SZ + MAX_SEQ_SZ * 2,
                                   /* use bigger NTRU size */
#ifdef WOLFSSL_ENCRYPTED_KEYS
    HEADER_ENCRYPTED_KEY_SIZE = 88,/* Extra header size for encrypted key */
#else
    HEADER_ENCRYPTED_KEY_SIZE = 0,
#endif
    TRAILING_ZERO       = 1,       /* Used for size of zero pad */
    MIN_VERSION_SZ      = 3,       /* Min bytes needed for GetMyVersion */
#if defined(OPENSSL_ALL)  || defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA)
    MAX_TIME_STRING_SZ  = 25,      /* Max length of formatted time string */
#endif

    PKCS5_SALT_SZ       = 8,

    PEM_LINE_LEN       = 80,       /* PEM line max + fudge */
};


enum Oid_Types {
    oidHashType         = 0,
    oidSigType          = 1,
    oidKeyType          = 2,
    oidCurveType        = 3,
    oidBlkType          = 4,
    oidOcspType         = 5,
    oidCertExtType      = 6,
    oidCertAuthInfoType = 7,
    oidCertPolicyType   = 8,
    oidCertAltNameType  = 9,
    oidCertKeyUseType   = 10,
    oidKdfType          = 11,
    oidKeyWrapType      = 12,
    oidCmsKeyAgreeType  = 13,
    oidPBEType          = 14,
    oidHmacType         = 15,
    oidIgnoreType
};


enum Hash_Sum  {
    MD2h    = 646,
    MD5h    = 649,
    SHAh    =  88,
    SHA224h = 417,
    SHA256h = 414,
    SHA384h = 415,
    SHA512h = 416
};


#if !defined(NO_DES3) || !defined(NO_AES)
enum Block_Sum {
#ifdef WOLFSSL_AES_128
    AES128CBCb = 414,
#endif
#ifdef WOLFSSL_AES_192
    AES192CBCb = 434,
#endif
#ifdef WOLFSSL_AES_256
    AES256CBCb = 454,
#endif
#ifndef NO_DES3
    DESb       = 69,
    DES3b      = 652
#endif
};
#endif /* !NO_DES3 || !NO_AES */


enum Key_Sum {
    DSAk     = 515,
    RSAk     = 645,
    NTRUk    = 274,
    ECDSAk   = 518,
    ED25519k = 256
};


#ifndef NO_AES
enum KeyWrap_Sum {
#ifdef WOLFSSL_AES_128
    AES128_WRAP = 417,
#endif
#ifdef WOLFSSL_AES_192
    AES192_WRAP = 437,
#endif
#ifdef WOLFSSL_AES_256
    AES256_WRAP = 457
#endif
};
#endif /* !NO_AES */

enum Key_Agree {
    dhSinglePass_stdDH_sha1kdf_scheme   = 464,
    dhSinglePass_stdDH_sha224kdf_scheme = 188,
    dhSinglePass_stdDH_sha256kdf_scheme = 189,
    dhSinglePass_stdDH_sha384kdf_scheme = 190,
    dhSinglePass_stdDH_sha512kdf_scheme = 191,
};


enum Ecc_Sum {
    ECC_SECP112R1_OID = 182,
    ECC_SECP112R2_OID = 183,
    ECC_SECP128R1_OID = 204,
    ECC_SECP128R2_OID = 205,
    ECC_SECP160R1_OID = 184,
    ECC_SECP160R2_OID = 206,
    ECC_SECP160K1_OID = 185,
    ECC_BRAINPOOLP160R1_OID = 98,
    ECC_SECP192R1_OID = 520,
    ECC_PRIME192V2_OID = 521,
    ECC_PRIME192V3_OID = 522,
    ECC_SECP192K1_OID = 207,
    ECC_BRAINPOOLP192R1_OID = 100,
    ECC_SECP224R1_OID = 209,
    ECC_SECP224K1_OID = 208,
    ECC_BRAINPOOLP224R1_OID = 102,
    ECC_PRIME239V1_OID = 523,
    ECC_PRIME239V2_OID = 524,
    ECC_PRIME239V3_OID = 525,
    ECC_SECP256R1_OID = 526,
    ECC_SECP256K1_OID = 186,
    ECC_BRAINPOOLP256R1_OID = 104,
    ECC_X25519_OID = 365,
    ECC_ED25519_OID = 256,
    ECC_BRAINPOOLP320R1_OID = 106,
    ECC_SECP384R1_OID = 210,
    ECC_BRAINPOOLP384R1_OID = 108,
    ECC_BRAINPOOLP512R1_OID = 110,
    ECC_SECP521R1_OID = 211,
};


enum KDF_Sum {
    PBKDF2_OID = 660
};


enum HMAC_Sum {
    HMAC_SHA224_OID = 652,
    HMAC_SHA256_OID = 653,
    HMAC_SHA384_OID = 654,
    HMAC_SHA512_OID = 655
};


enum Extensions_Sum {
    BASIC_CA_OID    = 133,
    ALT_NAMES_OID   = 131,
    CRL_DIST_OID    = 145,
    AUTH_INFO_OID   = 69, /* id-pe 1 */
    AUTH_KEY_OID    = 149,
    SUBJ_KEY_OID    = 128,
    CERT_POLICY_OID = 146,
    KEY_USAGE_OID   = 129,  /* 2.5.29.15 */
    INHIBIT_ANY_OID = 168,  /* 2.5.29.54 */
    EXT_KEY_USAGE_OID         = 151, /* 2.5.29.37 */
    NAME_CONS_OID             = 144, /* 2.5.29.30 */
    PRIV_KEY_USAGE_PERIOD_OID = 130, /* 2.5.29.16 */
    SUBJECT_INFO_ACCESS       = 79,  /* id-pe 11 */
    POLICY_MAP_OID            = 147,
    POLICY_CONST_OID          = 150,
    ISSUE_ALT_NAMES_OID       = 132,
    TLS_FEATURE_OID           = 92   /* id-pe 24 */
};

enum CertificatePolicy_Sum {
    CP_ANY_OID      = 146  /* id-ce 32 0 */
};

enum SepHardwareName_Sum {
    HW_NAME_OID     = 79   /* 1.3.6.1.5.5.7.8.4 from RFC 4108*/
};

enum AuthInfo_Sum {
    AIA_OCSP_OID      = 116, /* 1.3.6.1.5.5.7.48.1 */
    AIA_CA_ISSUER_OID = 117  /* 1.3.6.1.5.5.7.48.2 */
};

enum ExtKeyUsage_Sum { /* From RFC 5280 */
    EKU_ANY_OID         = 151, /* 2.5.29.37.0, anyExtendedKeyUsage         */
    EKU_SERVER_AUTH_OID = 71,  /* 1.3.6.1.5.5.7.3.1, id-kp-serverAuth      */
    EKU_CLIENT_AUTH_OID = 72,  /* 1.3.6.1.5.5.7.3.2, id-kp-clientAuth      */
    EKU_CODESIGNING_OID = 73,  /* 1.3.6.1.5.5.7.3.3, id-kp-codeSigning     */
    EKU_EMAILPROTECT_OID = 74, /* 1.3.6.1.5.5.7.3.4, id-kp-emailProtection */
    EKU_TIMESTAMP_OID   = 78,  /* 1.3.6.1.5.5.7.3.8, id-kp-timeStamping    */
    EKU_OCSP_SIGN_OID   = 79   /* 1.3.6.1.5.5.7.3.9, id-kp-OCSPSigning     */
};


enum VerifyType {
    NO_VERIFY   = 0,
    VERIFY      = 1,
    VERIFY_CRL  = 2,
    VERIFY_OCSP = 3
};

#ifdef WOLFSSL_CERT_EXT
enum KeyIdType {
    SKID_TYPE = 0,
    AKID_TYPE = 1
};
#endif

/* Key usage extension bits (based on RFC 5280) */
#define KEYUSE_DIGITAL_SIG    0x0080
#define KEYUSE_CONTENT_COMMIT 0x0040
#define KEYUSE_KEY_ENCIPHER   0x0020
#define KEYUSE_DATA_ENCIPHER  0x0010
#define KEYUSE_KEY_AGREE      0x0008
#define KEYUSE_KEY_CERT_SIGN  0x0004
#define KEYUSE_CRL_SIGN       0x0002
#define KEYUSE_ENCIPHER_ONLY  0x0001
#define KEYUSE_DECIPHER_ONLY  0x8000

/* Extended Key Usage bits (internal mapping only) */
#define EXTKEYUSE_USER        0x80
#define EXTKEYUSE_OCSP_SIGN   0x40
#define EXTKEYUSE_TIMESTAMP   0x20
#define EXTKEYUSE_EMAILPROT   0x10
#define EXTKEYUSE_CODESIGN    0x08
#define EXTKEYUSE_CLIENT_AUTH 0x04
#define EXTKEYUSE_SERVER_AUTH 0x02
#define EXTKEYUSE_ANY         0x01

typedef struct DNS_entry   DNS_entry;

struct DNS_entry {
    DNS_entry* next;   /* next on DNS list */
    int        type;   /* i.e. ASN_DNS_TYPE */
    int        len;    /* actual DNS len */
    char*      name;   /* actual DNS name */
};


typedef struct Base_entry  Base_entry;

struct Base_entry {
    Base_entry* next;   /* next on name base list */
    char*       name;   /* actual name base */
    int         nameSz; /* name length */
    byte        type;   /* Name base type (DNS or RFC822) */
};

#define DOMAIN_COMPONENT_MAX 10

struct DecodedName {
    char*   fullName;
    int     fullNameLen;
    int     entryCount;
    int     cnIdx;
    int     cnLen;
    int     snIdx;
    int     snLen;
    int     cIdx;
    int     cLen;
    int     lIdx;
    int     lLen;
    int     stIdx;
    int     stLen;
    int     oIdx;
    int     oLen;
    int     ouIdx;
    int     ouLen;
    int     emailIdx;
    int     emailLen;
    int     uidIdx;
    int     uidLen;
    int     serialIdx;
    int     serialLen;
    int     dcIdx[DOMAIN_COMPONENT_MAX];
    int     dcLen[DOMAIN_COMPONENT_MAX];
    int     dcNum;
    int     dcMode;
};

enum SignatureState {
    SIG_STATE_BEGIN,
    SIG_STATE_HASH,
    SIG_STATE_KEY,
    SIG_STATE_DO,
    SIG_STATE_CHECK,
};


#ifdef HAVE_PK_CALLBACKS
#ifdef HAVE_ECC
    typedef int (*wc_CallbackEccVerify)(
           const unsigned char* sig, unsigned int sigSz,
           const unsigned char* hash, unsigned int hashSz,
           const unsigned char* keyDer, unsigned int keySz,
           int* result, void* ctx);
#endif
#ifndef NO_RSA
    typedef int (*wc_CallbackRsaVerify)(
           unsigned char* sig, unsigned int sigSz,
           unsigned char** out,
           const unsigned char* keyDer, unsigned int keySz,
           void* ctx);
#endif
#endif /* HAVE_PK_CALLBACKS */

struct SignatureCtx {
    void* heap;
    byte* digest;
#ifndef NO_RSA
    byte* out;
    byte* plain;
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519)
    int verify;
#endif
    union {
    #ifndef NO_RSA
        struct RsaKey*      rsa;
    #endif
    #ifdef HAVE_ECC
        struct ecc_key*     ecc;
    #endif
    #ifdef HAVE_ED25519
        struct ed25519_key* ed25519;
    #endif
        void* ptr;
    } key;
    int devId;
    int state;
    int typeH;
    int digestSz;
    word32 keyOID;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV* asyncDev;
    void* asyncCtx;
#endif

#ifdef HAVE_PK_CALLBACKS
#ifdef HAVE_ECC
    wc_CallbackEccVerify pkCbEcc;
    void* pkCtxEcc;
#endif
#ifndef NO_RSA
    wc_CallbackRsaVerify pkCbRsa;
    void* pkCtxRsa;
#endif
#endif /* HAVE_PK_CALLBACKS */
};

enum CertSignState {
    CERTSIGN_STATE_BEGIN,
    CERTSIGN_STATE_DIGEST,
    CERTSIGN_STATE_ENCODE,
    CERTSIGN_STATE_DO,
};

struct CertSignCtx {
    byte* sig;
    byte* digest;
    #ifndef NO_RSA
        byte* encSig;
        int encSigSz;
    #endif
    int state; /* enum CertSignState */
};


typedef struct DecodedCert DecodedCert;
typedef struct DecodedName DecodedName;
typedef struct Signer      Signer;
#ifdef WOLFSSL_TRUST_PEER_CERT
typedef struct TrustedPeerCert TrustedPeerCert;
#endif /* WOLFSSL_TRUST_PEER_CERT */
typedef struct SignatureCtx SignatureCtx;
typedef struct CertSignCtx  CertSignCtx;


struct DecodedCert {
    byte*   publicKey;
    word32  pubKeySize;
    int     pubKeyStored;
    word32  certBegin;               /* offset to start of cert          */
    word32  sigIndex;                /* offset to start of signature     */
    word32  sigLength;               /* length of signature              */
    word32  signatureOID;            /* sum of algorithm object id       */
    word32  keyOID;                  /* sum of key algo  object id       */
    int     version;                 /* cert version, 1 or 3             */
    DNS_entry* altNames;             /* alt names list of dns entries    */
#ifndef IGNORE_NAME_CONSTRAINTS
    DNS_entry* altEmailNames;        /* alt names list of RFC822 entries */
    Base_entry* permittedNames;      /* Permitted name bases             */
    Base_entry* excludedNames;       /* Excluded name bases              */
#endif /* IGNORE_NAME_CONSTRAINTS */
    byte    subjectHash[KEYID_SIZE]; /* hash of all Names                */
    byte    issuerHash[KEYID_SIZE];  /* hash of all Names                */
#ifdef HAVE_OCSP
    byte    issuerKeyHash[KEYID_SIZE]; /* hash of the public Key         */
#endif /* HAVE_OCSP */
    byte*   signature;               /* not owned, points into raw cert  */
    char*   subjectCN;               /* CommonName                       */
    int     subjectCNLen;            /* CommonName Length                */
    char    subjectCNEnc;            /* CommonName Encoding              */
    char    issuer[ASN_NAME_MAX];    /* full name including common name  */
    char    subject[ASN_NAME_MAX];   /* full name including common name  */
    int     verify;                  /* Default to yes, but could be off */
    byte*   source;                  /* byte buffer holder cert, NOT owner */
    word32  srcIdx;                  /* current offset into buffer       */
    word32  maxIdx;                  /* max offset based on init size    */
    void*   heap;                    /* for user memory overrides        */
    byte    serial[EXTERNAL_SERIAL_SIZE];  /* raw serial number          */
    int     serialSz;                /* raw serial bytes stored */
    byte*   extensions;              /* not owned, points into raw cert  */
    int     extensionsSz;            /* length of cert extensions */
    word32  extensionsIdx;           /* if want to go back and parse later */
    byte*   extAuthInfo;             /* Authority Information Access URI */
    int     extAuthInfoSz;           /* length of the URI                */
    byte*   extCrlInfo;              /* CRL Distribution Points          */
    int     extCrlInfoSz;            /* length of the URI                */
    byte    extSubjKeyId[KEYID_SIZE]; /* Subject Key ID                  */
    byte    extAuthKeyId[KEYID_SIZE]; /* Authority Key ID                */
    byte    pathLength;              /* CA basic constraint path length  */
    word16  extKeyUsage;             /* Key usage bitfield               */
    byte    extExtKeyUsage;          /* Extended Key usage bitfield      */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    byte*   extExtKeyUsageSrc;
    word32  extExtKeyUsageSz;
    word32  extExtKeyUsageCount;
    byte*   extAuthKeyIdSrc;
    word32  extAuthKeyIdSz;
    byte*   extSubjKeyIdSrc;
    word32  extSubjKeyIdSz;
#endif

#if defined(HAVE_ECC) || defined(HAVE_ED25519)
    word32  pkCurveOID;           /* Public Key's curve OID */
#endif /* HAVE_ECC */
    byte*   beforeDate;
    int     beforeDateLen;
    byte*   afterDate;
    int     afterDateLen;
#ifdef HAVE_PKCS7
    byte*   issuerRaw;               /* pointer to issuer inside source */
    int     issuerRawLen;
#endif
#ifndef IGNORE_NAME_CONSTRAINT
    byte*   subjectRaw;               /* pointer to subject inside source */
    int     subjectRawLen;
#endif
#ifdef WOLFSSL_CERT_GEN
    /* easy access to subject info for other sign */
    char*   subjectSN;
    int     subjectSNLen;
    char    subjectSNEnc;
    char*   subjectC;
    int     subjectCLen;
    char    subjectCEnc;
    char*   subjectL;
    int     subjectLLen;
    char    subjectLEnc;
    char*   subjectST;
    int     subjectSTLen;
    char    subjectSTEnc;
    char*   subjectO;
    int     subjectOLen;
    char    subjectOEnc;
    char*   subjectOU;
    int     subjectOULen;
    char    subjectOUEnc;
    char*   subjectEmail;
    int     subjectEmailLen;
#endif /* WOLFSSL_CERT_GEN */
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    DecodedName issuerName;
    DecodedName subjectName;
#endif /* OPENSSL_EXTRA */
#ifdef WOLFSSL_SEP
    int     deviceTypeSz;
    byte*   deviceType;
    int     hwTypeSz;
    byte*   hwType;
    int     hwSerialNumSz;
    byte*   hwSerialNum;
#endif /* WOLFSSL_SEP */
#ifdef WOLFSSL_CERT_EXT
    char    extCertPolicies[MAX_CERTPOL_NB][MAX_CERTPOL_SZ];
    int     extCertPoliciesNb;
#endif /* WOLFSSL_CERT_EXT */

    Signer* ca;
    SignatureCtx sigCtx;

    /* Option Bits */
    byte subjectCNStored : 1;      /* have we saved a copy we own */
    byte extSubjKeyIdSet : 1;      /* Set when the SKID was read from cert */
    byte extAuthKeyIdSet : 1;      /* Set when the AKID was read from cert */
#ifndef IGNORE_NAME_CONSTRAINTS
    byte extNameConstraintSet : 1;
#endif
    byte isCA : 1;                 /* CA basic constraint true */
    byte pathLengthSet : 1;        /* CA basic const path length set */
    byte weOwnAltNames : 1;        /* altNames haven't been given to copy */
    byte extKeyUsageSet : 1;
    byte extExtKeyUsageSet : 1;    /* Extended Key Usage set */
    byte extCRLdistSet : 1;
    byte extAuthInfoSet : 1;
    byte extBasicConstSet : 1;
    byte extSubjAltNameSet : 1;
    byte inhibitAnyOidSet : 1;
#ifdef WOLFSSL_SEP
    byte extCertPolicySet : 1;
#endif
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    byte extCRLdistCrit : 1;
    byte extAuthInfoCrit : 1;
    byte extBasicConstCrit : 1;
    byte extSubjAltNameCrit : 1;
    byte extAuthKeyIdCrit : 1;
    #ifndef IGNORE_NAME_CONSTRAINTS
        byte extNameConstraintCrit : 1;
    #endif
    byte extSubjKeyIdCrit : 1;
    byte extKeyUsageCrit : 1;
    byte extExtKeyUsageCrit : 1;
#endif /* OPENSSL_EXTRA */
#ifdef WOLFSSL_SEP
    byte extCertPolicyCrit : 1;
#endif

};


#ifdef NO_SHA
    #define SIGNER_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
#else
    #define SIGNER_DIGEST_SIZE WC_SHA_DIGEST_SIZE
#endif

/* CA Signers */
/* if change layout change PERSIST_CERT_CACHE functions too */
struct Signer {
    word32  pubKeySize;
    word32  keyOID;                  /* key type */
    word16  keyUsage;
    byte    pathLength;
    byte    pathLengthSet;
    byte*   publicKey;
    int     nameLen;
    char*   name;                    /* common name */
#ifndef IGNORE_NAME_CONSTRAINTS
        Base_entry* permittedNames;
        Base_entry* excludedNames;
#endif /* IGNORE_NAME_CONSTRAINTS */
    byte    subjectNameHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of names in certificate */
    #ifndef NO_SKID
        byte    subjectKeyIdHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of names in certificate */
    #endif
#ifdef WOLFSSL_SIGNER_DER_CERT
    DerBuffer* derCert;
#endif
    Signer* next;
};


#ifdef WOLFSSL_TRUST_PEER_CERT
/* used for having trusted peer certs rather then CA */
struct TrustedPeerCert {
    int     nameLen;
    char*   name;                    /* common name */
    #ifndef IGNORE_NAME_CONSTRAINTS
        Base_entry* permittedNames;
        Base_entry* excludedNames;
    #endif /* IGNORE_NAME_CONSTRAINTS */
    byte    subjectNameHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of names in certificate */
    #ifndef NO_SKID
        byte    subjectKeyIdHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of names in certificate */
    #endif
    word32 sigLen;
    byte*  sig;
    struct TrustedPeerCert* next;
};
#endif /* WOLFSSL_TRUST_PEER_CERT */


/* for testing or custom openssl wrappers */
#if defined(WOLFSSL_TEST_CERT) || defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_EXTRA_X509_SMALL)
    #define WOLFSSL_ASN_API WOLFSSL_API
#else
    #define WOLFSSL_ASN_API WOLFSSL_LOCAL
#endif

WOLFSSL_ASN_API int wc_BerToDer(const byte* ber, word32 berSz, byte* der,
                                word32* derSz);

WOLFSSL_ASN_API void FreeAltNames(DNS_entry*, void*);
#ifndef IGNORE_NAME_CONSTRAINTS
    WOLFSSL_ASN_API void FreeNameSubtrees(Base_entry*, void*);
#endif /* IGNORE_NAME_CONSTRAINTS */
WOLFSSL_ASN_API void InitDecodedCert(DecodedCert*, byte*, word32, void*);
WOLFSSL_ASN_API void FreeDecodedCert(DecodedCert*);
WOLFSSL_ASN_API int  ParseCert(DecodedCert*, int type, int verify, void* cm);

WOLFSSL_LOCAL int DecodePolicyOID(char *o, word32 oSz, byte *in, word32 inSz);
WOLFSSL_LOCAL int ParseCertRelative(DecodedCert*,int type,int verify,void* cm);
WOLFSSL_LOCAL int DecodeToKey(DecodedCert*, int verify);

WOLFSSL_LOCAL const byte* OidFromId(word32 id, word32 type, word32* oidSz);
WOLFSSL_LOCAL Signer* MakeSigner(void*);
WOLFSSL_LOCAL void    FreeSigner(Signer*, void*);
WOLFSSL_LOCAL void    FreeSignerTable(Signer**, int, void*);
#ifdef WOLFSSL_TRUST_PEER_CERT
WOLFSSL_LOCAL void    FreeTrustedPeer(TrustedPeerCert*, void*);
WOLFSSL_LOCAL void    FreeTrustedPeerTable(TrustedPeerCert**, int, void*);
#endif /* WOLFSSL_TRUST_PEER_CERT */

WOLFSSL_ASN_API int ToTraditional(byte* buffer, word32 length);
WOLFSSL_LOCAL int ToTraditionalInline(const byte* input, word32* inOutIdx,
                                      word32 length);
WOLFSSL_LOCAL int ToTraditionalEnc(byte* buffer, word32 length,const char*,int);
WOLFSSL_ASN_API int UnTraditionalEnc(byte* key, word32 keySz, byte* out,
        word32* outSz, const char* password, int passwordSz, int vPKCS,
        int vAlgo, byte* salt, word32 saltSz, int itt, WC_RNG* rng, void* heap);
WOLFSSL_LOCAL int DecryptContent(byte* input, word32 sz,const char* psw,int pswSz);
WOLFSSL_LOCAL int EncryptContent(byte* input, word32 sz, byte* out, word32* outSz,
        const char* password,int passwordSz, int vPKCS, int vAlgo,
        byte* salt, word32 saltSz, int itt, WC_RNG* rng, void* heap);
WOLFSSL_LOCAL int wc_GetKeyOID(byte* key, word32 keySz, const byte** curveOID,
        word32* oidSz, int* algoID, void* heap);

typedef struct tm wolfssl_tm;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(OPENSSL_EXTRA) || \
    defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
WOLFSSL_LOCAL int GetTimeString(byte* date, int format, char* buf, int len);
#endif
WOLFSSL_LOCAL int ExtractDate(const unsigned char* date, unsigned char format,
                                                 wolfssl_tm* certTime, int* idx);
WOLFSSL_LOCAL int ValidateDate(const byte* date, byte format, int dateType);

/* ASN.1 helper functions */
#ifdef WOLFSSL_CERT_GEN
WOLFSSL_ASN_API int SetName(byte* output, word32 outputSz, CertName* name);
#endif
WOLFSSL_LOCAL int GetShortInt(const byte* input, word32* inOutIdx, int* number,
                              word32 maxIdx);
WOLFSSL_LOCAL char* GetSigName(int oid);
WOLFSSL_LOCAL int GetLength(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx);
WOLFSSL_LOCAL int GetSequence(const byte* input, word32* inOutIdx, int* len,
                             word32 maxIdx);
WOLFSSL_LOCAL int GetSet(const byte* input, word32* inOutIdx, int* len,
                        word32 maxIdx);
WOLFSSL_LOCAL int GetMyVersion(const byte* input, word32* inOutIdx,
                              int* version, word32 maxIdx);
WOLFSSL_LOCAL int GetInt(mp_int* mpi, const byte* input, word32* inOutIdx,
                        word32 maxIdx);
#ifdef HAVE_OID_ENCODING
    WOLFSSL_LOCAL int EncodeObjectId(const word16* in, word32 inSz,
        byte* out, word32* outSz);
#endif
#ifdef HAVE_OID_DECODING
    WOLFSSL_LOCAL int DecodeObjectId(const byte* in, word32 inSz,
        word16* out, word32* outSz);
#endif
WOLFSSL_LOCAL int GetObjectId(const byte* input, word32* inOutIdx, word32* oid,
                              word32 oidType, word32 maxIdx);
WOLFSSL_LOCAL int GetAlgoId(const byte* input, word32* inOutIdx, word32* oid,
                           word32 oidType, word32 maxIdx);
WOLFSSL_LOCAL word32 SetLength(word32 length, byte* output);
WOLFSSL_LOCAL word32 SetSequence(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetOctetString(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetImplicit(byte tag,byte number,word32 len,byte* output);
WOLFSSL_LOCAL word32 SetExplicit(byte number, word32 len, byte* output);
WOLFSSL_LOCAL word32 SetSet(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetAlgoID(int algoOID,byte* output,int type,int curveSz);
WOLFSSL_LOCAL int SetMyVersion(word32 version, byte* output, int header);
WOLFSSL_LOCAL int SetSerialNumber(const byte* sn, word32 snSz, byte* output,
    int maxSnSz);
WOLFSSL_LOCAL int GetSerialNumber(const byte* input, word32* inOutIdx,
    byte* serial, int* serialSz, word32 maxIdx);
WOLFSSL_LOCAL int GetNameHash(const byte* source, word32* idx, byte* hash,
                             int maxIdx);
WOLFSSL_LOCAL int wc_CheckPrivateKey(byte* key, word32 keySz, DecodedCert* der);
WOLFSSL_LOCAL int RsaPublicKeyDerSize(RsaKey* key, int with_header);

#ifdef HAVE_ECC
    /* ASN sig helpers */
    WOLFSSL_LOCAL int StoreECC_DSA_Sig(byte* out, word32* outLen, mp_int* r,
                                      mp_int* s);
    WOLFSSL_LOCAL int DecodeECC_DSA_Sig(const byte* sig, word32 sigLen,
                                       mp_int* r, mp_int* s);
#endif

WOLFSSL_LOCAL void InitSignatureCtx(SignatureCtx* sigCtx, void* heap, int devId);
WOLFSSL_LOCAL void FreeSignatureCtx(SignatureCtx* sigCtx);

#ifndef NO_CERTS

WOLFSSL_LOCAL int PemToDer(const unsigned char* buff, long sz, int type,
                          DerBuffer** pDer, void* heap, EncryptedInfo* info,
                          int* eccKey);
WOLFSSL_LOCAL int AllocDer(DerBuffer** der, word32 length, int type, void* heap);
WOLFSSL_LOCAL void FreeDer(DerBuffer** der);

#endif /* !NO_CERTS */

#ifdef WOLFSSL_CERT_GEN

enum cert_enums {
    NAME_ENTRIES    =  8,
    JOINT_LEN       =  2,
    EMAIL_JOINT_LEN =  9,
    PILOT_JOINT_LEN =  10,
    RSA_KEY         = 10,
    NTRU_KEY        = 11,
    ECC_KEY         = 12,
    ED25519_KEY     = 13
};

#endif /* WOLFSSL_CERT_GEN */



/* for pointer use */
typedef struct CertStatus CertStatus;

#ifdef HAVE_OCSP

enum Ocsp_Response_Status {
    OCSP_SUCCESSFUL        = 0, /* Response has valid confirmations */
    OCSP_MALFORMED_REQUEST = 1, /* Illegal confirmation request */
    OCSP_INTERNAL_ERROR    = 2, /* Internal error in issuer */
    OCSP_TRY_LATER         = 3, /* Try again later */
    OCSP_SIG_REQUIRED      = 5, /* Must sign the request (4 is skipped) */
    OCSP_UNAUTHROIZED      = 6  /* Request unauthorized */
};


enum Ocsp_Cert_Status {
    CERT_GOOD    = 0,
    CERT_REVOKED = 1,
    CERT_UNKNOWN = 2
};


enum Ocsp_Sums {
    OCSP_BASIC_OID = 117,
    OCSP_NONCE_OID = 118
};

#ifdef OPENSSL_EXTRA
enum Ocsp_Verify_Error {
    OCSP_VERIFY_ERROR_NONE = 0,
    OCSP_BAD_ISSUER = 1
};
#endif


typedef struct OcspRequest  OcspRequest;
typedef struct OcspResponse OcspResponse;


struct CertStatus {
    CertStatus* next;

    byte serial[EXTERNAL_SERIAL_SIZE];
    int serialSz;

    int status;

    byte thisDate[MAX_DATE_SIZE];
    byte nextDate[MAX_DATE_SIZE];
    byte thisDateFormat;
    byte nextDateFormat;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    byte* thisDateAsn;
    byte* nextDateAsn;
#endif

    byte*  rawOcspResponse;
    word32 rawOcspResponseSz;
};


struct OcspResponse {
    int     responseStatus;  /* return code from Responder */

    byte*   response;        /* Pointer to beginning of OCSP Response */
    word32  responseSz;      /* length of the OCSP Response */

    byte    producedDate[MAX_DATE_SIZE];
                             /* Date at which this response was signed */
    byte    producedDateFormat; /* format of the producedDate */
    byte*   issuerHash;
    byte*   issuerKeyHash;

    byte*   cert;
    word32  certSz;

    byte*   sig;             /* Pointer to sig in source */
    word32  sigSz;           /* Length in octets for the sig */
    word32  sigOID;          /* OID for hash used for sig */

    CertStatus* status;      /* certificate status to fill out */

    byte*   nonce;           /* pointer to nonce inside ASN.1 response */
    int     nonceSz;         /* length of the nonce string */

    byte*   source;          /* pointer to source buffer, not owned */
    word32  maxIdx;          /* max offset based on init size */

#ifdef OPENSSL_EXTRA
    int     verifyError;
#endif
};


struct OcspRequest {
    byte   issuerHash[KEYID_SIZE];
    byte   issuerKeyHash[KEYID_SIZE];
    byte*  serial;   /* copy of the serial number in source cert */
    int    serialSz;
    byte*  url;      /* copy of the extAuthInfo in source cert */
    int    urlSz;

    byte   nonce[MAX_OCSP_NONCE_SZ];
    int    nonceSz;
    void*  heap;
    void*  ssl;
};


WOLFSSL_LOCAL void InitOcspResponse(OcspResponse*, CertStatus*, byte*, word32);
WOLFSSL_LOCAL int  OcspResponseDecode(OcspResponse*, void*, void* heap, int);

WOLFSSL_LOCAL int    InitOcspRequest(OcspRequest*, DecodedCert*, byte, void*);
WOLFSSL_LOCAL void   FreeOcspRequest(OcspRequest*);
WOLFSSL_LOCAL int    EncodeOcspRequest(OcspRequest*, byte*, word32);
WOLFSSL_LOCAL word32 EncodeOcspRequestExtensions(OcspRequest*, byte*, word32);


WOLFSSL_LOCAL int  CompareOcspReqResp(OcspRequest*, OcspResponse*);


#endif /* HAVE_OCSP */


/* for pointer use */
typedef struct RevokedCert RevokedCert;

#ifdef HAVE_CRL

struct RevokedCert {
    byte         serialNumber[EXTERNAL_SERIAL_SIZE];
    int          serialSz;
    RevokedCert* next;
};

typedef struct DecodedCRL DecodedCRL;

struct DecodedCRL {
    word32  certBegin;               /* offset to start of cert          */
    word32  sigIndex;                /* offset to start of signature     */
    word32  sigLength;               /* length of signature              */
    word32  signatureOID;            /* sum of algorithm object id       */
    byte*   signature;               /* pointer into raw source, not owned */
    byte    issuerHash[SIGNER_DIGEST_SIZE]; /* issuer hash               */
    byte    crlHash[SIGNER_DIGEST_SIZE]; /* raw crl data hash            */
    byte    lastDate[MAX_DATE_SIZE]; /* last date updated  */
    byte    nextDate[MAX_DATE_SIZE]; /* next update date   */
    byte    lastDateFormat;          /* format of last date */
    byte    nextDateFormat;          /* format of next date */
    RevokedCert* certs;              /* revoked cert list  */
    int          totalCerts;         /* number on list     */
    void*   heap;
};

WOLFSSL_LOCAL void InitDecodedCRL(DecodedCRL*, void* heap);
WOLFSSL_LOCAL int VerifyCRL_Signature(SignatureCtx* sigCtx,
                                      const byte* toBeSigned, word32 tbsSz,
                                      const byte* signature, word32 sigSz,
                                      word32 signatureOID, Signer *ca,
                                      void* heap);
WOLFSSL_LOCAL int  ParseCRL(DecodedCRL*, const byte* buff, word32 sz, void* cm);
WOLFSSL_LOCAL void FreeDecodedCRL(DecodedCRL*);


#endif /* HAVE_CRL */


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* !NO_ASN */


#if !defined(NO_ASN) || !defined(NO_PWDBASED)

#ifndef MAX_KEY_SIZE
    #define MAX_KEY_SIZE    64  /* MAX PKCS Key length */
#endif
#ifndef MAX_UNICODE_SZ
    #define MAX_UNICODE_SZ  256
#endif

enum PBESTypes {
    PBE_MD5_DES      = 0,
    PBE_SHA1_RC4_128 = 1,
    PBE_SHA1_DES     = 2,
    PBE_SHA1_DES3    = 3,
    PBE_AES256_CBC   = 4,

    PBE_SHA1_RC4_128_SUM = 657,
    PBE_SHA1_DES3_SUM    = 659,
    PBES2            = 13       /* algo ID */
};

enum PKCSTypes {
    PKCS5v2             =   6,     /* PKCS #5 v2.0 */
    PKCS12v1            =  12,     /* PKCS #12 */
    PKCS5               =   5,     /* PKCS oid tag */
    PKCS8v0             =   0,     /* default PKCS#8 version */
};

#endif /* !NO_ASN || !NO_PWDBASED */

#endif /* WOLF_CRYPT_ASN_H */

