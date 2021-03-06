/*
Copyright (c) 2003-2006 Gino van den Bergen / Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/



#ifndef BT_SIMD__QUATERNION_H_
#define BT_SIMD__QUATERNION_H_


#include "btVector.h"


#ifdef BT_USE_DOUBLE_PRECISION
#define btQuaternionData btQuaternionDoubleData
#define btQuaternionDataName "btQuaternionDoubleData"
#else
#define btQuaternionData btQuaternionFloatData
#define btQuaternionDataName "btQuaternionFloatData"
#endif //BT_USE_DOUBLE_PRECISION



#if defined(BT_USE_SSE) 

#define vQInv (_mm_set_ps(+0.0f, -0.0f, -0.0f, -0.0f))
#define vPPPM (_mm_set_ps(-0.0f, +0.0f, +0.0f, +0.0f))

#elif defined(BT_USE_NEON)

const btSimdFloat4 ATTRIBUTE_ALIGNED16(vQInv) = {-0.0f, -0.0f, -0.0f, +0.0f};
const btSimdFloat4 ATTRIBUTE_ALIGNED16(vPPPM) = {+0.0f, +0.0f, +0.0f, -0.0f};

#endif


#ifdef __cplusplus

/**@brief The btQuaternion implements quaternion to perform linear algebra rotations in combination with btMatrix3x3, btVector3 and btTransform. */
class btQuaternion : public btVector {
public:
  /**@brief No initialization constructor */
	btQuaternion() {}

#if (defined(BT_USE_SSE_IN_API) && defined(BT_USE_SSE))|| defined(BT_USE_NEON) 
	// Set Vector 
	SIMD_FORCE_INLINE btQuaternion(const btSimdFloat4 vec)
	{
		mVec128 = vec;
	}

	SIMD_FORCE_INLINE btQuaternion(const btVector& rhs)
	{
		mVec128 = rhs.mVec128;
	}
#else
	SIMD_FORCE_INLINE btQuaternion(const btVector& v) : btVector(v) {
	}
#endif

	//		template <typename btScalar>
	//		explicit Quaternion(const btScalar *v) : Tuple4<btScalar>(v) {}
  /**@brief Constructor from scalars */
	btQuaternion(const btScalar& _x, const btScalar& _y, const btScalar& _z, const btScalar& _w) 
		: btVector(_x, _y, _z, _w) 
	{}
  /**@brief Axis angle Constructor
   * @param axis The axis which the rotation is around
   * @param angle The magnitude of the rotation around the angle (Radians) */
	btQuaternion(const btVector3& _axis, const btScalar& _angle) 
	{ 
		setRotation(_axis, _angle); 
	}
  /**@brief Constructor from Euler angles
   * @param yaw Angle around Y unless BT_EULER_DEFAULT_ZYX defined then Z
   * @param pitch Angle around X unless BT_EULER_DEFAULT_ZYX defined then Y
   * @param roll Angle around Z unless BT_EULER_DEFAULT_ZYX defined then X */
	btQuaternion(const btScalar& yaw, const btScalar& pitch, const btScalar& roll)
	{ 
#ifndef BT_EULER_DEFAULT_ZYX
		setEuler(yaw, pitch, roll); 
#else
		setEulerZYX(yaw, pitch, roll); 
#endif 
	}
  /**@brief Set the rotation using axis angle notation 
   * @param axis The axis around which to rotate
   * @param angle The magnitude of the rotation in Radians */
	SIMD_FORCE_INLINE void setRotation(const btVector3& axis, const btScalar& _angle);
	
  /**@brief Set the quaternion using Euler angles
   * @param yaw Angle around Y
   * @param pitch Angle around X
   * @param roll Angle around Z */
	SIMD_FORCE_INLINE void setEuler(const btScalar& yaw, const btScalar& pitch, const btScalar& roll);
	
  /**@brief Set the quaternion using euler angles 
   * @param yaw Angle around Z
   * @param pitch Angle around Y
   * @param roll Angle around X */
	SIMD_FORCE_INLINE void setEulerZYX(const btScalar& yaw, const btScalar& pitch, const btScalar& roll);
	
  /**@brief Add two quaternions
   * @param q The quaternion to add to this one */
	SIMD_FORCE_INLINE	btQuaternion& operator+=(const btQuaternion& q)
	{
		btVector_add(this, &q, BT_VEC4_MODE);
		return *this;
	}

  /**@brief Subtract out a quaternion
   * @param q The quaternion to subtract from this one */
	btQuaternion& operator-=(const btQuaternion& q) 
	{
		btVector_subtract(this, &q, BT_VEC4_MODE);
		return *this;
	}

  /**@brief Scale this quaternion
   * @param s The scalar to scale by */
	btQuaternion& operator*=(const btScalar& s)
	{
		btVector_scale(this, s, BT_VEC4_MODE);
		return *this;
	}

  /**@brief Multiply this quaternion by q on the right
   * @param q The other quaternion 
   * Equivilant to this = this * q */
	btQuaternion& operator*=(const btQuaternion& q);
	
  /**@brief Return the dot product between this quaternion and another
   * @param q The other quaternion */
	btScalar dot(const btQuaternion& q) const
	{
		return btVector_dot(this, &q, BT_VEC4_MODE);
	}

  /**@brief Return the length squared of the quaternion */
	btScalar length2() const
	{
		return btVector_length2(this, BT_VEC4_MODE);
	}

  /**@brief Return the length of the quaternion */
	btScalar length() const
	{
		return btVector_length(this, BT_VEC4_MODE);
	}

