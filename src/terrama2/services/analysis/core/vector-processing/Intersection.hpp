#ifndef __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__
#define __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__

#include "Operator.hpp"

namespace terrama2
{
  namespace analysis
  {
    namespace core
    {
      namespace vp
      {
        class Intersection : public Operator
        {
          public:
            Intersection()
              : Operator::Operator()
            { }

            ~Intersection() override;

            void execute() override { }

          private:
//            int a;
        };
      }
    }
  }
}

#endif // __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_INTERSECTION_HPP__
