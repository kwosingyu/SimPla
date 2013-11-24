/*
 * lua_parser_test.cpp
 *
 *  Created on: 2013年9月24日
 *      Author: salmon
 */

#include "lua_state.h"
#include "fetl/primitives.h"
#include <iostream>
using namespace simpla;
int main(int argc, char** argv)
{
	LuaObject pt;

	if (argc > 1)
	{
		pt.ParseFile(argv[1]);
	}
	else
	{
		pt.ParseString(
				"c=100 \n t1={a=5,b=6.0,c=\"text\",e={a=5,b=6.0}} \n t2={e=4,f=true} \n t3={1,3,4,5}\n"
						"tt={10,12,13} \n"
						"function f(x,y) \n"
						"    return x+y  \n"
						"end \n");
	}

	stackDump(pt.GetState());

	std::cout << "c \t=" << pt.GetChild("c").as<double>() << std::endl;

	std::cout << "c \t=" << pt.GetChild("c").as<int>() << std::endl;

	std::cout << "c \t=" << pt.Get("c", 1) << std::endl;

	std::cout << "b \t=" << pt.Get("b", 120) << std::endl;

	auto tt1 = pt.at("t1");

	std::cout << "t1 \t=" << tt1.as<double>() << std::endl;

	std::cout << "t2.f \t=" << std::boolalpha
			<< pt.GetChild("t2")["f"].as<bool>() << std::endl;

	double res;

	std::cout << "f(2,2.5) \t=" << pt["f"](2.0, 2.5).as<double>() << std::endl;

	std::cout << "f(1.2,2.5) \t=" << pt["f"](1.2, 2.5).as<double>()
			<< std::endl;

	std::cout << "f(3,2.5) \t=" << pt["f"](3.0, 2.5).as<double>() << std::endl;

	std::cout << "f(3,2.5) \t=" << pt["f"](2.0, 2.5).as<double>() << std::endl;

	std::cout << "t3 \t=" << pt.GetChild("t3").as<nTuple<3, double>>()
			<< std::endl;

	LuaObject * t1 = new LuaObject(pt.GetChild("t1"));

	LuaObject * e = new LuaObject(t1->GetChild("e"));

	delete t1;

	std::cout << "t1.e.a \t= " << e->GetChild("a").as<double>() << std::endl;

	delete e;

	for (auto const & p : pt.GetChild("tt"))
	{
		std::cout << p.as<int>() << std::endl;
		std::cout << p.as<int>() << std::endl;
	}

	for (auto const & p : pt.GetChild("tt"))
	{
		std::cout << p.as<int>() << std::endl;
	}
}
