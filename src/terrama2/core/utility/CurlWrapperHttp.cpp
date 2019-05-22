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
  \file terrama2/core/utility/CurlWrapperHttp.hpp
  \brief Utility classes for CurlWrapperHttp.

  \author Jean Souza
*/

#include "CurlWrapperHttp.hpp"
#include "Utils.hpp"
#include "../Exception.hpp"
#include "../interpreter/InterpreterFactory.hpp"

// Boost
#include <boost/algorithm/string/split.hpp>

// LibCurl
#include <curl/curl.h>


static std::vector<std::string> parseHTTPFiles(const std::vector<std::string>& bufferHTTPFiles)
{
  std::string httpServerHtml;

  for(auto const& s : bufferHTTPFiles) {
    httpServerHtml += s;
  }

  boost::replace_all(httpServerHtml, "\"", "\\\"");
  httpServerHtml.erase(std::remove(httpServerHtml.begin(), httpServerHtml.end(), '\n'), httpServerHtml.end());

  std::string scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/parse-http-server-html.py");
  std::string script = terrama2::core::readFileContents(scriptPath);
  boost::replace_all(script, "{HTML_CODE}", httpServerHtml);

  std::vector<std::string> vectorFiles;
  try
  {
    auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
    interpreter->runScript(script);
    auto fileNames = interpreter->getString("files");

    if(fileNames)
      boost::split(vectorFiles, *fileNames, [](char c){return c == ',';});
  }
  catch (const terrama2::core::InterpreterException& e)
  {
    QString errMsg = "Error listing files:\n";
    errMsg.append(boost::get_error_info<terrama2::ErrorDescription>(e));
    throw terrama2::core::DataRetrieverException() << terrama2::ErrorDescription(errMsg);
  }

  return vectorFiles;
}


void terrama2::core::CurlWrapperHttp::downloadFile(const std::string &url, std::FILE* file, te::common::TaskProgress* taskProgress)
{
  auto lock = getLock();

  clean();

  setOption(CURLOPT_FOLLOWLOCATION, 1L);
  setOption(CURLOPT_UNRESTRICTED_AUTH, 1L);

  downloadFile_(url, file, taskProgress);
}

std::vector<std::string> terrama2::core::CurlWrapperHttp::listFiles(const te::core::URI& uri)
{
  clean();

  auto normalizedURI = terrama2::core::normalizeURI(uri.uri());
  // Curl returns HTML page fragments
  auto bufferFileArray = listFiles_(normalizedURI);

  return parseHTTPFiles(bufferFileArray);
}