  /**@brief Normalize the quaternion 
   * Such that x^2 + y^2 + z^2 +w^2 = 1 */
	btQuaternion& normalize() 
	{
		btVector_normalize(this, BT_VEC4_MODE);
		return *this;
	}

  /**@brief Return a scaled version of this quaternion
   * @param s The scale factor */
	SIMD_FORCE_INLINE btQuaternion
	operator*(const btScalar& s) const
	{
		btQuaternion result;
		btVector_makeScaled(&result, this, s, BT_VEC4_MODE);
		return result;
	}

  /**@brief Return an inversely scaled versionof this quaternion
   * @param s The inverse scale factor */
	btQuaternion operator/(const btScalar& s) const
	{
		btQuaternion result;
		btVector_makeDivided(&result, this, s, BT_VEC4_MODE);
		return result;
	}

  /**@brief Inversely scale this quaternion
   * @param s The scale factor */
	btQuaternion& operator/=(const btScalar& s) 
	{
		btVector_divide(this, s, BT_VEC4_MODE);
	}

  /**@brief Return a normalized version of this quaternion */
	btQuaternion normalized() const 
	{
		return *this / length();
	} 
	/**@brief Return the ***half*** angle between this quaternion and the other
   * @param q The other quaternion */
	btScalar angle(const btQuaternion& q) const 
	{
		btScalar s = btSqrt(length2() * q.length2());
		btAssert(s != btScalar(0.0));
		return btAcos(dot(q) / s);
	}
	
	/**@brief Return the angle between this quaternion and the other along the shortest path
	* @param q The other quaternion */
	btScalar angleShortestPath(const btQuaternion& q) const 
	{
		btScalar s = btSqrt(length2() * q.length2());
		btAssert(s != btScalar(0.0));
		if (dot(q) < 0) // Take care of long angle case see http://en.wikipedia.org/wiki/Slerp
			return btAcos(dot(-q) / s) * btScalar(2.0);
		else 
			return btAcos(dot(q) / s) * btScalar(2.0);
	}

  /**@brief Return the angle of rotation represented by this quaternion */
	btScalar getAngle() const 
	{
		btScalar s = btScalar(2.) * btAcos(m_floats[3]);
		return s;
	}

	/**@brief Return the angle of rotation represented by this quaternion along the shortest path*/
	btScalar getAngleShortestPath() const 
	{
		btScalar s;
		if (dot(*this) < 0)
			s = btScalar(2.) * btAcos(m_floats[3]);
		else
			s = btScalar(2.) * btAcos(-m_floats[3]);

		return s;
	}


	/**@brief Return the axis of the rotation represented by this quaternion */
	btVector3 getAxis() const
	{
		btScalar s_squared = 1.f-m_floats[3]*m_floats[3];
		
		if (s_squared < btScalar(10.) * SIMD_EPSILON) //Check for divide by zero
			return btVector3(1.0, 0.0, 0.0);  // Arbitrary
		btScalar s = 1.f/btSqrt(s_squared);
		return btVector3(m_floats[0] * s, m_floats[1] * s, m_floats[2] * s);
	}

	/**@brief Return the inverse of this quaternion */
	btQuaternion inverse() const
	{
#if defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE)
		return btQuaternion(_mm_xor_ps(mVec128, vQInv));
#elif defined(BT_USE_NEON)
        return btQuaternion((btSimdFloat4)veorq_s32((int32x4_t)mVec128, (int32x4_t)vQInv));
#else	
		return btQuaternion(-m_floats[0], -m_floats[1], -m_floats[2], m_floats[3]);
#endif
	}

  /**@brief Return the sum of this quaternion and the other 
   * @param q2 The other quaternion */
	SIMD_FORCE_INLINE btQuaternion
	operator+(const btQuaternion& q2) const
	{
		btQuaternion result;
		btVector_makeSum(&result, this, &q2, BT_VEC4_MODE);
		return result;
	}

  /**@brief Return the difference between this quaternion and the other 
   * @param q2 The other quaternion */
	SIMD_FORCE_INLINE btQuaternion
	operator-(const btQuaternion& q2) const
	{
		btQuaternion result;
		btVector_makeDiff(&result, this, &q2, BT_VEC4_MODE);
		return result;
	}

  /**@brief Return the negative of this quaternion 
   * This simply negates each element */
	SIMD_FORCE_INLINE btQuaternion operator-() const
	{
		btQuaternion result;
		btVector_makeNegative(&result, this, BT_VEC4_MODE);
		return result;
	}
  /**@todo document this and it's use */
	SIMD_FORCE_INLINE btQuaternion farthest( const btQuaternion& qd) const 
	{
		btQuaternion diff,sum;
		diff = *this - qd;
		sum = *this + qd;
		if( diff.dot(diff) > sum.dot(sum) )
			return qd;
		return (-qd);
	}

	/**@todo document this and it's use */
	SIMD_FORCE_INLINE btQuaternion nearest( const btQuaternion& qd) const 
	{
		btQuaternion diff,sum;
		diff = *this - qd;
		sum = *this + qd;
		if( diff.dot(diff) < sum.dot(sum) )
			return qd;
		return (-qd);
	}


