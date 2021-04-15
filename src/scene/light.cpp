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

	if (pScene->softShadow)
	{
		vec3f V = d;
		vec3f up(0, 0, 1);
		vec3f u = (V.cross(up));
		vec3f v = (u.cross(V));
		u = (V.cross(v)).normalize();
		double size = 0.05;
		for (int i = 1; i < 25; i++)
		{
			double dx = (double)rand() / RAND_MAX * 2.0 * size - size;
			double dy = (double)rand() / RAND_MAX * 2.0 * size - size;
			vec3f dir = V + dx * u + dy * v;
			ray r1(P, dir.normalize());
			isect i1;
			if (pScene->intersect(r1, i1))
			{
				ret += prod(color, i1.getMaterial().kt);
			}
			else
			{
				ret += color;
			}
		}
		ret /= 25.0;
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

	if (pScene->softShadow)
	{
		vec3f V = d;
		vec3f up(0, 0, 1);
		vec3f u = (V.cross(up));
		vec3f v = (u.cross(V));
		u = (V.cross(v)).normalize();
		double size = 0.05;
		for (int i = 1; i < 25; i++)
		{
			double dx = (double)rand() / RAND_MAX * 2.0 * size - size;
			double dy = (double)rand() / RAND_MAX * 2.0 * size - size;
			vec3f dir = V + dx * u + dy * v;
			ray r1(P, dir.normalize());
			isect i1;
			if (pScene->intersect(r1, i1))
			{
				ret += prod(color, i1.getMaterial().kt);
			}
			else
			{
				ret += color;
			}
		}
		ret /= 25.0;
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

	if (pScene->softShadow)
	{
		vec3f V = d;
		vec3f up(0, 0, 1);
		vec3f u = (V.cross(up));
		vec3f v = (u.cross(V));
		u = (V.cross(v)).normalize();
		double size = 0.05;
		for (int i = 1; i < 25; i++)
		{
			double dx = (double)rand() / RAND_MAX * 2.0 * size - size;
			double dy = (double)rand() / RAND_MAX * 2.0 * size - size;
			vec3f dir = V + dx * u + dy * v;
			ray r1(P, dir.normalize());
			isect i1;
			if (pScene->intersect(r1, i1))
			{
				ret += prod(color, i1.getMaterial().kt);
			}
			else
			{
				ret += color;
			}
		}
		ret /= 25.0;
	}

	return ret;
}

double WarnModelLight::distanceAttenuation(const vec3f& P) const
{
	
	double x = P[0];
	double y = P[1];
	double t;
	vec4f p = matrix * vec4f({ p[0],p[1],p[2],1 });
	bool v = false;
	//printf("start show");
	switch (type)
	{
	case Type::kSquare:
		if (x > -size && x< size && y>-size && y < size) {
			v = true;
			
		}
		break;
	case Type::kTriangle:
		t = tan(3.141592653 / 3);
		if (y > -size / 2 && y < t*x + size && y < -t*x + size) {
			v = true;
		}
		break;
	case Type::kStar:
		t = tan(3.141592653 / 3);
		if ((y > -size / 2 && y < t * x + size && y < -t * x + size)
			|| (y < size / 2 && y > t * x - size && y > -t * x - size)){
			v = true;
			
		}
		break;
	default:
		//do nothing 
		
		v = false;
	}

	if (v == true) {
		
		return PointLight::distanceAttenuation(P);
	}
	else
	{
		return 0.0;
	}
	


}

