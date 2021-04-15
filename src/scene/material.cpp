#include "ray.h"
#include "material.h"
#include "light.h"

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

unsigned int Material::n = 0;

vec3f getTextureColor(const unsigned char* textureImg, int width, int height, float u, float v);

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i , const unsigned char* textureImg,const int width, const int height, const unsigned char* bump) const
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
		
		vec3f kdmp;
		if (textureImg) {
			double u, v;
			if(i.obj->getLocalUV(r, i, u, v)) {
				kdmp=getTextureColor(textureImg, width, height, u, v);
			}
			if(bump){
				int x = MAX(0, MIN(u * width, width - 1));
				int y = MAX(0, MIN(v * height, height - 1));
				double Bu = -bump[x + y * width] + bump[MIN(x + 1, width - 1) + y * width];
				double Bv = -bump[x + y * width] + bump[x + MIN(y + 1, height - 1) * width];
				vec3f Pu(0.0, 1.0, 0.0);
				vec3f Pv(1.0, 0.0, 0.0);
				N = N + (Bu * i.N).cross(Pv) + (Bv * i.N).cross(Pu);
				N= N.normalize();
				}
		}
		else {
			kdmp = kd;
		}
		
		
		double diffFactor = max(0.0, N.dot(L));
		vec3f R = L - 2 * L.dot(N) * N;
		double specBase = max(0.0, R.dot(V));
		double specFactor = pow(specBase, shininess * 128);

		vec3f ret = prod(intensity, distAtt * (ks * specFactor + prod(kdmp * diffFactor, one - kt)));
		ret = prod(ret, shadowAtt);

		color += ret;
	}

	return color;

}

vec3f getTextureColor(const unsigned char* textureImg, int width, int height, float u, float v)
{
	if (u < 0 || u > 1 || v < 0 || v > 1) {
		printf("Erro in size");
		return vec3f(1, 1, 1);
	}
	if (u == 1) u = 0;
	if (v == 1) v = 0;
	int x = u * width;
	int y = v * height;
	double val1 = textureImg[3 * (x + y * width)] / 255.0;
	double val2 = textureImg[3 * (x + y * width) + 1] / 255.0;
	double val3 = textureImg[3 * (x + y * width) + 2] / 255.0;
	return vec3f(val1, val2, val3);
}

