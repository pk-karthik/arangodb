////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Daniel H. Larkin
////////////////////////////////////////////////////////////////////////////////

#include "CacheValue.h"
#include <stdint.h>
#include <cstring>

using namespace arangodb;

uint8_t* CacheValue::key() {
  uint8_t* buf = reinterpret_cast<uint8_t*>(this);
  return (buf + sizeof(CacheValue));
}

uint8_t* CacheValue::value() {
  uint8_t* buf = reinterpret_cast<uint8_t*>(this);
  return (buf + sizeof(CacheValue) + keySize);
}

uint64_t CacheValue::size() {
  uint64_t size = sizeof(CacheValue);
  size += keySize;
  size += valueSize;
  return size;
}

void CacheValue::lease() { refCount++; }

void CacheValue::release() { refCount--; }

uint8_t* CacheValue::createCopy() {
  return CacheValue::construct(keySize, key(), valueSize, value());
}

uint8_t* CacheValue::construct(uint32_t kSize, uint8_t* k, uint64_t vSize,
                               uint8_t* v) {
  uint8_t* buf = new uint8_t[sizeof(CacheValue) + kSize + vSize];
  CacheValue* cv = reinterpret_cast<CacheValue*>(buf);

  cv->refCount = 0;
  cv->keySize = kSize;
  cv->valueSize = vSize;
  cv += sizeof(CacheValue);
  std::memcpy(cv, k, kSize);
  cv += kSize;
  std::memcpy(cv, v, vSize);

  return buf;
}
