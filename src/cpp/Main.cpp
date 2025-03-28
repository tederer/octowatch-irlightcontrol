#include <chrono>
#include <condition_variable>
#include <errno.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <signal.h>
#include <thread>

#include "IrLightControl.h"
#include "Logging.h"

using irlightcontrol::IrLightControl;

using namespace std::chrono_literals;

static std::mutex               mutex;
static std::mutex               stopMutex;
static std::condition_variable  quitCondition;
static std::function<void(int)> quitCallback;

void signalHandler(int signal) {
   quitCallback(signal);
}

class Impl {
   public:
      Impl() : log("Main"), stopping(false) {
         quitCallback = std::bind(&Impl::quit, this, std::placeholders::_1);
      }

      void start() {
         irLightControl.start();
      }
      
      void quit(int signal) {
         log.info("received signal", signal, "-> stopping");
         {
            std::unique_lock lock(stopMutex);
            if(stopping) {
               log.info("igonoring signal because stop already triggered");
               return;
            }
            stopping = true;
         }
         irLightControl.stop();
         log.info("notifying main thread to quit");
         quitCondition.notify_all();
      }
      
   private:
      logging::Logger log;
      IrLightControl  irLightControl;
      bool stopping;
};

int main() {
   logging::minLevel = INFO;
   
   Impl impl;
   impl.start();
   
   struct sigaction act;
   sigemptyset(&act.sa_mask);
   act.sa_flags   = SA_RESTART;
   act.sa_handler = signalHandler;

   if (sigaction(SIGINT, &act, NULL) != 0) {
      std::cout << "ERROR: failed to register SIGINT handler" << std::endl;
   }

   if (sigaction(SIGTERM, &act, NULL) != 0) {
      std::cout << "ERROR: failed to register SIGTERM handler" << std::endl;
   }

   if (sigaction(SIGCONT, &act, NULL) != 0) {
      std::cout << "ERROR: failed to register SIGCONT handler" << std::endl;
   }
   
   {
      std::unique_lock lock(mutex);
      quitCondition.wait(lock);
   }
   std::cout << "exiting because quit condition fullfilled" << std::endl;
}