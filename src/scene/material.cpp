#include "ray.h"
#include "material.h"
#include "light.h"

unsigned int Material::n = 0;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	const vec3f one(1, 1, 1);

	vec3f V = r.getDirection();
	vec3f N = i.N;
	vec3f P = r.at(i.t);

	vec3f color = ke + prod(ka, scene->ambientLight);

	// Light sources
	for (auto l = scene->beginLights(); l != scene->endLights(); l++)
	{
		Light* pLight = *l;
		vec3f L = pLight->getDirection(P);
		vec3f intensity = pLight->getColor(P);

		if (L.length_squared() == 0) // ambient light
		{
			color += prod(ka, intensity);
		}

		double distAtt = pLight->distanceAttenuation(P);
		vec3f shadowAtt = pLight->shadowAttenuation(P);

		double diffFactor = max(0.0, N.dot(L));

		vec3f R = L - 2 * L.dot(N) * N;
		double specBase = max(0.0, R.dot(V));
		double specFactor = pow(specBase, shininess * 128);

		vec3f ret = prod(intensity, distAtt * (ks * specFactor + prod(kd * diffFactor, one - kt)));
		ret = prod(ret, shadowAtt);

		color += ret;
	}

	return color;
}
