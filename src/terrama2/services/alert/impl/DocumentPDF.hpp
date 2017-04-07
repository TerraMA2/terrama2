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
#include "Utils.hpp"
#include "../core/Report.hpp"
#include "../core/Shared.hpp"

// Qt
#include <QtGui/QPainter>
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
              std::string body;

              body = "<b>" + report->author() + "</b><br/>";
              body += report->abstract() + "<br/>";
              body += report->description() + "<br/>";


              body+= terrama2::services::alert::core::dataSetHtmlTable(report->retrieveData());

              body += report->copyright() + "<br/>";

              std::string path = "/home/vinicius/pdftest.pdf";

              QPdfWriter writer(QString::fromStdString(path));
              writer.setPageSize(QPagedPaintDevice::A4);
            //  writer.setPageMargins(QMargins(30, 30, 30, 30));

              QPagedPaintDevice::Margins margins;

              margins.bottom = 30;
              margins.left = 30;
              margins.right = 30;
              margins.top = 30;
              writer.setMargins(margins);

              QPainter painter(&writer);
              painter.translate(QPointF(50,50));
              painter.setPen(Qt::black);
              painter.setFont(QFont("Times", 120));

              QTextDocument td;
              td.setHtml(QString::fromStdString(body));
              td.setDefaultFont(QFont("Times", 120));

              td.drawContents(&painter);

              painter.end();

              return path;
            }

        } /* documentPDF */
      } /* impl */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_IMPL_DOCUMENT_PDF_HPP__
