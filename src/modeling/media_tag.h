/*
 * media_tag.h
 *
 *  Created on: 2013年12月15日
 *      Author: salmon
 */

#ifndef MEDIA_TAG_H_
#define MEDIA_TAG_H_

#include <algorithm>
#include <bitset>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../fetl/primitives.h"
#include "../utilities/log.h"
#include "../utilities/pretty_stream.h"
#include "pointinpolygen.h"
#include "select.h"
namespace simpla
{

template<typename TM>
class MediaTag
{

public:
	static constexpr int MAX_NUM_OF_MEIDA_TYPE = 64;
	typedef TM mesh_type;

	typedef std::bitset<MAX_NUM_OF_MEIDA_TYPE> tag_type;
	typedef typename mesh_type::index_type index_type;
	typedef typename mesh_type::coordinates_type coordinates_type;
private:

	const tag_type null_tag;

	mesh_type const &mesh;
	std::vector<tag_type> tags_[mesh_type::NUM_OF_COMPONENT_TYPE];
	std::map<std::string, tag_type> register_tag_;
	unsigned int max_tag_;
public:

	enum
	{
		NONE = 0, VACUUM = 1, PLASMA, CORE, BOUNDARY, PLATEAU, LIMTER,
		// @NOTE: add tags for different physical area or media
		CUSTOM = 20
	};

	MediaTag(mesh_type const & m)
			: null_tag(1 << NONE), mesh(m), max_tag_(CUSTOM + 1)
	{
		register_tag_.emplace("NONE", null_tag);

		register_tag_.emplace("Vacuum", tag_type(1 << VACUUM));
		register_tag_.emplace("Plasma", tag_type(1 << PLASMA));
		register_tag_.emplace("Core", tag_type(1 << CORE));
		register_tag_.emplace("Boundary", tag_type(1 << BOUNDARY));
		register_tag_.emplace("Plateau", tag_type(1 << PLATEAU));
		register_tag_.emplace("Limter", tag_type(1 << LIMTER));

	}
	~MediaTag()
	{
	}

	bool empty() const
	{
		return tags_[VERTEX].empty();
	}

	operator bool() const
	{
		return tags_[VERTEX].empty();
	}

	tag_type RegisterTag(std::string const & name)
	{
		tag_type res;
		if (register_tag_.find(name) != register_tag_.end())
		{
			res = register_tag_[name];
		}
		else if (max_tag_ < MAX_NUM_OF_MEIDA_TYPE)
		{
			res.set(max_tag_);
			++max_tag_;
		}
		else
		{
			ERROR << "Too much media Type";
		}
		return res;
	}

	unsigned int GetNumMediaType() const
	{
		return max_tag_;
	}
	tag_type GetTagFromNumber(unsigned int tag_pos) const
	{
		tag_type res;
		res.set(tag_pos);
		return std::move(res);
	}
	tag_type GetTagFromString(std::string const &name) const
	{
		return std::move(register_tag_.at(name));
	}
	tag_type GetTagFromString(std::string const &name)
	{
		return std::move(RegisterTag(name));
	}
	tag_type operator[](std::string const &name)
	{
		auto it = register_tag_.find(name);
		if (it != register_tag_.end())
		{
			RegisterTag(name);
		}

		return std::move(register_tag_.at(name));
	}

	void ClearAll()
	{
		for (auto &v : tags_[0])
		{
			v.reset();
		}

		Update();
	}

	template<typename TCfg>
	void Load(TCfg const & cfg)
	{
		if (cfg)
		{
			for (auto const & p : cfg)
			{
				Modify(p.second);
			}
		}

	}
	std::ostream & Save(std::ostream &os) const
	{

		os << "{ \n" << "\t -- register media type\n";

		for (auto const& p : register_tag_)
		{
			os << std::setw(10) << p.first << " = 0x" << std::hex << p.second.to_ulong() << std::dec << ", \n";
		}

//		<< Data(&tmp[0][0], "tag0", mesh.GetShape(0)) << ","
//
//		<< Data(&tmp[1][0], "tag1", mesh.GetShape(1)) << ","
//
//		<< Data(&tmp[2][0], "tag2", mesh.GetShape(2)) << ","
//
//		<< Data(&tmp[3][0], "tag3", mesh.GetShape(3)) << ",";

		os << " }\n"

		;
		return os;
	}

