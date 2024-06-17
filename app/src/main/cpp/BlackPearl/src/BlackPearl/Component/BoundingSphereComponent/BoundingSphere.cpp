#include "pch.h"
#include "BoundingSphere.h"


namespace BlackPearl {

    //this function is based on DirectXCollision.h : BoundingSphere::CreateFromPoints
    //-----------------------------------------------------------------------------
    // Find the approximate smallest enclosing bounding sphere for a set of
    // points. Exact computation of the smallest enclosing bounding sphere is
    // possible but is slower and requires a more complex algorithm.
    // The algorithm is based on  Jack Ritter, "An Efficient Bounding Sphere",
    // Graphics Gems.
    //-----------------------------------------------------------------------------
	 void BoundingSphere::CreateFromPoints(BoundingSphere &sphere, size_t Count,  donut::math::float3* pPoints, size_t Stride) {
        GE_ASSERT(Count > 0, "Count <=0");
        GE_ASSERT(pPoints ,"pPoints = nullptr");

        // Find the points with minimum and maximum x, y, and z
        donut::math::float3 MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

        MinX = MaxX = MinY = MaxY = MinZ = MaxZ = *(pPoints);

        for (size_t i = 1; i < Count; ++i)
        {
            const donut::math::float3* p = reinterpret_cast<const donut::math::float3*>(reinterpret_cast<const uint8_t*>(pPoints)+ i * Stride);
            donut::math::float3 Point = *p;//(reinterpret_cast<const donut::math::float3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            float px = (Point.x);
            float py = (Point.y);
            float pz = (Point.z);

            if (px < MinX.x)
                MinX = Point;

            if (px > (MaxX.x))
                MaxX = Point;

            if (py < (MinY.y))
                MinY = Point;

            if (py > (MaxY.y))
                MaxY = Point;

            if (pz < (MinZ.z))
                MinZ = Point;

            if (pz > (MaxZ.z))
                MaxZ = Point;
        }

        // Use the min/max pair that are farthest apart to form the initial sphere.
        donut::math::float3 DeltaX = (MaxX - MinX);
        donut::math::float3 DistX = donut::math::length(DeltaX);

        donut::math::float3 DeltaY = (MaxY - MinY);
        donut::math::float3 DistY = donut::math::length(DeltaY);

        donut::math::float3 DeltaZ = (MaxZ - MinZ);
        donut::math::float3 DistZ = donut::math::length(DeltaZ);

        donut::math::float3 vCenter;
        donut::math::float3 vRadius;

        if (DistX.x > DistY.x)
        {
            if (DistX.x >DistZ.x)
            {
                // Use min/max x.
                vCenter = donut::math::lerp(MaxX, MinX, 0.5f);
                vRadius = (DistX * 0.5f);
            }
            else
            {
                // Use min/max z.
                vCenter = donut::math::lerp(MaxZ, MinZ, 0.5f);
                vRadius = (DistZ*0.5f);
            }
        }
        else // Y >= X
        {
            if (DistY.x> DistZ.x)
            {
                // Use min/max y.
                vCenter = donut::math::lerp(MaxY, MinY, 0.5f);
                vRadius = (DistY*0.5f);
            }
            else
            {
                // Use min/max z.
                vCenter = donut::math::lerp(MaxZ, MinZ, 0.5f);
                vRadius = (DistZ*0.5f);
            }
        }

        // Add any points not inside the sphere.
        for (size_t i = 0; i < Count; ++i)
        {
            donut::math::float3 Point = pPoints[i];// XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            donut::math::float3 Delta = (Point-vCenter);

            donut::math::float3 Dist = donut::math::length(Delta);

            if ((Dist.x > vRadius.x))
            {
                // Adjust sphere to include the new point.
                vRadius = (vRadius + Dist)*0.5f;
                vCenter = vCenter+ (donut::math::float3 (1.0f) - (vRadius/Dist)) * Delta;
            }
        }

        sphere.m_Center = vCenter;
        sphere.m_Radius = vRadius.x;
        //DirectX::XMStoreFloat3(&sphere.m_Center, vCenter);
        //DirectX::XMStoreFloat(&sphere.m_Radius, vRadius);

	}
    //-----------------------------------------------------------------------------
// Creates a bounding sphere that contains two other bounding spheres
//-----------------------------------------------------------------------------
    
     void  BoundingSphere::CreateMerged(BoundingSphere& Out, const BoundingSphere& S1, const BoundingSphere& S2)
    {
        donut::math::float3 Center1 = S1.m_Center;
        float r1 = S1.m_Radius;

        donut::math::float3 Center2 = S2.m_Center;
        float r2 = S2.m_Radius;

        donut::math::float3 V = Center2-Center1;

        //float Dist = donut::math::length(V);

        float Dist = donut::math::length(V);

        if (r1 + r2 >= Dist)
        {
            if (r1 - r2 >= Dist)
            {
                Out = S1;
                return;
            }
            else if (r2 - r1 >= Dist)
            {
                Out = S2;
                return;
            }
        }

        donut::math::float3 N = (V/Dist);

        float t1 = donut::math::min(-r1, Dist - r2);
        float t2 = donut::math::max(r1, Dist + r2);
        float t_5 = (t2 - t1) * 0.5f;

        //donut::math::float3 NCenter = XMVectorAdd(Center1, XMVectorMultiply(N, XMVectorReplicate(t_5 + t1)));
        donut::math::float3 NCenter = Center1 + N * (t_5 + t1);

        Out.m_Center=NCenter;
        Out.m_Radius = t_5;
    }

}