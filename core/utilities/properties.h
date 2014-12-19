/**
 * \file properties.h
 *
 * \date    2014年7月13日  上午7:27:37 
 * \author salmon
 */

#ifndef PROPERTIES_H_
#define PROPERTIES_H_
#include <map>
#include <string>
#include "ntuple.h"
#include "any.h"
namespace simpla
{
/**
 *   Properties Tree
 *  @todo using shared_ptr storage data
 */
class Properties: public Any, public std::map<std::string, Properties>
{

private:

	typedef Properties this_type;

	typedef Any value_type;
	typedef std::string key_type;
	typedef std::map<key_type, this_type> map_type;

	static const Properties fail_safe_;
public:
	Properties()
	{
	}
	template<typename T>
	Properties(T const &v) :
			value_type(v)
	{
	}
	~Properties()
	{
	}

	using value_type::operator=;

	inline bool empty() const // STL style
	{
		return value_type::empty() && map_type::empty();
	}
	inline bool IsNull() const
	{
		return empty();
	}
	operator bool() const
	{
		return !empty();
	}

	Properties & get(std::string const & key)
	{
		if (key == "")
		{
			return *this;
		}
		else
		{
			return map_type::operator[](key);
		}
	}

	Properties const &get(std::string const & key) const
	{
		auto it = map_type::find(key);
		if (it == map_type::end())
		{
			return *this;
		}
		else
		{
			return it->second;
		}
	}

	template<typename T>
	auto as()
			DECL_RET_TYPE((value_type::template as<typename array_to_ntuple_convert<T>::type>()))

	template<typename T>
	auto as() const
			DECL_RET_TYPE((value_type::template as<typename array_to_ntuple_convert<T>::type>()))

	template<typename T>
	typename array_to_ntuple_convert<T>::type as(T const & default_v) const
	{
		typename array_to_ntuple_convert<T>::type res = default_v;
		if (!value_type::empty())
		{
			res = value_type::template as<
					typename array_to_ntuple_convert<T>::type>();
		}

		return std::move(res);
	}

	template<typename T>
	typename array_to_ntuple_convert<T>::type get(std::string const & key,
			T const & default_v) const
	{
		typename array_to_ntuple_convert<T>::type res = default_v;

		auto it = map_type::find(key);

		if (it != map_type::end() && it->second.is_same<T>())
		{
			res = it->second.template as<T>();
		}
		return std::move(res);
	}

	template<typename T>
	void set(std::string const & key, T v)
	{
		get(key) = v;
	}
	template<typename T>
	void operator()(std::string const & key, T && v)
	{
		set(key, std::forward<T>(v));
	}
	inline Properties & operator[](key_type const & key)
	{
		return get(key);
	}
	inline Properties & operator[](const char key[])
	{
		return get(key);
	}

	Properties const & operator()(std::string const & key) const
	{
		return get(key);
	}
	Properties & operator()(std::string const & key)
	{
		return get(key);
	}

//	Any & value()
//	{
//		return value_;
//	}
//	Any const& value() const
//	{
//		return value_;
//	}
	inline Properties const& operator[](key_type const & key) const
	{
		return get(key);
	}
	inline Properties const& operator[](const char key[]) const
	{
		return get(key);
	}

	template<typename OS>
	OS &print(OS & os) const
	{
		for (auto const& item : *this)
		{

			os << item.first << " =  ";
			item.second.value_type::print(os);
			os << " , ";
			if (item.second.size() > 0)
			{
				item.second.print(os);
			}

		}
		return os;
	}

//	template<typename T>
//	bool set(std::string const & key, T && v)
//	{
//		auto it = base_type::find(key);
//		if (it == base_type::end())
//		{
//			base_type::emplace(key, Any(v));
//		}
//		else if (it->second.value_.Is<T>())
//		{
//			it->second.value_.AnyCast<T>() = v;
//		}
//		else
//		{
//			ERROR("try to assign a value with incompatible type ");
//		}
//	}
};
}  // namespace simpla

#endif /* PROPERTIES_H_ */
