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
#include "HangmanHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "HangmanStats.h"
#include <folly/dynamic.h>
#include <folly/json.h>

using namespace proxygen;
using folly::dynamic;

namespace HangmanService {

  HangmanHandler::HangmanHandler(HangmanStats* stats): stats_(stats) {

  }

  void HangmanHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
    std::cout << "Client IP:" << headers->getClientIP() << std::endl;
    std::cout << "Client Port:" << headers->getClientPort() << std::endl;
    std::cout << "OnRequest functions is called." << std::endl;
    stats_->recordRequest();
  }

  void HangmanHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
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

  void HangmanHandler::onEOM() noexcept {

    folly::fbstring str;
    std::string response_str;
    
    if(body_) {
      str = body_->moveToFbString();
      std::cout << "Str is " << str << std::endl;

      dynamic parsed = folly::parseJson(str);   
      std::cout << "Request type is" << parsed["request_type"] << std::endl;
      
      if(parsed["request_type"] == "request_guess") {
        std::string masked_sentence = parsed["masked_sentence"];
        std::vector<std::string>  wrong_guesses= parsed["wrong_guesses"];
      }
	
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

  void HangmanHandler::onUpgrade(UpgradeProtocol protocol) noexcept {
    // handler doesn't support upgrades
  }

  void HangmanHandler::requestComplete() noexcept {
    delete this;
  }

  void HangmanHandler::onError(ProxygenError err) noexcept {
    delete this;
  }

}