  /**@brief Return the quaternion which is the result of Spherical Linear Interpolation between this and the other quaternion
   * @param q The other quaternion to interpolate with 
   * @param t The ratio between this and q to interpolate.  If t = 0 the result is this, if t=1 the result is q.
   * Slerp interpolates assuming constant velocity.  */
	btQuaternion slerp(const btQuaternion& q, const btScalar& t) const
	{
	  btScalar magnitude = btSqrt(length2() * q.length2()); 
	  btAssert(magnitude > btScalar(0));

    btScalar product = dot(q) / magnitude;
    if (btFabs(product) < btScalar(1))
		{
      // Take care of long angle case see http://en.wikipedia.org/wiki/Slerp
      const btScalar sign = (product < 0) ? btScalar(-1) : btScalar(1);

      const btScalar theta = btAcos(sign * product);
      const btScalar s1 = btSin(sign * t * theta);   
      const btScalar d = btScalar(1.0) / btSin(theta);
      const btScalar s0 = btSin((btScalar(1.0) - t) * theta);

      return btQuaternion(
          (m_floats[0] * s0 + q.x() * s1) * d,
          (m_floats[1] * s0 + q.y() * s1) * d,
          (m_floats[2] * s0 + q.z() * s1) * d,
          (m_floats[3] * s0 + q.m_floats[3] * s1) * d);
		}
		else
		{
			return *this;
		}
	}

	static const btQuaternion&	getIdentity()
	{
		static const btQuaternion identityQuat(btScalar(0.),btScalar(0.),btScalar(0.),btScalar(1.));
		return identityQuat;
	}
};

typedef btVectorFloatData btQuaternionFloatData;
typedef btVectorDoubleData btQuaternionDoubleData;

/**@brief Return the product of two quaternions */
SIMD_FORCE_INLINE btQuaternion
operator*(const btQuaternion& q1, const btQuaternion& q2);

SIMD_FORCE_INLINE btQuaternion
operator*(const btQuaternion& q, const btVector3& w);

SIMD_FORCE_INLINE btQuaternion
operator*(const btVector3& w, const btQuaternion& q);


/**@brief Calculate the dot product between two quaternions */
SIMD_FORCE_INLINE btScalar 
dot(const btQuaternion& q1, const btQuaternion& q2) 
{ 
	return q1.dot(q2); 
}


/**@brief Return the length of a quaternion */
SIMD_FORCE_INLINE btScalar
length(const btQuaternion& q) 
{ 
	return q.length(); 
}

/**@brief Return the angle between two quaternions*/
SIMD_FORCE_INLINE btScalar
btAngle(const btQuaternion& q1, const btQuaternion& q2) 
{ 
	return q1.angle(q2); 
}

/**@brief Return the inverse of a quaternion*/
SIMD_FORCE_INLINE btQuaternion
inverse(const btQuaternion& q) 
{
	return q.inverse();
}

/**@brief Return the result of spherical linear interpolation betwen two quaternions 
 * @param q1 The first quaternion
 * @param q2 The second quaternion 
 * @param t The ration between q1 and q2.  t = 0 return q1, t=1 returns q2 
 * Slerp assumes constant velocity between positions. */
SIMD_FORCE_INLINE btQuaternion
slerp(const btQuaternion& q1, const btQuaternion& q2, const btScalar& t) 
{
	return q1.slerp(q2, t);
}

SIMD_FORCE_INLINE btVector3 
quatRotate(const btQuaternion& rotation, const btVector3& v) 
{
	btQuaternion q = rotation * v;
	q *= rotation.inverse();
#if defined BT_USE_SIMD_VECTOR3 && defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE)
	return btVector3(_mm_and_ps(q.get128(), btvFFF0fMask));
#elif defined(BT_USE_NEON)
    return btVector3((float32x4_t)vandq_s32((int32x4_t)q.get128(), btvFFF0Mask));
#else	
	return btVector3(q.getX(),q.getY(),q.getZ());
#endif
}

SIMD_FORCE_INLINE btQuaternion 
shortestArcQuat(const btVector3& v0, const btVector3& v1) // Game Programming Gems 2.10. make sure v0,v1 are normalized
{
	btVector3 c = v0.cross(v1);
	btScalar  d = v0.dot(v1);

	if (d < -1.0 + SIMD_EPSILON)
	{
		btVector3 n,unused;
		btPlaneSpace1(v0,n,unused);
		return btQuaternion(n.x(),n.y(),n.z(),0.0f); // just pick any vector that is orthogonal to v0
	}

	btScalar  s = btSqrt((1.0f + d) * 2.0f);
	btScalar rs = 1.0f / s;

	return btQuaternion(c.getX()*rs,c.getY()*rs,c.getZ()*rs,s * 0.5f);
}

SIMD_FORCE_INLINE btQuaternion 
shortestArcQuatNormalize2(btVector3& v0,btVector3& v1)
{
	v0.normalize();
	v1.normalize();
	return shortestArcQuat(v0,v1);
}

#else //__cplusplus
typedef btVector btQuaternion;

// Fake constructor for non-C++
#define btQuaternion(x, y, z, w) btVector(x, y, z, w)
#endif //__cplusplus

#define btQuaternion_copy(target, src) btVector_copy(target, src)

#if (defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE) )|| defined (BT_USE_NEON)
#define btQuaternion_fromSimd(v) btVector_fromSimd(v)
#endif // #if (defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE) )|| defined (BT_USE_NEON)

#define btQuaternion_cmp(v1, v2) btVector_cmp(v1, v2)

#define btQuaternion_add(self, v) btVector_add(self, v, BT_VEC4_MODE)

#define btQuaternion_makeSum(result, a, b) btVector_makeSum(result, a, b, BT_VEC4_MODE)

