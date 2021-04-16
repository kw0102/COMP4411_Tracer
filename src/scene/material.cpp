#include "ray.h"
#include "material.h"
#include "light.h"

unsigned int Material::n = 0;

vec3f getTextureColor(const unsigned char* textureImg, int width, int height, float u, float v);
double getPixelIntensity(unsigned char* bump, int width, int height, int x, int y);
// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i , const unsigned char* textureImg,const int width, const int height, unsigned char* bump) const
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
				
				int x = min(width - 1, int(u * double(width)));
				int y = min(height - 1, int(v * double(height)));

				if (x >= 1 && x <= width - 1 && y >= 1 && y <= width) {
					double Gx = getPixelIntensity(bump, width, height, x - 1, y + 1) * -1 +
						getPixelIntensity(bump, width, height, x - 1, y) * -2 +
						getPixelIntensity(bump, width, height, x - 1, y - 1) * -1 +
						getPixelIntensity(bump, width, height, x + 1, y + 1) * 1 +
						getPixelIntensity(bump, width, height, x - 1, y) * 2 +
						getPixelIntensity(bump, width, height, x - 1, y - 1) * 1;

					double Gy = getPixelIntensity(bump, width, height, x - 1, y + 1) * -1 +
						getPixelIntensity(bump, width, height, x, y + 1) * -2 +
						getPixelIntensity(bump, width, height, x + 1, y + 1) * -1 +
						getPixelIntensity(bump, width, height, x - 1, y - 1) * 1 +
						getPixelIntensity(bump, width, height, x, y - 1) * 2 +
						getPixelIntensity(bump, width, height, x + 1, y - 1) * 1;


					vec3f Pu(0.0, 1.0, 0.0);
					vec3f Pv(1.0, 0.0, 0.0);
					Gx *= 35.0;
					Gy *= 35.0;
					if (sqrt(Gx * Gx + Gy * Gy) > 0.9) {
						N = N + Gx*Pv + Gy*Pv;
					}
					N = N.normalize();
				}
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
		color.clamp();
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


double getPixelIntensity(unsigned char* bump, int width, int height, int x, int y) {
	if (x >= 0 && x < width && y >= 0 && y < height) {
		unsigned char* pixel = bump + (x + y * width) * 3;
		int r = (int)*pixel;
		int g = (int)*(pixel + 1);
		int b = (int)*(pixel + 2);
		vec3f color = vec3f((float)r / float(255), (float)g / 255.0f, (float)b / 255.0f).clamp();
		double result = 0.299 * color[0] + 0.587 * color[1] + 0.114 * color[2];
		
		return result;
		
	}
	else {
		printf("zero");
		return 0.0;
	}
}