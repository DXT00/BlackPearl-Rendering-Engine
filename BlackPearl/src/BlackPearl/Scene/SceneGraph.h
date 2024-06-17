#pragma once
#include <memory>
namespace BlackPearl {
	class SceneGraph : public std::enable_shared_from_this<SceneGraph>
	{
    private:
        friend class Node;
        std::shared_ptr<Node> m_Root;
       /* ResourceTracker<Material> m_Materials;
        ResourceTracker<MeshInfo> m_Meshes;*/
        size_t m_GeometryCount = 0;
        size_t m_GeometryInstancesCount = 0;
  /*      std::vector<std::shared_ptr<MeshInstance>> m_MeshInstances;
        std::vector<std::shared_ptr<SkinnedMeshInstance>> m_SkinnedMeshInstances;
        std::vector<std::shared_ptr<SceneGraphAnimation>> m_Animations;
        std::vector<std::shared_ptr<SceneCamera>> m_Cameras;
        std::vector<std::shared_ptr<Light>> m_Lights;*/

    protected:
        virtual void RegisterLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf);
        virtual void UnregisterLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf);

    public:
        SceneGraph() = default;
        virtual ~SceneGraph() = default;

 /*       SceneResourceCallback<MeshInfo> OnMeshAdded;
        SceneResourceCallback<MeshInfo> OnMeshRemoved;
        SceneResourceCallback<Material> OnMaterialAdded;
        SceneResourceCallback<Material> OnMaterialRemoved;*/

        [[nodiscard]] const std::shared_ptr<Node>& GetRootNode() const { return m_Root; }
        //[[nodiscard]] const ResourceTracker<Material>& GetMaterials() const { return m_Materials; }
        //[[nodiscard]] const ResourceTracker<MeshInfo>& GetMeshes() const { return m_Meshes; }
        [[nodiscard]] const size_t GetGeometryCount() const { return m_GeometryCount; }
        [[nodiscard]] const size_t GetGeometryInstancesCount() const { return m_GeometryInstancesCount; }
       /* [[nodiscard]] const std::vector<std::shared_ptr<MeshInstance>>& GetMeshInstances() const { return m_MeshInstances; }
        [[nodiscard]] const std::vector<std::shared_ptr<SkinnedMeshInstance>>& GetSkinnedMeshInstances() const { return m_SkinnedMeshInstances; }
        [[nodiscard]] const std::vector<std::shared_ptr<SceneGraphAnimation>>& GetAnimations() const { return m_Animations; }
        [[nodiscard]] const std::vector<std::shared_ptr<SceneCamera>>& GetCameras() const { return m_Cameras; }
        [[nodiscard]] const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }
        [[nodiscard]] bool HasPendingStructureChanges() const { return m_Root && (m_Root->m_Dirty & Node::DirtyFlags::SubgraphStructure) != 0; }
        [[nodiscard]] bool HasPendingTransformChanges() const { return m_Root && (m_Root->m_Dirty & (Node::DirtyFlags::SubgraphTransforms | Node::DirtyFlags::SubgraphPrevTransforms)) != 0; }*/

        std::shared_ptr<Node> SetRootNode(const std::shared_ptr<Node>& root);
        std::shared_ptr<Node> Attach(const std::shared_ptr<Node>& parent, const std::shared_ptr<Node>& child);
        std::shared_ptr<Node> AttachLeafNode(const std::shared_ptr<Node>& parent, const std::shared_ptr<Node>& leaf);
        std::shared_ptr<Node> Detach(const std::shared_ptr<Node>& node);

        [[nodiscard]] std::shared_ptr<Node> FindNode(const std::filesystem::path& path, Node* context = nullptr) const;

        void Refresh(uint32_t frameIndex);
	};


    class SceneGraphWalker final
    {
    private:
        Node* m_Current;
        Node* m_Scope;
    public:
        SceneGraphWalker() = default;

        explicit SceneGraphWalker(Node* scope)
            : m_Current(scope)
            , m_Scope(scope)
        { }

        SceneGraphWalker(Node* current, Node* scope)
            : m_Current(current)
            , m_Scope(scope)
        { }

        [[nodiscard]] Node* Get() const { return m_Current; }
        [[nodiscard]] operator bool() const { return m_Current != nullptr; }
        Node* operator->() const { return m_Current; }

        // Moves the pointer to the first child of the current node, if it exists, and if allowChildren = true.
        // Otherwise, moves the pointer to the next sibling of the current node, if it exists.
        // Otherwise, goes up and tries to find the next sibiling up the hierarchy.
        // Returns the depth of the new node relative to the current node.
        int Next(bool allowChildren);

        // Moves the pointer to the parent of the current node, up to the scope.
        // Note that using Up and Next together may result in an infinite loop.
        // Returns the depth of the new node relative to the current node.
        int Up();
    };

}


