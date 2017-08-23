#include "InterpolatorFactories.h"

#include "Interpolator.hpp"
#include "InterpolatorParams.hpp"
#include "Typedef.hpp"

// TerraLib
#include <terralib/common/STLUtils.h>

std::vector<terrama2::services::interpolator::core::InterpolatorFactories*> terrama2::services::interpolator::core::InterpolatorFactories::factories_;

terrama2::services::interpolator::core::InterpolatorFactories::InterpolatorFactories(const int& key) :
  te::common::ParameterizedAbstractFactory<Interpolator, int, const InterpolatorParams&>(key)
{

}

void terrama2::services::interpolator::core::InterpolatorFactories::initialize()
{
  factories_.push_back(new NNInterpolatorFactory);
  factories_.push_back(new BLInterpolatorFactory);
  factories_.push_back(new BCInterpolatorFactory);
}

void terrama2::services::interpolator::core::InterpolatorFactories::finalize()
{
  te::common::FreeContents(factories_);
}

terrama2::services::interpolator::core::NNInterpolatorFactory::NNInterpolatorFactory() :
  InterpolatorFactories(NEARESTNEIGHBOR)
{
}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::NNInterpolatorFactory::build(const InterpolatorParams& p)
{
//  InterpolatorParams* p1 = &p;
  const NNInterpolatorParams* pp = dynamic_cast<const NNInterpolatorParams*>(&p);

  InterpolatorParamsPtr pptr(new NNInterpolatorParams(*pp));

  NNInterpolator* i = new NNInterpolator(pptr);

  return i;
}

terrama2::services::interpolator::core::BLInterpolatorFactory::BLInterpolatorFactory() :
  InterpolatorFactories(BILINEAR)
{
}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::BLInterpolatorFactory::build(const InterpolatorParams& p)
{
  const BLInterpolatorParams* pp = dynamic_cast<const BLInterpolatorParams*>(&p);

  InterpolatorParamsPtr pptr(new BLInterpolatorParams(*pp));

  BLInterpolator* i = new BLInterpolator(pptr);

  return i;
}

terrama2::services::interpolator::core::BCInterpolatorFactory::BCInterpolatorFactory() :
  InterpolatorFactories(BICUBIC)
{

}

terrama2::services::interpolator::core::Interpolator* terrama2::services::interpolator::core::BCInterpolatorFactory::build(const InterpolatorParams& p)
{
  const BCInterpolatorParams* pp = dynamic_cast<const BCInterpolatorParams*>(&p);

  InterpolatorParamsPtr pptr(new BCInterpolatorParams(*pp));

  BCInterpolator* i = new BCInterpolator(pptr);

  return i;
}
