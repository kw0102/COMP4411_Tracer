#include <cmath>

#include "Sphere.h"

bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	vec3f v = -r.getPosition();
	double b = v.dot(r.getDirection());
	double discriminant = b*b - v.dot(v) + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	double t1 = b - discriminant;

	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		i.N = r.at( t1 ).normalize();
	} else {
		i.t = t2;
		i.N = r.at( t2 ).normalize();
	}

	return true;
}


bool Sphere::getLocalUV(const ray& r, const isect& i, double& u, double& v) const
{
	
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	ray localRay(pos, dir);
	isect icopy = i;
	if (intersectLocal(localRay, icopy)) {

		vec3f Sn = localRay.at(icopy.t);
		vec3f Sp(0, 0, 1);
		vec3f Se(1, 0, 0);

		double cosphi = - Sp.dot(Sn);
		double sinphi = sqrt(1 - cosphi * cosphi);
		double phi = acosf(Sp.dot(Sn));
		v = phi / 3.141592653;
		double theta = acosf(((Se.dot(Sn))) / sinphi);
		double ut = theta/2/3.131592653;
		if ((Sp.cross(Se)).dot(Sn) > 0) u = ut;
		else u = 1 - ut;

		return true;
	}
	else
	{
		return false;
	}

}