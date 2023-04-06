#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color(0,0,0);
    if(render_world.ambient_color != nullptr)
        color = render_world.ambient_intensity * render_world.ambient_color->Get_Color(hit.uv) * color_ambient->Get_Color(hit.uv);

    for(unsigned int i = 0; i<render_world.lights.size(); i++){
        const Light* light = render_world.lights[i];
        vec3 lightDirection = light->position - intersection_point;
        Ray camera(intersection_point + ((lightDirection.normalized())*small_t), lightDirection.normalized());
    
        if(light->name.length() == 0){//kinda hacky but ambient light doesn't have a name assigned to it
            color += light->Emitted_Light(lightDirection) * color_ambient->Get_Color(hit.uv);
        }
        else{
            double intersectDistance = render_world.Closest_Intersection(camera).second.dist;
            if((intersectDistance <= lightDirection.magnitude()) &&  (intersectDistance>0) && render_world.enable_shadows){
                continue;
            }
            else{
                color += color_diffuse->Get_Color(hit.uv) * light->Emitted_Light(lightDirection) * std::max(dot(lightDirection.normalized(), normal.normalized()), 0.0) +
                    color_specular->Get_Color(hit.uv) * light->Emitted_Light(lightDirection) * std::pow(std::max(dot((ray.direction*-1).normalized(), (2*dot(lightDirection.normalized(), normal.normalized())*normal.normalized()-lightDirection.normalized()).normalized()), 0.0), specular_power);
            }
        }
    } //determine the color
    return color;
}
