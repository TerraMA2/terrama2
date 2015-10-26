#include "WebProxyAdapter.hpp"

terrama2::ws::collector::client::WebProxyAdapter::WebProxyAdapter()
{

}

terrama2::ws::collector::client::WebProxyAdapter::WebProxyAdapter(const std::string server)
: server_(server)
{
  webProxy_ = new WebProxy(server_.c_str());
}

terrama2::ws::collector::client::WebProxyAdapter::~WebProxyAdapter()
{
  if(webProxy_ != nullptr)
    delete webProxy_;
}

void terrama2::ws::collector::client::WebProxyAdapter::destroy()
{
  return webProxy_->destroy();
}

int terrama2::ws::collector::client::WebProxyAdapter::ping(std::string &answer)
{
  return webProxy_->ping(answer);
}

int terrama2::ws::collector::client::WebProxyAdapter::addDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult)
{
  return webProxy_->addDataProvider(dataProvider, dataProviderResult);
}


int terrama2::ws::collector::client::WebProxyAdapter::addDataSet(struct DataSet dataset, struct DataSet& datasetResult)
{
  return webProxy_->addDataSet(dataset, datasetResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::updateDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult)
{
  return webProxy_->updateDataProvider(dataProvider, dataProviderResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::updateDataSet(struct DataSet dataset, struct DataSet& datasetResult)
{
  return webProxy_->updateDataSet(dataset, datasetResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::removeDataProvider(uint64_t id)
{
  return webProxy_->send_removeDataProvider(id);
}

int terrama2::ws::collector::client::WebProxyAdapter::removeDataSet(uint64_t id)
{
  return webProxy_->send_removeDataSet(id);
}

int terrama2::ws::collector::client::WebProxyAdapter::findDataProvider(uint64_t id, struct DataProvider& dataProviderResult)
{
  return webProxy_->findDataProvider(id, dataProviderResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::findDataSet(uint64_t id, struct DataSet& datasetResult)
{
  return webProxy_->findDataSet(id, datasetResult);
}

int terrama2::ws::collector::client::WebProxyAdapter::listDataProvider(std::vector<struct DataProvider>& providers)
{
  return webProxy_->listDataProvider(providers);
}

int terrama2::ws::collector::client::WebProxyAdapter::listDataSet(std::vector<struct DataSet>& datasets)
{
  return webProxy_->listDataSet(datasets);
}

std::string terrama2::ws::collector::client::WebProxyAdapter::faultString()
{
  return webProxy_->soap_fault_string();
}

std::string terrama2::ws::collector::client::WebProxyAdapter::faultDetail()
{
  return webProxy_->soap_fault_detail();
}

int terrama2::ws::collector::client::WebProxyAdapter::recvRemoveDataProviderEmptyResponse()
{
  return webProxy_->recv_removeDataProvider_empty_response();
}

int terrama2::ws::collector::client::WebProxyAdapter::recvRemoveDatasetEmptyResponse()
{
  return webProxy_->recv_removeDataSet_empty_response();
}

int terrama2::ws::collector::client::WebProxyAdapter::shutdown()
{
  return webProxy_->send_shutdown();
}

int terrama2::ws::collector::client::WebProxyAdapter::recvShutdownEmptyResponse()
{
  return webProxy_->recv_shutdown_empty_response();
}
