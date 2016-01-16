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
/// @author Dr. Frank Celler
/// @author Achim Brandt
////////////////////////////////////////////////////////////////////////////////

#ifndef LIB_BASICS_THREAD_H
#define LIB_BASICS_THREAD_H 1

#include "Basics/Common.h"

#include "Basics/threads.h"

namespace arangodb {
struct WorkDescription;

namespace velocypack {
class Builder;
}
}

namespace arangodb {
namespace basics {
class ConditionVariable;


////////////////////////////////////////////////////////////////////////////////
/// @brief thread
///
/// Each subclass must implement a method run. A thread can be started by
/// start and is stopped either when the method run ends or when stop is
/// called.
////////////////////////////////////////////////////////////////////////////////

class Thread {
  Thread(Thread const&) = delete;
  Thread& operator=(Thread const&) = delete;

  
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the process id
  //////////////////////////////////////////////////////////////////////////////

  static TRI_pid_t currentProcessId();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the thread process id
  //////////////////////////////////////////////////////////////////////////////

  static TRI_tpid_t currentThreadProcessId();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the thread id
  //////////////////////////////////////////////////////////////////////////////

  static TRI_tid_t currentThreadId();

  
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief constructs a thread
  //////////////////////////////////////////////////////////////////////////////

  explicit Thread(std::string const& name);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief deletes the thread
  //////////////////////////////////////////////////////////////////////////////

  virtual ~Thread();

  
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the thread is chatty on shutdown
  //////////////////////////////////////////////////////////////////////////////

  virtual bool isSilent();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief name of a thread
  //////////////////////////////////////////////////////////////////////////////

  std::string const& name() const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief getter for running
  //////////////////////////////////////////////////////////////////////////////

  bool isRunning();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns a thread identifier
  //////////////////////////////////////////////////////////////////////////////

  TRI_tid_t threadId();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief starts the thread
  //////////////////////////////////////////////////////////////////////////////

  bool start(ConditionVariable* _finishedCondition = nullptr);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief stops the thread
  //////////////////////////////////////////////////////////////////////////////

  int stop();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief joins the thread
  //////////////////////////////////////////////////////////////////////////////

  int join();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief stops and joins the thread
  //////////////////////////////////////////////////////////////////////////////

  int shutdown();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the process affinity
  //////////////////////////////////////////////////////////////////////////////

  void setProcessorAffinity(size_t c);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the current work description
  //////////////////////////////////////////////////////////////////////////////

  arangodb::WorkDescription* workDescription();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the current work description
  //////////////////////////////////////////////////////////////////////////////

  void setWorkDescription(arangodb::WorkDescription*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets the previous work description
  //////////////////////////////////////////////////////////////////////////////

  arangodb::WorkDescription* setPrevWorkDescription();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sets status
  //////////////////////////////////////////////////////////////////////////////

  virtual void addStatus(arangodb::velocypack::Builder* b);

  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief allows asynchronous cancelation
  //////////////////////////////////////////////////////////////////////////////

  void allowAsynchronousCancelation();

  
 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief allows the sub-class to perform cleanup
  //////////////////////////////////////////////////////////////////////////////

  virtual void cleanup() {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief the thread program
  //////////////////////////////////////////////////////////////////////////////

  virtual void run() = 0;

  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief static started with access to the private variables
  //////////////////////////////////////////////////////////////////////////////

  static void startThread(void* arg);

  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief runner
  //////////////////////////////////////////////////////////////////////////////

  void runMe();

  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief name of the thread
  //////////////////////////////////////////////////////////////////////////////

  std::string const _name;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief asynchronous cancelation is allowed
  //////////////////////////////////////////////////////////////////////////////

  bool _asynchronousCancelation;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief thread pointer
  //////////////////////////////////////////////////////////////////////////////

  TRI_thread_t _thread;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief thread id
  //////////////////////////////////////////////////////////////////////////////

  TRI_tid_t _threadId;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief condition variable for done
  //////////////////////////////////////////////////////////////////////////////

  ConditionVariable* _finishedCondition;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief status
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<bool> _started;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief status
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<bool> _running;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the thread was joined
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<bool> _joined;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief processor affinity
  //////////////////////////////////////////////////////////////////////////////

  int _affinity;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief current work description
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<arangodb::WorkDescription*> _workDescription;
};
}
}

#endif


