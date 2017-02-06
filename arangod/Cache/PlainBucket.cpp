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

#include "Cache/PlainBucket.h"
#include "Basics/Common.h"
#include "Cache/CachedValue.h"

#include <stdint.h>
#include <atomic>

using namespace arangodb::cache;

uint32_t PlainBucket::FLAG_LOCK = 0x01;
uint32_t PlainBucket::FLAG_MIGRATED = 0x02;

size_t PlainBucket::SLOTS_DATA = 5;

bool PlainBucket::lock(int64_t maxWait) {
  // TODO: implement this
  // TODO: check on Windows nanosleep behavior, possibly change to maxTries
  return false;
}

void PlainBucket::unlock() {
  // TODO: implement this
}

bool PlainBucket::isLocked() {
  // TODO: implement this
  return true;
}

bool PlainBucket::isMigrated() {
  // TODO: implement this
  return false;
}

bool PlainBucket::isFull() {
  // TODO: implement this
  return true;
}

CachedValue* PlainBucket::find(uint32_t hash, uint32_t keySize, uint8_t* key) {
  // TODO: implement this
  return nullptr;
}

void PlainBucket::insert(uint32_t hash, CachedValue* value) {
  // TODO: implement this
}

CachedValue* PlainBucket::remove(uint32_t hash, uint32_t keySize,
                                 uint8_t* key) {
  // TODO: implement this
  return nullptr;
}

CachedValue* PlainBucket::evictionCandidate() {
  // TODO: implement this
  return nullptr;
}

void PlainBucket::evict(CachedValue* value) {
  // TODO: implement this
}

void PlainBucket::toggleMigrated() {
  // TODO: implement this
}