	void Init(int I = VERTEX)
	{
		if (tags_[I].empty())
		{
			tags_[I].resize(mesh.GetNumOfElements(I), null_tag);
		}
	}

	template<typename TCmd>
	void Modify(TCmd const& cmd)
	{
		std::string op = "";
		std::string type = "";

		cmd["Op"].template as<std::string>(&op);
		cmd["Type"].template as<std::string>(&type);

		if (type == "")
		{
			WARNING << "Illegal input! [ undefine type ]";
			return;
		}

		auto select = cmd["Select"];
		if (select.empty())
		{
			std::vector<coordinates_type> region;

			cmd["Region"].as(&region);

			if (op == "Set")
			{
				Set(type, region);
			}
			else if (op == "Remove")
			{
				Remove(type, region);
			}
			else if (op == "Add")
			{
				Add(type, region);
			}
		}
		else
		{
			if (op == "Set")
			{
				Set(type, select);
			}
			else if (op == "Remove")
			{
				Remove(type, select);
			}
			else if (op == "Add")
			{
				Add(type, select);
			}
		}

		LOGGER << op << " media " << type << DONE;
	}

	template<typename ...Args> inline
	void Set(std::string media_tag, Args const & ... args)
	{
		Set(GetTagFromString(media_tag), std::forward<Args const &>(args)...);
	}
	template<typename ...Args> inline
	void Set(unsigned int media_tag, Args const & ... args)
	{
		Set(GetTagFromNumber(media_tag), std::forward<Args const &>(args)...);
	}

	template<typename ...Args> inline
	void Add(std::string media_tag, Args const & ... args)
	{
		Add(GetTagFromString(media_tag), std::forward<Args const &>(args)...);
	}
	template<typename ...Args> inline
	void Add(unsigned int media_tag, Args const & ... args)
	{
		Add(GetTagFromNumber(media_tag), std::forward<Args const &>(args)...);
	}

	template<typename ...Args> inline
	void Remove(std::string media_tag, Args const & ... args)
	{
		Set(GetTagFromString(media_tag), std::forward<Args const &>(args)...);
	}
	template<typename ...Args> inline
	void Remove(unsigned int media_tag, Args const & ... args)
	{
		Set(GetTagFromNumber(media_tag), std::forward<Args const &>(args)...);
	}

	/**
	 * Set media tag on vertics
	 * @param tag media tag is  set to 1<<tag
	 * @param args args are trans-forward to
	 *      SelectVerticsInRegion(<lambda function>,mesh,args)
	 */
	template<typename ...Args>
	void Set(tag_type media_tag, Args const & ... args)
	{
		_ForEachVertics([&]( tag_type &v)
		{	v=media_tag;},

		std::forward<Args const &>(args)...);
	}

	template<typename ...Args>
	void Add(tag_type media_tag, Args const & ... args)
	{

		_ForEachVertics([&]( tag_type &v)
		{	v|=media_tag;},

		std::forward<Args const &>(args)...);
	}

	template<typename ...Args>
	void Remove(tag_type media_tag, Args const & ... args)
	{

		_ForEachVertics([&]( tag_type &v)
		{	v^=media_tag;},

		std::forward<Args const &>(args)...);
	}

	/**
	 *  Update media tag on edge ,face and cell, base on media tag on vertics
	 */
	void Update()
	{
		_UpdateTags<EDGE>();
		_UpdateTags<FACE>();
		_UpdateTags<VOLUME>();
	}

	typedef std::function<void(index_type const &, coordinates_type const &)> Fun;
	/**
	 *  Choice elements that most close to and out of the interface,
	 *  No element cross interface.
	 * @param
	 * @param fun
	 * @param in_tag
	 * @param out_tag
	 * @param flag
	 */
	template<int IFORM>
	void SelectBoundary(Fun const &fun, tag_type in, tag_type out) const;

	template<int IFORM>
	void SelectElements(Fun const &fun, tag_type tag) const;

	template<int IFORM, typename TDict>
	void Select(Fun const &fun, TDict const & dict) const;

private:

	/**
	 * Set media tag on vertics
	 * @param tag media tag is  set to 1<<tag
	 * @param args args are trans-forward to
	 *      SelectVerticsInRegion(<lambda function>,mesh,args)
	 */
	template<typename ...Args>
	void _ForEachVertics(std::function<void(tag_type&)> fun, Args const & ... args)
	{
		Init();

		SelectFromMesh<VERTEX>(mesh, [&]( index_type const &s,coordinates_type const & )
		{	fun( tags_[VERTEX][s]);}, std::forward<Args const&>(args)...);
	}

