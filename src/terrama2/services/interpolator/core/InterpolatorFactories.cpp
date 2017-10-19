#include "InterpolatorFactories.h"

#include "Interpolator.hpp"
#include "InterpolatorParams.hpp"
#include "Typedef.hpp"

// TerraLib
#include <terralib/common/STLUtils.h>

std::vector<terrama2::services::interpolator::core::InterpolatorFactories*> terrama2::services::interpolator::core::InterpolatorFactories::factories_;

terrama2::services::interpolator::core::InterpolatorFactories::InterpolatorFactories(const int& key) :
  te::common::ParameterizedAbstractFactory<Interpolator, int, InterpolatorParamsPtr>(key)
{

}

void terrama2::services::interpolator::core::InterpolatorFactories::initialize()
{
  factories_.push_back(new NNInterpolatorFactory);
  factories_.push_back(new AvgDistInterpolatorFactory);
  factories_.push_back(new SqrAvgDistInterpolatorFactory);
}

void terrama2::services::interpolator::core::InterpolatorFactories::finalize()
{
  te::common::FreeContents(factories_);
}

terrama2::services::interpolator::core::NNInterpolatorFactory::NNInterpolatorFactory() :
  InterpolatorFactories(NEARESTNEIGHBOR)
{
}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::NNInterpolatorFactory::build(InterpolatorParamsPtr p)
{
  InterpolatorParamsPtr pp(dynamic_cast<NNInterpolatorParams*>(p.get()));

  NNInterpolator* i = new NNInterpolator(pp);

  return i;
}

terrama2::services::interpolator::core::AvgDistInterpolatorFactory::AvgDistInterpolatorFactory() :
  InterpolatorFactories(AVGDIST)
{
}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::AvgDistInterpolatorFactory::build(InterpolatorParamsPtr p)
{
  InterpolatorParamsPtr pp(dynamic_cast<AvgDistInterpolatorParams*>(p.get()));

  Interpolator* i = new AvgDistInterpolator(pp);

  return i;
}

terrama2::services::interpolator::core::SqrAvgDistInterpolatorFactory::SqrAvgDistInterpolatorFactory() :
  InterpolatorFactories(SQRAVGDIST)
{

}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::SqrAvgDistInterpolatorFactory::build(InterpolatorParamsPtr p)
{
  InterpolatorParamsPtr pp(dynamic_cast<SqrAvgDistInterpolatorParams*>(p.get()));

  Interpolator* i = new SqrAvgDistInterpolator(pp);

  return i;
}
