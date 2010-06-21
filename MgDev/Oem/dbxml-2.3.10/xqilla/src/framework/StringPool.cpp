/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: StringPool.cpp,v 1.7 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/StringPool.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <xercesc/util/XMLUni.hpp>

#include <iostream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static const unsigned int initialModulus = 233;
static const unsigned int lengthThreshold = 100;

StringPool::StringPool(MemoryManager *mm)
  : _mm(mm),
    _bucketList(0),
    _modulus(initialModulus),
    _count(0),
    _hits(0),
    _misses(0),
    _toobig(0)
{
  _bucketList = (Bucket**)_mm->allocate(_modulus * sizeof(Bucket*));
  memset(_bucketList, 0, _modulus * sizeof(Bucket*));
}

StringPool::~StringPool()
{
  Bucket *bucket, *next;
  for(unsigned int i = 0; i < _modulus; ++i) {
    bucket = _bucketList[i];
    while(bucket != 0) {
      next = bucket->next;
      _mm->deallocate((void*)bucket->value);
      _mm->deallocate(bucket);
      bucket = next;
    }
  }
  _mm->deallocate(_bucketList);
}

const XMLCh *StringPool::getPooledString(const XMLCh *src)
{
  if(src == 0) return 0;
  if(*src == 0) return XMLUni::fgZeroLenString;

  // strings longer than lengthThreshold bytes are not pooled, as it is not probable they can be recycled
  const XMLCh* pszTmp = src + 1;
  while(*pszTmp) ++pszTmp;
  unsigned int length = (unsigned int)(pszTmp - src);

  if(length > lengthThreshold) {
    ++_toobig;
    return replicate(src, length);
  }

  unsigned int hashVal = hash(src);
  unsigned int modHashVal = hashVal % _modulus;

  const Bucket *bucket = _bucketList[modHashVal];
  while(bucket) {
    if(bucket->length == length &&
       XPath2Utils::equals(bucket->value, src)) {
      break;
    }
    bucket = bucket->next;
  }

  if(bucket) {
    ++_hits;
    return bucket->value;
  }
  else {
    ++_misses;
    if(_count >= (_modulus * 3 / 4)) {
      resize();
      modHashVal = hashVal % _modulus;
    }

    const XMLCh *result = replicate(src, length);
    _bucketList[modHashVal] = new (_mm->allocate(sizeof(Bucket)))
      Bucket(result, length, hashVal, _bucketList[modHashVal]);
    ++_count;

    return result;
  }
}

const XMLCh *StringPool::getPooledString(const char *src)
{
  if(src == 0) return 0;
  if(*src == 0) return XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;

  XMLCh *transcoded = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(src, _mm);
  if(!transcoded) return 0;

  // strings longer than lengthThreshold bytes are not pooled, as it is not probable they can be recycled
  const XMLCh* pszTmp = transcoded + 1;
  while(*pszTmp) ++pszTmp;
  unsigned int length = (unsigned int)(pszTmp - transcoded);

  if(length > lengthThreshold) {
    ++_toobig;
    return transcoded;
  }

  unsigned int hashVal = hash(transcoded);
  unsigned int modHashVal = hashVal % _modulus;

  const Bucket *bucket = _bucketList[modHashVal];
  while(bucket) {
    if(bucket->length == length &&
       XPath2Utils::equals(bucket->value, transcoded)) {
      break;
    }
    bucket = bucket->next;
  }

  if(bucket) {
    ++_hits;
    _mm->deallocate(transcoded);
    return bucket->value;
  }
  else {
    ++_misses;
    if(_count >= (_modulus * 3 / 4)) {
      resize();
      modHashVal = hashVal % _modulus;
    }

    _bucketList[modHashVal] = new (_mm->allocate(sizeof(Bucket)))
      Bucket(transcoded, length, hashVal, _bucketList[modHashVal]);
    ++_count;

    return transcoded;
  }
}

void StringPool::resize()
{
  unsigned int new_modulus  = (_modulus << 1) + 1;
  Bucket **new_bucketList = (Bucket**)_mm->allocate(new_modulus * sizeof(Bucket*));
  memset(new_bucketList, 0, new_modulus * sizeof(Bucket*));

  unsigned int modHashVal;
  Bucket *bucket, *next;
  for(unsigned int i = 0; i < _modulus; ++i) {
    bucket = _bucketList[i];
    while(bucket != 0) {
      next = bucket->next;

      modHashVal = bucket->hashValue % new_modulus;
      bucket->next = new_bucketList[modHashVal];
      new_bucketList[modHashVal] = bucket;
      
      bucket = next;
    }
  }

  _mm->deallocate(_bucketList);
  _bucketList = new_bucketList;
  _modulus = new_modulus;
}

void StringPool::dumpStatistics() const
{
  std::cout << "\tStringPool count:         " << _count << std::endl;
  std::cout << "\tStringPool hits:          " << _hits << std::endl;
  std::cout << "\tStringPool misses:        " << _misses << std::endl;
  std::cout << "\tStringPool too big:       " << _toobig << std::endl;
  std::cout << "\tStringPool modulo:        " << _modulus << std::endl;
  unsigned int zero=0, maxn=0;
  for(unsigned int i=0;i<_modulus;i++)
      if(_bucketList[i]==NULL)
          zero++;
      else
      {
          unsigned int n=0;
          const Bucket *bucket = _bucketList[i];
          while(bucket) {
            n++;
            bucket = bucket->next;
          }
          if (n > maxn)
            maxn = n;		
      }
  std::cout << "\tStringPool empty buckets: " << zero << std::endl;
  std::cout << "\tStringPool max buckets:   " << maxn << std::endl;
}
