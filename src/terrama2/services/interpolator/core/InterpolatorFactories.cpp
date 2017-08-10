#include "InterpolatorFactories.h"

#include "Interpolator.hpp"
#include "Typedef.hpp"

terrama2::services::interpolator::core::InterpolatorFactories::InterpolatorFactories(const int& key) :
  te::common::ParameterizedAbstractFactory<Interpolator, int, InterpolatorParams>(key)
{

}

terrama2::services::interpolator::core::NNInterpolatorFactory::NNInterpolatorFactory() :
  InterpolatorFactories(NEARESTNEIGHBOR)
{
}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::NNInterpolatorFactory::build(InterpolatorParams p)
{
  InterpolatorParamsPtr pptr(&p);

  NNInterpolator* i = new NNInterpolator(pptr);

  return i;
}

terrama2::services::interpolator::core::BLInterpolatorFactory::BLInterpolatorFactory() :
  InterpolatorFactories(BILINEAR)
{
}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::BLInterpolatorFactory::build(InterpolatorParams p)
{
  InterpolatorParamsPtr pptr(&p);

  BLInterpolator* i = new BLInterpolator(pptr);

  return i;
}

terrama2::services::interpolator::core::BCInterpolatorFactory::BCInterpolatorFactory() :
  InterpolatorFactories(BICUBIC)
{

}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::BCInterpolatorFactory::build(InterpolatorParams p)
{
  InterpolatorParamsPtr pptr(&p);

  BCInterpolator* i = new BCInterpolator(pptr);

  return i;
}
