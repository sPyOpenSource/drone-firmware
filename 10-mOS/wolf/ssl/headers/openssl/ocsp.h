/* ocsp.h
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

/* ocsp.h for libcurl */

#ifndef WOLFSSL_OCSP_H_
#define WOLFSSL_OCSP_H_

#ifdef HAVE_OCSP
#include <wolf/ssl/headers/ocsp.h>

#define OCSP_REQUEST              OcspRequest
#define OCSP_RESPONSE             OcspResponse
#define OCSP_BASICRESP            WOLFSSL_OCSP_BASICRESP
#define OCSP_CERTID               WOLFSSL_OCSP_CERTID
#define OCSP_ONEREQ               WOLFSSL_OCSP_ONEREQ

#define OCSP_RESPONSE_STATUS_SUCCESSFUL  0
#define V_OCSP_CERTSTATUS_GOOD           0

#define OCSP_resp_find_status     wolfSSL_OCSP_resp_find_status
#define OCSP_cert_status_str      wolfSSL_OCSP_cert_status_str
#define OCSP_check_validity       wolfSSL_OCSP_check_validity

#define OCSP_CERTID_free          wolfSSL_OCSP_CERTID_free
#define OCSP_cert_to_id           wolfSSL_OCSP_cert_to_id

#define OCSP_BASICRESP_free       wolfSSL_OCSP_BASICRESP_free
#define OCSP_basic_verify         wolfSSL_OCSP_basic_verify

#define OCSP_RESPONSE_free        wolfSSL_OCSP_RESPONSE_free
#define d2i_OCSP_RESPONSE_bio     wolfSSL_d2i_OCSP_RESPONSE_bio
#define d2i_OCSP_RESPONSE         wolfSSL_d2i_OCSP_RESPONSE
#define i2d_OCSP_RESPONSE         wolfSSL_i2d_OCSP_RESPONSE
#define OCSP_response_status      wolfSSL_OCSP_response_status
#define OCSP_response_status_str  wolfSSL_OCSP_response_status_str
#define OCSP_response_get1_basic  wolfSSL_OCSP_response_get1_basic

#define OCSP_REQUEST_new          wolfSSL_OCSP_REQUEST_new
#define OCSP_REQUEST_free         wolfSSL_OCSP_REQUEST_free
#define i2d_OCSP_REQUEST          wolfSSL_i2d_OCSP_REQUEST
#define OCSP_request_add0_id      wolfSSL_OCSP_request_add0_id

#endif /* HAVE_OCSP */

#endif /* WOLFSSL_OCSP_H_ */


