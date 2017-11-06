/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/services/alert/core/utility/DocumentFactory.hpp

  \brief

  \author  Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_DOCUMENT_FACTORY_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_DOCUMENT_FACTORY_HPP__

#include "../Shared.hpp"

// TerraLib
#include <terralib/common/Singleton.h>
#include <terralib/core/uri/URI.h>

// STL
#include <functional>
#include <map>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        class DocumentFactory : public te::common::Singleton<DocumentFactory>
        {
          public:
            //! DataAccessor constructor function.
            typedef std::function<te::core::URI (ReportPtr report)> FactoryFnctType;
            //! Register a new DataAccessor constructor associated with the DataSeriesSemantics.
            void add(const std::string& documentCode, FactoryFnctType f);
            //! Remove the DataAccessor constructor associated with the DataSeriesSemantics.
            void remove(const std::string& documentCode);
            //! Check if there is a DataAccessor constructor associated with the DataSeriesSemantics.
            bool find(const std::string& documentCode);

            /*!
              * \brief makeDocument
              * \param documentCode
              * \param alert
              * \param report
              * \return
              */
             te::core::URI makeDocument(const std::string& documentCode, ReportPtr report);

          protected:
            friend class te::common::Singleton<DocumentFactory>;

            //! Default constructor.
            DocumentFactory() = default;
            //! Default destructor
            virtual ~DocumentFactory() = default;

            DocumentFactory(const DocumentFactory& other) = delete;
            DocumentFactory(DocumentFactory&& other) = delete;
            DocumentFactory& operator=(const DocumentFactory& other) = delete;
            DocumentFactory& operator=(DocumentFactory&& other) = delete;

          private:

            std::map<std::string, FactoryFnctType> factoriesMap_;
        };
      } /* core */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_CORE_DOCUMENT_FACTORY_HPP__
