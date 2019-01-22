#ifndef __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_OPERATOR_HPP__
#define __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_OPERATOR_HPP__

namespace terrama2
{
  namespace analysis
  {
    namespace core
    {
      namespace vp
      {
        class Operator
        {
          public:
            Operator();
            virtual ~Operator();

            virtual void execute() = 0;

          protected:

        };
      }
    }
  }
}

#endif // __TERRAMA2_ANALYSIS_VECTOR_PROCESSING_OPERATOR_HPP__
