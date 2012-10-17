/*Copyright (C) 2007-2011 YU Zhi. All rights reserved.
 * $Id$
 * Maxwell/PML.h
 *
 *  Created on: 2010-12-7
 *      Author: salmon
 */

#ifndef SRC_EMFIELD_PML_H_
#define SRC_EMFIELD_PML_H_
#include "include/simpla_defs.h"
#include "fetl/fetl.h"
#include "fetl/grid/uniform_rect.h"
#include "engine/context.h"
#include "engine/modules.h"

namespace simpla
{
namespace em
{

template<typename TV, typename TG>
class PML: public Module
{
public:

	typedef PML ThisType;
	typedef TR1::shared_ptr<ThisType> Holder;

	DEFINE_FIELDS(TV, TG)

	PML(Context<TG> & d, const ptree & properties);

	virtual ~PML()
	{
	}

	virtual void Eval();

private:
	Context<TG> & ctx;
	Grid const & grid;

	nTuple<SIX, int> bc_;

	const Real dt;
	const Real mu0;
	const Real epsilon0;
	const Real speed_of_light;

	TwoForm X10, X11, X12;
	OneForm X20, X21, X22;

// alpha
	RScalarField a0, a1, a2;
// sigma
	RScalarField s0, s1, s2;

//input
	TR1::shared_ptr<const OneForm> J1;
//output
	TR1::shared_ptr<OneForm> E1;
	TR1::shared_ptr<TwoForm> B1;

};
template<> PML<Real, UniformRectGrid>::PML(Context<UniformRectGrid> & d,
		const ptree & properties);
template<> void PML<Real, UniformRectGrid>::Eval();
} // namespace em
} // namespace simpla
#endif  // SRC_EMFIELD_PML_H_
