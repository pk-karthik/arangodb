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

#include "Cache/CachedValue.h"

#include <stdint.h>
#include <cstring>

using namespace arangodb::cache;

uint8_t* CachedValue::key() {
  uint8_t* buf = reinterpret_cast<uint8_t*>(this);
  return (buf + sizeof(CachedValue));
}

uint8_t* CachedValue::value() {
  uint8_t* buf = reinterpret_cast<uint8_t*>(this);
  return (buf + sizeof(CachedValue) + keySize);
}

uint64_t CachedValue::size() {
  uint64_t size = sizeof(CachedValue);
  size += keySize;
  size += valueSize;
  return size;
}

void CachedValue::lease() { refCount++; }

void CachedValue::release() { refCount--; }

uint8_t* CachedValue::createCopy() {
  return CachedValue::construct(keySize, key(), valueSize, value());
}

uint8_t* CachedValue::construct(uint32_t kSize, uint8_t* k, uint64_t vSize,
                                uint8_t* v) {
  uint8_t* buf = new uint8_t[sizeof(CachedValue) + kSize + vSize];
  CachedValue* cv = reinterpret_cast<CachedValue*>(buf);

  cv->refCount = 0;
  cv->keySize = kSize;
  cv->valueSize = vSize;
  cv += sizeof(CachedValue);
  std::memcpy(cv, k, kSize);
  cv += kSize;
  std::memcpy(cv, v, vSize);

  return buf;
}
