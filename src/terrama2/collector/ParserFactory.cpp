
#include "ParserFactory.hpp"
#include "ParserOGR.hpp"

terrama2::collector::ParserPtr terrama2::collector::ParserFactory::getParser(terrama2::core::DataSetItem::Kind datasetItemKind)
{
  switch (datasetItemKind) {
    case core::DataSetItem::PCD_INPE_TYPE:
    case core::DataSetItem::PCD_TOA5_TYPE:
    {
      ParserPtr newParser(new ParserOGR());
      return newParser;
    }

    default:
      break;
  }

  return ParserPtr();
}
