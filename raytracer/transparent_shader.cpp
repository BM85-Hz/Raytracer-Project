#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color;
    vec3 surface_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

    if (opacity == 0.0)
        return surface_color;

    double n_i = 1.0; //index of refraction for air 
    double n_o = index_of_refraction;
    //double refract_ratio = n_i / n_o;
    double cos_theta = dot(ray.direction, normal);
    double cos_phi_root = 1 - (pow(n_i, 2) * (1 - pow(cos_theta, 2)) / pow(n_o, 2));
    double cos_phi_root_flipped = 1 - (pow(n_o, 2) * (1 - pow(cos_theta, 2)) / pow(n_i, 2));
    double cos_phi = sqrt(cos_phi_root);
    double cos_phi_flipped = sqrt(cos_phi_root_flipped);

    vec3 refraction_direction;
    vec3 reflect_direction = (-2*cos_theta*normal) + ray.direction;

    if(cos_phi_root_flipped < 0 && cos_theta >= 0){//total internal reflection
        Ray reflected_ray = Ray(intersection_point + reflect_direction.normalized() * small_t, reflect_direction);
        color = opacity * surface_color + (1 - opacity) * render_world.Cast_Ray(reflected_ray, recursion_depth-1);

        return color;
    }
    else if (cos_phi_root_flipped >= 0 && cos_theta >= 0){
        cos_theta = dot(normal, refraction_direction);
        refraction_direction = ((n_o * (ray.direction - normal * cos_theta)) / n_i) + normal * cos_phi_flipped;
    }
    else if(cos_phi_root >= 0 && cos_theta < 0){
        refraction_direction = ((n_i * (cos_theta * -1) - cos_phi) / n_o) * normal + (n_i/n_o) * ray.direction;
    }
    else{//cos_phi_root < 0 && cos_theta < 0
    
    }

    double R_0 = (n_i - n_o) / (n_o + n_i);
    R_0 *= R_0;
    double schlick = R_0 + (1 - R_0) * pow(1 - abs(cos_theta), 5);

    Ray reflected_ray = Ray(intersection_point + reflect_direction.normalized() * small_t, reflect_direction);
    Ray refracted_ray = Ray(intersection_point + refraction_direction.normalized() * small_t, refraction_direction);

    color = (opacity * surface_color + (1 - opacity) * schlick * render_world.Cast_Ray(reflected_ray, recursion_depth-1));
    
    if(cos_phi_root_flipped >= 0 || cos_phi_root >= 0)
        color = color + (1 - schlick) * render_world.Cast_Ray(refracted_ray, recursion_depth-1);
    
    return color;
}