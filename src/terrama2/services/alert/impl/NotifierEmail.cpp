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
  \file terrama2/services/alert/impl/NotifierEmail.cpp

  \brief

  \author Jano Simas
          Vinicius Campanha
*/

// TerraMA2
#include "NotifierEmail.hpp"
#include "Utils.hpp"

#include "../../../core/utility/Logger.hpp"
#include "../core/SimpleCertificateVerifier.hpp"
#include "../core/Report.hpp"
#include "../core/Utils.hpp"
#include "../core/Exception.hpp"

// TerraLib
#include <terralib/core/uri/URI.h>

// Qt
#include <QString>
#include <QObject>
#include <QFileInfo>

vmime::shared_ptr <vmime::net::session> terrama2::services::alert::impl::NotifierEmail::session_ = vmime::net::session::create();

terrama2::services::alert::impl::NotifierEmail::NotifierEmail(const std::map<std::string, std::string>& serverMap, core::ReportPtr report)
    : Notifier(serverMap, report)
{
  if(serverMap_.at("email_server").empty())
  {
    QString errMsg = QObject::tr("Email host was not informed!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NotifierException() << ErrorDescription(errMsg);
  }
}

void terrama2::services::alert::impl::NotifierEmail::send(const core::Notification& notification) const
{
  te::core::URI emailServer(serverMap_.at("email_server"));

  vmime::messageBuilder mb;

  // Fill in the basic fields
  mb.setExpeditor(vmime::mailbox(emailServer.user()));

  vmime::addressList to;

  for(auto target : notification.targets)
  {
    to.appendAddress(vmime::make_shared <vmime::mailbox>(target));
  }

  mb.setRecipients(to);
  mb.setSubject(vmime::text(report_->title()));

  // Message body
  std::string body;

  if(report_->dataSeriesType() == terrama2::core::DataSeriesType::GRID)
  {
    body = core::gridReportText();
  }
  else if(report_->dataSeriesType() == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
  {
    body = core::monitoredObjectReportText();
  }
  else
  {
    QString errMsg = QObject::tr("Email notifier is not implemented for this data series!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NotifierException() << ErrorDescription(errMsg);
  }

  core::replaceReportTags(body, report_);

  mb.constructTextPart(vmime::mediaType(vmime::mediaTypes::TEXT, vmime::mediaTypes::TEXT_HTML));
  vmime::shared_ptr<vmime::htmlTextPart> textPart = vmime::dynamicCast<vmime::htmlTextPart>(mb.getTextPart());
  textPart->setCharset(vmime::charsets::ISO8859_15);

  textPart->setText(vmime::make_shared <vmime::stringContentHandler>(body));

  // add attachments
  if(!notification.includeReport.empty())
  {
    QString documentURI = QString::fromStdString(report_->documentURI());

    if(documentURI.isEmpty())
    {
      QString errMsg = QObject::tr("Couldn't find document: File URI was not informed!");
      throw NotifierException() << ErrorDescription(errMsg);
    }

    QFileInfo fileURI(documentURI);

    if(!fileURI.exists())
    {
      QString errMsg = QObject::tr("Couldn't find document: File does not exists!");
      throw NotifierException() << ErrorDescription(errMsg);
    }

    vmime::shared_ptr<vmime::fileAttachment> att = vmime::make_shared<vmime::fileAttachment>(fileURI.absoluteFilePath().toStdString(),
                                                                                             vmime::mediaType("application/pdf"),
                                                                                             vmime::text(""));

    mb.appendAttachment(att);
  }

  // Construction
  vmime::shared_ptr <vmime::message> msg = mb.construct();

  // email server
  vmime::utility::url url(emailServer.scheme() + "://" +emailServer.host() + ":" + emailServer.port());
  vmime::shared_ptr<vmime::net::transport> tr = session_->getTransport(url);
  tr->setProperty("connection.tls", true);
  tr->setProperty("auth.username", emailServer.user());
  tr->setProperty("auth.password", emailServer.password());
  tr->setProperty("options.need-authentication", true);

  tr->setCertificateVerifier(vmime::make_shared<SimpleCertificateVerifier>());
  tr->connect();
  tr->send(msg);

  TERRAMA2_LOG_INFO() << QObject::tr("Report email sent to '%1'").arg(QString::fromStdString(emailServer.user()));
}
