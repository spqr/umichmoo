
/*
 ***************************************************************************
 *  Copyright 2007,2008 Impinj, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ***************************************************************************
 */


struct LLRP_SLibXMLTextDecoder;
struct LLRP_SLibXMLTextDecoderStream;
struct LLRP_SXMLTextEncoder;
struct LLRP_SXMLTextEncoderStream;

typedef struct LLRP_SLibXMLTextDecoder         LLRP_tSLibXMLTextDecoder;
typedef struct LLRP_SLibXMLTextDecoderStream   LLRP_tSLibXMLTextDecoderStream;
typedef struct LLRP_SXMLTextEncoder         LLRP_tSXMLTextEncoder;
typedef struct LLRP_SXMLTextEncoderStream   LLRP_tSXMLTextEncoderStream;

struct LLRP_SLibXMLTextDecoder
{
    LLRP_tSDecoder              decoderHdr;
    struct _xmlDoc *     	    doc;
    struct _xmlNode *           pxmlNodeTree;
};

struct LLRP_SLibXMLTextDecoderStream
{
    LLRP_tSDecoderStream        decoderStreamHdr;

    LLRP_tSLibXMLTextDecoder *          pDecoder;
    LLRP_tSLibXMLTextDecoderStream *    pEnclosingDecoderStream;
    struct _xmlNode *                   pTargetNode;
    struct _xmlNode *                   pCurrentChildNode;
    struct _xmlNode *                   pLastFieldNode;
    const LLRP_tSTypeDescriptor *       pRefType;
};

extern LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  unsigned char *               pBuffer,
  unsigned int                  nBuffer);


struct LLRP_SXMLTextEncoder
{
    LLRP_tSEncoder              encoderHdr;

    unsigned char *             pBuffer;
    unsigned int                nBuffer;

    unsigned int                iNext;

    int                         bOverflow;
};

struct LLRP_SXMLTextEncoderStream
{
    LLRP_tSEncoderStream        encoderStreamHdr;

    LLRP_tSXMLTextEncoder *       pEncoder;
    LLRP_tSXMLTextEncoderStream * pEnclosingEncoderStream;
    const LLRP_tSTypeDescriptor *pRefType;
    unsigned int                nDepth;
};

extern LLRP_tSXMLTextEncoder *
LLRP_XMLTextEncoder_construct (
  unsigned char *               pBuffer,
  unsigned int                  nBuffer);

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct_file (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  char *                        fname);

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct_nodetree (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  struct _xmlNode *             pNodeTree);
