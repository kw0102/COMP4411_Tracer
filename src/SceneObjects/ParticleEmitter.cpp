#include "ParticleEmitter.h"
#include <cmath>

void ParticleEmitter::init()
{
	particles.clear();

	int frame = 0;

	while (frame < iteration)
	{
		// Emit
		for (int i = 0; i < numEmit; i++)
		{
			if (particles.size() < maxNumParticles)
			{
				Particle p;
				double x = frand(-0.707, 0.707);
				double y = frand(-0.707, 0.707);
				double z = frand(-0.707, 0.707);
				p.origin = p.p = vec3f(x, y, z) * size;
				x = frand(-0.5, 0.5);
				y = frand(-0.5, 0.5);
				z = frand(-0.5, 0.5);
				p.v = vec3f(x, y, z).normalize() * frand(minSpeed, maxSpeed);
				double factor = frand(0, 1);
				p.startColor = startColorMin * factor + startColorMax * (1 - factor);
				factor = frand(0, 1);
				p.endColor = endColorMin * factor + endColorMax * (1 - factor);
				p.color = p.startColor;
				p.life = 0;
				p.maxLife = (int)frand(minLife, maxLife);

				particles.push_back(p);
			}
			else
			{
				break;
			}
		}

		// Proceed
		for (Particle& p : particles)
		{
			p.p = p.origin + p.v * p.life + 0.5 * force * p.life * p.life;
			p.life++;
			double lifePercent = (double)p.life / (double)p.maxLife;
			p.color = (1 - lifePercent) * p.startColor + (lifePercent) * p.endColor;
			p.color *= (1 - lifePercent);
		}

		// remove dead particles
		for (int i = particles.size() - 1; i >= 0; i--)
		{
			if (particles[i].life > particles[i].maxLife)
			{
				particles.erase(particles.begin() + i);
			}
		}

		frame++;
	}
	
	printf("emitter initialized");

}

bool ParticleEmitter::intersectLocal(const ray& r, isect& i) const
{
	double tMin = 1.0e308; // infinity
	vec3f color;

	vec3f lookDir = r.getDirection();
	bool hasOne = false;

	for (const Particle& p : particles)
	{
		vec3f dir = (r.getPosition() - p.p).normalize();
		if (abs(abs(dir * lookDir) - 1) < RAY_EPSILON)
		{
			hasOne = true;
			double t = (r.getPosition() - p.p).length();
			if (t < tMin)
			{
				tMin = t;
				color = p.color;
			}
		}
	}

	if (hasOne)
	{
		Material* m = new Material;
		m->ke = color;
		i.t = tMin;
		i.N = -lookDir;
		i.material = m;
	}

	return hasOne;
};