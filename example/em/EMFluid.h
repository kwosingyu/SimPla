/**
 * @file em_fluid.h
 * @author salmon
 * @date 2016-01-13.
 */

#ifndef SIMPLA_EM_FLUID_H
#define SIMPLA_EM_FLUID_H

#include "../../src/field/Field.h"
#include "../../src/physics/PhysicalConstants.h"
#include "../../src/task_flow/Worker.h"
#include "../../src/mesh/Mesh.h"
#include "../../src/mesh/MeshEntity.h"

namespace simpla
{
using namespace simpla::mesh;


template<typename TM>
class EMFluid : public task_flow::Worker
{
    typedef EMFluid<TM> this_type;
    typedef task_flow::Worker base_type;

public:
    virtual bool is_a(std::type_info const &info) const
    {
        return typeid(this_type) == info || task_flow::Worker::is_a(info);
    }

    template<typename _UOTHER_> bool is_a() const { return is_a(typeid(_UOTHER_)); }


    virtual std::string get_class_name() const { return class_name(); }


    static std::string class_name() { return "EMFluid<" + traits::type_id<TM>::name() + ">"; }


public:
    typedef TM mesh_type;
    typedef typename mesh_type::scalar_type scalar_type;

    EMFluid(mesh_type const &mesh) : base_type(mesh) { }

    virtual ~EMFluid()noexcept { }

    virtual void next_step(Real dt);

    virtual void tear_down();

    virtual io::IOStream &check_point(io::IOStream &) const;

    mesh::MeshEntityRange limiter_boundary;
    mesh::MeshEntityRange vertex_boundary;
    mesh::MeshEntityRange edge_boundary;
    mesh::MeshEntityRange face_boundary;

    mesh::MeshEntityRange plasma_region_volume;
    mesh::MeshEntityRange plasma_region_vertex;
    mesh::MeshEntityRange J_src;

    template<typename ValueType, size_t IFORM> using field_t =  Field<ValueType, TM, std::integral_constant<size_t, IFORM> >;;

//    std::function< Vec3(Real, point_type const &)   >    J_src_fun;


    typedef field_t<scalar_type, FACE> TB;
    typedef field_t<scalar_type, EDGE> TE;
    typedef field_t<scalar_type, EDGE> TJ;
    typedef field_t<scalar_type, VERTEX> TRho;
    typedef field_t<vector_type, VERTEX> TJv;

    field_t<scalar_type, EDGE> E0/*   */{*this, "E0"};
    field_t<scalar_type, FACE> B0/*   */{*this, "B0"};
    field_t<vector_type, VERTEX> B0v/**/{*this, "B0v"};
    field_t<scalar_type, VERTEX> BB/* */{*this, "BB"};
    field_t<vector_type, VERTEX> Ev/* */{*this, "Ev"};
    field_t<vector_type, VERTEX> Bv/* */{*this, "Bv"};
    field_t<scalar_type, FACE> B1/*   */{*this, "B1"};
    field_t<scalar_type, EDGE> E1/*   */{*this, "E1"};
    field_t<scalar_type, EDGE> J1/*   */{*this, "J1"};

    field_t<scalar_type, VERTEX> rho0{m};


    struct fluid_s
    {
        Real mass;
        Real charge;
        field_t<scalar_type, VERTEX> rho1;
        field_t<vector_type, VERTEX> J1;
    };

    std::map<std::string, fluid_s> fluid_sp;

    std::pair<typename std::map<std::string, fluid_s>::iterator, bool>
    add_particle(std::string const &name, Real mass, Real charge)
    {
        return fluid_sp.emplace(
                std::make_pair(name,
                               fluid_s{mass, charge,
                                       TRho{*this, "n_" + name},
                                       TJv{*this, "J_" + name}}));

    }


};


template<typename TM> void EMFluid<TM>::tear_down() { }

template<typename TM> io::IOStream &EMFluid<TM>::check_point(io::IOStream &os) const { return os; }

template<typename TM>
void EMFluid<TM>::next_step(Real dt)
{

    DEFINE_PHYSICAL_CONST

    LOGGER << " Time = [" << time() << "] Count = [" << step_count() << "]" << std::endl;

    J1.clear();

//    J1.accept(J_src, [&](id_type s, Real &v) { J1.add(s, J_src_fun(time(), m->point(s))); });

    LOG_CMD(B1 -= curl(E1) * (dt * 0.5));

    B1.accept(face_boundary, [&](id_type, Real &v) { v = 0; });

    LOG_CMD(E1 += (curl(B1) * speed_of_light2 - J1 / epsilon0) * dt);

    E1.accept(edge_boundary, [&](id_type, Real &v) { v = 0; });

    field_t<vector_type, VERTEX> dE{m};

    //particle::absorb(ion, limiter_boundary);
    if (fluid_sp.size() > 0)
    {

        field_t<vector_type, VERTEX> Q{m};
        field_t<vector_type, VERTEX> K{m};

        field_t<scalar_type, VERTEX> a{m};
        field_t<scalar_type, VERTEX> b{m};
        field_t<scalar_type, VERTEX> c{m};

        a.clear();
        b.clear();
        c.clear();

        Q = map_to<VERTEX>(E1) - Ev;


        for (auto &p :   fluid_sp)
        {

            Real ms = p.second.mass;
            Real qs = p.second.charge;


            field_t<scalar_type, VERTEX> &ns = p.second.rho1;

            field_t<vector_type, VERTEX> &Js = p.second.J1;;


            Real as = (dt * qs) / (2.0 * ms);

            Q -= 0.5 * dt / epsilon0 * Js;

            K = (Ev * qs * ns * 2.0 + cross(Js, B0v)) * as + Js;

            Js = (K + cross(K, B0v) * as + B0v * (dot(K, B0v) * as * as)) / (BB * as * as + 1);

            Q -= 0.5 * dt / epsilon0 * Js;

            a += qs * ns * (as / (BB * as * as + 1));
            b += qs * ns * (as * as / (BB * as * as + 1));
            c += qs * ns * (as * as * as / (BB * as * as + 1));


        }

        a *= 0.5 * dt / epsilon0;
        b *= 0.5 * dt / epsilon0;
        c *= 0.5 * dt / epsilon0;
        a += 1;


        LOG_CMD(dE = (Q * a - cross(Q, B0v) * b + B0v * (dot(Q, B0v) * (b * b - c * a) / (a + c * BB))) /
                     (b * b * BB + a * a));

        for (auto &p :   fluid_sp)
        {
            Real ms = p.second.mass;
            Real qs = p.second.charge;
            field_t<scalar_type, VERTEX> &ns = p.second.rho1;
            field_t<vector_type, VERTEX> &Js = p.second.J1;;


            Real as = (dt * qs) / (2.0 * ms);

            K = dE * ns * qs * as;
            Js += (K + cross(K, B0v) * as + B0v * (dot(K, B0v) * as * as)) / (BB * as * as + 1);
        }
        Ev += dE;

//        LOG_CMD(E1 += map_to<EDGE>(Ev) - E1);
    }


    LOG_CMD(B1 -= curl(E1) * (dt * 0.5));

    B1.accept(face_boundary, [&](id_type const &, Real &v) { v = 0; });

    base_type::next_step(dt);
}


}//namespace simpla  {
#endif //SIMPLA_EM_FLUID_H