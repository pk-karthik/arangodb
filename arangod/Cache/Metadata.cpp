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

#include "Cache/Metadata.h"
#include "Cache/Cache.h"

#include <atomic>
#include <cstdint>

using namespace arangodb::cache;

uint32_t Metadata::FLAG_LOCK = 0x01;
uint32_t Metadata::FLAG_MIGRATING = 0x02;

Metadata::Metadata(Cache* cache, uint64_t limit, uint8_t* table,
                   uint32_t logSize)
    : _state(0),
      _cache(cache),
      _usage(0),
      _softLimit(limit),
      _hardLimit(limit),
      _table(table),
      _auxiliaryTable(nullptr),
      _logSize(logSize),
      _auxiliaryLogSize(0) {}

void Metadata::lock() {
  uint32_t expected;
  while (true) {
    // expect unlocked, but need to preserve migrating status
    expected = _state.load() & (~Metadata::FLAG_LOCK);
    bool success = _state.compare_exchange_weak(
        expected, (expected | Metadata::FLAG_LOCK));  // try to lock
    if (success) {
      break;
    }
    // TODO: exponential back-off for failure
  }
}

void Metadata::unlock() { _state &= ~Metadata::FLAG_LOCK; }

bool Metadata::isLocked() { return ((_state.load() & FLAG_LOCK) > 0); }

uint32_t Metadata::logSize() { return _logSize; }

uint32_t Metadata::auxiliaryLogSize() { return _auxiliaryLogSize; }

uint8_t* Metadata::table() { return _table; }

uint8_t* Metadata::auxiliaryTable() { return _auxiliaryTable; }

uint64_t Metadata::usage() { return _usage; }

uint64_t Metadata::softLimit() { return _softLimit; }

uint64_t Metadata::hardLimit() { return _hardLimit; }

bool Metadata::adjustUsageIfAllowed(int64_t usageChange) {
  if (usageChange < 0) {
    _usage -= static_cast<uint64_t>(-usageChange);
    return true;
  }

  if ((static_cast<uint64_t>(usageChange) + _usage < _softLimit) ||
      ((_usage > _softLimit) &&
       (static_cast<uint64_t>(usageChange) + _usage < _hardLimit))) {
    _usage += static_cast<uint64_t>(usageChange);
    return true;
  }

  return false;
}

bool Metadata::adjustLimits(uint64_t softLimit, uint64_t hardLimit) {
  if (hardLimit < _usage) {
    return false;
  }

  _softLimit = softLimit;
  _hardLimit = hardLimit;

  return true;
}

bool Metadata::isMigrating() {
  return ((_state & Metadata::FLAG_MIGRATING) > 0);
}

void Metadata::toggleMigrating() { _state ^= Metadata::FLAG_MIGRATING; }

void Metadata::grantAuxiliaryTable(uint8_t* table, uint32_t logSize) {
  _auxiliaryTable = table;
  _auxiliaryLogSize = logSize;
}

void Metadata::swapTables() {
  std::swap(_table, _auxiliaryTable);
  std::swap(_logSize, _auxiliaryLogSize);
}

uint8_t* Metadata::releaseTable() {
  uint8_t* table = _table;
  _table = nullptr;
  _logSize = 0;
  return table;
}

uint8_t* Metadata::releaseAuxiliaryTable() {
  uint8_t* table = _auxiliaryTable;
  _auxiliaryTable = nullptr;
  _auxiliaryLogSize = 0;
  return table;
}