	template<int I>
	void _UpdateTags()
	{
		Init(I);

		mesh.ParallelTraversal(I,

		[&](index_type const & s )
		{
			index_type v[mesh_type::MAX_NUM_VERTEX_PER_CEL];

			int n=mesh.template GetAdjacentCells(Int2Type<I>(),Int2Type<VERTEX>(),s,v);
			tag_type flag = null_tag;
			for(int i=0;i<n;++i)
			{
				flag|=tags_[VERTEX][v[i]];
			}
			tags_[I][s]=flag;

		});
	}
};
template<typename TM>
inline std::ostream & operator<<(std::ostream & os, MediaTag<TM> const &self)
{
	return self.Save(os);
}

template<typename TM> template<int IFORM>
void MediaTag<TM>::SelectBoundary(Fun const &fun, tag_type in, tag_type out) const
{

	// Good
	//  +----------#----------+
	//  |          #          |
	//  |    A     #-> B   C  |
	//  |          #          |
	//  +----------#----------+
	//
	//  +--------------------+
	//  |         ^          |
	//  |       B |     C    |
	//  |     ########       |
	//  |     #      #       |
	//  |     #  A   #       |
	//  |     #      #       |
	//  |     ########       |
	//  +--------------------+
	//
	//             +----------+
	//             |      C   |
	//  +----------######     |
	//  |          | A  #     |
	//  |    A     | &  #  B  |
	//  |          | B  #->   |
	//  +----------######     |
	//             |          |
	//             +----------+
	//
	//     	       +----------+
	//       C     |          |
	//  +----------#----+     |
	//  |          # A  |     |
	//  |    B   <-# &  |  A  |
	//  |          # B  |     |
	//  +----------#----+     |
	//             |          |
	//             +----------+
	//
	//
	// 	 Bad
	//
	//  +--------------------+
	//  |                    |
	//  |        A           |
	//  |     ########       |
	//  |     #      #       |
	//  |     #->B C #       |
	//  |     #      #       |
	//  |     ########       |
	//  +--------------------+
	//
	// 	            +----------+
	//              |          |
	//   +-------+  |          |
	//   |       |  |          |
	//   |   B   |  |    A     |
	//   |       |  |          |
	//   +-------+  |          |
	//              |          |
	//              +----------+

	mesh.SerialTraversal(IFORM,

	[&]( index_type const&s , coordinates_type const &x)
	{

		if((this->tags_[IFORM].at(s)&in).none() && (this->tags_[IFORM].at(s)&out).any() )
		{
			index_type neighbours[mesh_type::MAX_NUM_NEIGHBOUR_ELEMENT];

			int num=this->mesh.GetAdjacentCells(Int2Type<IFORM>(),Int2Type<VOLUME>(),s,neighbours );

			for(int i=0;i<num;++i)
			{

				if(((this->tags_[VOLUME].at(neighbours[i])&in) ).any())
				{
					fun( s,x );
					break;
				}
			}
		}

	});

}

template<typename TM>
template<int IFORM>
void MediaTag<TM>::SelectElements(Fun const &fun, tag_type tag) const
{

	auto const & tags = tags_[VOLUME];
	mesh.SerialTraversal(IFORM,

	[&]( index_type const&s , coordinates_type const &x)
	{
		if(((this->tags_[IFORM].at(s)&tag) ).any())
		{
			fun( s,x );
		}
	});
}

template<typename TM>
template<int IFORM, typename TDict>
void MediaTag<TM>::Select(Fun const &fun, TDict const & dict) const
{

	if (dict["Type"])
	{
		auto type = dict["Type"].template as<std::string>("");

		if (type == "Boundary")
		{
			auto tag = GetTagFromString(dict["Tag"].template as<std::string>());
			SelectBoundary<IFORM>(fun, tag, null_tag);

		}
		else if (type == "Interface")
		{
			auto in = GetTagFromString(dict["In"].template as<std::string>());
			auto out = GetTagFromString(dict["Out"].template as<std::string>());
			SelectBoundary<IFORM>(fun, in, out);
		}
		else if (type == "Element")
		{
			auto tag = GetTagFromString(dict["Tag"].template as<std::string>());
			SelectElements<IFORM>(fun, tag);
		}
	}

}

}
// namespace simpla

#endif /* MEDIA_TAG_H_ */