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

#include "Cache/TransactionalCache.h"
#include "Basics/Common.h"
#include "Cache/Cache.h"
#include "Cache/CachedValue.h"
#include "Cache/FrequencyBuffer.h"
#include "Cache/Metadata.h"
#include "Cache/TransactionalBucket.h"

#include <stdint.h>
#include <atomic>
#include <chrono>
#include <list>

using namespace arangodb::cache;

uint32_t TransactionalCache::FLAG_LOCK = 0x01;
uint32_t TransactionalCache::FLAG_MIGRATING = 0x02;

uint8_t TransactionalCache::STAT_EVICTION = 0x01;
uint8_t TransactionalCache::STAT_NO_EVICTION = 0x02;

TransactionalCache::TransactionalCache(Manager* manager,
                                       uint64_t requestedLimit,
                                       bool allowGrowth)
    : _state(0),
      _allowGrowth(allowGrowth),
      _evictionStats(1024),
      _manager(manager) {
  // TODO: implement this
}

TransactionalCache::~TransactionalCache() {
  // TODO: implement this
}

TransactionalCache::Finding::Finding(CachedValue* v) : _value(v) {
  if (_value != nullptr) {
    _value->lease();
  }
}

TransactionalCache::Finding::~Finding() {
  if (_value != nullptr) {
    _value->release();
  }
}

bool TransactionalCache::Finding::found() { return (_value != nullptr); }

CachedValue* TransactionalCache::Finding::value() { return _value; }

CachedValue* TransactionalCache::Finding::copy() {
  return ((_value == nullptr) ? nullptr : _value->copy());
}

TransactionalCache::Finding TransactionalCache::lookup(uint32_t keySize,
                                                       uint8_t* key) {
  // TODO: implement this;
  return TransactionalCache::Finding(nullptr);
}

void TransactionalCache::insert(CachedValue* value) {
  // TODO: implement this
}

bool TransactionalCache::remove(uint32_t keySize, uint8_t* key) {
  // TODO: implement this
  return false;
}

void TransactionalCache::blackList(uint32_t keySize, uint8_t* key) {
  // TODO: implement this
}

void TransactionalCache::freeMemory() {
  // TODO: implement this
}

void TransactionalCache::migrate() {
  // TODO: implement this
}

void TransactionalCache::requestResize(uint64_t requestedLimit) {
  // TODO: implement this
}

void TransactionalCache::requestMigrate(uint32_t requestedLogSize) {
  // TODO: implement this
}
