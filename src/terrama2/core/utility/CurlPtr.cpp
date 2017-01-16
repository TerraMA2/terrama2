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
  \file terrama2/core/utility/CurlPtr.cpp

  \brief This class is responsible for operation of class CurlPtr.

  \author Evandro Delatin
*/


// STL
#include <memory>
#include <cassert>
#include <iostream>
#include <functional>

// TerraMA2
#include "CurlPtr.hpp"
#include "Raii.hpp"
#include "../../impl/DataRetrieverFTP.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// LibCurl
#include <curl/curl.h>

// QT
#include <QObject>


terrama2::core::CurlPtr::CurlPtr()
{
  curl_ = curl_easy_init();
}

terrama2::core::CurlPtr::CurlPtr(terrama2::core::CurlPtr&& other)
  : curl_(other.curl_)
{
  other.curl_ = nullptr;
}

terrama2::core::CurlPtr& terrama2::core::CurlPtr::operator=(terrama2::core::CurlPtr&& other)
{
  curl_ = other.curl_;
  other.curl_ = nullptr;
  return *this;
}

void terrama2::core::CurlPtr::init()
{
  curl_easy_cleanup(curl_);
  curl_ = curl_easy_init();

  // solve curl bug
  // http://stackoverflow.com/questions/9191668/error-longjmp-causes-uninitialized-stack-frame
  curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1);
}

CURL* terrama2::core::CurlPtr::fcurl() const
{
  return curl_;
}

CURLcode terrama2::core::CurlPtr::verifyURL(std::string url)
{
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_FTPLISTONLY, 1);
  curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 3);
  curl_easy_setopt(curl_, CURLOPT_NOBODY, 1);

  return curl_easy_perform(curl_);
}

std::vector<std::string> terrama2::core::CurlPtr::getFtpList(const std::string& url,
                                                               size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data)) const
{
  // An url for a directory must have '/' in the last character, otherwise it won't list the files.
  std::string completeUrl = url;
  if(url.back() != '/')
    completeUrl += "/";

  std::vector<std::string> vectorFiles;
  curl_easy_setopt(curl_, CURLOPT_URL, completeUrl.c_str());
  curl_easy_setopt(curl_, CURLOPT_DIRLISTONLY, 0);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_vector);

  std::string block;
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&block);

  CURLcode status = curl_easy_perform(curl_);

  if (status == CURLE_OK)
  {
    boost::split(vectorFiles, block, boost::is_any_of("\n"));

    if(!vectorFiles.empty() && vectorFiles.back().empty())
      vectorFiles.pop_back();
  }
  else
  {
    QString errMsg = QObject::tr("Could not list files in the FTP server. \n\n");
    errMsg.append(curl_easy_strerror(status));

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  return vectorFiles;
}

std::vector<std::string> terrama2::core::CurlPtr::getFtpListDir(const std::string& url,
                                                                size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data)) const
{
  std::vector<std::string> list = getFtpList(url, write_vector);

  std::vector<std::string> dirList;

  for(const auto& item : list)
  {
    if(item[0] == 'd')
    {
      dirList.push_back(item.substr(item.find_last_of(' ')+1));
    }
  }

  return dirList;
}


std::vector<std::string> terrama2::core::CurlPtr::getFtpListFiles(const std::string& url,
                                                                size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data)) const
{
  std::vector<std::string> list = getFtpList(url, write_vector);

  std::vector<std::string> fileList;

  for(const auto& item : list)
  {
    if(item[0] != 'd')
    {
      fileList.push_back(item.substr(item.find_last_of(' ')+1));
    }
  }

  return fileList;
}

CURLcode terrama2::core::CurlPtr::getDownloadFiles(std::string url,
                                                   size_t(*write_response)(void *ptr, size_t size, size_t nmemb, void *data),
                                                   std::string filePath)
{
  terrama2::core::FilePtr opener(filePath.c_str(), "wb");
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_response);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, opener.file());

  return curl_easy_perform(curl_);
}

terrama2::core::CurlPtr::~CurlPtr()
{
  curl_easy_cleanup(curl_);
}
