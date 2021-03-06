/**
 * @file field_continue.h
 *
 * @date 2015-1-30
 * @author  salmon
 */

#ifndef COREFieldField_CONTINUE_H_
#define COREFieldField_CONTINUE_H_

#include <stddef.h>
#include <cstdbool>
#include <memory>
#include <string>
#include <type_traits>

#include "../../gtl/check_concept.h"
#include "../application/sp_object.h"
#include "../gtl/expression_template.h"

namespace simpla
{
template<typename ...>struct Field;

/** @ingroup field
 *  @brief field using  sequence container,i.e.  'vector'
 */
template<typename TM, typename TContainer>
struct Field<TM, TContainer, _impl::is_sequence_container> : public SpObject
{

public:
	typedef TM mesh_type;

	typedef typename mesh_type::id_type id_type;
	typedef typename mesh_type::coordinate_tuple coordinate_tuple;

	typedef TContainer container_type;
	typedef typename container_type::value_type value_type;

	typedef Field<mesh_type, container_type, _impl::is_sequence_container> this_type;

private:

	mesh_type mesh_;

	std::shared_ptr<container_type> data_;

public:
	template<typename ...Args>
	Field(mesh_type const & d, Args && ...args)
			: mesh_(d), data_(
					std::make_shared<container_type>(
							std::forward<Args>(args...))

							)
	{
	}
	Field(this_type const & that)
			: mesh_(that.mesh_), data_(that.data_)
	{
	}
	~Field()
	{
	}

	std::string get_type_as_string() const
	{
		return "field<" + mesh_.get_type_as_string() + ">";
	}
	mesh_type const & get_mesh() const
	{
		return mesh_;
	}
	template<typename TU> using cloneField_type=
	Field<TM,typename replace_template_type<0,TU,container_type>::type,
	_impl::is_sequence_container>;

	template<typename TU>
	cloneField_type<TU> clone() const
	{
		return cloneField_type<TU>(mesh_);
	}
	/** @name entity_id_range concept
	 * @{
	 */

	template<typename ...Args>
	Field(this_type & that, Args && ...args)
			: mesh_(that.mesh_, std::forward<Args>(args)...), data_(that.data_)
	{
	}
	bool empty() const
	{
		return mesh_.empty();
	}
	bool is_divisible() const
	{
		return mesh_.is_divisible();
	}

	/**@}*/

	/**
	 * @name assignment
	 * @{
	 */

	inline Field<AssignmentExpression<_impl::_assign, this_type, this_type>> operator =(
			this_type const &that)
	{
		allocate();
		return std::move(
				Field<
						AssignmentExpression<_impl::_assign, this_type,
								this_type>>(*this, that));
	}

	template<typename TR>
	inline Field<AssignmentExpression<_impl::_assign, this_type, TR>> operator =(
			TR const &that)
	{
		allocate();
		return std::move(
				Field<AssignmentExpression<_impl::_assign, this_type, TR>>(
						*this, that));
	}

	template<typename TFun> void pull_back(TFun const &fun)
	{
		allocate();
		mesh_.pull_back(*this, fun);
	}

	/** @} */

	/** @name access
	 *  @{*/

	typedef typename mesh_type::template field_value_type<value_type> field_value_type;

	field_value_type gather(coordinate_tuple const& x) const
	{
		return std::move(mesh_.gather(*this, x));
	}

	template<typename ...Args>
	void scatter(Args && ... args)
	{
		mesh_.scatter(*this, std::forward<Args>(args)...);
	}

	/** @} */

//	dataset dump_data() const
//	{
//		return dataset();
//	}
private:
	void allocate()
	{
		size_t num = mesh_.max_hash();
		if (data_ == nullptr || data_->size() < num)
		{
			data_ = std::make_shared<container_type>(num);
		}
	}

public:
	value_type & operator[](id_type const & s)
	{
		return data_.get()[mesh_.hash(s)];
	}
	value_type const & operator[](id_type const & s) const
	{
		return data_.get()[mesh_.hash(s)];
	}
	void clear()
	{
		allocate();
		*this = 0;
	}

//private:
//	HAS_CONST_MEMBER_FUNCTION(hash);
//
//	HAS_CONST_MEMBER_FUNCTION(max_hash);
//
//	static_assert(has_const_member_function_hash<TM,typename TM::id_type>::value &&
//			has_const_member_function_max_hash<TM>::value,
//			"get_mesh do not support 'sequence container'"
//	);
};

}
// namespace simpla

#endif /* COREFieldField_CONTINUE_H_ */
