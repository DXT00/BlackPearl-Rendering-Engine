#pragma once
/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include "BlackPearl/Node/Node.h"
#include "BlackPearl/Math/frustum.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/SceneType.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Scene/SceneGraph.h"
#include "BlackPearl/Scene/Scene.h"

namespace BlackPearl {
    struct DrawItem
    {
        //TODO::
        const MeshInstance* instance;
        const MeshInfo* mesh;
        const MeshGeometry* geometry;
        const Material* material;
        const BufferGroup* buffers;
        float distanceToCamera;
        RasterCullMode cullMode;
    };

    class IDrawStrategy
    {
    public:
        virtual void PrepareForView(
            const std::shared_ptr<Node>& rootNode,
            Scene* Scene,
            const IView& view) = 0;

        virtual const DrawItem* GetNextItem() = 0;

        virtual std::vector<Object*> GetDrawObjects() const = 0;

        virtual ~IDrawStrategy() = default;
    };

    class PassthroughDrawStrategy : public IDrawStrategy
    {
    private:
        const DrawItem* m_Data = nullptr;
        size_t m_Count = 0;

    public:
        void PrepareForView(
            const std::shared_ptr<Node>& rootNode,
            Scene* Scene,
            const IView& view) override { }

        const DrawItem* GetNextItem() override;
        std::vector<Object*> GetDrawObjects() const override { return m_VisibleObjs; }

        void SetData(const DrawItem* data, size_t count);
    private:
        std::vector<Object*> m_VisibleObjs;
    };

    class InstancedOpaqueDrawStrategy : public IDrawStrategy
    {
    private:
        donut::math::frustum m_ViewFrustum;
        //TODO::
        SceneGraphWalker m_Walker;
        std::vector<DrawItem> m_InstanceChunk;
        std::vector<const DrawItem*> m_InstancePtrChunk;
        size_t m_ReadPtr = 0;
        size_t m_ChunkSize = 128;

        void FillChunk();

    public:

        void PrepareForView(
            const std::shared_ptr<Node>& rootNode,
            Scene* Scene,
            const IView& view) override;

        const DrawItem* GetNextItem() override;
        void _PrepareDrawItems();

        std::vector<Object*> GetDrawObjects() const override { return m_VisibleObjs; }

        [[nodiscard]] size_t GetChunkSize() const { return m_ChunkSize; }
        void SetChunkSize(size_t size) { m_ChunkSize = std::max<size_t>(size, 1u); }
    private:
        std::vector<Object*> m_VisibleObjs;
    };

    class TransparentDrawStrategy : public IDrawStrategy
    {
    private:
        std::vector<DrawItem> m_InstancesToDraw;
        std::vector<const DrawItem*> m_InstancePtrsToDraw;
        size_t m_ReadPtr = 0;

    public:
        bool DrawDoubleSidedMaterialsSeparately = true;

        void PrepareForView(
            const std::shared_ptr<Node>& rootNode,
            Scene* Scene,
            const IView& view) override;

        const DrawItem* GetNextItem() override;
    private:
        std::vector<Object*> m_VisibleObjs;
    };

}

