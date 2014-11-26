/*
 * probe_particle.h
 *
 *  Created on: 2014年7月12日
 *      Author: salmon
 */

#ifndef CORE_PARTICLE_TRACABLE_PARTICLE_H_
#define CORE_PARTICLE_TRACABLE_PARTICLE_H_

#include <iostream>
#include <memory>
#include <string>

#include "../physics/physical_object.h"
#include "../utilities/log.h"
#include "../utilities/primitives.h"
#include "../data_structure/data_set.h"
namespace simpla
{

struct IsTracable;
template<typename ...> struct _Particle;
/**
 *  @brief Particle<IsTracable> is a container of particle trajectory.
 *
 *  It can cache the history of particle position.
 *
 *  * function next_timestep(Point * p, Real dt, Args && ...)
 *
 *    p - m0  particle position  at m0 steps before
 *
 *    p - 1   particle position  at last time step
 *
 *    p       particle position  at current time step
 *
 *    p + 1   particle position  at next time step
 *
 *    p + 2   predicate particle position  after next two time steps
 *
 *    p + m1  predicate particle position  after next m1 time steps
 *
 *    default: m0=m1=0
 *
 */

template<typename Engine, typename TDomain>
struct _Particle<Engine, TDomain, IsTracable> : public Engine,
		public PhysicalObject
{
	typedef TDomain domain_type;

	typedef Engine engine_type;

	typedef _Particle<domain_type, engine_type, IsTracable> this_type;

	typedef typename Engine::Point_s Point_s;

	typedef Point_s * iterator;

	typedef typename domain_type::coordinates_type coordinates_type;

	//****************************************************************
	// Constructor
	template<typename ...Others>
	Particle(std::shared_ptr<domain_type>& pdomain, Others && ...);

	// Destroy
	~Particle();

	static std::string get_type_as_string_static()
	{
		return engine_type::get_type_as_string();
	}
	std::string get_type_as_string() const
	{
		return get_type_as_string_static();
	}

	template<typename TDict>
	void load(TDict const & dict)
	{
		engine_type::load(dict);
	}

	bool update();

	DataSet dataset() const;

	void swap(this_type &);

	template<typename OS>
	OS& print(OS& os) const;

	template<typename ...Args>
	void next_n_steps(size_t step_num, Real dt, Args && ...);

	template<typename ...Args>
	void next_step(Real dt, Args && ...);

	void push_back(Point_s const &);

	template<typename TI>
	void push_back(TI const & b, TI const & e);

	template<typename ...Args>
	void emplace_back(Args && ...args);

	void cache_length(size_t l)
	{
		cache_length_ = l;
	}
	size_t cache_length() const
	{
		return cache_length_;
	}

private:
	std::shared_ptr<domain_type> domain_;

	std::shared_ptr<Point_s> data_;

	size_t num_of_points_ = 1024;
	size_t cache_length_ = 1;
	bool is_cached_ = false;
	size_t cache_depth_ = 10;

};

template<typename Engine, typename TDomain>
template<typename OS>
OS& _Particle<Engine, TDomain, IsTracable>::print(OS & os) const
{
	engine_type::print(os);
	return os;
}

template<typename Engine, typename TDomain>
template<typename ... Others>
_Particle<Engine, TDomain, IsTracable>::Particle(
		std::shared_ptr<TDomain> & pdomain, Others && ...others) :
		domain_(pdomain->shared_from_this()), engine_type(
				std::forward<Others>(others)...)
{
//	engine_type::load(std::forward<Others>(others)...);
}

template<typename Engine, typename TDomain>
_Particle<Engine, TDomain, IsTracable>::~Particle()
{
}
template<typename Engine, typename TDomain>
bool _Particle<Engine, TDomain, IsTracable>::update()
{
	return true;
}
template<typename Engine, typename TDomain>
DataSet _Particle<Engine, TDomain, IsTracable>::dataset() const
{
	DataSet res;
	return (res);
}

template<typename Engine, typename TDomain>
template<typename ... Args>
void _Particle<Engine, TDomain, IsTracable>::next_n_steps(size_t step,
		Real dt, Args && ... args)
{

	LOGGER << "Push probe particles   [ " << get_type_as_string() << "]"
			<< std::endl;

//	auto p = &*(this->begin());
//	for (auto & p : *this)
//	{
//		for (int s = 0; s < step; ++s)
//		{
//			engine_type::next_timestep(p + s, dt, std::forward<Args>(args)...);
//		}
//	}

	LOGGER << DONE;
}

template<typename Engine, typename TDomain>
template<typename ... Args>
void _Particle<Engine, TDomain, IsTracable>::next_step(Real dt,
		Args && ... args)
{

	LOGGER << "Push probe particles   [ " << get_type_as_string() << "]"
			<< std::endl;

//	auto p = &*(this->begin());
//	for (auto & p : *this)
//	{
//			engine_type::next_timestep(p + s, dt, std::forward<Args>(args)...);
//	}

	LOGGER << DONE;
}
template<typename Engine, typename TDomain>
using ParticleTrajectory=_Particle<Engine, TDomain, IsTracable>;
}  // namespace simpla

#endif /* CORE_PARTICLE_TRACABLE_PARTICLE_H_ */