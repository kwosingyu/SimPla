/*
 * field.h
 *
 *  Created on: 2013-7-19
 *      Author: salmon
 */

#ifndef FIELD_H_
#define FIELD_H_

#include "primitives.h"
#include "../utilities/log.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <utility>
#include <mutex>
namespace simpla
{
template<typename TG, int IFORM, typename TValue> struct Field;

/***
 *
 * @brief Field
 *
 * @ingroup Field Expression
 *
 */

template<typename TM, int IFORM, typename TValue>
struct Field
{
	std::mutex write_lock_;
public:

	typedef TM mesh_type;

	static constexpr unsigned int IForm = IFORM;

	typedef TValue value_type;

	typedef Field<mesh_type, IForm, value_type> this_type;

	static const int NDIMS = mesh_type::NDIMS;

	typedef typename mesh_type::coordinates_type coordinates_type;

	typedef typename mesh_type::index_type index_type;

	typedef std::shared_ptr<value_type> container_type;

	typedef typename std::conditional<(IForm == VERTEX || IForm == VOLUME),  //
	        value_type, nTuple<NDIMS, value_type> >::type field_value_type;

	container_type data_;

	mesh_type const &mesh;

	Field(mesh_type const &pmesh)
			: mesh(pmesh), data_(nullptr)
	{
	}

	Field(mesh_type const &pmesh, value_type d_value)
			: mesh(pmesh), data_(nullptr)
	{
		*this = d_value;
	}
	/**
	 *  Copy/clone Construct only copy mesh reference, but do not copy/move data, which is designed to
	 *  initializie stl containers, such as std::vector
	 *    \code
	 *       Field<...> default_value(mesh);
	 *       std::vector<Field<...> > v(4,default_value);
	 *    \endcode
	 *  the element in v have same mesh reference.
	 *
	 * @param rhs
	 */

	Field(this_type const & rhs)
			: mesh(rhs.mesh), data_(nullptr)
	{
	}

	/// Move Construct copy mesh, and move data,
	Field(this_type &&rhs)
			: mesh(rhs.mesh), data_(rhs.data_)
	{
	}

	~Field()
	{
	}

	void swap(this_type & rhs)
	{
		ASSERT(mesh == rhs.mesh);

		std::swap(data_, rhs.data_);
	}

	auto GetShape() const
	DECL_RET_TYPE(mesh.GetShape( IForm ))

	container_type & data()
	{
		return data_;
	}

	const container_type & data() const
	{
		return data_;
	}
	size_t size() const
	{
		return mesh.GetNumOfElements(IForm);
	}
	bool empty() const
	{
		return data_ == nullptr;
	}

	template<typename TC>
	struct iterator_
	{
		TC data_;

		typename mesh_type::iterator it_;

		typedef decltype(*data_) value_type;

		typedef iterator_<TC> this_type;

		iterator_(TC d, typename mesh_type::iterator s)
				: data_(d), it_(s)
		{

		}
		~iterator_()
		{
		}

		value_type & operator*()
		{
			return *(data_.get() + make_hash(it_));
		}
		value_type const& operator*() const
		{
			return *(data_.get() + make_hash(it_));
		}

		index_type * operator ->()
		{
			return (data_.get() + make_hash(it_));
		}
		index_type const* operator ->() const
		{
			return (data_.get() + make_hash(it_));
		}

		this_type & operator++()
		{
			++it_;

			return *this;
		}

		bool operator!=(this_type const &rhs) const
		{
			return it_ != rhs.it_ || data_ != rhs.data_;
		}
	};

	typedef iterator_<container_type> iterator;

	typedef iterator_<const container_type> const_iterator;

	iterator begin()
	{
		Update();
		return iterator_<container_type>(data_, mesh.begin(IForm));
	}

	iterator end()
	{
		Update();
		return iterator_<container_type>(data_, mesh.end(IForm));
	}

	const_iterator begin() const
	{
		return iterator_<const container_type>(data_, mesh.begin(IForm));
	}

	const_iterator end() const
	{
		return iterator_<const container_type>(data_, mesh.end(IForm));
	}

	inline value_type & operator[](index_type s)
	{
		return get(s);
	}

	inline value_type const & operator[](index_type s) const
	{
		return get(s);
	}

	inline value_type & at(index_type s)
	{
		return get(s);
	}

	inline value_type const & at(index_type s) const
	{
		return get(s);
	}

	inline value_type & get(index_type s)
	{
		return *(data_.get() + mesh.Hash(s));
	}

	inline value_type const & get(index_type s) const
	{
		return *(data_.get() + mesh.Hash(s));
	}
	void Init()
	{
		Update();
	}
	void Update()
	{
		if (data_ == nullptr)
		{
			data_ = mesh.template MakeContainer<IForm, value_type>();
		}

	}

	template<typename TD>
	void Fill(TD default_value)
	{
		Update();

		for (auto s : mesh.GetRegion(IForm))
		{
			this->get(s) = default_value;
		}
	}

	void Clear()
	{
		Fill(0);
	}

	this_type & operator =(value_type rhs)
	{
		Fill(rhs);
		return (*this);
	}
	this_type & operator =(this_type const & rhs)
	{
		Update();

		for (auto s : mesh.GetRegion(IForm))
		{
			this->get(s) = rhs.get(s);
		}
		return (*this);
	}
	template<typename TR>
	this_type & operator =(Field<mesh_type, IForm, TR> const & rhs)
	{
		Update();

		for (auto s : mesh.GetRegion(IForm))
		{
			this->get(s) = rhs.get(s);
		}

		return (*this);
	}

#define DECL_SELF_ASSIGN( _OP_ )                                                                   \
		template<typename TR> inline this_type &                                                   \
		operator _OP_##= (TR const & rhs)                                                          \
		{	Update(); *this = *this _OP_ rhs;                                                      \
			return (*this) ;                                                                        \
		}                                                                                          \


	DECL_SELF_ASSIGN(+ )

DECL_SELF_ASSIGN	(- )

	DECL_SELF_ASSIGN(* )

	DECL_SELF_ASSIGN(/ )
#undef DECL_SELF_ASSIGN

	inline field_value_type operator()(coordinates_type const &x) const
	{
		return mesh.Gather(*this,x);
	}

}
;

}
// namespace simpla

#endif /* FIELD_H_ */