#define btQuaternion_subtract(self, v) btVector_subtract(self, v, BT_VEC4_MODE)

#define btQuaternion_makeDiff(result, a, b) btVector_makeDiff(result, a, b, BT_VEC4_MODE)

#define btQuaternion_scale(self, s) btVector_scale(self, s, BT_VEC4_MODE)

#define btQuaternion_makeScaled(result, v, s) btVector_makeScaled(result, v, s, BT_VEC4_MODE)

#define btQuaternion_divide(self, s) btVector_divide(self, s, BT_VEC4_MODE)

#define btQuaternion_makeDivided(result, v, s) btVector_makeDivided(result, v, s, BT_VEC4_MODE)

#define btQuaternion_negate(self) btVector_negate(self, BT_VEC4_MODE)

#define btQuaternion_makeNegative(result, v) btVector_makeNegative(result, v, BT_VEC4_MODE)

#define btQuaternion_multiply(self, v) btVector_multiply(self, v, BT_VEC4_MODE)

#define btQuaternion_makeMultiplication(result, a, b) btVector_makeMultiplication(result, a, b, BT_VEC4_MODE)

#define btQuaternion_multiplyInv(self, v) btVector_multiplyInv(self, v, BT_VEC4_MODE)

#define btQuaternion_makeMultiplicationInv(result, a, b) btVector_makeMultiplicationInv(result, a, b, BT_VEC4_MODE)

#define btQuaternion_dot(a, b) btVector_dot(a, b, BT_VEC4_MODE)

#define btQuaternion_length2(self) btVector_length2(self, BT_VEC4_MODE)

#define btQuaternion_length(self) btVector_length(self, BT_VEC4_MODE)

#define btQuaternion_normalize(self) btVector_normalize(self, BT_VEC4_MODE)

static SIMD_FORCE_INLINE btQuaternion btQuaternion_create(btScalar x, btScalar y, btScalar z, btScalar w) {
	return btQuaternion(x, y, z, w);
}

// TODO: Add btQuaternion common functions here

/**@brief Set the rotation using axis angle notation
 * @param self The quaternion to change
 * @param axis The axis around which to rotate
 * @param angle The magnitude of the rotation in Radians */
static SIMD_FORCE_INLINE void btQuaternion_setRotation(btQuaternion* BT_RESTRICT self, const btVector3* BT_RESTRICT axis, btScalar angle) {
	btScalar d = btVector3_length(axis);
	btAssert(d != btScalar(0.0));
	btScalar halfAngle = angle * btScalar(0.5);
	btScalar s = btSin(halfAngle) / d;
	
	self->m_floats[0] = axis->m_floats[0] * s;
	self->m_floats[1] = axis->m_floats[1] * s;
	self->m_floats[2] = axis->m_floats[2] * s;
	self->m_floats[3] = btCos(halfAngle);
}
	
/**@brief Set the quaternion using Euler angles
 * @param self The quaternion to change
 * @param yaw Angle around Y
 * @param pitch Angle around X
 * @param roll Angle around Z */
static SIMD_FORCE_INLINE void btQuaternion_setEuler(btQuaternion* self, const btScalar yaw, const btScalar pitch, const btScalar roll)
{
	btScalar halfYaw = btScalar(yaw) * btScalar(0.5);
	btScalar halfPitch = btScalar(pitch) * btScalar(0.5);
	btScalar halfRoll = btScalar(roll) * btScalar(0.5);
	btScalar cosYaw = btCos(halfYaw);
	btScalar sinYaw = btSin(halfYaw);
	btScalar cosPitch = btCos(halfPitch);
	btScalar sinPitch = btSin(halfPitch);
	btScalar cosRoll = btCos(halfRoll);
	btScalar sinRoll = btSin(halfRoll);
	
	self->m_floats[0] = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
	self->m_floats[1] = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
	self->m_floats[2] = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
	self->m_floats[3] = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
}

/**@brief Set the quaternion using euler angles
 * @param self The quaternion to change
 * @param yaw Angle around Z
 * @param pitch Angle around Y
 * @param roll Angle around X */
static SIMD_FORCE_INLINE void btQuaternion_setEulerZYX(btQuaternion* self, const btScalar yaw, const btScalar pitch, const btScalar roll)
{
	btScalar halfYaw = btScalar(yaw) * btScalar(0.5);  
	btScalar halfPitch = btScalar(pitch) * btScalar(0.5);  
	btScalar halfRoll = btScalar(roll) * btScalar(0.5);  
	btScalar cosYaw = btCos(halfYaw);
	btScalar sinYaw = btSin(halfYaw);
	btScalar cosPitch = btCos(halfPitch);
	btScalar sinPitch = btSin(halfPitch);
	btScalar cosRoll = btCos(halfRoll);
	btScalar sinRoll = btSin(halfRoll);
	
	self->m_floats[0] = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw; //x
	self->m_floats[1] = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw; //y
	self->m_floats[2] = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw; //z
	self->m_floats[3] = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw; //formerly yzx
}

/**@brief Multiply this quaternion by q on the right
 * @param self The quaternion to change
 * @param q The other quaternion 
 */
