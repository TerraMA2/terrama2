#include "ServiceHandler.hpp"

ServiceHandler::ServiceHandler(QMainWindow* app)
  : configManager_(new ConfigManager(app))
{

}

ServiceHandler::~ServiceHandler()
{

}

void ServiceHandler::loadConfiguration(QString filepath)
{
  configManager_->loadConfiguration(filepath);
}
