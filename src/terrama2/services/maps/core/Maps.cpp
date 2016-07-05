
// TerraLib
#include <terralib/common/StringUtils.h>
#include <terralib/qt/widgets/canvas/Canvas.h>

// TerraMA2
#include "Maps.hpp"
#include "MapStyle.hpp"
#include "MemoryDataSetLayer.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"

void terrama2::services::maps::core::makeMap(MapsId mapId, std::shared_ptr< terrama2::services::maps::core::MapsLogger > logger, std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    return;
  }

  try
  {

  }
  catch(const terrama2::Exception&)
  {
    TERRAMA2_LOG_INFO() << QObject::tr("Build of map %1 finished with error(s).").arg(mapId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << QObject::tr("Build of %1 finished with error(s).").arg(mapId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << QObject::tr("Build of map %1 finished with error(s).").arg(mapId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unkown error.");
    TERRAMA2_LOG_INFO() << QObject::tr("Build of map %1 finished with error(s).").arg(mapId);
  }
}



void terrama2::services::maps::core::drawSeriesList(std::vector<std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries>>& seriesList)
{
  std::vector< std::shared_ptr<te::map::MemoryDataSetLayer> > layersList;

  // Create layers from series
  for(auto& serie : seriesList)
  {
    std::shared_ptr<te::da::DataSet> dataSet = serie.begin()->second.syncDataSet->dataset();
    std::shared_ptr<te::da::DataSetType> teDataSetType = serie.begin()->second.teDataSetType;

    std::size_t rpos = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);

    dataSet->moveFirst();
    auto raster(dataSet->getRaster(rpos));

    te::gm::Envelope* extent = raster->getExtent();

    // Creates a DataSetLayer of raster
    // VINICIUS: check de Layer ID
    std::shared_ptr<te::map::MemoryDataSetLayer> rasterLayer(new te::map::MemoryDataSetLayer(te::common::Convert2String(1), raster->getName(), dataSet, teDataSetType));
    rasterLayer->setDataSetName(teDataSetType->getDatasetName());
    rasterLayer->setExtent(*extent);
    rasterLayer->setRendererType("ABSTRACT_LAYER_RENDERER");
    rasterLayer->setSRID(raster->getSRID());

    // VINICIUS: Set Style
    MONO_0_Style(rasterLayer);

    layersList.push_back(rasterLayer);
  }

  // Draw layers

  te::gm::Envelope extent;
  int srid = 0;

  for(auto& layer : layersList)
  {
    if(!extent.isValid())
      extent = layer->getExtent();
    else
      extent.Union(layer->getExtent());

    // VINICIUS: which SRID use? using from the first layer for now
    if(srid == 0)
      srid = layer->getSRID();
  }

  // Creates a canvas
  double llx = extent.m_llx;
  double lly = extent.m_lly;
  double urx = extent.m_urx;
  double ury = extent.m_ury;

  // VINICIUS: check the canvas initial size.
  std::unique_ptr<te::qt::widgets::Canvas> canvas(new te::qt::widgets::Canvas(800, 600));
  canvas->calcAspectRatio(llx, lly, urx, ury);
  canvas->setWindow(llx, lly, urx, ury);
  canvas->setBackgroundColor(te::color::RGBAColor(255, 255, 255, TE_OPAQUE));

  bool cancel = false;

  for(auto& layer : layersList)
  {
    layer->draw(canvas.get(), extent, srid, 0, &cancel);
  }

  // Save map

  canvas->save("GeneretadImage", te::map::PNG);

  canvas->clear();
}
