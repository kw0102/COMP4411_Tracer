#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	Scene* pScene = getScene();
	vec3f d = getDirection(P);
	isect i;
	ray r(P, d);

	vec3f ret = color;
	if (pScene->intersect(r, i))
	{
		ret = prod(color, i.getMaterial().kt);
	}

    return ret;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double d = vec3f(P - position).length();
	Scene* pScene = getScene();
	double a = constantAtt;
	double b = linearAtt;
	double c = quadricAtt;

	return min(1.0, 1.0 / (a + b * d + c * d * d));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	Scene* pScene = getScene();
	vec3f d = getDirection(P);
	isect i;
	ray r(P, d);

	vec3f ret = color;
	if (pScene->intersect(r, i))
	{
		ret = prod(color, i.getMaterial().kt);
	}

    return ret;
}

double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	return vec3f(1,1,1);
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	return vec3f(0,0,0);
}

vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.

	Scene* pScene = getScene();
	vec3f d = getDirection(P);

	if (-d * orientation.normalize() < cos(angle))
		return vec3f(0, 0, 0);

	isect i;
	ray r(P, d);

	vec3f ret = color;
	if (pScene->intersect(r, i))
	{
		ret = prod(color, i.getMaterial().kt);
	}

	return ret;
}

