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
  \file terrama2/core/data-access/SynchronizedInterpolator.cpp

  \brief Thread-safe access to a interpolator.

  \author Paulo R. M. Oliveira
*/

#include "SynchronizedInterpolator.hpp"

terrama2::core::SynchronizedInterpolator::SynchronizedInterpolator(std::shared_ptr<te::rst::Interpolator> interpolator)
: interpolator_(interpolator)
{
}

void terrama2::core::SynchronizedInterpolator::getValue(const double& c, const double& r, std::complex<double>& v,
                                                        const std::size_t& b)
{
  std::lock_guard<std::mutex> lockGuard(mutex_);
  interpolator_->getValue(c, r, v, b);
}

void terrama2::core::SynchronizedInterpolator::getValues(const double& c, const double& r,
                                                         std::vector<std::complex<double> >& values)
{
  std::lock_guard<std::mutex> lockGuard(mutex_);
  interpolator_->getValues(c, r, values);
}
