#include "pch.h"
#include "BoundingSphere.h"
#include <DirectXCollision.h>
#include <DirectXMathConvert.inl>
using namespace DirectX;

namespace BlackPearl {

    //this function is based on DirectXCollision.h : BoundingSphere::CreateFromPoints
    //-----------------------------------------------------------------------------
    // Find the approximate smallest enclosing bounding sphere for a set of
    // points. Exact computation of the smallest enclosing bounding sphere is
    // possible but is slower and requires a more complex algorithm.
    // The algorithm is based on  Jack Ritter, "An Efficient Bounding Sphere",
    // Graphics Gems.
    //-----------------------------------------------------------------------------
	 void BoundingSphere::CreateFromPoints(BoundingSphere& sphere, size_t Count, const XMFLOAT3* pPoints, size_t Stride) {
        GE_ASSERT(Count > 0, "Count <=0");
        GE_ASSERT(pPoints ,"pPoints = nullptr");

        // Find the points with minimum and maximum x, y, and z
        XMVECTOR MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

        MinX = MaxX = MinY = MaxY = MinZ = MaxZ = XMLoadFloat3(pPoints);

        for (size_t i = 1; i < Count; ++i)
        {
            XMVECTOR Point = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            float px = XMVectorGetX(Point);
            float py = XMVectorGetY(Point);
            float pz = XMVectorGetZ(Point);

            if (px < XMVectorGetX(MinX))
                MinX = Point;

            if (px > XMVectorGetX(MaxX))
                MaxX = Point;

            if (py < XMVectorGetY(MinY))
                MinY = Point;

            if (py > XMVectorGetY(MaxY))
                MaxY = Point;

            if (pz < XMVectorGetZ(MinZ))
                MinZ = Point;

            if (pz > XMVectorGetZ(MaxZ))
                MaxZ = Point;
        }

        // Use the min/max pair that are farthest apart to form the initial sphere.
        XMVECTOR DeltaX = XMVectorSubtract(MaxX, MinX);
        XMVECTOR DistX = XMVector3Length(DeltaX);

        XMVECTOR DeltaY = XMVectorSubtract(MaxY, MinY);
        XMVECTOR DistY = XMVector3Length(DeltaY);

        XMVECTOR DeltaZ = XMVectorSubtract(MaxZ, MinZ);
        XMVECTOR DistZ = XMVector3Length(DeltaZ);

        XMVECTOR vCenter;
        XMVECTOR vRadius;

        if (XMVector3Greater(DistX, DistY))
        {
            if (XMVector3Greater(DistX, DistZ))
            {
                // Use min/max x.
                vCenter = XMVectorLerp(MaxX, MinX, 0.5f);
                vRadius = XMVectorScale(DistX, 0.5f);
            }
            else
            {
                // Use min/max z.
                vCenter = XMVectorLerp(MaxZ, MinZ, 0.5f);
                vRadius = XMVectorScale(DistZ, 0.5f);
            }
        }
        else // Y >= X
        {
            if (XMVector3Greater(DistY, DistZ))
            {
                // Use min/max y.
                vCenter = XMVectorLerp(MaxY, MinY, 0.5f);
                vRadius = XMVectorScale(DistY, 0.5f);
            }
            else
            {
                // Use min/max z.
                vCenter = XMVectorLerp(MaxZ, MinZ, 0.5f);
                vRadius = XMVectorScale(DistZ, 0.5f);
            }
        }

        // Add any points not inside the sphere.
        for (size_t i = 0; i < Count; ++i)
        {
            XMVECTOR Point = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            XMVECTOR Delta = XMVectorSubtract(Point, vCenter);

            XMVECTOR Dist = XMVector3Length(Delta);

            if (XMVector3Greater(Dist, vRadius))
            {
                // Adjust sphere to include the new point.
                vRadius = XMVectorScale(XMVectorAdd(vRadius, Dist), 0.5f);
                vCenter = XMVectorAdd(vCenter, XMVectorMultiply(XMVectorSubtract(XMVectorReplicate(1.0f), XMVectorDivide(vRadius, Dist)), Delta));
            }
        }

        DirectX::XMStoreFloat3(&sphere.m_Center, vCenter);
        DirectX::XMStoreFloat(&sphere.m_Radius, vRadius);

	}
    //-----------------------------------------------------------------------------
// Creates a bounding sphere that contains two other bounding spheres
//-----------------------------------------------------------------------------
    
     void  BoundingSphere::CreateMerged(BoundingSphere& Out, const BoundingSphere& S1, const BoundingSphere& S2)
    {
        XMVECTOR Center1 = XMLoadFloat3(&S1.m_Center);
        float r1 = S1.m_Radius;

        XMVECTOR Center2 = XMLoadFloat3(&S2.m_Center);
        float r2 = S2.m_Radius;

        XMVECTOR V = XMVectorSubtract(Center2, Center1);

        XMVECTOR Dist = XMVector3Length(V);

        float d = XMVectorGetX(Dist);

        if (r1 + r2 >= d)
        {
            if (r1 - r2 >= d)
            {
                Out = S1;
                return;
            }
            else if (r2 - r1 >= d)
            {
                Out = S2;
                return;
            }
        }

        XMVECTOR N = XMVectorDivide(V, Dist);

        float t1 = XMMin(-r1, d - r2);
        float t2 = XMMax(r1, d + r2);
        float t_5 = (t2 - t1) * 0.5f;

        XMVECTOR NCenter = XMVectorAdd(Center1, XMVectorMultiply(N, XMVectorReplicate(t_5 + t1)));

        XMStoreFloat3(&Out.m_Center, NCenter);
        Out.m_Radius = t_5;
    }

}