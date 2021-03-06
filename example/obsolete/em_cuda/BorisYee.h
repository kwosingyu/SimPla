//
// Created by salmon on 16-6-11.
//

#ifndef SIMPLA_BORISYEE_H
#define SIMPLA_BORISYEE_H

#include "../../sp_lite/spParticle.h"
#include "../../sp_lite/spMesh.h"
#include "../../sp_lite/spField.h"

void spInitializeParticle_BorisYee(spMesh *ctx, sp_particle_type *pg,
		size_type NUM_OF_PIC);

void spUpdateParticle_BorisYee(spMesh *ctx, Real dt, sp_particle_type *pg,
		const sp_field_type * fE, const sp_field_type * fB,
		sp_field_type * fRho, sp_field_type * fJ);

void spUpdateField_Yee(spMesh *ctx, Real dt, const sp_field_type * fRho,
		const sp_field_type * fJ, sp_field_type * fE, sp_field_type * fB);

#endif //SIMPLA_BORISYEE_H
