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
  \file terrama2/services/alert/impl/DocumentPDF.hpp
  \brief

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_ALERT_IMPL_DOCUMENT_PDF_HPP__
#define __TERRAMA2_SERVICES_ALERT_IMPL_DOCUMENT_PDF_HPP__

// TerraMA2
#include "../core/Report.hpp"
#include "../core/Shared.hpp"
#include "../core/Utils.hpp"
#include "../../../core/utility/Logger.hpp"

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPagedPaintDevice>
#include <QtGui/QPdfWriter>
#include <QtGui/QTextDocument>

// STL
#include <string>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace impl
      {
        namespace documentPDF {

            static std::string documentCode() { return "PDF"; }

            static std::string makeDocument(core::ReportPtr report)
            {
              // Message body
              std::string body;

              if(report->dataSeriesType() == terrama2::core::DataSeriesType::GRID)
              {
                body = core::gridReportText();
              }
              else
              {
                body = core::monitoredObjectReportText();
              }

              core::replaceReportTags(body, report);

              std::string documentName = "";
              std::string path = report->documentSavePath() + "/" + documentName;

              QPdfWriter writer(QString::fromStdString(path));
              writer.setPageSize(QPagedPaintDevice::A4);

              // Qt > 5.2
//              writer.setPageMargins(QMargins(30, 30, 30, 30));
//              writer.setResolution(100);

              // Qt < 5.3
              QPagedPaintDevice::Margins margins;
              margins.bottom = 10;
              margins.left = 10;
              margins.right = 10;
              margins.top = 10;
              writer.setMargins(margins);

              QTextDocument td;
              td.setHtml(QString::fromStdString(body));
              td.setDefaultFont(QFont("Times", 12));
              td.setTextWidth(12);

              td.adjustSize();

              td.print(&writer);

              TERRAMA2_LOG_INFO() << QObject::tr("Report document generated at '%1'").arg(QString::fromStdString(path));

              return path;
            }

        } /* documentPDF */
      } /* impl */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_IMPL_DOCUMENT_PDF_HPP__
