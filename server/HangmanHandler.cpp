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

#include "HangmanStats.h"
#include "HangmanHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <folly/dynamic.h>
#include <folly/json.h>

using namespace proxygen;
using folly::dynamic;

namespace HangmanService {

  HangmanHandler::HangmanHandler(HangmanStats* stats): stats_(stats) {
    std::cout << "New HangmanHandler created." << std::endl;
  }

  void HangmanHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
    std::cout << "Client IP:" << headers->getClientIP() << std::endl;
    std::cout << "Client Port:" << headers->getClientPort() << std::endl;
    std::cout << "OnRequest functions is called." << std::endl;
    stats_->recordRequest();
  }

  void HangmanHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
    std::cout << "Onbody functions is called." << std::endl;
    if (body_) {	
      body_->prependChain(std::move(body));
    } else {
      body_ = std::move(body);
    }
  }

  void HangmanHandler::onEOM() noexcept {

    folly::fbstring received_str;
    std::vector<folly::fbstring>  wrong_guesses;
    folly::fbstring masked_sentence;
    folly::fbstring response_json;

    if(body_) {
      received_str = body_->moveToFbString();
      std::cout << "Received string is " << received_str << std::endl;

      dynamic parsed = folly::parseJson(received_str);   
      std::cout << "Request type is " << parsed["request_type"] << std::endl;
      /*
       * If the HTTP request is asking for guess
       */
      if(parsed["request_type"] == "request_guess") {
        masked_sentence = parsed["masked_sentence"].getString();
        for(size_t i = 0; i < parsed["wrong_guesses"].size(); i++) {
          wrong_guesses.push_back(parsed["wrong_guesses"][i].getString());
        }
        folly::fbstring response_str = guesser_.guess(masked_sentence, wrong_guesses);
        std::cout << "Guesser returned a value: " << response_str << std::endl;

        folly::dynamic d = dynamic::object("guess_result", response_str);
        response_json = folly::toJson(d);
      }
      /*
       * If the HTTP request is asking for a new sentence, i.e. start a game
       */
      else if(parsed["request_type"] == "request_sentence"){

        std::cout << "Received a new sentence request" << std::endl;
        folly::fbstring sentence_id = parsed["sentence_id"].getString();
        folly::fbstring response_str = game_creator_.genSentence(sentence_id);
        folly::dynamic d = dynamic::object("new_sentence", response_str);
        response_json = folly::toJson(d);
      }
    }

    std::cout << "JSON response is " << response_json << std::endl;

    ResponseBuilder(downstream_)
      .status(200, "OK")
      .header("Request-Number", folly::to<std::string>(stats_->getRequestCount()))
      .header("Access-Control-Allow-Origin", "*")
      //.body(std::move(body_))
      .body(response_json)
      .sendWithEOM();

    // For debug use
    std::cout << "Response Sent: " << response_json << std::endl;

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
