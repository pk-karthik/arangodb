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

#include "Cache/TransactionalBucket.h"
#include "Basics/Common.h"
#include "Cache/CachedValue.h"

#include <stdint.h>
#include <atomic>

using namespace arangodb::cache;

uint32_t TransactionalBucket::FLAG_LOCK = 0x01;
uint32_t TransactionalBucket::FLAG_MIGRATED = 0x02;
uint32_t TransactionalBucket::FLAG_BLACKLISTED = 0x04;

size_t TransactionalBucket::SLOTS_DATA = 3;
size_t TransactionalBucket::SLOTS_BLACKLIST = 4;

bool TransactionalBucket::lock(int64_t maxWait) {
  // TODO: implement this
  // TODO: check on Windows nanosleep behavior, possibly change to maxTries
  return false;
}

void TransactionalBucket::unlock() {
  // TODO: implement this
}

bool TransactionalBucket::isLocked() {
  // TODO: implement this
  return true;
}

bool TransactionalBucket::isMigrated() {
  // TODO: implement this
  return false;
}

bool TransactionalBucket::isFullyBlacklisted() {
  // TODO: implement this
  return true;
}

bool TransactionalBucket::isFull() {
  // TODO: implement this
  return true;
}

CachedValue* TransactionalBucket::find(uint32_t hash, uint32_t keySize,
                                       uint8_t* key) {
  // TODO: implement this
  return nullptr;
}

void TransactionalBucket::insert(uint32_t hash, CachedValue* value) {
  // TODO: implement this
}

CachedValue* TransactionalBucket::remove(uint32_t hash, uint32_t keySize,
                                         uint8_t* key) {
  // TODO: implement this
  return nullptr;
}

void TransactionalBucket::blacklist(uint32_t hash) {
  // TODO: implement this
}

void TransactionalBucket::updateBlacklistTerm(uint64_t term) {
  // TODO: implement this
}

bool TransactionalBucket::isBlacklisted(uint32_t hash) {
  // TODO: implement this
  return true;
}

CachedValue* TransactionalBucket::evictionCandidate() {
  // TODO: implement this
  return nullptr;
}

void TransactionalBucket::evict(CachedValue* value) {
  // TODO: implement this
}

void TransactionalBucket::toggleMigrated() {
  // TODO: implement this
}
