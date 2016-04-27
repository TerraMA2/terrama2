#include "BufferMemory.hpp"
#include "../../../core/data-access/SyncronizedDataSet.hpp"

#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/GeometryProperty.h>

std::shared_ptr<te::mem::DataSet> terrama2::services::analysis::core::createBuffer(std::vector<std::shared_ptr<te::gm::Geometry> >& geometries, std::shared_ptr<te::gm::Envelope>& box, double distance, BufferType bufferType)
{

  // Creates memory dataset for buffer
  te::da::DataSetType* dt = new te::da::DataSetType("buffer");

  assert(geometries.size() > 0);
  auto geomSample = geometries[0];

  te::gm::GeometryProperty* prop = new te::gm::GeometryProperty("geom", 0, te::gm::MultiPolygonType, true);
  prop->setSRID(geomSample->getSRID());
  dt->add(prop);

  std::shared_ptr<te::mem::DataSet> dsOut(new te::mem::DataSet(dt));



  // Inserts each geometry in the rtree, if there is a conflict, it makes the union of the two geoemtries
  te::sam::rtree::Index<te::gm::Geometry*, 4> rtree;

  for(size_t i = 0; i < geometries.size(); ++i)
  {
    auto geom = geometries[i];
    auto buffer = geom->buffer(distance, 16, te::gm::CapButtType);

    std::vector<te::gm::Geometry*> vec;

    rtree.search(*(buffer->getMBR()), vec);

    for(std::size_t t = 0; t < vec.size(); ++t)
    {
      if(buffer->intersects(vec[t]))
      {
        buffer = buffer->Union(vec[t]);
        rtree.remove(*(vec[t]->getMBR()), vec[t]);
      }
    }

    rtree.insert(*(buffer->getMBR()), buffer);

  }

  // Fills the memory dataset with the geometries
  std::vector<te::gm::Geometry*> geomVec;

  rtree.search(*(box.get()), geomVec);

  for (size_t i = 0; i < geomVec.size(); i++)
  {
    auto item = new te::mem::DataSetItem(dsOut.get());
    item->setGeometry(0, geomVec[i]);
    dsOut->add(item);
  }

  return dsOut;

}
