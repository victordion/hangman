/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "EchoHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "EchoStats.h"
#include <folly/dynamic.h>
#include <folly/json.h>

using namespace proxygen;
using folly::dynamic;

namespace EchoService {

  EchoHandler::EchoHandler(EchoStats* stats): stats_(stats) {

  }

  void EchoHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
    std::cout << "Client IP:" << headers->getClientIP() << std::endl;
    std::cout << "Client Port:" << headers->getClientPort() << std::endl;
    std::cout << "OnRequest functions is called." << std::endl;
    stats_->recordRequest();
  }

  void EchoHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
    std::cout << "Onbody functions is called." << std::endl;
    /*
       if(body) {
       std::cout << "Incoming body data is " << body->moveToFbString() << std::endl;
       }
       else {
       std::cout << "Incoming body is null." << std::endl;
       }
       */
    if (body_) {	
      body_->prependChain(std::move(body));
    } else {
      body_ = std::move(body);
    }
  }

  void EchoHandler::onEOM() noexcept {

    folly::fbstring str;
    if(body_) {
      str = body_->moveToFbString();
      std::cout << "Str is " << str << std::endl;

      dynamic parsed = folly::parseJson(str);   
      std::cout << "Request type is" << parsed["request_type"] << std::endl;	
    }

    ResponseBuilder(downstream_)
      .status(200, "OK")
      .header("Request-Number", folly::to<std::string>(stats_->getRequestCount()))
      .header("Access-Control-Allow-Origin", "*")
      //.body(std::move(body_))
      .body("HiHiHi")
      .sendWithEOM();
    std::cout << "Response Sent: " << str << std::endl;

    //std::cout << str << std::endl;	
  }

  void EchoHandler::onUpgrade(UpgradeProtocol protocol) noexcept {
    // handler doesn't support upgrades
  }

  void EchoHandler::requestComplete() noexcept {
    delete this;
  }

  void EchoHandler::onError(ProxygenError err) noexcept {
    delete this;
  }

}