static SIMD_FORCE_INLINE void btQuaternion_multiplyQuat(btQuaternion* BT_RESTRICT self, const btQuaternion* BT_RESTRICT q)
{
#if defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE)
	__m128 vQ2 = q->mVec128;
	
	__m128 A1 = bt_pshufd_ps(self->mVec128, BT_SHUFFLE(0,1,2,0));
	__m128 B1 = bt_pshufd_ps(vQ2, BT_SHUFFLE(3,3,3,0));
	
	A1 = A1 * B1;
	
	__m128 A2 = bt_pshufd_ps(self->mVec128, BT_SHUFFLE(1,2,0,1));
	__m128 B2 = bt_pshufd_ps(vQ2, BT_SHUFFLE(2,0,1,1));
	
	A2 = A2 * B2;
	
	B1 = bt_pshufd_ps(self->mVec128, BT_SHUFFLE(2,0,1,2));
	B2 = bt_pshufd_ps(vQ2, BT_SHUFFLE(1,2,0,2));
	
	B1 = B1 * B2;	//	A3 *= B3
	
	self->mVec128 = bt_splat_ps(self->mVec128, 3);	//	A0
	self->mVec128 = self->mVec128 * vQ2;	//	A0 * B0
	
	A1 = A1 + A2;	//	AB12
	self->mVec128 = self->mVec128 - B1;	//	AB03 = AB0 - AB3 
	A1 = _mm_xor_ps(A1, vPPPM);	//	change sign of the last element
	self->mVec128 = self->mVec128 + A1;	//	AB03 + AB12

#elif defined(BT_USE_NEON)     

    float32x4_t vQ1 = self->mVec128;
    float32x4_t vQ2 = q->mVec128;
    float32x4_t A0, A1, B1, A2, B2, A3, B3;
    float32x2_t vQ1zx, vQ2wx, vQ1yz, vQ2zx, vQ2yz, vQ2xz;
    
    {
    float32x2x2_t tmp;
    tmp = vtrn_f32( vget_high_f32(vQ1), vget_low_f32(vQ1) );       // {z x}, {w y}
    vQ1zx = tmp.val[0];

    tmp = vtrn_f32( vget_high_f32(vQ2), vget_low_f32(vQ2) );       // {z x}, {w y}
    vQ2zx = tmp.val[0];
    }
    vQ2wx = vext_f32(vget_high_f32(vQ2), vget_low_f32(vQ2), 1); 

    vQ1yz = vext_f32(vget_low_f32(vQ1), vget_high_f32(vQ1), 1);

    vQ2yz = vext_f32(vget_low_f32(vQ2), vget_high_f32(vQ2), 1);
    vQ2xz = vext_f32(vQ2zx, vQ2zx, 1);

    A1 = vcombine_f32(vget_low_f32(vQ1), vQ1zx);                    // X Y  z x 
    B1 = vcombine_f32(vdup_lane_f32(vget_high_f32(vQ2), 1), vQ2wx); // W W  W X 

    A2 = vcombine_f32(vQ1yz, vget_low_f32(vQ1));
    B2 = vcombine_f32(vQ2zx, vdup_lane_f32(vget_low_f32(vQ2), 1));

    A3 = vcombine_f32(vQ1zx, vQ1yz);        // Z X Y Z
    B3 = vcombine_f32(vQ2yz, vQ2xz);        // Y Z x z

    A1 = vmulq_f32(A1, B1);
    A2 = vmulq_f32(A2, B2);
    A3 = vmulq_f32(A3, B3);	//	A3 *= B3
    A0 = vmulq_lane_f32(vQ2, vget_high_f32(vQ1), 1); //	A0 * B0

    A1 = vaddq_f32(A1, A2);	//	AB12 = AB1 + AB2
    A0 = vsubq_f32(A0, A3);	//	AB03 = AB0 - AB3 
    
    //	change the sign of the last element
    A1 = (btSimdFloat4)veorq_s32((int32x4_t)A1, (int32x4_t)vPPPM);	
    A0 = vaddq_f32(A0, A1);	//	AB03 + AB12
    
    self->mVec128 = A0;
    
#else

	btScalar Ax = self->m_floats[0], Ay = self->m_floats[1], Az = self->m_floats[2], Aw = self->m_floats[3];
	btScalar Bx = q->m_floats[0], By = q->m_floats[1], Bz = q->m_floats[2], Bw = q->m_floats[3];
	
	self->m_floats[0] = Aw * Bx + Ax * Bw + Ay * Bz - Az * By;
	self->m_floats[1] = Aw * By + Ay * Bw + Az * Bx - Ax * Bz;
	self->m_floats[2] = Aw * Bz + Az * Bw + Ax * By - Ay * Bx;
	self->m_floats[3] = Aw * Bw - Ax * Bx - Ay * By - Az * Bz;
#endif
}

