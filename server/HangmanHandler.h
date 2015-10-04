/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 *  Modified on Oct 1 2015
 *  By Jianwei Cui
 */
#pragma once

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include "Guesser.h"

namespace proxygen {
  class ResponseHandler;
}

namespace HangmanService {

  class HangmanStats;

  class HangmanHandler : public proxygen::RequestHandler {
    public:
      explicit HangmanHandler(HangmanStats* stats);

      void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

      void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

      void onEOM() noexcept override;

      void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

      void requestComplete() noexcept override;

      void onError(proxygen::ProxygenError err) noexcept override;

    private:
      HangmanStats* const stats_{nullptr};
      Guesser guesser_;
      std::unique_ptr<folly::IOBuf> body_;
  };

}
