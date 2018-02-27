
#include "DataSetUtils.hpp"

#include <terralib/geometry/GeometryProperty.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/NumericProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

#include <QString>
#include <QObject>

#include "Logger.hpp"

bool terrama2::core::isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember)
{
  std::string newPropertyName = newProperty->getName();
  std::transform(newPropertyName.begin(), newPropertyName.end(), newPropertyName.begin(), ::tolower);

  std::string oldPropertyName = oldMember->getName();
  std::transform(oldPropertyName.begin(), oldPropertyName.end(), oldPropertyName.begin(), ::tolower);

  bool noEqual = newPropertyName == oldPropertyName;
  if(!noEqual)
    return false;

  if(newProperty->getType() != oldMember->getType())
  {
    QString errMsg = QObject::tr("Wrong column type: %1").arg(QString::fromStdString(newProperty->getName()));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  return true;
}

std::unique_ptr<te::dt::Property> terrama2::core::copyProperty(te::dt::Property* property)
{
  auto name = property->getName();
  auto type = property->getType();
  switch (type)
  {
    case te::dt::INT16_TYPE:
    case te::dt::UINT16_TYPE:
    case te::dt::INT32_TYPE:
    case te::dt::UINT32_TYPE:
    case te::dt::INT64_TYPE:
    case te::dt::UINT64_TYPE:
    case te::dt::BOOLEAN_TYPE:
    case te::dt::FLOAT_TYPE:
    case te::dt::DOUBLE_TYPE:
      return std::unique_ptr<te::dt::Property>(new te::dt::SimpleProperty(name, type));
    case te::dt::STRING_TYPE:
      {
        auto stringProperty = dynamic_cast<te::dt::StringProperty*>(property);
        if(!stringProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::dt::StringProperty(name,
                                                                            stringProperty->getSubType(),
                                                                            stringProperty->size()));
      }
    case te::dt::DATETIME_TYPE:
      {
        auto dateTime = dynamic_cast<te::dt::DateTimeProperty*>(property);
        if(!dateTime)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::dt::DateTimeProperty(name,
                                                                              dateTime->getSubType(),
                                                                              dateTime->getPrecision()));
      }
    case te::dt::NUMERIC_TYPE:
      {
        auto numericProperty = dynamic_cast<te::dt::NumericProperty*>(property);
        if(!numericProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::dt::NumericProperty(name,
                                                                             numericProperty->getPrecision(),
                                                                             numericProperty->getScale()));
      }
    case te::dt::GEOMETRY_TYPE:
      {
        auto geomProperty = dynamic_cast<te::gm::GeometryProperty*>(property);
        if(!geomProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        return std::unique_ptr<te::dt::Property>(new te::gm::GeometryProperty(name,
                                                                              geomProperty->getSRID(),
                                                                              geomProperty->getGeometryType()));
      }
    case te::dt::RASTER_TYPE:
      {
        auto rasterProperty = dynamic_cast<te::rst::RasterProperty*>(property);
        if(!rasterProperty)
        {
          QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataStoragerException() << ErrorDescription(errMsg);
        }

        auto newProperty =  std::unique_ptr<te::dt::Property>(new te::rst::RasterProperty(rasterProperty->getGrid(),
                                                                                          rasterProperty->getBandProperties(),
                                                                                          rasterProperty->getInfo()));
        newProperty->setName(name);
        return newProperty;
      }
    default:
      {
        QString errMsg = QObject::tr("Invalid property %1 with type %2").arg(QString::fromStdString(name), int(type));
        TERRAMA2_LOG_ERROR() << errMsg;
        throw DataStoragerException() << ErrorDescription(errMsg);
      }

  }
}

std::shared_ptr<te::da::DataSetType> terrama2::core::copyDataSetType(std::shared_ptr<te::da::DataSetType> dataSetType, const std::string& newDataSetName)
{
  std::shared_ptr< te::da::DataSetType > newDatasetType = std::make_shared<te::da::DataSetType>(newDataSetName);

  for(const auto& property : dataSetType->getProperties())
  {
    auto newProperty = copyProperty(property);
    newDatasetType->add(newProperty.release());
  }

  return newDatasetType;
}