/**@brief Return the product of two quaternions */
static SIMD_FORCE_INLINE void btQuaternion_makeQuatMultiplication(btQuaternion* BT_RESTRICT result, const btQuaternion* BT_RESTRICT q1, const btQuaternion* BT_RESTRICT q2) 
{
#if defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE)
	__m128 vQ1 = q1->mVec128;
	__m128 vQ2 = q2->mVec128;
	__m128 A0, A1, B1, A2, B2;
    
	A1 = bt_pshufd_ps(vQ1, BT_SHUFFLE(0,1,2,0)); // X Y  z x     //      vtrn
	B1 = bt_pshufd_ps(vQ2, BT_SHUFFLE(3,3,3,0)); // W W  W X     // vdup vext

	A1 = A1 * B1;
	
	A2 = bt_pshufd_ps(vQ1, BT_SHUFFLE(1,2,0,1)); // Y Z  X Y     // vext 
	B2 = bt_pshufd_ps(vQ2, BT_SHUFFLE(2,0,1,1)); // z x  Y Y     // vtrn vdup

	A2 = A2 * B2;

	B1 = bt_pshufd_ps(vQ1, BT_SHUFFLE(2,0,1,2)); // z x Y Z      // vtrn vext
	B2 = bt_pshufd_ps(vQ2, BT_SHUFFLE(1,2,0,2)); // Y Z x z      // vext vtrn
	
	B1 = B1 * B2;	//	A3 *= B3

	A0 = bt_splat_ps(vQ1, 3);	//	A0
	A0 = A0 * vQ2;	//	A0 * B0

	A1 = A1 + A2;	//	AB12
	A0 =  A0 - B1;	//	AB03 = AB0 - AB3 
	
    A1 = _mm_xor_ps(A1, vPPPM);	//	change sign of the last element
	A0 = A0 + A1;	//	AB03 + AB12
	
	result->mVec128 = A0;

#elif defined(BT_USE_NEON)     

	float32x4_t vQ1 = q1->mVec128;
	float32x4_t vQ2 = q2->mVec128;
	float32x4_t A0, A1, B1, A2, B2, A3, B3;
    float32x2_t vQ1zx, vQ2wx, vQ1yz, vQ2zx, vQ2yz, vQ2xz;
    
    {
    float32x2x2_t tmp;
    tmp = vtrn_f32( vget_high_f32(vQ1), vget_low_f32(vQ1) );       // {z x}, {w y}
    vQ1zx = tmp.val[0];

    tmp = vtrn_f32( vget_high_f32(vQ2), vget_low_f32(vQ2) );       // {z x}, {w y}
    vQ2zx = tmp.val[0];
    }
    vQ2wx = vext_f32(vget_high_f32(vQ2), vget_low_f32(vQ2), 1); 

    vQ1yz = vext_f32(vget_low_f32(vQ1), vget_high_f32(vQ1), 1);

    vQ2yz = vext_f32(vget_low_f32(vQ2), vget_high_f32(vQ2), 1);
    vQ2xz = vext_f32(vQ2zx, vQ2zx, 1);

    A1 = vcombine_f32(vget_low_f32(vQ1), vQ1zx);                    // X Y  z x 
    B1 = vcombine_f32(vdup_lane_f32(vget_high_f32(vQ2), 1), vQ2wx); // W W  W X 

	A2 = vcombine_f32(vQ1yz, vget_low_f32(vQ1));
    B2 = vcombine_f32(vQ2zx, vdup_lane_f32(vget_low_f32(vQ2), 1));

    A3 = vcombine_f32(vQ1zx, vQ1yz);        // Z X Y Z
    B3 = vcombine_f32(vQ2yz, vQ2xz);        // Y Z x z

	A1 = vmulq_f32(A1, B1);
	A2 = vmulq_f32(A2, B2);
	A3 = vmulq_f32(A3, B3);	//	A3 *= B3
	A0 = vmulq_lane_f32(vQ2, vget_high_f32(vQ1), 1); //	A0 * B0

	A1 = vaddq_f32(A1, A2);	//	AB12 = AB1 + AB2
	A0 = vsubq_f32(A0, A3);	//	AB03 = AB0 - AB3 
	
    //	change the sign of the last element
    A1 = (btSimdFloat4)veorq_s32((int32x4_t)A1, (int32x4_t)vPPPM);	
	A0 = vaddq_f32(A0, A1);	//	AB03 + AB12
	
	result->mVec128 = A0;

#else

	btScalar Ax = q1->m_floats[0], Ay = q1->m_floats[1], Az = q1->m_floats[2], Aw = q1->m_floats[3];
	btScalar Bx = q2->m_floats[0], By = q2->m_floats[1], Bz = q2->m_floats[2], Bw = q2->m_floats[3];
	
	result->m_floats[0] = Aw * Bx + Ax * Bw + Ay * Bz - Az * By;
	result->m_floats[1] = Aw * By + Ay * Bw + Az * Bx - Ax * Bz;
	result->m_floats[2] = Aw * Bz + Az * Bw + Ax * By - Ay * Bx;
	result->m_floats[3] = Aw * Bw - Ax * Bx - Ay * By - Az * Bz;
#endif
}

