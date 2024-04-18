#include <mutex>

#include "IrLightControl.h"
#include "Logging.h"

using irlightcontrol::IrLightControl;
using logging::Logger;

std::unique_ptr<logging::Logger> LOGGER = Logger::create("Main"); 
std::condition_variable quitCondition;
static std::mutex quitConditionMutex;

static void quitSignalHandler(int) {
   LOGGER->info("Received signal to quit");
	std::unique_lock<std::mutex> lock(quitConditionMutex);
   quitCondition.notify_all(); 
}

int main() {
   logging::minLevel = INFO;

   IrLightControl irLightControl;
   irLightControl.start();
   
   signal(SIGINT, quitSignalHandler); 
   signal(SIGTERM, quitSignalHandler); 
   signal(SIGCONT, quitSignalHandler); 

   std::unique_lock<std::mutex> lock(quitConditionMutex);
   quitCondition.wait(lock);
}