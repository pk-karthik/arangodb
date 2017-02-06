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

#ifndef ARANGODB_CACHE_PLAIN_CACHE_H
#define ARANGODB_CACHE_PLAIN_CACHE_H

#include "Basics/Common.h"
#include "Cache/Cache.h"
#include "Cache/CachedValue.h"
#include "Cache/FrequencyBuffer.h"
#include "Cache/Metadata.h"
#include "Cache/PlainBucket.h"

#include <stdint.h>
#include <atomic>
#include <chrono>
#include <list>

namespace arangodb {
namespace cache {

class Manager;  // forward declaration

class PlainCache : public Cache {
  // simple state variable for locking and other purposes
  std::atomic<uint32_t> _state;

  // state flags
  static uint32_t FLAG_LOCK;
  static uint32_t FLAG_MIGRATING;

  // whether to allow the cache to resize larger when it fills
  bool _allowGrowth;

  // structure to handle eviction-upon-insertion rate
  FrequencyBuffer<uint8_t> _evictionStats;

  // eviction stats definitions
  static uint8_t STAT_EVICTION;
  static uint8_t STAT_NO_EVICTION;

  // allow communication with manager
  Manager* _manager;
  std::list<Metadata>::iterator _metadata;

  // main table info
  PlainBucket* _table;
  uint64_t _tableSize;
  uint32_t _logSize;

  // auxiliary table info
  PlainBucket* _auxiliaryTable;
  uint64_t _auxiliaryTableSize;
  uint32_t _auxiliaryLogSize;

  // times to wait until requesting is allowed again
  std::time_t _migrateRequestTime;
  std::time_t _resizeRequestTime;

 public:
  PlainCache(Manager* manager, uint64_t requestedLimit,
             bool allowGrowth);  // TODO: create CacheManagerFeature so
                                 // first parameter can be removed
  ~PlainCache();

  class Finding {
   private:
    CachedValue* _value;

   public:
    Finding(CachedValue* v);
    ~Finding();

    bool found();
    CachedValue* value();
    CachedValue* copy();
  };

  Finding lookup(uint32_t keySize, uint8_t* key);
  void insert(CachedValue* value);
  bool remove(uint32_t keySize, uint8_t* key);

  void freeMemory();
  void migrate();

  void requestResize(uint64_t requestedLimit);

 private:
  void requestMigrate(uint32_t requestedLogSize);
};

};  // end namespace cache
};  // end namespace arangodb

#endif
