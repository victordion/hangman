/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include <gflags/gflags.h>
#include <folly/Memory.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <unistd.h>
void initialize();
#include "HangmanHandler.h"
#include "HangmanStats.h"

using namespace HangmanService;
using namespace proxygen;

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;

using Protocol = HTTPServer::Protocol;

DEFINE_int32(http_port, 11000, "Port to listen on with HTTP protocol");
DEFINE_int32(spdy_port, 11001, "Port to listen on with SPDY protocol");
DEFINE_string(ip, "0.0.0.0", "IP/Hostname to bind to");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 " "will use the number of cores on this machine.");

class HangmanHandlerFactory : public RequestHandlerFactory {
  public:
    void onServerStart() noexcept override {
      stats_.reset(new HangmanStats);
    }

    void onServerStop() noexcept override {
      stats_.reset();
    }

    RequestHandler* onRequest(RequestHandler*, HTTPMessage*) noexcept override {
      return new HangmanHandler(stats_.get());
    }

  private:
    folly::ThreadLocalPtr<HangmanStats> stats_;
};

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  std::vector<HTTPServer::IPConfig> IPs = {
    {SocketAddress(FLAGS_ip, FLAGS_http_port, true), Protocol::HTTP},
    {SocketAddress(FLAGS_ip, FLAGS_spdy_port, true), Protocol::SPDY},
  };

  if (FLAGS_threads <= 0) {
    FLAGS_threads = sysconf(_SC_NPROCESSORS_ONLN);
    CHECK(FLAGS_threads > 0);
  }
//Syed added
  initialize();
  HTTPServerOptions options;
  options.threads = static_cast<size_t>(FLAGS_threads);
  options.idleTimeout = std::chrono::milliseconds(60000);
  options.shutdownOn = {SIGINT, SIGTERM};
  options.enableContentCompression = true;
  options.handlerFactories = RequestHandlerChain()
    .addThen<HangmanHandlerFactory>()
    .build();

  HTTPServer server(std::move(options));
  server.bind(IPs);

  // Start HTTPServer mainloop in a separate thread
  std::thread t([&] () {
      server.start();
      });

  t.join();
  return 0;
}
