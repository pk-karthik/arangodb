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

#include "CacheMetadata.h"
#include <atomic>
#include <cstdint>

using namespace arangodb;

size_t CacheMetadata::FLAG_LOCK = 0x01;
size_t CacheMetadata::FLAG_MIGRATING = 0x02;

CacheMetadata::CacheMetadata(uint64_t limit, uint8_t* table, uint32_t logSize)
    : _state(0),
      _usage(0),
      _softLimit(limit),
      _hardLimit(limit),
      _table(table),
      _auxiliaryTable(nullptr),
      _logSize(logSize),
      _auxiliaryLogSize(0) {}

void CacheMetadata::lock() {
  uint32_t expected;
  while (true) {
    // expect unlocked, but need to preserve migrating status
    expected = _state.load() & (~CacheMetadata::FLAG_LOCK);
    bool success = _state.compare_exchange_weak(
        expected, (expected | CacheMetadata::FLAG_LOCK));  // try to lock
    if (success) {
      break;
    }
    // TODO: exponential back-off for failure
  }
}

void CacheMetadata::unlock() { _state &= ~CacheMetadata::FLAG_LOCK; }

bool CacheMetadata::isLocked() { return ((_state.load() & FLAG_LOCK) > 0); }

uint32_t CacheMetadata::logSize() { return _logSize; }

uint32_t CacheMetadata::auxiliaryLogSize() { return _auxiliaryLogSize; }

uint8_t* CacheMetadata::table() { return _table; }

uint8_t* CacheMetadata::auxiliaryTable() { return _auxiliaryTable; }

uint64_t CacheMetadata::usage() { return _usage; }

uint64_t CacheMetadata::softLimit() { return _softLimit; }

uint64_t CacheMetadata::hardLimit() { return _hardLimit; }

bool CacheMetadata::adjustUsageIfAllowed(int64_t usageChange) {
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

bool CacheMetadata::adjustLimits(uint64_t softLimit, uint64_t hardLimit) {
  if (hardLimit < _usage) {
    return false;
  }

  _softLimit = softLimit;
  _hardLimit = hardLimit;

  return true;
}

bool CacheMetadata::isMigrating() {
  return ((_state & CacheMetadata::FLAG_MIGRATING) > 0);
}

void CacheMetadata::toggleMigrating() {
  _state ^= CacheMetadata::FLAG_MIGRATING;
}

void CacheMetadata::grantAuxiliaryTable(uint8_t* table, uint32_t logSize) {
  _auxiliaryTable = table;
  _auxiliaryLogSize = logSize;
}

void CacheMetadata::swapTables() {
  std::swap(_table, _auxiliaryTable);
  std::swap(_logSize, _auxiliaryLogSize);
}

uint8_t* CacheMetadata::releaseTable() {
  uint8_t* table = _table;
  _table = nullptr;
  _logSize = 0;
  return table;
}

uint8_t* CacheMetadata::releaseAuxiliaryTable() {
  uint8_t* table = _auxiliaryTable;
  _auxiliaryTable = nullptr;
  _auxiliaryLogSize = 0;
  return table;
}
