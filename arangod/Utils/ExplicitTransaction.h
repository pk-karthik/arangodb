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

#ifndef ARANGOD_UTILS_EXPLICIT_TRANSACTION_H
#define ARANGOD_UTILS_EXPLICIT_TRANSACTION_H 1

#include "Basics/Common.h"

#include "Utils/Transaction.h"
#include "Utils/V8TransactionContext.h"

namespace arangodb {

class ExplicitTransaction final : public Transaction {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief create the transaction
  //////////////////////////////////////////////////////////////////////////////

  ExplicitTransaction(std::shared_ptr<V8TransactionContext> transactionContext,
                      std::vector<std::string> const& readCollections,
                      std::vector<std::string> const& writeCollections,
                      std::vector<std::string> const& exclusiveCollections,
                      double lockTimeout, bool waitForSync,
                      bool allowImplicitCollections)
      : Transaction(transactionContext) {
    addHint(TransactionHints::Hint::LOCK_ENTIRELY, false);

    if (lockTimeout >= 0.0) {
      setTimeout(lockTimeout);
    }

    if (waitForSync) {
      setWaitForSync();
    }
    
    for (auto const& it : exclusiveCollections) {
      addCollection(it, AccessMode::Type::EXCLUSIVE);
    }

    for (auto const& it : writeCollections) {
      addCollection(it, AccessMode::Type::WRITE);
    }

    for (auto const& it : readCollections) {
      addCollection(it, AccessMode::Type::READ);
    }
    
    setAllowImplicitCollections(allowImplicitCollections);
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief end the transaction
  //////////////////////////////////////////////////////////////////////////////

  ~ExplicitTransaction() {}
};
}

#endif
