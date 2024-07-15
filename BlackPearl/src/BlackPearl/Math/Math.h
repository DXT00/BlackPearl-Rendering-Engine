#pragma once
#include <random>
#include "glm/glm.hpp"

//namespace dm = BlackPearl::math;

#include "basics.h"
#include "vector.h"
#include "matrix.h"
#include "affine.h"
#include "box.h"
#include "color.h"
#include "quat.h"
#include "sphere.h"
#include "frustum.h"

namespace BlackPearl {

	class Math {
	public:

		static int Rand_I();
		static unsigned int Rand_UI();
		static float Rand_F();
		static double Rand_D();
		static const float PI;
		static size_t Fit2Square(size_t size);

		template <typename T>
		static T Min(const std::vector<T>& v)
		{
			if (v.empty()) {
				return static_cast<T>(0);
			}
			T min_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				min_v = glm::min(min_v, v[i]);
			}
			return min_v;
		}

		//template <typename T>
		static math::float3 Min(const math::float3& a, const math::float3& b)
		{
			math::float3 ret;
			ret.x = math::min(a.x, b.x);
			ret.y = math::min(a.y, b.y);
			ret.z = math::min(a.z, b.z);

			return ret;
		}

		//template <typename T>
		static float Max(const std::vector<float>& v)
		{
			if (v.empty()) {
				return static_cast<float>(0);
			}
			float max_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				max_v = glm::max<float>(max_v, v[i]);
			}
			return max_v;
		}

		//template <typename T>
		static math::float3 Max(const math::float3& a, const math::float3& b)
		{
			math::float3 ret;
			ret.x = math::max(a.x, b.x);
			ret.y = math::max(a.y, b.y);
			ret.z = math::max(a.z, b.z);

			return ret;
		}


		template <typename T>
		static T Min(T a, T b)
		{
			return (a < b) ? a : b;
		}

		template <typename T>
		static T Max(T a, T b)
		{
			return (a > b) ? a : b;
		}

		static glm::vec3 Max(const std::vector<glm::vec3>& v)
		{
			if (v.empty()) {
				return static_cast<glm::vec3>(0);
			}
			glm::vec3 max_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				max_v = glm::max(max_v, v[i]);
			}
			return max_v;
		}
		static math::float2 ToFloat2(const glm::vec2& v) {
			return math::float2(v.x, v.y);
		}


		static math::float3 ToFloat3(const glm::vec3& v) {
			return math::float3(v.x,v.y,v.z);
		}

		static math::float4 ToFloat4(const glm::vec4& v) {
			return math::float4(v.x, v.y, v.z, v.w);
		}

		static glm::vec2  ToVec2(const math::float2& v) {
			return glm::vec2(v.x, v.y);
		}

		static glm::vec3 ToVec3(const math::float3& v) {
			return glm::vec3(v.x, v.y, v.z);
		}

		static glm::vec4 ToVec4(const math::float4& v) {
			return glm::vec4(v.x, v.y, v.z, v.w);
		}

		static math::float4x4 ToFloat4x4(const glm::mat4& m) {
			math::float4x4 ret;
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
					ret[i][j] = m[i][j];
			}
			return ret;
		}



		/**
		 * Returns non-zero if any element in Vec1 is greater than the corresponding element in Vec2, otherwise 0.
		 *
		 * @param Vec1			1st source vector
		 * @param Vec2			2nd source vector
		 * @return				Non-zero integer if (Vec1.x > Vec2.x) || (Vec1.y > Vec2.y) || (Vec1.z > Vec2.z) || (Vec1.w > Vec2.w)
		 */
		static bool VectorAnyGreaterThan(const math::float4& Vec1, const math::float4& Vec2)
		{
			return (Vec1.x > Vec2.x) || (Vec1.y > Vec2.y) || (Vec1.z > Vec2.z) || (Vec1.w > Vec2.w);
		}

		static int VectorAnyGreaterThan(const math::float3& Vec1, const math::float3& Vec2)
		{
			return (Vec1.x > Vec2.x) || (Vec1.y > Vec2.y) || (Vec1.z > Vec2.z);
		}

		/**
		 * Creates a four-part mask based on component-wise > compares of the input vectors
		 *
		 * @param Vec1	1st vector
		 * @param Vec2	2nd vector
		 * @return		VectorRegister4Float( Vec1.x > Vec2.x ? 0xFFFFFFFF : 0, same for yzw )
		 */
		static math::float4 VectorCompareGT(const math::float4& Vec1, const math::float4& Vec2)
		{
			return math::float4(
				Vec1.x > Vec2.x ? 0xFFFFFFFF : 0,
				Vec1.y > Vec2.y ? 0xFFFFFFFF : 0,
				Vec1.z > Vec2.z ? 0xFFFFFFFF : 0,
				Vec1.w > Vec2.w ? 0xFFFFFFFF : 0);
		}
		static math::float3 VectorCompareGT(const math::float3& Vec1, const math::float3& Vec2)
		{
			return math::float3(
				Vec1.x > Vec2.x ? 0xFFFFFFFF : 0,
				Vec1.y > Vec2.y ? 0xFFFFFFFF : 0,
				Vec1.z > Vec2.z ? 0xFFFFFFFF : 0);
		}

		/**
		 * Returns an integer bit-mask (0x00 - 0x0f) based on the sign-bit for each component in a vector.
		 *
		 * @param VecMask		Vector
		 * @return				Bit 0 = sign(VecMask.x), Bit 1 = sign(VecMask.y), Bit 2 = sign(VecMask.z), Bit 3 = sign(VecMask.w)
		 */
		static int VectorMaskBits(const math::float4& VecMask)
		{
			int x = ((int)VecMask.x & 0x80000000) == 0? 0:1;
			int y = ((int)VecMask.y & 0x80000000) == 0 ? 0 : 1;
			int z = ((int)VecMask.z & 0x80000000) == 0 ? 0 : 1;
			int w = ((int)VecMask.w & 0x80000000) == 0 ? 0 : 1;
			return (x < 3 | y < 2 | z < 1 | w);
		}

		/**
		 * Returns an integer bit-mask (0x00 - 0x0f) based on the sign-bit for each component in a vector.
		 *
		 * @param VecMask		Vector
		 * @return				Bit 0 = sign(VecMask.x), Bit 1 = sign(VecMask.y), Bit 2 = sign(VecMask.z), Bit 3 = sign(VecMask.w)
		 */
		static int VectorMaskBits(const math::float3& VecMask)
		{
			int x = ((int)VecMask.x & 0x80000000) == 0 ? 0 : 1;
			int y = ((int)VecMask.y & 0x80000000) == 0 ? 0 : 1;
			int z = ((int)VecMask.z & 0x80000000) == 0 ? 0 : 1;
			
			return (x < 2 | y < 1 | z  );
		}

		static math::int4 VectorIntAnd(math::int4 a, math::int4 b) {
			return math::int4(a.x & b.x, a.y & b.y, a.z & b.z, a.w & b.w);
		}
		static math::int4 VectorIntCompareEQ(math::int4 a, math::int4 b) {
			return math::int4(a.x == b.x ? 1:0 ,
				a.y == b.y ? 1 : 0,
				a.z == b.z ? 1 : 0,
				a.w == b.w ? 1 : 0);
		}
		
		static math::float4 VectorSelect(const math::float4& Mask, const math::float4& Vec1, const math::float4& Vec2)
		{
			math::float4 Result;
//#if !UE_PLATFORM_MATH_USE_AVX
//			Result.XY = VectorSelect(Mask.XY, Vec1.XY, Vec2.XY);
//			Result.ZW = VectorSelect(Mask.ZW, Vec1.ZW, Vec2.ZW);
//#else
//			Result = _mm256_xor_pd(Vec2, _mm256_and_pd(Mask, _mm256_xor_pd(Vec1, Vec2)));
//#endif
			for (size_t i = 0; i < 4; i++)
			{
				if ((unsigned int)(Mask[i]) << i == 1) {
					Result[i] = Vec1[i];
				}
				else {
					Result[i] = Vec2[i];
				}
			}
			return Result;
		}

		template <class T>
		static constexpr T DivideAndRoundUp(T Dividend, T Divisor)
		{
			return (Dividend + Divisor - 1) / Divisor;
		}


	};

}