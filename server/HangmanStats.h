/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

namespace HangmanService {

/**
 * Just some dummy class containing request count. Since we keep
 * one instance of this in each class, there is no need of
 * synchronization
 */
class HangmanStats {
 public:
  virtual ~HangmanStats() {
  }

  // NOTE: We make the following methods `virtual` so that we can
  //       mock them using Gmock for our C++ unit-tests. HangmanStats
  //       is an external dependency to handler and we should be
  //       able to mock it.

  virtual void recordRequest() {
    ++reqCount_;
  }

  virtual long long  getRequestCount() {
    return reqCount_;
  }

 private:
  long long reqCount_{0};
 };

}
