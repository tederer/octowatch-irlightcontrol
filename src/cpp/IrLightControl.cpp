#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>
#include <stdio.h>

#include "IrLightControl.h"

#define PORT                  8886
#define GPIO_PIN              12
#define PWM_FREQUENCY_IN_HZ   30000

using logging::Logger;
using network::Connection;
using network::TcpConnection;
using network::TcpServer;
using irlightcontrol::IrLightControl;

IrLightControl::IrLightControl() : log("IrLightControl") {}

IrLightControl::~IrLightControl() {
   stop();
}

void IrLightControl::setDutycycle(int dutycycle) {
   if (gpioHardwarePWM(GPIO_PIN, PWM_FREQUENCY_IN_HZ, dutycycle)) {
      log.error("failed to set dutycycle");
   } else {
      log.info("set dutycycle", dutycycle);
   }
}

void IrLightControl::start() {
   tcpServer.reset(new TcpServer(PORT, "IrLightControl", *this));
   
   gpioCfgInterfaces(PI_DISABLE_SOCK_IF);
   
   if (gpioInitialise() == PI_INIT_FAILED) {
      log.error("failed to initialize PIGPIO");
      return;
   }
   
   setDutycycle(0);
   
   int frequency = gpioGetPWMfrequency(GPIO_PIN);
   if (frequency != PI_BAD_USER_GPIO) {
      log.info("PWM frequency =", frequency, "Hz");
   } else {
      log.error("failed to read PWM frequenency");
   }
   
   maxDutyCycleValue = gpioGetPWMrange(GPIO_PIN);
   if (maxDutyCycleValue != PI_BAD_USER_GPIO) {
      log.info("maxDutyCycleValue =", maxDutyCycleValue);
      tcpServer->start();
   } else {
      log.error("failed to read PWM range");
   }  
}

void IrLightControl::stop() {
   if(tcpServer) {
      tcpServer->stop();
   }
}
   
void IrLightControl::onNewConnection(std::unique_ptr<Connection> connection) {
   log.info("accepted new connection");
   this->connection = std::move(connection);
}

void IrLightControl::onConnectionClosed() {
   log.info("connection lost");
   connection.release();
}

void IrLightControl::onCommandReceived(const std::string& command) {
   log.info("received command:", command);
   
   try {
      const char* start = command.c_str();
      char*       end   = NULL;

      float percent = strtof(start, &end);

      if ((percent == 0) && (end == start)) {
        log.error("failed to parse command");
      } else {
         if ((percent < 0) || (percent > 1)) {
            log.error("ERROR: value is not within [0,1] -> ignoring it");
         } else {
            setDutycycle((int)(maxDutyCycleValue * percent));
         }
      }
   }
   catch (std::invalid_argument const& ex) {
      log.error("failed to parse command:", ex.what());
   }
}
