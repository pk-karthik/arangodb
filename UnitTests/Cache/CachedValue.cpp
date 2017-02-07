////////////////////////////////////////////////////////////////////////////////
/// @brief test suite for arangodb::cache::CachedValue
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2017 triagens GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Daniel H. Larkin
/// @author Copyright 2017, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "Basics/Common.h"

#define BOOST_TEST_MODULE "C Unit Tests CachedValue"
#include <boost/test/included/unit_test.hpp>

#include "Cache/CachedValue.h"

#include <stdint.h>
#include <string>

using namespace arangodb::cache;

// -----------------------------------------------------------------------------
// --SECTION--                                                 setup / tear-down
// -----------------------------------------------------------------------------

struct CCacheCachedValueSetup {
  CCacheCachedValueSetup() { BOOST_TEST_MESSAGE("setup CachedValue"); }

  ~CCacheCachedValueSetup() { BOOST_TEST_MESSAGE("tear-down CachedValue"); }
};
// -----------------------------------------------------------------------------
// --SECTION--                                                        test suite
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief setup
////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE(CCacheCachedValueTest, CCacheCachedValueSetup)

////////////////////////////////////////////////////////////////////////////////
/// @brief test construct with valid data
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(tst_construct_valid) {
  uint64_t k = 1;
  std::string v("test");

  CachedValue* cv =
      CachedValue::construct(&k, sizeof(uint64_t), v.data(), v.size());
  BOOST_CHECK_EQUAL(sizeof(uint64_t), cv->keySize);
  BOOST_CHECK_EQUAL(v.size(), cv->valueSize);
  BOOST_CHECK_EQUAL(sizeof(CachedValue) + sizeof(uint64_t) + v.size(),
                    cv->size());
  BOOST_CHECK_EQUAL(k, *reinterpret_cast<uint64_t const*>(cv->key()));
  BOOST_CHECK_EQUAL(0, memcmp(v.data(), cv->value(), v.size()));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief generate tests
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|//
// --SECTION--\\|/// @\\}\\)"
// End:
