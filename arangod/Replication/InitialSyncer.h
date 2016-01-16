////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_REPLICATION_INITIAL_SYNCER_H
#define ARANGOD_REPLICATION_INITIAL_SYNCER_H 1

#include "Basics/Common.h"
#include "Replication/Syncer.h"
#include "Utils/transactions.h"


struct TRI_json_t;
class TRI_replication_applier_configuration_t;
struct TRI_transaction_collection_s;
struct TRI_vocbase_t;

namespace arangodb {

namespace httpclient {
class SimpleHttpResult;
}

namespace arango {


class InitialSyncer : public Syncer {
  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief apply phases
  //////////////////////////////////////////////////////////////////////////////

  typedef enum {
    PHASE_NONE,
    PHASE_INIT,
    PHASE_VALIDATE,
    PHASE_DROP_CREATE,
    PHASE_DUMP
  } sync_phase_e;

  
 public:

  InitialSyncer(TRI_vocbase_t*, TRI_replication_applier_configuration_t const*,
                std::unordered_map<std::string, bool> const&,
                std::string const&, bool);


  ~InitialSyncer();

  
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief run method, performs a full synchronization
  //////////////////////////////////////////////////////////////////////////////

  int run(std::string&, bool);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the last log tick of the master at start
  //////////////////////////////////////////////////////////////////////////////

  TRI_voc_tick_t getLastLogTick() const { return _masterInfo._lastLogTick; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief translate a phase to a phase name
  //////////////////////////////////////////////////////////////////////////////

  std::string translatePhase(sync_phase_e phase) const {
    switch (phase) {
      case PHASE_INIT:
        return "init";
      case PHASE_VALIDATE:
        return "validate";
      case PHASE_DROP_CREATE:
        return "drop-create";
      case PHASE_DUMP:
        return "dump";
      case PHASE_NONE:
        break;
    }
    return "none";
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the collections that were synced
  //////////////////////////////////////////////////////////////////////////////

  std::map<TRI_voc_cid_t, std::string> const& getProcessedCollections() const {
    return _processedCollections;
  }

  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief set a progress message
  //////////////////////////////////////////////////////////////////////////////

  void setProgress(std::string const& msg) {
    _progress = msg;

    if (_verbose) {
      LOG_INFO("synchronization progress: %s", msg.c_str());
    }

    if (_vocbase->_replicationApplier != nullptr) {
      _vocbase->_replicationApplier->setProgress(msg.c_str(), true);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief send a WAL flush command
  //////////////////////////////////////////////////////////////////////////////

  int sendFlush(std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief send a "start batch" command
  //////////////////////////////////////////////////////////////////////////////

  int sendStartBatch(std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief send an "extend batch" command
  //////////////////////////////////////////////////////////////////////////////

  int sendExtendBatch();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief send a "finish batch" command
  //////////////////////////////////////////////////////////////////////////////

  int sendFinishBatch();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief check whether the initial synchronization should be aborted
  //////////////////////////////////////////////////////////////////////////////

  bool checkAborted();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief apply the data from a collection dump
  //////////////////////////////////////////////////////////////////////////////

  int applyCollectionDump(arangodb::arango::Transaction*,
                          struct TRI_transaction_collection_s*,
                          httpclient::SimpleHttpResult*, uint64_t&,
                          std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief incrementally fetch data from a collection
  //////////////////////////////////////////////////////////////////////////////

  int handleCollectionDump(arangodb::arango::Transaction*, std::string const&,
                           struct TRI_transaction_collection_s*,
                           std::string const&, TRI_voc_tick_t, std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief incrementally fetch data from a collection
  //////////////////////////////////////////////////////////////////////////////

  int handleCollectionSync(std::string const&,
                           SingleCollectionWriteTransaction<UINT64_MAX>&,
                           std::string const&, TRI_voc_tick_t, std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief incrementally fetch data from a collection
  //////////////////////////////////////////////////////////////////////////////

  int handleSyncKeys(std::string const&, std::string const&,
                     SingleCollectionWriteTransaction<UINT64_MAX>&,
                     std::string const&, TRI_voc_tick_t, std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief changes the properties of a collection, based on the JSON provided
  //////////////////////////////////////////////////////////////////////////////

  int changeCollection(TRI_vocbase_col_t*, struct TRI_json_t const*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle the information about a collection
  //////////////////////////////////////////////////////////////////////////////

  int handleCollection(struct TRI_json_t const*, struct TRI_json_t const*, bool,
                       std::string&, sync_phase_e);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handle the inventory response of the master
  //////////////////////////////////////////////////////////////////////////////

  int handleInventoryResponse(struct TRI_json_t const*, bool, std::string&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief iterate over all collections from an array and apply an action
  //////////////////////////////////////////////////////////////////////////////

  int iterateCollections(
      std::vector<
          std::pair<struct TRI_json_t const*, struct TRI_json_t const*>> const&,
      bool, std::string&, sync_phase_e);

  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief progress message
  //////////////////////////////////////////////////////////////////////////////

  std::string _progress;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief collection restriction
  //////////////////////////////////////////////////////////////////////////////

  std::unordered_map<std::string, bool> _restrictCollections;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief collection restriction type
  //////////////////////////////////////////////////////////////////////////////

  std::string const _restrictType;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief collections synced
  //////////////////////////////////////////////////////////////////////////////

  std::map<TRI_voc_cid_t, std::string> _processedCollections;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief dump batch id
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _batchId;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief dump batch last update time
  //////////////////////////////////////////////////////////////////////////////

  double _batchUpdateTime;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief ttl for batches
  //////////////////////////////////////////////////////////////////////////////

  int _batchTtl;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief include system collections in the dump?
  //////////////////////////////////////////////////////////////////////////////

  bool _includeSystem;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief chunk size to use
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _chunkSize;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief verbosity
  //////////////////////////////////////////////////////////////////////////////

  bool _verbose;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the WAL on the remote server has been flushed by us
  //////////////////////////////////////////////////////////////////////////////

  bool _hasFlushed;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief maximum internal value for chunkSize
  //////////////////////////////////////////////////////////////////////////////

  static size_t const MaxChunkSize;
};
}
}

#endif


