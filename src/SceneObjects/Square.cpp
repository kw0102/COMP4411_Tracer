#include <cmath>

#include "Square.h"

bool Square::intersectLocal( const ray& r, isect& i ) const
{
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	if( d[2] == 0.0 ) {
		return false;
	}

	double t = -p[2]/d[2];

	if( t <= RAY_EPSILON ) {
		return false;
	}

	vec3f P = r.at( t );

	if( P[0] < -0.5 || P[0] > 0.5 ) {	
		return false;
	}

	if( P[1] < -0.5 || P[1] > 0.5 ) {	
		return false;
	}

	i.obj = this;
	i.t = t;
	if( d[2] > 0.0 ) {
		i.N = vec3f( 0.0, 0.0, -1.0 );
	} else {
		i.N = vec3f( 0.0, 0.0, 1.0 );
	}

	return true;
}

bool Square::getLocalUV(const ray& r, const isect& i, double& u, double& v) const
{
	
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	ray localRay(pos, dir);
	isect icopy = i;
	if (intersectLocal(localRay, icopy)) {
		vec3f localIscePoint = localRay.at(icopy.t);
		u = localIscePoint[0] + 0.5;
		v = localIscePoint[1] + 0.5;
		return true;
	}
	else {
		return false;
	}
	
}
