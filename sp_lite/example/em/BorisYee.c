//
// Created by salmon on 16-6-11.
//

#include "BorisYee.h"
#include "Boris.h"

#include "../../src/sp_config.h"
#include "../../src/particle/BucketContainer.h"
//#include "../../src/particle/ParticleUtility.h"

#define CACHE_EXTENT_X 4
#define CACHE_EXTENT_Y 4
#define CACHE_EXTENT_Z 4
#define CACHE_SIZE (CACHE_EXTENT_X*CACHE_EXTENT_Y*CACHE_EXTENT_Z)

#define IX  1
#define IY  CACHE_EXTENT_X
#define IZ  CACHE_EXTENT_X*CACHE_EXTENT_Y

#define ll 0
#define rr 1.0

MC_DEVICE void cache_gather(Real *v, Real const f[CACHE_SIZE], Real const *r0,
                            const Real *r1)
{
    Real r[3] = {r0[0] - r1[0], r0[1] - r1[1], r0[2] - r1[2]};
    id_type s = (int) (r[0]) * IX + (int) (r[1]) * IY + (int) (r[2]) * IZ;

    *v = f[s + IX + IY + IZ /*  */] * (r[0] - ll) * (r[1] - ll) * (r[2] - ll)
         + f[s + IX + IY /*     */] * (r[0] - ll) * (r[1] - ll) * (rr - r[2])
         + f[s + IX + IZ /*     */] * (r[0] - ll) * (rr - r[1]) * (r[2] - ll)
         + f[s + IX /*          */] * (r[0] - ll) * (rr - r[1]) * (rr - r[2])
         + f[s + IY + IZ /*     */] * (rr - r[0]) * (r[1] - ll) * (r[2] - ll)
         + f[s + IY /*          */] * (rr - r[0]) * (r[1] - ll) * (rr - r[2])
         + f[s + IZ /*          */] * (rr - r[0]) * (rr - r[1]) * (r[2] - ll)
         + f[s /*               */] * (rr - r[0]) * (rr - r[1])
           * (rr - r[2]);
}

MC_DEVICE void cache_scatter(Real f[CACHE_SIZE], Real v, Real const *r0,
                             Real const *r1)
{
    Real r[3] = {r0[0] - r1[0], r0[1] - r1[1], r0[2] - r1[2]};
    id_type s = (int) (r[0]) * IX + (int) (r[1]) * IY + (int) (r[2]) * IZ;

    f[s + IX + IY + IZ /*  */] += v * (r[0] - ll) * (r[1] - ll) * (r[2] - ll);
    f[s + IX + IY /*       */] += v * (r[0] - ll) * (r[1] - ll) * (rr - r[2]);
    f[s + IX + IZ /*       */] += v * (r[0] - ll) * (rr - r[1]) * (r[2] - ll);
    f[s + IX /*            */] += v * (r[0] - ll) * (rr - r[1]) * (rr - r[2]);
    f[s + IY + IZ /*       */] += v * (rr - r[0]) * (r[1] - ll) * (r[2] - ll);
    f[s + IY /*            */] += v * (rr - r[0]) * (r[1] - ll) * (rr - r[2]);
    f[s + IZ /*            */] += v * (rr - r[0]) * (rr - r[1]) * (r[2] - ll);
    f[s/*                  */] += v * (rr - r[0]) * (rr - r[1]) * (rr - r[2]);

}

#undef ll
#undef rr
#undef IX
#undef IY
#undef IZ

/**
 *\verbatim
 *                ^y
 *               /
 *        z     /
 *        ^    /
 *    PIXEL0 110-------------111 VOXEL
 *        |  /|              /|
 *        | / |             / |
 *        |/  |    PIXEL1  /  |
 * EDGE2 100--|----------101  |
 *        | m |           |   |
 *        |  010----------|--011 PIXEL2
 *        |  / EDGE1      |  /
 *        | /             | /
 *        |/              |/
 *       000-------------001---> x
 *                       EDGE0
 *
 *\endverbatim
 */

/* @formatter:off*/
#define _R 1.0
MC_DEVICE Real id_to_shift_[][3] = { //
		{ 0, 0, 0 },           // 000
				{ _R, 0, 0 },           // 001
				{ 0, _R, 0 },           // 010
				{ 0, 0, _R },          // 011
				{ _R, _R, 0 },           // 100
				{ _R, 0, _R },          // 101
				{ 0, _R, _R },          // 110
				{ 0, _R, _R },          // 111
		};
MC_DEVICE int sub_index_to_id_[4][3] = { //
		{ 0, 0, 0 }, /*VERTEX*/
		{ 1, 2, 4 }, /*EDGE*/
		{ 6, 5, 3 }, /*FACE*/
		{ 7, 7, 7 } /*VOLUME*/

		};

