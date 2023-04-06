//Student Name: Braxton Mott
//Student ID: 862208656

#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include <cfloat>

extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    std::pair<Shaded_Object,Hit> nearest;
    nearest.second.dist = DBL_MAX;

    //double min_t = std::numeric_limits<double>::max();

    for(unsigned int i = 0; i < objects.size(); i++){

        Hit nearestTemp = (this->objects[i]).object->Intersection(ray, -1);
    
        Debug_Scope scope;
        if(nearestTemp.dist > 0)
            Pixel_Print("closest intersection; obj: ",objects.at(i).object->name,"; hit: ",nearestTemp);
        else
            Pixel_Print("no intersection with ",objects.at(i).object->name);
        
        if((nearestTemp.dist > 0) && (nearestTemp.dist < nearest.second.dist)){
            nearest.first = this->objects[i];
            nearest.second = nearestTemp;
        }
    }

    if(nearest.second.dist == DBL_MAX)
		nearest.second.dist = -1;

    return nearest;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    Ray ray;
    ray.endpoint = camera.position; //endpoint at start of start
    vec3 z = camera.World_Position(pixel_index); //endpoint camera points to
    ray.direction = (z - ray.endpoint).normalized();

    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    Debug_Scope scope; 
    Pixel_Print("cast ray: ",ray);

    //if(recursion_depth == 0){
    //    vec3 black(0,0,0);
    ///    return black;
    //}
    
    std::pair<Shaded_Object, Hit> hitPair = this->Closest_Intersection(ray);
    vec3 color;
    vec3 normal;
    vec3 rayPoint = ray.Point(hitPair.second.dist);

    if(hitPair.second.dist > 0){
        normal = hitPair.first.object -> Normal(ray, hitPair.second);
        color = hitPair.first.shader->Shade_Surface(*this, ray, hitPair.second, rayPoint, normal, recursion_depth);
    
        Pixel_Print("call Shade_Surface with location ",rayPoint,"; normal: ",normal);
        Debug_Scope scope;
        Pixel_Print("flat shader; color: ",color);

        return color;
    }
    else{
        
        if(background_shader != nullptr){
            return background_shader->Shade_Surface(*this, ray, hitPair.second, rayPoint, normal, recursion_depth);
        }
        else{
            vec3 black(0,0,0);
            return black;
        }
    }
}
