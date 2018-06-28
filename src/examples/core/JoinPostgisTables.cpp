
#include <terrama2/core/utility/Raii.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/JoinDataSet.hpp>

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSet.h>

#include <string>
#include <iostream>
#include <memory>

int main()
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  {
      std::string uri = "postgis://postgres:postgres@localhost:5432/terrama2";
      // creates a DataSource to the data and filters the dataset,
      // also joins if the DCP comes from separated files
      std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS", uri));

      // RAII for open/closing the datasource
      terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(datasource);

      if(!datasource->isOpened())
      {
        assert(0);
      }

      // get a transactor to interact to the data source
      std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

      std::unique_ptr<te::da::DataSet> estados(transactor->getDataSet("estados"));
      std::unique_ptr<te::da::DataSetType> estadosType(transactor->getDataSetType("estados"));
      std::unique_ptr<te::da::DataSet> estados2(transactor->getDataSet("estados2"));
      std::unique_ptr<te::da::DataSetType> estados2Type(transactor->getDataSetType("estados2"));

      terrama2::core::JoinDataSet join(std::move(estadosType), std::move(estados), "id", std::move(estados2Type), std::move(estados2), "id");


      for(std::size_t i = 0; i < join.getNumProperties(); ++i)
      {
        std::cout << join.getPropertyName(i) << "\t";
      }
      std::cout << std::endl;

      for(std::size_t j = 0; j < join.size(); ++j)
      {
        join.move(j);
        for(std::size_t i = 0; i < join.getNumProperties(); ++i)
        {
          if(i == 1)
            std::cout << "geom\t";
          else
            std::cout << join.getAsString(i) << "\t";
        }
        std::cout << std::endl;
      }

  }

  return 0;
}
