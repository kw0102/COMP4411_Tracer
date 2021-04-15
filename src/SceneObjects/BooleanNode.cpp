#include "BooleanNode.h"

const Material& BooleanNode::getMaterial() const {
	return a->getMaterial();
};

void BooleanNode::setMaterial(Material* m) {
	return a->setMaterial(m);
};

bool UnionNode::intersect(const ray& r, isect& i) const
{
	isect aIsect, bIsect;
	bool aIntersects = a->intersect(r, aIsect);
	bool bIntersects = b->intersect(r, bIsect);

	if (!aIntersects && !bIntersects) return false;

	if (aIntersects && (aIsect.t < bIsect.t || !bIntersects)) {
		i = aIsect;
		i.obj = a;
	}
	else
	{
		i = bIsect;
		i.obj = b;
	}
	
	return true;
};

bool IntersectionNode::intersect(const ray& r, isect& i) const
{

	isect aIsect, bIsect;
	bool aIntersects = a->intersect(r, aIsect);
	bool bIntersects = b->intersect(r, bIsect);

	if (!aIntersects || !bIntersects) return false;

	SceneObject* nearObj = (aIsect.t < bIsect.t) ? a : b;
	SceneObject* farObj = (aIsect.t > bIsect.t) ? a : b;
	double tMin = (aIsect.t < bIsect.t) ? aIsect.t : bIsect.t;

	isect nearIsect, farIsect;
	ray r2(r.at(tMin + RAY_EPSILON), r.getDirection());

	nearObj->intersect(r2, nearIsect);
	farObj->intersect(r2, farIsect);

	if (farIsect.t < nearIsect.t)
	{
		i = farIsect;
		i.obj = farObj;
		i.t = tMin + farIsect.t;
		return true;
	}
	else
	{
		return false;
	}

};

bool SubtractNode::intersect(const ray& r, isect& i) const
{
	isect aInIsect, bInIsect;
	bool aInIntersects = a->intersect(r, aInIsect);
	bool bInIntersects = b->intersect(r, bInIsect);

	isect aOutIsect, bOutIsect;
	bool aOutIntersects = false, bOutIntersects = false;
	if (aInIntersects)
	{
		ray r2(r.at(aInIsect.t + RAY_EPSILON), r.getDirection());
		aOutIntersects = a->intersect(r2, aOutIsect);
		if (aOutIntersects) aOutIsect.t += aInIsect.t;
	}
	if (bInIntersects)
	{
		ray r2(r.at(bInIsect.t + RAY_EPSILON), r.getDirection());
		bOutIntersects = b->intersect(r2, bOutIsect);
		if (bOutIntersects) bOutIsect.t += bInIsect.t;
	}

	if (!aInIntersects)
	{
		return false;
	}
	else if (!bInIntersects)
	{
		// A
		i = aInIsect;
		i.obj = a;
		return true;
	}
	else if (aOutIntersects && bOutIntersects)
	{
		if (aInIsect.t < bInIsect.t)
		{
			// [A] B ..
			i = aInIsect;
			i.obj = a;
			return true;
		}

		if (bOutIsect.t < aInIsect.t)
		{
			// B B [A]
			i = aInIsect;
			i.obj = a;
			return true;
		}

		if (bOutIsect.t < aOutIsect.t)
		{
			// B A [B] A
			i = bOutIsect;
			i.N *= -1;
			i.obj = a;
			return true;
		}

		return false;

	}
	else if (!aOutIntersects && bOutIntersects)
	{
		if (aInIsect.t > bInIsect.t)
		{
			i = bInIsect;
			i.obj = a;
			return true;
		}
		else
		{
			i = aInIsect;
			i.obj = a;
			return true;
		}
	}
	else if (aOutIntersects && !bOutIntersects)
	{
		if (bInIsect.t < aInIsect.t)
		{
			i = bInIsect;
			i.obj = a;
			i.N *= -1;
			return true;
		}
		else
		{
			i = aInIsect;
			i.obj = a;
			return true;
		}
	}
	else
	{
		if (bInIsect.t < aInIsect.t)
		{
			i = bInIsect;
			i.obj = a;
			i.N *= -1;
			return true;
		}
		return false;
	}



	/*
	isect aIsect, bIsect;
	bool aIntersects = a->intersect(r, aIsect);
	bool bIntersects = b->intersect(r, bIsect);

	if (!aIntersects) return false;

	if (aIsect.t < bIsect.t || !bIntersects)
	{
		i = aIsect;
		return true;
	}

	// Enter substracting volume
	isect aIsect2, bIsect2;
	ray r2(r.at(bIsect.t + RAY_EPSILON), r.getDirection());

	aIntersects = a->intersect(r2, aIsect2);
	bIntersects = b->intersect(r2, bIsect2);

	if (!bIntersects) return false;

	if (aIsect2.t < bIsect2.t)
	{
		// B A ..
		isect aIsect3, bIsect3;
		ray r3(r2.at(aIsect2.t + RAY_EPSILON), r2.getDirection());
		aIntersects = a->intersect(r3, aIsect3);
		bIntersects = b->intersect(r3, bIsect3);

		if (!bIntersects) return false;

		if (aIntersects && aIsect3.t < bIsect3.t)
		{
			// B A A ..
			return false;
		}
		else
		{
			// B A [B] A ..
			i = bIsect3;
			i.obj = a;
			i.N *= -1;
			i.t = bIsect.t + aIsect2.t + bIsect3.t;
			return true;
		}

	}
	else
	{
		// B B ..
		i = aIsect2;
		i.obj = a;
		i.t = bIsect.t + aIsect2.t;
		return true;
	}
	*/
};