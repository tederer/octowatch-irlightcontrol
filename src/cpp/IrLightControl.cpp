#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>
#include <stdio.h>

#include "IrLightControl.h"

#define PORT                  8886
#define GPIO_PIN              12
#define PWM_FREQUENCY_IN_HZ   30000
#define CFG_SOCKET_PORT       31008  // any tcp port in user accessible range


using logging::Logger;
using network::Connection;
using network::TcpConnection;
using network::TcpServer;
using irlightcontrol::IrLightControl;

IrLightControl::IrLightControl() : logger(Logger::create("IrLightControl")) {}

void IrLightControl::setDutycycle(int dutycycle) {
   if (gpioHardwarePWM(GPIO_PIN, PWM_FREQUENCY_IN_HZ, dutycycle)) {
      logger->error("failed to set dutycycle");
   } else {
      logger->info("set dutycycle", dutycycle);
   }
}

void IrLightControl::start() {
   tcpServer.reset(new TcpServer(PORT, "IrLightControl", *this));
   
   gpioCfgSocketPort(CFG_SOCKET_PORT);
   if (gpioInitialise() == PI_INIT_FAILED) {
      logger->error("failed to initialize PIGPIO");
      return;
   }
   
   setDutycycle(0);
   
   int frequency = gpioGetPWMfrequency(GPIO_PIN);
   if (frequency != PI_BAD_USER_GPIO) {
      logger->info("PWM frequency =", frequency, "Hz");
   } else {
      logger->error("failed to read PWM frequenency");
   }
   
   maxDutyCycleValue = gpioGetPWMrange(GPIO_PIN);
   if (maxDutyCycleValue != PI_BAD_USER_GPIO) {
      logger->info("maxDutyCycleValue =", maxDutyCycleValue);
      tcpServer->start();
   } else {
      logger->error("failed to read PWM range");
   }  
}
         
void IrLightControl::onNewConnection(std::unique_ptr<Connection> connection) {
   logger->info("accepted new connection");
   this->connection = std::move(connection);
}

void IrLightControl::onConnectionClosed() {
   logger->info("connection lost");
   connection.release();
}

void IrLightControl::onCommandReceived(const std::string& command) {
   logger->info("received command:", command);
   
   try {
      const char* start = command.c_str();
      char*       end   = NULL;

      float percent = strtof(start, &end);

      if ((percent == 0) && (end == start)) {
        logger->error("failed to parse command");
      } else {
         if ((percent < 0) || (percent > 1)) {
            logger->error("ERROR: value is not within [0,1] -> ignoring it");
         } else {
            setDutycycle((int)(maxDutyCycleValue * percent));
         }
      }
   }
   catch (std::invalid_argument const& ex) {
      logger->error("failed to parse command:", ex.what());
   }
}
