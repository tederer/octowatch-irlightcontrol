#ifndef IRLIGHTCONTROL_H
#define IRLIGHTCONTROL_H

#include <functional>
#include <memory>
#include <optional>
#include <boost/asio.hpp>

#include "Logging.h"
#include "TcpServer.h"


namespace irlightcontrol {
   
   /**
    * Accepts only one TCP connection at the same time and provides received 
    * commands and connection status updates to the user.
    */
   class IrLightControl : public network::TcpServer::Listener {
      public:
         IrLightControl();
         
         ~IrLightControl();
         
         /**
          * Starts listening for incoming TCP connections.
          */
         void start();
         
         void stop();
         
         // callbacks of the listener interface of the TcpServer
         void onNewConnection(std::unique_ptr<network::Connection> connection) override;

         void onConnectionClosed() override;
         
         void onCommandReceived(const std::string& command) override;
         
      private:
         void setDutycycle(int dutycycle);
         
         logging::Logger                      log;
         std::unique_ptr<network::TcpServer>  tcpServer;
         std::unique_ptr<network::Connection> connection;
         int                                  maxDutyCycleValue;
   };
}
#endif
