#include "InterpolatorFactories.h"

#include "Typedef.hpp"

terrama2::services::interpolator::core::InterpolatorFactories::InterpolatorFactories(const int& key) :
  te::common::ParameterizedAbstractFactory<Interpolator, int, InterpolatorParams>(key)
{

}

terrama2::services::interpolator::core::NNInterpolatorFactory::NNInterpolatorFactory() :
  InterpolatorFactories(NEARESTNEIGHBOR)
{

}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::NNInterpolatorFactory::build(const InterpolatorParams& p)
{

}

terrama2::services::interpolator::core::BLInterpolatorFactory::BLInterpolatorFactory() :
  InterpolatorFactories(BILINEAR)
{

}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::BLInterpolatorFactory::build(const InterpolatorParams& p)
{

}

terrama2::services::interpolator::core::BCInterpolatorFactory::BCInterpolatorFactory() :
  InterpolatorFactories(BICUBIC)
{

}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::BCInterpolatorFactory::build(const InterpolatorParams& p)
{

}
