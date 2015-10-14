/*
 * field_io_cache.h
 *
 *  created on: 2013-10-31
 *      Author: salmon
 */

#ifndef FIELD_IO_CACHE_H_
#define FIELD_IO_CACHE_H_

#include <vector>

#include "../utilities/log.h"
#include "../utilities/pretty_stream.h"
#include "../utilities/type_traits.h"
#include "field.h"
#include "cache.h"

namespace simpla
{
template<typename ...> class Field;

template<typename TM, typename TContainer>
class Field<TM, Cache<const Field<TM, TContainer> > >
{

public:

	typedef Field<TM, TContainer> field_type;

	typedef Field<TM, Cache<const Field<TM, TContainer> > > this_type;

	typedef TM mesh_type;

	typedef typename mesh_type::iterator iterator;

	typedef typename mesh_type::coordinate_tuple coordinate_tuple;

	static const unsigned int IForm = TM::iform;

	typedef typename field_type::value_type value_type;

	typedef typename field_type::field_value_type field_value_type;

	mesh_type const &mesh;

	field_value_type mean_;

private:
	field_type const & f_;

	iterator cell_idx_;

	int affect_Range_;

	size_t num_of_points_;

	std::vector<iterator> points_;

	std::vector<value_type> cache_;

public:

	Field(this_type const& r) :
			mesh(r.mesh), f_(r.f_),

			cell_idx_(r.cell_idx_), affect_Range_(r.affect_Range_),

			num_of_points_(r.num_of_points_),

			points_(r.points_), cache_(r.cache_)

	{
	}

	Field(this_type && r) :
			mesh(r.mesh), f_(r.f_),

			cell_idx_(r.cell_idx_), affect_Range_(r.affect_Range_),

			num_of_points_(r.num_of_points_),

			points_(r.points_), cache_(r.cache_)

	{
	}

	Field(field_type const & f, iterator const &s, unsigned int affect_Range =
			2) :
			mesh(f.mesh), f_(f), cell_idx_(s), affect_Range_(affect_Range), num_of_points_(
					0)
	{
	}

	~Field()
	{
	}

	void RefreshCache(size_t s)
	{
//		cell_idx_ = s;
//		num_of_points_ = (manifold.GetAffectedPoints(std::integral_constant<unsigned int ,IForm>(), cell_idx_,
//				nullptr, affect_Range_));
//		if (num_of_points_ == 0)
//		{
//			WARNING << "Empty Cache!";
//			return;
//		}
//		points_.resize(num_of_points_);
//		cache_.resize(num_of_points_);
//
//		manifold.GetAffectedPoints(std::integral_constant<unsigned int ,IForm>(), cell_idx_, &points_[0],
//				affect_Range_);
//
//		for (size_t i = 0; i < num_of_points_; ++i)
//		{
//			cache_[i] = f_[points_[i]];
//		}
//
//		updateMeanValue(std::integral_constant<unsigned int ,IForm>());
	}

private:
	void updateMeanValue(std::integral_constant<unsigned int, 0>)
	{
		mesh.template GetMeanValue<IForm>(&cache_[0], &mean_, affect_Range_);
	}
	void updateMeanValue(std::integral_constant<unsigned int, 3>)
	{
		mesh.template GetMeanValue<IForm>(&cache_[0], &mean_, affect_Range_);
	}
	void updateMeanValue(std::integral_constant<unsigned int, 1>)
	{
		mesh.template GetMeanValue<IForm>(&cache_[0], &mean_[0], affect_Range_);
	}
	void updateMeanValue(std::integral_constant<unsigned int, 2>)
	{
		mesh.template GetMeanValue<IForm>(&cache_[0], &mean_[0], affect_Range_);
	}
public:

	template<typename TI>
	inline value_type get(TI s) const
	{
		return f_.get(s);
	}
	template<typename TI>
	inline value_type& get(TI s)
	{
		return f_.get(s);
	}

	template<typename TI>
	inline value_type operator[](TI s) const
	{
		return f_.get(s);
	}
	template<typename TI>
	inline value_type& operator[](TI s)
	{
		return f_.get(s);
	}

	template<typename TI>
	inline value_type at(TI s) const
	{
		return f_.get(s);
	}
	template<typename TI>
	inline value_type& at(TI s)
	{
		return f_.get(s);
	}
	inline field_value_type operator()(coordinate_tuple const &x) const
	{
//		coordinate_tuple pcoords;
//
//		iterator idx = manifold.SearchCell(cell_idx_, x, &(pcoords[0]));
//
//		field_value_type res;
//
//		if (idx == cell_idx_)
//		{
//			manifold.template Gather(std::integral_constant<unsigned int ,IForm>(), &pcoords[0], &cache_[0],
//					&res, affect_Range_);
//		}
//		else //failsafe
//		{
//			res = f_(idx, &pcoords[0]);
//		}
//		return res;

		return f_(x);

	}

	inline field_value_type const & mean(coordinate_tuple const &) const
	{
		return mean_;
	}

}
;

template<typename TM, typename TContainer>
class Field<TM, Cache<Field<TM, TContainer> *> >
{

public:

	typedef Field<TM, TContainer> field_type;

	typedef Field<TM, Cache<field_type> > this_type;

	typedef TM mesh_type;

