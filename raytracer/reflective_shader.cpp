#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
    double reflection = 1 - reflectivity;

    if (recursion_depth < render_world.recursion_depth_limit){
        Ray reflect_direction;
        reflect_direction.direction = ray.direction - 2 * dot(normal, ray.direction) * normal;
        reflect_direction.endpoint = intersection_point;

        color = (reflection * color) + (reflectivity * render_world.Cast_Ray(reflect_direction, ++recursion_depth));
    }
    else{
        color *= reflection;
    }

    return color;
}
