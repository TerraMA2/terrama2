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
  \file terrama2/collector/DataRetrieverFTP.cpp

  \brief Data Retriever FTP.

  \author Jano Simas
  \author Evandro Delatin
*/

// STL
#include <iostream>
#include <fstream>
#include <vector>

// TerraMA2
#include "DataRetrieverFTP.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "Log.hpp"
#include "FileOpener.hpp"
#include "CurlOpener.hpp"

// Libcurl
#include <curl/curl.h>

// Boost
#include <boost/algorithm/string.hpp>

// QT
#include <QObject>

terrama2::collector::DataRetrieverFTP::DataRetrieverFTP(const terrama2::core::DataProvider& dataprovider)
  : DataRetriever(dataprovider)
{
  temporaryFolder_ = "/tmp/";
  scheme_ = "file://";
}

bool terrama2::collector::DataRetrieverFTP::isRetrivable() const noexcept
{
  return true;
}

terrama2::collector::DataRetrieverFTP::~DataRetrieverFTP()
{
  std::string path;
// Remove the files in the tmp folder
  for(std::string file: vectorNames_)
  {
     path = temporaryFolder_+file;
     std::remove(path.c_str()); // delete file
   }
}

void terrama2::collector::DataRetrieverFTP::open()
{

}

bool terrama2::collector::DataRetrieverFTP::isOpen()
{
  CURLcode status;

  CurlOpener curl;

  curl.init();

// Verifies that the FTP address is valid
  if(curl.fcurl())
  {
    curl_easy_setopt(curl.fcurl(), CURLOPT_URL, dataprovider_.uri().c_str());
    curl_easy_setopt(curl.fcurl(), CURLOPT_FTPLISTONLY, 1);
    curl_easy_setopt(curl.fcurl(), CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl.fcurl(), CURLOPT_NOBODY, 1);
// performs the configurations of curl_easy_setop
    status = curl_easy_perform(curl.fcurl());

    if (status != CURLE_OK)   
      return false;

  }

  return true;
}

void terrama2::collector::DataRetrieverFTP::close()
{

}

size_t terrama2::collector::DataRetrieverFTP::write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}

size_t terrama2::collector::DataRetrieverFTP::write_vector(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t sizeRead = size * nmemb;

  std::string* block = (std::string*) data;
  block->append((char *)ptr, sizeRead);

  return sizeRead;
}


std::string terrama2::collector::DataRetrieverFTP::retrieveData(const terrama2::core::DataSetItem& datasetitem,
                                                                DataFilterPtr filter,
                                                                std::vector<TransferenceData>& transferenceDataVec)
{
  std::string uri_origin;
  std::string uriInput;
  CURLcode status;
  std::vector<std::string> vectorFiles;
  std::string block;

  CurlOpener curl;

  curl.init();

  try
  {
// Get a file listing from server
    if(curl.fcurl())
    {
// Get a file listing from server
      uriInput = dataprovider_.uri() + datasetitem.path();
// The host part of the URL contains the address of the server that you want to connect to
      curl_easy_setopt(curl.fcurl(), CURLOPT_URL, uriInput.c_str());
// List files and directories FTP server
      curl_easy_setopt(curl.fcurl(), CURLOPT_DIRLISTONLY, 1);
// Get data to be written in vector
      curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEFUNCTION, boost::bind(&terrama2::collector::DataRetrieverFTP::write_vector, this, _1, _2, _3, _4));
// Set a pointer to our block data
      curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEDATA, (void *)&block);
// performs the configurations of curl_easy_setop
      status = curl_easy_perform(curl.fcurl());

      if (status == CURLE_OK)
      {
        boost::split(vectorFiles, block, boost::is_any_of("\n"));

        if(vectorFiles.size() && vectorFiles.back().empty())
          vectorFiles.pop_back();
      }
      else
      {
        QString messageError = QObject::tr("Could not list files in the FTP server. \n\n");
        messageError.append(curl_easy_strerror(status));
        throw DataRetrieverFTPException() << ErrorDescription(messageError);
      }

// filter file names that should be downloaded.
      vectorNames_ = filter->filterNames(vectorFiles);

      for (std::string file: vectorNames_)
      {
        CURLcode res;

        CurlOpener curlDown;

        curlDown.init();

// Performs the download of files in the vectorNames
        if (curlDown.fcurl())
        {
          uri_origin = dataprovider_.uri() + datasetitem.path() + file;
          std::string filePath = temporaryFolder_+file;
          FileOpener opener(filePath.c_str(),"wb");
          curl_easy_setopt(curlDown.fcurl(), CURLOPT_URL, uri_origin.c_str());
// Get data to be written in file
          curl_easy_setopt(curlDown.fcurl(), CURLOPT_WRITEFUNCTION, boost::bind(&terrama2::collector::DataRetrieverFTP::write_response, this, _1, _2, _3, _4 ));
// Set a pointer to our block data
          curl_easy_setopt(curlDown.fcurl(), CURLOPT_WRITEDATA, opener.file());
// performs the configurations of curl_easy_setop
          res = curl_easy_perform(curlDown.fcurl());

          if (res != CURLE_OK)
          {
            QString messageError = QObject::tr("Could not perform the download. \n\n");
            messageError.append(curl_easy_strerror(res));
            throw DataRetrieverFTPException() << ErrorDescription(messageError);
          }
          else
          {
            TransferenceData tmp;
            tmp.uri_origin = uri_origin;
            tmp.uri_temporary = "file://"+filePath;
            transferenceDataVec.push_back(tmp);
          }
        }
      }
    }

  }
  catch(const std::exception& e)
  {
    QString messageError = QObject::tr("Could not perform the download files! \n\n Details: \n");
    messageError.append(e.what());

    throw DataRetrieverFTPException() << ErrorDescription(messageError);
  }

  catch(...)
  {
    throw DataRetrieverFTPException() << ErrorDescription(QObject::tr("Unknown Error, Could not perform the download files!"));
  }

  // returns the absolute path of the folder that contains the files that have been made the download.
  return scheme_+temporaryFolder_;
}