static SIMD_FORCE_INLINE void btQuaternion_makeQuatVector3Multiplication(btQuaternion* BT_RESTRICT result, const btQuaternion* BT_RESTRICT q, const btVector3* BT_RESTRICT v) 
{
#if defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE)
	__m128 vQ1 = q->mVec128;
	__m128 vQ2 = v->mVec128;
	__m128 A1, B1, A2, B2, A3, B3;
	
	A1 = bt_pshufd_ps(vQ1, BT_SHUFFLE(3,3,3,0));
	B1 = bt_pshufd_ps(vQ2, BT_SHUFFLE(0,1,2,0));

	A1 = A1 * B1;
	
	A2 = bt_pshufd_ps(vQ1, BT_SHUFFLE(1,2,0,1));
	B2 = bt_pshufd_ps(vQ2, BT_SHUFFLE(2,0,1,1));

	A2 = A2 * B2;

	A3 = bt_pshufd_ps(vQ1, BT_SHUFFLE(2,0,1,2));
	B3 = bt_pshufd_ps(vQ2, BT_SHUFFLE(1,2,0,2));
	
	A3 = A3 * B3;	//	A3 *= B3

	A1 = A1 + A2;	//	AB12
	A1 = _mm_xor_ps(A1, vPPPM);	//	change sign of the last element
    A1 = A1 - A3;	//	AB123 = AB12 - AB3 
	
	result->mVec128 = A1;
    
#elif defined(BT_USE_NEON)     

	float32x4_t vQ1 = q->mVec128;
	float32x4_t vQ2 = v->mVec128;
	float32x4_t A1, B1, A2, B2, A3, B3;
    float32x2_t vQ1wx, vQ2zx, vQ1yz, vQ2yz, vQ1zx, vQ2xz;
    
    vQ1wx = vext_f32(vget_high_f32(vQ1), vget_low_f32(vQ1), 1); 
    {
    float32x2x2_t tmp;

    tmp = vtrn_f32( vget_high_f32(vQ2), vget_low_f32(vQ2) );       // {z x}, {w y}
    vQ2zx = tmp.val[0];

    tmp = vtrn_f32( vget_high_f32(vQ1), vget_low_f32(vQ1) );       // {z x}, {w y}
    vQ1zx = tmp.val[0];
    }

    vQ1yz = vext_f32(vget_low_f32(vQ1), vget_high_f32(vQ1), 1);

    vQ2yz = vext_f32(vget_low_f32(vQ2), vget_high_f32(vQ2), 1);
    vQ2xz = vext_f32(vQ2zx, vQ2zx, 1);

    A1 = vcombine_f32(vdup_lane_f32(vget_high_f32(vQ1), 1), vQ1wx); // W W  W X 
    B1 = vcombine_f32(vget_low_f32(vQ2), vQ2zx);                    // X Y  z x 

	A2 = vcombine_f32(vQ1yz, vget_low_f32(vQ1));
    B2 = vcombine_f32(vQ2zx, vdup_lane_f32(vget_low_f32(vQ2), 1));

    A3 = vcombine_f32(vQ1zx, vQ1yz);        // Z X Y Z
    B3 = vcombine_f32(vQ2yz, vQ2xz);        // Y Z x z

	A1 = vmulq_f32(A1, B1);
	A2 = vmulq_f32(A2, B2);
	A3 = vmulq_f32(A3, B3);	//	A3 *= B3

	A1 = vaddq_f32(A1, A2);	//	AB12 = AB1 + AB2
	
    //	change the sign of the last element
    A1 = (btSimdFloat4)veorq_s32((int32x4_t)A1, (int32x4_t)vPPPM);	
	
    A1 = vsubq_f32(A1, A3);	//	AB123 = AB12 - AB3
	
	result->mVec128 = A1;
    
#else

	btScalar Ax = q->m_floats[0], Ay = q->m_floats[1], Az = q->m_floats[2], Aw = q->m_floats[3];
	btScalar Bx = v->m_floats[0], By = v->m_floats[1], Bz = v->m_floats[2];
	
	result->m_floats[0] = Aw * Bx + Ay * Bz - Az * By;
	result->m_floats[1] = Aw * By + Az * Bx - Ax * Bz;
	result->m_floats[2] = Aw * Bz + Ax * By - Ay * Bx;
	result->m_floats[3] = -(Ax * Bx + Ay * By + Az * Bz);
#endif
}

static SIMD_FORCE_INLINE void btQuaternion_makeVector3QuatMultiplication(btQuaternion* BT_RESTRICT result, const btVector3* BT_RESTRICT v, const btQuaternion* BT_RESTRICT q) 
{
#if defined (BT_USE_SSE_IN_API) && defined (BT_USE_SSE)
	__m128 vQ1 = v->mVec128;
	__m128 vQ2 = q->mVec128;
	__m128 A1, B1, A2, B2, A3, B3;
	
	A1 = bt_pshufd_ps(vQ1, BT_SHUFFLE(0,1,2,0));  // X Y  z x
	B1 = bt_pshufd_ps(vQ2, BT_SHUFFLE(3,3,3,0));  // W W  W X 

	A1 = A1 * B1;
	
	A2 = bt_pshufd_ps(vQ1, BT_SHUFFLE(1,2,0,1));
	B2 = bt_pshufd_ps(vQ2, BT_SHUFFLE(2,0,1,1));

	A2 = A2 *B2;

	A3 = bt_pshufd_ps(vQ1, BT_SHUFFLE(2,0,1,2));
	B3 = bt_pshufd_ps(vQ2, BT_SHUFFLE(1,2,0,2));
	
	A3 = A3 * B3;	//	A3 *= B3

	A1 = A1 + A2;	//	AB12
	A1 = _mm_xor_ps(A1, vPPPM);	//	change sign of the last element
	A1 = A1 - A3;	//	AB123 = AB12 - AB3 
	
	result->mVec128 = A1;

#elif defined(BT_USE_NEON)     

	float32x4_t vQ1 = v->mVec128;
	float32x4_t vQ2 = q->mVec128;
	float32x4_t  A1, B1, A2, B2, A3, B3;
    float32x2_t vQ1zx, vQ2wx, vQ1yz, vQ2zx, vQ2yz, vQ2xz;
    
    {
    float32x2x2_t tmp;
   
    tmp = vtrn_f32( vget_high_f32(vQ1), vget_low_f32(vQ1) );       // {z x}, {w y}
    vQ1zx = tmp.val[0];

    tmp = vtrn_f32( vget_high_f32(vQ2), vget_low_f32(vQ2) );       // {z x}, {w y}
    vQ2zx = tmp.val[0];
    }
    vQ2wx = vext_f32(vget_high_f32(vQ2), vget_low_f32(vQ2), 1); 

    vQ1yz = vext_f32(vget_low_f32(vQ1), vget_high_f32(vQ1), 1);

    vQ2yz = vext_f32(vget_low_f32(vQ2), vget_high_f32(vQ2), 1);
    vQ2xz = vext_f32(vQ2zx, vQ2zx, 1);

    A1 = vcombine_f32(vget_low_f32(vQ1), vQ1zx);                    // X Y  z x 
    B1 = vcombine_f32(vdup_lane_f32(vget_high_f32(vQ2), 1), vQ2wx); // W W  W X 

	A2 = vcombine_f32(vQ1yz, vget_low_f32(vQ1));
    B2 = vcombine_f32(vQ2zx, vdup_lane_f32(vget_low_f32(vQ2), 1));

    A3 = vcombine_f32(vQ1zx, vQ1yz);        // Z X Y Z
    B3 = vcombine_f32(vQ2yz, vQ2xz);        // Y Z x z

	A1 = vmulq_f32(A1, B1);
	A2 = vmulq_f32(A2, B2);
	A3 = vmulq_f32(A3, B3);	//	A3 *= B3

	A1 = vaddq_f32(A1, A2);	//	AB12 = AB1 + AB2
	
    //	change the sign of the last element
    A1 = (btSimdFloat4)veorq_s32((int32x4_t)A1, (int32x4_t)vPPPM);	
	
    A1 = vsubq_f32(A1, A3);	//	AB123 = AB12 - AB3
	
	result->mVec128 = A1;
    
#else

	btScalar Ax = v->m_floats[0], Ay = v->m_floats[1], Az = v->m_floats[2];
	btScalar Bx = q->m_floats[0], By = q->m_floats[1], Bz = q->m_floats[2], Bw = q->m_floats[3];
	
	result->m_floats[0] = Ax * Bw + Ay * Bz - Az * By;
	result->m_floats[1] = Ay * Bw + Az * Bx - Ax * Bz;
	result->m_floats[2] = Az * Bw + Ax * By - Ay * Bx;
	result->m_floats[3] = -(Ax * Bx + Ay * By + Az * Bz);
#endif
}

