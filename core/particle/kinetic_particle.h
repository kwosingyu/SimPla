/**
 * @file  kinetic_particle.h
 *
 * \date    2014年9月1日  下午2:25:26 
 * \author salmon
 */

#ifndef CORE_PARTICLE_KINETIC_PARTICLE_H_
#define CORE_PARTICLE_KINETIC_PARTICLE_H_

#include <iostream>
#include <memory>
#include <string>

#include "../physics/physical_object.h"
#include "../gtl/cache.h"
#include "../utilities/utilities.h"
namespace simpla
{

/**
 * @ingroup particle
 */
/**
 * @brief Kinetic Particle
 *  -  KineticParticle is a container of untracable particle  .
 *  -  KineticParticle can be sorted;
 *  -  KineticParticle is an unordered container;
 */
template<typename Engine, typename TDomain>
struct KineticParticle: public SpObject,
						public Engine,
						public enable_create_from_this<
								KineticParticle<Engine, TDomain>>
{

public:

	typedef Engine engine_type;

	typedef KineticParticle<engine_type, Others...> this_type;

	typedef typename engine_type::Point_s Point_s;

	template<typename ...Args>
	KineticParticle(domain_type const & pdomain, Args && ...);

	KineticParticle(this_type const&);

	~KineticParticle();

	using engine_type::properties;

	std::ostream& print(std::ostream & os) const
	{
		engine_type::print(os);
		return os;
	}

	template<typename TDict, typename ...Others>
	void load(TDict const & dict, Others && ...others);

	bool update();

	void sync();

	DataSet dataset() const;

	static std::string get_type_as_string_staic()
	{
		return engine_type::get_type_as_string();
	}
	std::string get_type_as_string() const
	{
		return get_type_as_string_staic();
	}

	//! @name   @ref splittable
	//! @{

	KineticParticle(this_type&, split);

	bool empty() const;

	bool is_divisible() const;

	//! @}
	/**
	 *
	 * @param args arguments
	 *
	 * - Semantics
	 @code
	 for( Point_s & point: all particle)
	 {
	 engine_type::next_timestep(& point,std::forward<Args>(args)... );
	 }
	 @endcode
	 *
	 */
	template<typename ...Args>
	void next_timestep(Args && ...args);

	/**
	 *  push_back and emplace will invalid data in the cache
	 * @param args
	 */

	template<typename TFun, typename ...Args>
	void foreach(TFun const & fun, Args && ...);

	template<typename ...Args>
	void push_back(Args && ...args)
	{
		if (cache_is_valid())
		{
			download_cache();
		}

		data_.push_back(std::forward<Args>(args)...);
		cache_is_valid_ = false;
	}

	template<typename ...Args>
	void emplac_back(Args && ...args)
	{
		if (cache_is_valid())
		{
			download_cache();
		}

		data_.emplac_back(std::forward<Args>(args)...);

		cache_is_valid_ = false;
	}

	template<typename TIterator>
	void insert(TIterator const & b, TIterator const &e)
	{
		if (!cache_is_valid())
		{
			upload_cache();
		}

		upload_to_cache(b, e);

	}

	void upload_cache();

	void download_cache();

	void sort();

private:

	domain_type domain_;

	std::vector<Point_s> data_;

};

template<typename Engine, typename TDomain>
template<typename ... Args>
KineticParticle<Engine, TDomain>::KineticParticle(domain_type const & pdomain,
		Args && ...args) :
		engine_type(std::forward<Args>(args)...), domain_(pdomain)
{

}

template<typename Engine, typename TDomain>
KineticParticle<Engine, TDomain>::~KineticParticle()
{
}
template<typename Engine, typename TDomain>
template<typename TDict, typename ...Others>
void KineticParticle<Engine, TDomain>::load(TDict const & dict,
		Others && ...others)
{
	engine_type::load(dict, std::forward<Others>(others)...);
}

template<typename Engine, typename TDomain>
bool KineticParticle<Engine, TDomain>::update()
{

	hash_fun_ = [& ](Point_s const & p)->id_type
	{
		return std::get<0>(domain_.manifold()->coordinates_global_to_local(
						std::get<0>(engine_type::pull_back(p))));
	};

	return true;
}

template<typename Engine, typename TDomain>
void KineticParticle<Engine, TDomain>::sync()
{
}

template<typename Engine, typename TDomain>
template<typename TFun, typename ...Args>
void KineticParticle<Engine, TDomain>::foreach(TFun const & fun,
		Args && ...args)
{
	if (!is_cached(std::forward<Args>(args)...))
	{
		foreach(fun, Cache<Args>(hint,args)...);
	}
	else
	{
		parallel_foreach(data_,

		[&](std::pair<id_type,point_index_s*> & item)
		{
			point_index_s p = item.second;
			id_type hint = item.first;
			while (p != nullptr)
			{
				fun(p->data, std::forward<Args>(args)...);
				p=p->next;
			}

		}

		);
	}
}

template<typename Engine, typename TDomain>
DataSet KineticParticle<Engine, TDomain>::dataset() const
{
	return DataSet();
}

template<typename Engine, typename TDomain, typename ...Others>
auto make_kinetic_particle(std::shared_ptr<TDomain> d,
		Others && ... others)
				DECL_RET_TYPE((std::make_shared<KineticParticle<Domain<TDomain,VERTEX>,Engine >>(
										Domain<TDomain,VERTEX>(d),std::forward<Others>(others)...)))

}  // namespace simpla

#endif /* CORE_PARTICLE_KINETIC_PARTICLE_H_ */