	typedef typename mesh_type::iterator iterator;

	typedef typename mesh_type::coordinate_tuple coordinate_tuple;

	static const unsigned int IForm = TM::iform;

	typedef typename field_type::value_type value_type;

	typedef typename field_type::field_value_type field_value_type;

	mesh_type const &mesh;
private:
	field_type * f_;

	iterator cell_idx_;

	int affect_Range_;

	size_t num_of_points_;

	std::vector<iterator> points_;

	std::vector<value_type> cache_;

	bool is_fresh_;

public:

	Field(this_type && r) :
			mesh(r.mesh), f_(r.f_),

			cell_idx_(r.cell_idx_), affect_Range_(r.affect_Range_),

			num_of_points_(r.num_of_points_),

			points_(r.points_), cache_(r.cache_), is_fresh_(r.is_fresh_)
	{
	}
	Field(this_type const& r) :
			mesh(r.mesh), f_(r.f_),

			cell_idx_(r.cell_idx_), affect_Range_(r.affect_Range_),

			num_of_points_(r.num_of_points_),

			points_(r.points_), cache_(r.cache_), is_fresh_(r.is_fresh_)

	{
	}

	Field(field_type * f, unsigned int affect_Range = 2) :
			mesh(f->mesh), f_(f), affect_Range_(affect_Range), num_of_points_(
					0), is_fresh_(false)
	{
	}

	~Field()
	{
		FlushCache();
	}

	void FlushCache()
	{
		f_->unlock();
		if (num_of_points_ > 0 && !is_fresh_)
		{
//			f_->Collect(num_of_points_, &points_[0], &cache_[0]);
			is_fresh_ = true;
		}
	}

	template<typename TI>
	inline value_type get(TI s) const
	{
		return f_->get(s);
	}

	template<typename TI>
	inline value_type& get(TI s)
	{
		return f_->get(s);
	}

	template<typename TI>
	inline value_type at(TI s) const
	{
		return f_->get(s);
	}

	template<typename TI>
	inline value_type& at(TI s)
	{
		return f_->get(s);
	}

	template<typename TI>
	inline value_type operator[](TI s) const
	{
		return f_->get(s);
	}

	template<typename TI>
	inline value_type& operator[](TI s)
	{
		return f_->get(s);
	}
	void RefreshCache(size_t s)
	{
//		cell_idx_ = s;
//
//		num_of_points_ = (manifold.GetAffectedPoints(std::integral_constant<unsigned int ,IForm>(), cell_idx_,
//				nullptr, affect_Range_));
//
//		if (num_of_points_ == 0)
//		{
//			WARNING << "Empty Cache!";
//			return;
//		}
//		points_.resize(num_of_points_);
//		cache_.resize(num_of_points_);
//
//		manifold.GetAffectedPoints(std::integral_constant<unsigned int ,IForm>(), cell_idx_, &points_[0],
//				affect_Range_);
//
//		value_type zero_value_;
//
//		zero_value_ *= 0;
//
//		std::fill(cache_.begin(), cache_.end(), zero_value_);

		f_->lock();
		is_fresh_ = false;
	}

	template<typename TV>
	inline void Collect(TV const &v, coordinate_tuple const &x)
	{
//		coordinate_tuple pcoords;
//
//		iterator idx = manifold.SearchCell(cell_idx_, x, &pcoords[0]);
//
//		if (idx == cell_idx_)
//		{
//			field_value_type vv;
//			vv = v;
//			manifold.Scatter(std::integral_constant<unsigned int ,IForm>(), &pcoords[0], vv, &cache_[0],
//					affect_Range_);
//		}
//		else //failsafe
//		{
//			f_->Collect(v, idx, &pcoords[0], affect_Range_);
//		}
	}

};

template<typename TM, typename TContainer>
struct Cache<const Field<TM, TContainer> >
{

	typedef Field<TM, Cache<const Field<TM, TContainer> > > type;

	typedef typename Field<TM, TContainer>::value_type value_type;

	template<typename ... Args>
	Cache(Field<TM, TContainer> const & f, Args && ... args) :
			f_(f, std::forward<Args >(args)...)
	{
		VERBOSE << "Field read cache applied!";
	}

	type & operator*()
	{
		return f_;
	}

	type const & operator*() const
	{
		return f_;
	}
private:
	type f_;
};

template<typename TM, typename TContainer>
struct Cache<Field<TM, TContainer>*>
{
	typedef Cache<Field<TM, TContainer>*> this_type;

	typedef Field<TM, this_type> type;

	typedef typename Field<TM, TContainer>::value_type value_type;

	template<typename ... Args>
	Cache(Field<TM, TContainer>* f, Args && ... args) :
			f_(f, std::forward<Args >(args)...)
	{
		VERBOSE << "Field write cache applied!";
	}

	type * operator*()
	{
		return &f_;
	}
private:
	type f_;
};

template<typename TM, typename TF>
void RefreshCache(size_t s, Field<TM, Cache<TF>> & f)
{
	f.RefreshCache(s);
}

template<typename TM, typename TF>
void FlushCache(Field<TM, Cache<TF*>> & f)
{
	f.FlushCache();
}

}  // namespace simpla

#endif /* FIELD_IO_CACHE_H_ */
