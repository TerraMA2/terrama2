/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file terralib/se/serialization/xml/Style.h

  \brief Auxiliary classes and functions to serialize style informations from a XML document.
*/

// TODO: Remove this file after TerraLib fix the style serialization

#ifndef __TERRALIB_SE_SERIALIZATION_XML_INTERNAL_STYLE_H_FIX
#define __TERRALIB_SE_SERIALIZATION_XML_INTERNAL_STYLE_H_FIX

// TerraLib
#include <terralib/common/Singleton.h>
#include <terralib/se/Config.h>

// STL
#include <map>
#include <string>

// Boost
#include <boost/function.hpp>

namespace te
{
  namespace se { class Style; }

  namespace xml
  {
    class AbstractWriter;
    class Reader;
  }

  namespace se
  {
    class Style;

    namespace serialize
    {
      class TESEEXPORT Style : public te::common::Singleton<Style>
      {
        friend class te::common::Singleton<Style>;

        public:

          typedef boost::function<te::se::Style* (te::xml::Reader&)> StyleReadFnctType;
          typedef boost::function<void (const te::se::Style*, te::xml::AbstractWriter&)> StyleWriteFnctType;
          typedef std::pair<StyleReadFnctType, StyleWriteFnctType> StyleFnctSerializeType;
          typedef std::map<std::string, StyleFnctSerializeType> StyleFnctIdxType;

          void reg(const std::string& styleType, const StyleFnctSerializeType& fncts);

          te::se::Style* read(te::xml::Reader& reader) const;

          void write(const te::se::Style* style, te::xml::AbstractWriter& writer) const;

          ~Style();

        protected:

          Style();

        private:

          StyleFnctIdxType m_fncts;
      };
    }  //end namespace serialize
  }    // end namespace serialize
}      // end namespace te

#endif  // __TERRALIB_SE_SERIALIZATION_XML_INTERNAL_STYLE_H_FIX
