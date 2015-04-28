/**
 * @file simple_particle_generator.h
 *
 * @date 2015年2月13日
 * @author salmon
 */

#ifndef CORE_PARTICLE_SIMPLE_PARTICLE_GENERATOR_H_
#define CORE_PARTICLE_SIMPLE_PARTICLE_GENERATOR_H_
#include "../numeric/rectangle_distribution.h"
#include "../numeric/multi_normal_distribution.h"
#include "particle_generator.h"
namespace simpla
{

template<typename EngineType, typename TFun>
ParticleGenerator<EngineType, rectangle_distribution<3>,
		multi_normal_distribution<3>, TFun> simple_particle_generator(
		EngineType const& engine,
		std::pair<nTuple<Real, 3>, nTuple<Real, 3> > const & extents, Real T,
		TFun const & fun)
{
	return std::move(
			ParticleGenerator<EngineType, rectangle_distribution<3>,
					multi_normal_distribution<3>,TFun>(engine,
					rectangle_distribution<3>(extents),
					multi_normal_distribution<3>(T), fun));
}
}  // namespace simpla

#endif /* CORE_PARTICLE_SIMPLE_PARTICLE_GENERATOR_H_ */
