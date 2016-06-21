#include <terralib/maptools.h>

#include "MemoryDataSetRenderer.hpp"


namespace te
{
  namespace map
  {
    class MemoryDataSetLayer : public DataSetLayer
    {
    public:
      MemoryDataSetLayer(const std::string& id, const std::string& title, std::shared_ptr< te::da::DataSet > dataSet, std::shared_ptr< LayerSchema > memoryLayerSchema)
        : DataSetLayer(id, title),
          dataSet_(dataSet),
          memoryLayerSchema_(memoryLayerSchema)
      { }

      virtual ~MemoryDataSetLayer() = default;


      void setSRID(int srid) override
      {
        // set the srid associated to the parent AbstractLayer
        m_srid=srid;

        // propagate it to my cached dataset schema
        if(memoryLayerSchema_)
        {
          if (memoryLayerSchema_->hasGeom())
          {
            gm::GeometryProperty* myGeom = te::da::GetFirstGeomProperty(memoryLayerSchema_.get());
            myGeom->setSRID(srid);
          }
          else if (memoryLayerSchema_->hasRaster())
          {
            rst::RasterProperty* rstProp = te::da::GetFirstRasterProperty(memoryLayerSchema_.get());
            rstProp->getGrid()->setSRID(srid);
          }
        }
      }


      std::auto_ptr<te::map::LayerSchema> getSchema() const override
      {
        return std::auto_ptr< LayerSchema >(dynamic_cast<LayerSchema*>(memoryLayerSchema_->clone()));
      }


      std::auto_ptr<te::da::DataSet> getData(te::common::TraverseType /*travType*/,
                                             const te::common::AccessPolicy /*accessPolicy*/) const override
      {
        throw Exception((TE_TR("This method was not implemented.!")));
      }


      std::auto_ptr<te::da::DataSet> getData(const std::string& /*propertyName*/,
                                             const te::gm::Envelope* /*e*/,
                                             te::gm::SpatialRelation /*r*/,
                                             te::common::TraverseType /*travType*/,
                                             const te::common::AccessPolicy /*accessPolicy*/) const override
      {
        throw Exception((TE_TR("This method was not implemented.!")));
      }


      std::auto_ptr<te::da::DataSet> getData(const std::string& /*propertyName*/,
                                             const te::gm::Geometry* /*g*/,
                                             te::gm::SpatialRelation /*r*/,
                                             te::common::TraverseType /*travType*/,
                                             const te::common::AccessPolicy /*accessPolicy*/) const override
      {
        throw Exception((TE_TR("This method was not implemented.!")));
      }


      std::auto_ptr<te::da::DataSet> getData(te::da::Expression* /*restriction*/,
                                             te::common::TraverseType /*travType*/,
                                             const te::common::AccessPolicy /*accessPolicy*/) const override
      {
        throw Exception((TE_TR("This method was not implemented.!")));
      }


      std::auto_ptr<te::da::DataSet> getData(const te::da::ObjectIdSet* /*oids*/,
                                             te::common::TraverseType /*travType*/,
                                             const te::common::AccessPolicy /*accessPolicy*/) const override
      {
        throw Exception((TE_TR("This method was not implemented.!")));
      }


      bool isValid() const override
      {
        if(m_datasetName.empty())
          return false;

        if(!dataSet_)
          return false;

        if(!memoryLayerSchema_)
          return false;

        return true;
      }


      void draw(Canvas* canvas, const te::gm::Envelope& bbox, int srid, const double& scale, bool* cancel) override
      {
        // Try get the defined renderer
        std::shared_ptr<MemoryDataSetRenderer> renderer(new MemoryDataSetRenderer(dataSet_, memoryLayerSchema_));
        if(renderer.get() == 0)
          throw Exception((boost::format(TE_TR("Could not draw the memory data set layer %1%. The memory data set renderer could not be created!")) % getTitle()).str());

        renderer->draw(this, canvas, bbox, srid, scale, cancel);
      }

    private:

      std::shared_ptr< te::da::DataSet > dataSet_;
      mutable std::shared_ptr< LayerSchema > memoryLayerSchema_;     //!< The dataset schema.
    };
  }
}
