#include "Metaball.h"

void Metaball::add(Ball ball)
{
	balls.push_back(ball);
};

bool Metaball::intersectLocal(const ray& r, isect& i) const
{
	/*
	*  Based on: http://www.geisswerks.com/ryan/BLOBS/blobs.html
	*/

	vector<Intersection> intersections;

	for (const Ball& ball : balls)
	{
		double tMin, tMax;
		if (intersectBoundingSphere(ball, r, tMin, tMax))
		{
			intersections.push_back({ tMin, true, ball });
			intersections.push_back({ tMax, false, ball });
		}
	}

	std::sort(intersections.begin(), intersections.end(), [](Intersection a, Intersection b) {return a.t < b.t;});

	vector<Ball> active;
	unsigned int count = 0;

	for (const Intersection& isect : intersections)
	{
		count++;
		if (isect.isEntry)
		{
			active.push_back(isect.ball);
		}
		else
		{
			for (auto it = active.begin(); it != active.end(); it++)
			{
				if (&*it == &isect.ball)
				{
					active.erase(it);
					break;
				}
			}
			
			if (count == intersections.size()) break;
		}

		double dt = (intersections[count].t - isect.t) / 100.0;
		if (isect.t < RAY_EPSILON) continue;
		vec3f N;
		double t = isect.t;

		for (int j = 0; j < 100; j++)
		{
			vec3f P = r.at(t);
			if (totalStrength(active, P) >= threshold)
			{
				// Calculate normal
				
				N = vec3f(0, 0, 0);
				for (const Ball& b : active)
				{
					vec3f Ni = P - b.pos;
					N += totalStrength({b}, P) * Ni;
				}

				if (r.getDirection().dot(N) > 0)
					N *= -1;

				i.obj = this;
				i.N = N.normalize();
				i.t = t + RAY_EPSILON;
				return true;
			}

			t += dt;
		}
		
	}

	return false;

};

bool Metaball::intersectBoundingSphere(const Ball& ball, const ray& r, double& tMin, double& tMax) const
{
	vec3f d = r.getDirection();
	vec3f ballP = ball.pos;
	vec3f P = r.getPosition();
	double str = ball.str;
	double a = d.length_squared();
	double b = 2 * d.dot(P - ballP);
	double c = P.dot(P) - 2 * P.dot(ballP) + ballP.dot(ballP) - str * str;

	double discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return false;
	}

	discriminant = sqrt(discriminant);
	double t2 = -b + discriminant;
	t2 /= 2 * a;

	double t1 = -b - discriminant;
	t1 /= 2 * a;

	if (t1 < t2) {
		tMin = t1;
		tMax = t2;
	}
	else {
		tMin = t2;
		tMax = t1;
	}

	return true;
}

double Metaball::totalStrength(vector<Ball> balls, vec3f p) const
{
	double sum = 0.0;
	for (const Ball& ball : balls)
	{
		double r = (ball.pos - p).length();
		r /= ball.str;
		double q = (1 - r * r * r * (r * (r * 6 - 15) + 10)) * ball.str;
		if (q > 0) sum += q;
	}
	return sum;
}