// Advanced constrctors here

/**@brief Axis angle Constructor
 * @param axis The axis which the rotation is around
 * @param angle The magnitude of the rotation around the angle (Radians) */
static SIMD_FORCE_INLINE btQuaternion btQuaternion_fromAxisAngle(const btVector3* axis, btScalar angle) {
	btQuaternion quat;
	btQuaternion_setRotation(&quat, axis, angle);
	return quat;
}

/**@brief Constructor from Euler angles
 * @param yaw Angle around Y unless BT_EULER_DEFAULT_ZYX defined then Z
 * @param pitch Angle around X unless BT_EULER_DEFAULT_ZYX defined then Y
 * @param roll Angle around Z unless BT_EULER_DEFAULT_ZYX defined then X */
static SIMD_FORCE_INLINE btQuaternion btQuaternion_fromEuler(btScalar yaw, btScalar pitch, btScalar roll) {
	btQuaternion quat;
#ifndef BT_EULER_DEFAULT_ZYX
		btQuaternion_setEuler(&quat, yaw, pitch, roll);
#else
		btQuaternion_setEulerZYX(&quat, yaw, pitch, roll);
#endif
	return quat;
}


#ifdef __cplusplus

/**@brief Set the rotation using axis angle notation 
 * @param axis The axis around which to rotate
 * @param angle The magnitude of the rotation in Radians */
void SIMD_FORCE_INLINE btQuaternion::setRotation(const btVector3& axis, const btScalar& _angle)
{
	btQuaternion_setRotation(this, &axis, _angle);
}
	
/**@brief Set the quaternion using Euler angles
 * @param yaw Angle around Y
 * @param pitch Angle around X
 * @param roll Angle around Z */
SIMD_FORCE_INLINE void btQuaternion::setEuler(const btScalar& yaw, const btScalar& pitch, const btScalar& roll)
{
	btQuaternion_setEuler(this, yaw, pitch, roll);
}

/**@brief Set the quaternion using euler angles 
 * @param yaw Angle around Z
 * @param pitch Angle around Y
 * @param roll Angle around X */
SIMD_FORCE_INLINE void btQuaternion::setEulerZYX(const btScalar& yaw, const btScalar& pitch, const btScalar& roll)
{
	btQuaternion_setEulerZYX(this, yaw, pitch, roll);
}

/**@brief Multiply this quaternion by q on the right
 * @param q The other quaternion 
 * Equivilant to this = this * q */
SIMD_FORCE_INLINE btQuaternion& btQuaternion::operator*=(const btQuaternion& q)
{
	btQuaternion_multiplyQuat(this, &q);
	return *this;
}

/**@brief Return the product of two quaternions */
SIMD_FORCE_INLINE btQuaternion
operator*(const btQuaternion& q1, const btQuaternion& q2) 
{
	btQuaternion result;
	btQuaternion_makeQuatMultiplication(&result, &q1, &q2);
	return result;
}

SIMD_FORCE_INLINE btQuaternion
operator*(const btQuaternion& q, const btVector3& v)
{
	btQuaternion result;
	btQuaternion_makeQuatVector3Multiplication(&result, &q, &v);
	return result;
}

SIMD_FORCE_INLINE btQuaternion
operator*(const btVector3& v, const btQuaternion& q)
{
	btQuaternion result;
	btQuaternion_makeVector3QuatMultiplication(&result, &v, &q);
	return result;
}

#endif //__cplusplus

#endif //BT_SIMD__QUATERNION_H_
