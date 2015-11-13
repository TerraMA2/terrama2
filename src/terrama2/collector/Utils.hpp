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
  \file terrama2/collector/Collector.hpp

  \brief Aquire data from server.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_UTILS_HPP__
#define __TERRAMA2_COLLECTOR_UTILS_HPP__

//STd
#include <exception>

//Boost
#include <boost/exception/all.hpp>

namespace terrama2
{
  namespace collector
  {
    //! Class for Resource Acquisition Is Initialization (RAII) of classes the need to open and close.
    template<class T> class OpenClose
    {
    public:
      /*!
        \brief The constructor will open the object and keep a reference to it.

        \note The object must not be open, no verification is made.

        \param obj Object to be open.
       */
      OpenClose(T& obj)
        : obj_(obj)
      {
        obj_->open();
      }

      /*!
        \brief The destructor will close the object.

        Any esception thrown will be caught and logged, if unable to log nothing will be done.

       */
      ~OpenClose()
      {
        try
        {
          obj_->close();
        }
        catch(boost::exception& e)
        {
          try
          {
            //TODO: LOG here!!!
          }
          catch(...) { }
        }
        catch(std::exception& e)
        {
          try
          {
            //TODO: LOG here!!!
          }
          catch(...) { }
        }
        catch(...) { }
      }

    private:
      T& obj_;
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_UTILS_HPP__
