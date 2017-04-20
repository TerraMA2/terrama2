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
  \file terrama2/unittest/core/MockCurlWrapper.hpp
  \brief Mock Class CurlWrapper
  \author Evandro Delatin
*/

#ifndef __TERRAMA2_UNITTEST_CORE_MOCKCURLWRAPPER__
#define __TERRAMA2_UNITTEST_CORE_MOCKCURLWRAPPER__

// TerraMA2
#include <terrama2/core/utility/CurlWrapperFtp.hpp>
#include <terrama2/impl/DataRetrieverFTP.hpp>

// Libcurl
#include <curl/curl.h>

// GMock
#include <gmock/gmock.h>

class MockCurlWrapper: public terrama2::core::CurlWrapperFtp
{
public:

  MockCurlWrapper() {};

  MOCK_CONST_METHOD2(verifyURL,void(std::string url, uint32_t timeout));

  MOCK_METHOD3(getListFiles, std::vector<std::string>(std::string url,
                             size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data),
                             std::string block));

  MOCK_METHOD3(getDownloadFiles, CURLcode(std::string url,
                                 size_t(*write_response)(void *ptr, size_t size, size_t nmemb, void *data),
                                 std::string filePath));
};

#endif //__TERRAMA2_UNITTEST_CORE_MOCKCURLWRAPPER__
