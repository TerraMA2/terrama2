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
  \file terrama2/services/view/data-access/Geoserver.cpp

  \brief Communication class between View service and GeoServer

  \author Vinicius Campanha
*/

// TODO: only in linux?
#include <sys/stat.h>

// cURL
#include <curl/curl.h>

// TerraMA2
#include "Geoserver.hpp"

//TODO: remove
#include <iostream>


terrama2::services::view::data_access::GeoServer::GeoServer(std::string address,
                                                            std::string port,
                                                            std::string user,
                                                            std::string password)
  : address_(address), port_(port), user_(user), password_(password)
{

}


void terrama2::services::view::data_access::GeoServer::registerStyle(std::string name, std::string style)
{
  CURL* curl;

  //  Register a style
  {
    curl = curl_easy_init();

    if(curl)
    {
      std::string url = address_ + ":" + port_ + "/geoserver/rest/styles";
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
      curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());

      struct curl_slist* headers= nullptr;
      headers = curl_slist_append(headers, "Content-Type: text/xml");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


      std::string name = "<style><name>example_style</name><filename>example.sld</filename></style>";
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, name.c_str());

      CURLcode res;
      res = curl_easy_perform(curl);

      curl_slist_free_all(headers);

      if(res != CURLE_OK)
      {
        //        curl_easy_strerror(res)
      }
    }

    curl_easy_cleanup(curl);
  }

  // Load the sld
  {
    curl = curl_easy_init();

    if(curl)
    {
      std::string url = address_ + ":" + port_ + "/geoserver/rest/styles/example_style.sld";
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
      curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());

      struct curl_slist* headers= nullptr;
      headers = curl_slist_append(headers, "Content-type: application/vnd.ogc.se+xml");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

      FILE *file;
      struct stat file_info;

      file = fopen("/home/vinicius/example_style.sld", "rb");

      if(!file)
      {

      }

      if(fstat(fileno(file), &file_info) != 0)
      {

      }

      curl_easy_setopt(curl, CURLOPT_UPLOAD, true);

      curl_easy_setopt(curl, CURLOPT_READDATA, file);

      curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

      CURLcode res;
      res = curl_easy_perform(curl);

      curl_slist_free_all(headers);

      if(res != CURLE_OK)
      {
        std::cout << curl_easy_strerror(res) << std::endl;
      }
    }

    fclose(file);
    curl_easy_cleanup(curl);
  }

}
