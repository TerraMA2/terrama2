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
              std::string body = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Excel To HTML using codebeautify.org</title></head><body><!DOCTYPE html><html><head><style>body{background-color:#ffffff;}h1{color:blue;text-align:center;}p{font-family:\"Times New Roman\";}</style></head><body><h1>%TITLE%</h1><p>%ABSTRACT%</p><p>%DESCRIPTION%</p>"
                                 "<hr>%COMPLETE_DATA%<hr>"
                                 "<hr><p>%MAXVALUE_DATA%</p><hr>"
                                 "<hr><p>%MINVALUE_DATA%</p><hr>"
                                 "<hr><p>%MEANVALUE_DATA%</p><hr>"
                                 "<p>%COPYRIGHT%</p></body></html>";

              core::replaceReportTags(body, report);

              std::string path = "/home/vinicius/pdftest.pdf";

              QPdfWriter writer(QString::fromStdString(path));
              writer.setPageSize(QPagedPaintDevice::A4);

              // Qt > 5.2
              writer.setPageMargins(QMargins(30, 30, 30, 30));
              writer.setResolution(100);

              // Qt < 5.3
//              QPagedPaintDevice::Margins margins;
//              margins.bottom = 30;
//              margins.left = 30;
//              margins.right = 30;
//              margins.top = 30;
//              writer.setMargins(margins);

              QTextDocument td;
              td.setHtml(QString::fromStdString(body));
              td.setDefaultFont(QFont("Times", 12));
              td.setTextWidth(12);

              td.adjustSize();

              td.print(&writer);

              return path;
            }

        } /* documentPDF */
      } /* impl */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_IMPL_DOCUMENT_PDF_HPP__
