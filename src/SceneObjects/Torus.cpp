#include "Torus.h"
#include "../quartic.h"
#include <vector>

/*
*  Based on: http://blog.marcinchwedczuk.pl/ray-tracing-torus
*  Quartic solver: https://github.com/sasamil/Quartic
*/

bool Torus::intersectLocal(const ray& r, isect& i) const
{
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	
	double u1 = d.length_squared();
	double u2 = p.dot(d);
	double u3 = p.length_squared() + (r1 * r1 - r2 * r2);

	double c4 = u1 * u1;
	double c3 = 4 * u1 * u2;
	double c2 = 2 * u1 * u3 + 4 * u2 * u2 - 4 * r1 * r1 * (d[0] * d[0] + d[1] * d[1]);
	double c1 = 4 * u3 * u2 - 8 * r1 * r1 * (p[0] * d[0] + p[1] * d[1]);
	double c0 = u3 * u3 - 4 * r1 * r1 * (p[0] * p[0] + p[1] * p[1]);
	

	auto* sols = solve_quartic(c3 / c4, c2 / c4, c1 / c4, c0 / c4);

	vector<double> t;

	for (int i = 0; i < 4; i++)
	{
		if (sols[i].imag() == 0.0 && sols[i].real() > RAY_EPSILON)
			t.push_back(sols[i].real());
	}
	delete[] sols;

	if (t.empty()) return false;

	sort(t.begin(), t.end());

	i.obj = this;
	i.t = t[0];
	
	vec3f P = r.at(i.t);

	double alpha = 1.0 - r1 / sqrt(P[0] * P[0] + P[1] * P[1]);
	i.N = vec3f(alpha * P[0], alpha * P[1], P[2]).normalize();

	return true;

}