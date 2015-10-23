#ifndef __TERRAMA2_UNITTEST_WS_COLLECTOR_WEBPROXYADAPTEREMOCK__
#define __TERRAMA2_UNITTEST_WS_COLLECTOR_WEBPROXYADAPTEREMOCK__


// TerraMA2
#include <soapWebProxy.h>
#include <terrama2/ws/collector/client/WebProxyAdapter.hpp>

// GMock
#include <gmock/gmock.h>

class WebProxyAdapterMock : public terrama2::ws::collector::client::WebProxyAdapter
{
  public:

    MOCK_METHOD1(ping, int(std::string &answer));
    MOCK_METHOD0(destroy, void());
    MOCK_METHOD2(addDataProvider, int(struct DataProvider dataProvider, struct DataProvider& dataProviderResult));
    MOCK_METHOD2(addDataSet, int(struct DataSet dataset, struct DataSet& datasetResult));
    MOCK_METHOD2(updateDataProvider, int(struct DataProvider dataProvider, struct DataProvider& dataProviderResult));
    MOCK_METHOD2(updateDataSet, int(struct DataSet dataset, struct DataSet& datasetResult));
    MOCK_METHOD1(removeDataProvider, int(uint64_t id));
    MOCK_METHOD0(recvRemoveDataProviderEmptyResponse, int());
    MOCK_METHOD1(removeDataSet, int(uint64_t id));
    MOCK_METHOD0(recvRemoveDatasetEmptyResponse, int());
    MOCK_METHOD2(findDataProvider, int(uint64_t id, struct DataProvider& dataProviderResult));
    MOCK_METHOD2(findDataSet, int(uint64_t id, struct DataSet& datasetResult));
    MOCK_METHOD1(listDataProvider, int(std::vector<struct DataProvider>& providers));
    MOCK_METHOD1(listDataSet, int(std::vector<struct DataSet>& datasets));
    MOCK_METHOD0(faultString, std::string());
    MOCK_METHOD0(faultDetail, std::string());
};
#endif //__TERRAMA2_UNITTEST_WS_COLLECTOR_WEBPROXYADAPTEREMOCK__
