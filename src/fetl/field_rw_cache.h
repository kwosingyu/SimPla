/*
 * field_io_cache.h
 *
 *  Created on: 2013年10月31日
 *      Author: salmon
 */

#ifndef FIELD_IO_CACHE_H_
#define FIELD_IO_CACHE_H_

//#include <algorithm>
#include <vector>

#include "../utilities/log.h"
#include "../utilities/type_utilites.h"
#include "field.h"
#include "proxycache.h"

namespace simpla
{

template<typename TGeometry, typename TValue>
class Field<TGeometry, ProxyCache<const Field<TGeometry, TValue> > >
{

public:

	typedef TGeometry geometry_type;

	typedef const Field<TGeometry, TValue> field_type;

	typedef Field<TGeometry, ProxyCache<const Field<TGeometry, TValue> > > this_type;

	typedef typename geometry_type::mesh_type mesh_type;

	typedef typename mesh_type::index_type index_type;

	typedef typename mesh_type::coordinates_type coordinates_type;

	static const int IForm = geometry_type::IForm;

	typedef typename field_type::value_type value_type;

	typedef typename field_type::field_value_type field_value_type;

	mesh_type const &mesh;

private:
	field_type const & f_;

	index_type cell_idx_;

	int affect_region_;

	size_t num_of_points_;

	std::vector<index_type> points_;

	std::vector<value_type> cache_;

public:

//	Field(this_type const& r) :
//			mesh(r.mesh), f_(r.f_),
//
//			cell_idx_(r.cell_idx_), affect_region_(r.affect_region_),
//
//			num_of_points_(r.num_of_points_),
//
//			points_(r.points_), cache_(r.cache_)
//
//	{
//	}
//
//	Field(this_type && r) :
//			mesh(r.mesh), f_(r.f_),
//
//			cell_idx_(r.cell_idx_), affect_region_(r.affect_region_),
//
//			num_of_points_(r.num_of_points_),
//
//			points_(r.points_), cache_(r.cache_)
//
//	{
//	}

//	Field(this_type && r) = delete;
//	Field(this_type const& r) = delete;

	Field(field_type const & f, index_type const &s, int affect_region = 1) :
			mesh(f.mesh), f_(f),

			cell_idx_(s), affect_region_(affect_region),

			num_of_points_(mesh.GetAffectedPoints(Int2Type<IForm>(), cell_idx_, nullptr, affect_region_)),

			points_(num_of_points_), cache_(num_of_points_)

	{
		mesh.GetAffectedPoints(Int2Type<IForm>(), cell_idx_, &points_[0], affect_region_);

//		for (size_t i = 0; i < num_of_points_; ++i)
//		{
//			cache_[i] = f_[points_[i]];
//		}
	}

	~Field()
	{
	}

	inline field_value_type operator()(coordinates_type const &x) const
	{
		coordinates_type pcoords;

		index_type idx = mesh.SearchCell(cell_idx_, x, &(pcoords[0]));

		field_value_type res;

		if (idx == cell_idx_)
		{
			mesh.GatherFromMesh(Int2Type<IForm>(), &pcoords[0], &cache_[0], &res, affect_region_);
		}
		else
		{
			res = f_(idx, &pcoords[0]);
		}

		return res;

	}

};

template<typename TGeometry, typename TValue>
class Field<TGeometry, ProxyCache<Field<TGeometry, TValue> *> >
{

public:

	typedef TGeometry geometry_type;

	typedef Field<TGeometry, TValue> field_type;

	typedef Field<geometry_type, ProxyCache<field_type> > this_type;

	typedef typename geometry_type::mesh_type mesh_type;

	typedef typename mesh_type::index_type index_type;

	typedef typename mesh_type::coordinates_type coordinates_type;

	static const int IForm = geometry_type::IForm;

	typedef typename field_type::value_type value_type;

	typedef typename field_type::field_value_type field_value_type;

	mesh_type const &mesh;
private:
	field_type * f_;

	index_type cell_idx_;

	int affect_region_;

	const size_t num_of_points_;

	std::vector<index_type> points_;

	std::vector<value_type> cache_;

public:

//	Field(this_type && r) :
//			mesh(r.mesh), f_(r.f_),
//
//			cell_idx_(r.cell_idx_), affect_region_(r.affect_region_),
//
//			num_of_points_(r.num_of_points_),
//
//			points_(r.points_), cache_(r.cache_)
//
//	{
//	}
//	Field(this_type const& r) :
//			mesh(r.mesh), f_(r.f_),
//
//			cell_idx_(r.cell_idx_), affect_region_(r.affect_region_),
//
//			num_of_points_(r.num_of_points_),
//
//			points_(r.points_), cache_(r.cache_)
//
//	{
//	}

//	Field(this_type && r) = delete;
//	Field(this_type const& r) = delete;

	Field(field_type * f, index_type const &s, int affect_region = 1) :
			mesh(f->mesh), f_(f),

			cell_idx_(s), affect_region_(affect_region),

			num_of_points_(mesh.GetAffectedPoints(Int2Type<IForm>(), cell_idx_, nullptr, affect_region_)),

			points_(num_of_points_), cache_(num_of_points_)

	{
		mesh.GetAffectedPoints(Int2Type<IForm>(), cell_idx_, &points_[0], affect_region_);

//		value_type zero_value_;
//
//		zero_value_ *= 0;
//
//		std::fill(cache_.begin(), cache_.end(), zero_value_);
	}

	~Field()
	{
		f_->Collect(points_, cache_);
	}

	template<typename TV>
	inline void Collect(TV const &v, coordinates_type const &x) const
	{
//		coordinates_type pcoords;
//
//		index_type idx = mesh.SearchCell(cell_idx_, x, &pcoords[0]);
//
//		if (idx == cell_idx_)
//		{
//			field_value_type vv;
//			vv = v;
//			mesh.ScatterToMesh(Int2Type<IForm>(), &pcoords[0], vv, &cache_[0], affect_region_);
//		}
//		else
//		{
//			f_->Collect(v, idx, &pcoords[0], affect_region_);
//		}
	}

};

template<typename TGeometry, typename TValue>
struct ProxyCache<const Field<TGeometry, TValue> >
{
	typedef Field<TGeometry, TValue> src_type;

	typedef Field<TGeometry, ProxyCache<const src_type> > type;

	template<typename ...TI>
	static inline type Eval(src_type const& f, TI ... hint_idx)
	{
		return std::move(type(f, hint_idx...));
	}

};

template<typename TGeometry, typename TValue>
struct ProxyCache<Field<TGeometry, TValue>*>			// 0 1 0

{
	typedef Field<TGeometry, TValue> src_type;

	typedef Field<TGeometry, ProxyCache<src_type*> > type;

	template<typename ... TI>
	static inline type Eval(src_type* f, TI ... hint_idx)
	{
		return std::move(type(f, hint_idx...));
	}
};

}  // namespace simpla

#endif /* FIELD_IO_CACHE_H_ */
