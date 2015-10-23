#ifndef TERRAMA2_WS_COLLECTOR_CLIENT_WEBPROXYADAPTER_HPP
#define TERRAMA2_WS_COLLECTOR_CLIENT_WEBPROXYADAPTER_HPP

// gSOAP
#include "soapWebProxy.h"

// STL
#include <vector>
#include <string>

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace client
      {
        class WebProxyAdapter
        {
          public:
            WebProxyAdapter(const std::string server);

            virtual ~WebProxyAdapter();

            virtual void destroy();

            virtual int shutdown();

            virtual int recvShutdownEmptyResponse();

            virtual	int ping(std::string &answer);

            virtual	int addDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult);

            virtual	int addDataSet(struct DataSet dataset, struct DataSet& datasetResult);

            virtual	int updateDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult);

            virtual	int updateDataSet(struct DataSet dataset, struct DataSet& datasetResult);

            virtual	int removeDataProvider(uint64_t id);

            virtual int recvRemoveDataProviderEmptyResponse();

            virtual	int removeDataSet(uint64_t id);

            virtual int recvRemoveDatasetEmptyResponse();

            virtual	int findDataProvider(uint64_t id, struct DataProvider& dataProviderResult);

            virtual	int findDataSet(uint64_t id, struct DataSet& datasetResult);

            virtual	int listDataProvider(std::vector<struct DataProvider>& providers);

            virtual	int listDataSet(std::vector<struct DataSet>& datasets);

            virtual	std::string faultString();

            virtual	std::string faultDetail();

          protected:
            WebProxyAdapter();

          private:
            WebProxy* webProxy_;
        };
      }
    }
  }
}



#endif //TERRAMA2_WS_COLLECTOR_CLIENT_WEBPROXYADAPTER_HPP
