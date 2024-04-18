
#include <algorithm>
#include <chrono>
#include <sstream>
#include <thread>

#include "TcpServer.h"

using network::Connection;
using network::TcpConnection;
using network::TcpServer;
using logging::Logger;

using namespace std::chrono_literals;

TcpServer::TcpServer(unsigned int port, const std::string& name, TcpServer::Listener& tcpServerListener)
 : name(name),
   port(port), 
   listener(tcpServerListener), 
   logger(Logger::create((std::string("TcpServer-").append(name)).c_str())),
   stopped(false) {}

TcpServer::~TcpServer() {
   stop();
   if (thread != nullptr) {
      logger->info("waiting for thread to finish its execution");
      thread->join();
      logger->info("thread finished");
   }
}

void TcpServer::start() {
   ioContext.reset(new boost::asio::io_context());
   acceptor.reset(new boost::asio::ip::tcp::acceptor(*ioContext, 
                           boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))); 
   
   stopped = false;
   
   thread = std::unique_ptr<std::thread>(new std::thread([this](){
      while(!stopped) {
         std::unique_ptr<TcpConnection> newConnection = TcpConnection::create(*ioContext, logger);
         auto callback = [this, &newConnection](const boost::system::error_code& error){
            handle_accept(std::move(newConnection), error);
         };
         logger->info("listening on port", port);
         acceptor->async_accept(newConnection->getSocket(), callback);
         logger->info("running ioContext");
         ioContext->run();
         logger->info("finished running ioContext");
         if (!stopped) {
            logger->info("restarting ioContext");
            ioContext->restart();
         }
      }
      logger->info("left loop for accepting incoming connections");
   }));
}

void TcpServer::stop() {
   stopped = true;
   logger->info("stopping");
   
   if (acceptor != nullptr) {
      logger->info("canceling boost::asio::acceptor");
      acceptor->cancel();
      logger->info("closing boost::asio::acceptor");
      acceptor->close();
      acceptor.reset();
   }
   
   if (ioContext != nullptr) {
      logger->info("stopping boost::asio::io_context");
      ioContext->stop();
      logger->info("waiting for boost::asio::io_context to stop");
      int repetitions = 30;
      while((repetitions > 0) && !ioContext->stopped()) {
         std::this_thread::sleep_for(50ms);
         repetitions--;
      }
      if (ioContext->stopped()) {
         logger->info("boost::asio::io_context stopped");
      } else {
         logger->error("waiting for termination of boost::asio::io_context timed out");
      }
      ioContext.reset();
   }
}

void TcpServer::onConnectionClosed() {
   listener.onConnectionClosed();
}

void TcpServer::onCommandReceived(const std::string& command) {
   listener.onCommandReceived(command);
}

void TcpServer::handle_accept(std::unique_ptr<TcpConnection> newConnection, 
                              const boost::system::error_code& error) {
   if (error) {
      if (error == boost::asio::error::operation_aborted) {
         logger->info("aborted async accept");
      } else {
         logger->error("failed to accept connection:", error.message());
      }
   } else {
      newConnection->start(
         std::bind(&TcpServer::onConnectionClosed, this),
         std::bind(&TcpServer::onCommandReceived, this, std::placeholders::_1));
      std::unique_ptr<Connection> connection(new Connection(std::move(newConnection)));
      listener.onNewConnection(std::move(connection));
   }
}