#undef _R

/* @formatter:on*/

#ifndef __CUDACC__
void spBorisYeeUpdate(BorisYeeUpdateArgs const *args, Real dt,
                      spPage **first, spPage **second, spPagePool *pool, const Real *fE,
                      const Real *fB, Real *fRho, Real *fJ)

{
//
//	MC_SHARED
//	Real tE[3][CACHE_SIZE], tB[3][CACHE_SIZE], tJ[4][CACHE_SIZE];
//	MC_SHARED spPage *read_buffer[CACHE_SIZE];
//	MC_SHARED spPage *write_buffer[CACHE_SIZE];
//	MC_SHARED
//	bucket_page_status_flag_t shift_flag[CACHE_SIZE];
//
//	size_type ele_size_in_byte = args->ele_size_in_type;
//
//	for (size_type _blk_s = 0, _blk_e = args->number_of_idx; _blk_s < _blk_e; ++_blk_s)
//	{
//		size_type cell_idx = args->cell_idx[_blk_s];
//
//		// read tE,tB from E,B
//		// clear tJ
//		MC_SHARED spPage **self;
//
//		// TODO load data to cache
//
//		for (int n = 0; n < CACHE_SIZE; ++n)
//		{
//			spPage const *from = read_buffer[n];
//
//			bucket_page_status_flag_t p_flag = shift_flag[n];
//
//			while (from != 0x0)
//			{
//				byte_type const *v = from->data;
//
//				for (int i = 0; i < SP_NUMBER_OF_ENTITIES_IN_PAGE; ++i)
//				{
//					bucket_page_status_flag_t flag = 0x1UL << i;
//
//					boris_point_s *p = (boris_point_s * )(v + entity_size_in_byte * i);
//
//					if ((from->flag & flag) != 0 && ((p->_tag & 0x3F) == p_flag))
//					{
//
//						Real E[3], B[3];
//
//						cache_gather(&E[0], tE[0], p->r, id_to_shift_[sub_index_to_id_[1/*EDGE*/][0]]);
//						cache_gather(&E[1], tE[1], p->r, id_to_shift_[sub_index_to_id_[1/*EDGE*/][1]]);
//						cache_gather(&E[2], tE[2], p->r, id_to_shift_[sub_index_to_id_[1/*EDGE*/][2]]);
//
//						cache_gather(&B[0], tB[0], p->r, id_to_shift_[sub_index_to_id_[2/*FACE*/][0]]);
//						cache_gather(&B[1], tB[1], p->r, id_to_shift_[sub_index_to_id_[2/*FACE*/][1]]);
//						cache_gather(&B[2], tB[2], p->r, id_to_shift_[sub_index_to_id_[2/*FACE*/][2]]);
//
//						boris_point_s *p_next = (boris_point_s *) spAtomicInsert(self,
//								args->pool);
//						p_next->_tag = p->_tag;
//
//						spBorisPushOne(p, p_next, args->cmr, dt, E, B, args->inv_dx);
//
//						p_next->_tag &= ~(0x3F);
//						// TODO r -> tag
//						p_next->_tag |= spParticleFlag(p_next->r);
//
//						cache_scatter(tJ[0], spBorisGetRho(p_next), p_next->r,
//								id_to_shift_[sub_index_to_id_[0/*VERTEX*/][0]]);
//						cache_scatter(tJ[1], spBorisGetJ(p_next, 0), p_next->r,
//								id_to_shift_[sub_index_to_id_[1/*EDGE*/][0]]);
//						cache_scatter(tJ[2], spBorisGetJ(p_next, 1), p_next->r,
//								id_to_shift_[sub_index_to_id_[1/*EDGE*/][1]]);
//						cache_scatter(tJ[3], spBorisGetJ(p_next, 2), p_next->r,
//								id_to_shift_[sub_index_to_id_[1/*EDGE*/][2]]);
//
//					}
//				}
//				from = from->next;
//
//			}        //   while (from != 0x0)
//
//		}        // foreach CACHE
//
//		spPushFront(self, &(next[cell_idx]));
//				 //TODO atomic_add tJ to fJ
//		for (int s = 0; s < CACHE_SIZE; ++s)
//		{
//			size_type idx = posFromCacheIdx(s, args->i_dims);
//			atomicAdd(&(fRho[idx]), tJ[0][idx]);
//			atomicAdd(&(fJ[idx * 3 + 0]), tJ[1][idx]);
//			atomicAdd(&(fJ[idx * 3 + 0]), tJ[2][idx]);
//			atomicAdd(&(fJ[idx * 3 + 0]), tJ[3][idx]);
//		}
//
//	}        //foreach block

}

#endif