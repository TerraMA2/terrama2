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

  \brief .

  \author Jano Simas
  \author Evandro Delatin
*/

// STL
#include <iostream>
#include <fstream>

// TerraMA2
#include "DataRetrieverFTP.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "Log.hpp"

// libcurl
#include <curl/curl.h>

// QT
#include <QTranslator>

terrama2::collector::DataRetrieverFTP::DataRetrieverFTP(const terrama2::core::DataProvider& dataprovider)
  : DataRetriever(dataprovider)
{

}

void terrama2::collector::DataRetrieverFTP::open()
{
  // local file name to store the file
  ftpfile = std::fopen("files.txt", "wb"); /* b is binary, needed on win32 */

  curl = curl_easy_init();
}

bool terrama2::collector::DataRetrieverFTP::isOpen()
{
// check if connection is open

  CURLcode status;
  if(curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, dataprovider_.uri().c_str());
    curl_easy_setopt(curl, CURLOPT_FTPLISTONLY, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

    status = curl_easy_perform(curl);

    if (status != CURLE_OK)
      return false;
  }

  curl_easy_cleanup(curl);
  curl = curl_easy_init();

  return true;

}

void terrama2::collector::DataRetrieverFTP::close()
{
  // always cleanup
  curl_easy_cleanup(curl);
}

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}

std::string terrama2::collector::DataRetrieverFTP::retrieveData(const terrama2::core::DataSetItem& datasetitem, DataFilterPtr filter)
{
  std::string uri;
  std::string url;
  std::string line;
  CURLcode status;
  std::vector<std::string> vectorFiles;

  try
  {
    if(curl)
    {
      /* Get a file listing from server */
      // curl_easy_setopt(curl, CURLOPT_URL, "ftp://username@localhost:21/");
      //FIXME: How to differentiate directories files (CURLOPT_WILDCARDMATCH).
      url = dataprovider_.uri() + datasetitem.path();
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      //curl_easy_setopt(curl, CURLOPT_URL, dataprovider_.uri() + datasetitem.path());
      // get only the list of files and directories
      curl_easy_setopt(curl, CURLOPT_FTPLISTONLY, ftpfile);
      curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, ftpfile);
      // performs the configurations of curl_easy_setopt
      status = curl_easy_perform(curl);

      if (status == CURLE_OK)
        std::fclose(ftpfile);
      else
      {
        QString messageError = QObject::tr("Could not list the FTP server files. \n\n Details: \n");
        messageError.append(curl_easy_strerror(status));
        throw DataRetrieverFTPError() << ErrorDescription(messageError);
      }

      // Reads the file and write each line of the file in the file vector.
      try
      {
        std::ifstream fileList("files.txt");
        if (fileList.is_open())
        {
          while(!fileList.eof())
          {
            getline(fileList,line);
            vectorFiles.push_back(line);
          }
        }
      }
      catch(const std::ios_base::failure& e)
      {
        QString messageError = QObject::tr("Could not open file! \n\n Details: \n");
        messageError.append(e.what());

        throw DataRetrieverFTPError() << ErrorDescription(messageError);
      }

      catch(const std::exception& e)
      {
        QString messageError = QObject::tr("Could not open file! \n\n Details: \n");
        messageError.append(e.what());

        throw DataRetrieverFTPError() << ErrorDescription(messageError);
      }

      catch(...)
      {
        throw DataRetrieverFTPError() << ErrorDescription(QObject::tr("Unknown Error, could not open file!"));
      }

      // filter file names that should be downloaded.
      std::vector<std::string> names = filter->filterNames(vectorFiles);

      for (std::string file: names)
      {
        CURL *curl;
        FILE *destFilePath;
        CURLcode res;

        curl = curl_easy_init();

        if (curl)
        {
          uri = dataprovider_.uri() + datasetitem.path() + file;
          destFilePath = fopen(("/tmp/"+file).c_str(),"wb");
          curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, destFilePath);
          res = curl_easy_perform(curl);

          if (res != CURLE_OK)
          {
            QString messageError = QObject::tr("Could not perform the download files. \n\n Details: \n");
            messageError.append(curl_easy_strerror(res));
            throw DataRetrieverFTPError() << ErrorDescription(messageError);
          }
          else
          {
            curl_easy_cleanup(curl);

            fclose(destFilePath);
          }
        }
      }
    }

  }
  catch(const std::exception& e)
  {
    QString messageError = QObject::tr("Could not perform the download files! \n\n Details: \n");
    messageError.append(e.what());

    throw DataRetrieverFTPError() << ErrorDescription(messageError);
  }

  catch(...)
  {
    throw DataRetrieverFTPError() << ErrorDescription(QObject::tr("Unknown Error, Could not perform the download files!"));
  }

  // returns the absolute path of the folder that contains the files that have been made the download.
  return "file:///tmp/";
}
