#include "pch.h"
#include "SceneGraph.h"
#include "BlackPearl/Node/Node.h"

namespace BlackPearl {
    std::shared_ptr<Node> SceneGraph::SetRootNode(const std::shared_ptr<Node>& root)
    {
        auto oldRoot = m_Root;
        if (m_Root)
            Detach(m_Root);

        Attach(nullptr, root);

        return oldRoot;
    }
    std::shared_ptr<Node> SceneGraph::Attach(const std::shared_ptr<Node>& parent, const std::shared_ptr<Node>& child)
    {
       auto parentGraph = parent ? parent->m_Graph.lock() : shared_from_this();
        auto childGraph = child->m_Graph.lock();

    if (!parentGraph && !childGraph)
    {
        // operating on an orphaned subgraph - do not copy or register anything

        assert(parent);
        child->m_NextSibling = parent->m_FirstChild;
        child->m_Parent = parent.get();
        parent->m_FirstChild = child;
        return child;
    }

    assert(parentGraph.get() == this);
    std::shared_ptr<Node> attachedChild;
    
    if (childGraph)
    {
        // attaching a subgraph that already belongs to a graph - this one or another
        // copy the subgraph first

        // keep a mapping of old nodes to new nodes to patch the copied animations
        std::unordered_map<Node*, std::shared_ptr<Node>> nodeMap;
        
        Node* currentParent = parent.get();
        SceneGraphWalker walker(child.get());
        while (walker)
        {
            // make a copy of the current node
            std::shared_ptr<Node> copy = std::make_shared<Node>();
            nodeMap[walker.Get()] = copy;

            copy->m_Name = walker->m_Name;
            copy->m_Parent = currentParent;
            copy->m_Graph = weak_from_this();
            copy->m_Dirty = walker->m_Dirty;

            if (walker->m_HasLocalTransform)
            {
                copy->SetTransform(&walker->m_Translation, &walker->m_Rotation, &walker->m_Scaling);
            }

            if (walker->m_Leaf)
            {
                auto leafCopy = walker->m_Leaf->Clone();
                copy->SetLeaf(leafCopy);
            }

            // attach the copy to the new parent
            if (currentParent)
            {
                copy->m_NextSibling = currentParent->m_FirstChild;
                currentParent->m_FirstChild = copy;
            }
            else
            {
                m_Root = copy;
            }

            // if it's the first node we copied, store it as the new root
            if (!attachedChild)
                attachedChild = copy;

            // go to the next node
            int deltaDepth = walker.Next(true);

            if (deltaDepth > 0)
            {
                currentParent = copy.get();
            }
            else
            {
                while (deltaDepth++ < 0)
                {
                    // reverse the children list of this parent to make them consistent with the original
                    currentParent->ReverseChildren();

                    // go up the new tree
                    currentParent = currentParent->m_Parent;
                }
            }
        }

        // go over the new nodes and patch the cloned animations and skinned groups to use the *new* nodes
        walker = SceneGraphWalker(attachedChild.get());
        while (walker)
        {
            if (auto animation = dynamic_cast<SceneGraphAnimation*>(walker->m_Leaf.get()))
            {
                for (const auto& channel : animation->GetChannels())
                {
                    auto newNode = nodeMap[channel->GetTargetNode().get()];
                    if (newNode)
                    {
                        channel->SetTargetNode(newNode);
                    }
                }
            }
            else if (auto skinnedInstance = dynamic_cast<SkinnedMeshInstance*>(walker->m_Leaf.get()))
            {
                for (auto& joint : skinnedInstance->joints)
                {
                    auto jointNode = joint.node.lock();
                    auto newNode = nodeMap[jointNode.get()];
                    if (newNode)
                    {
                        joint.node = newNode;
                    }
                }
            }
            else if (auto meshReference = dynamic_cast<SkinnedMeshReference*>(walker->m_Leaf.get()))
            {
                auto instance = meshReference->m_Instance.lock();
                if (instance)
                {
                    auto oldNode = instance->GetNode();

                    auto newNode = nodeMap[oldNode];
                    if (newNode)
                        meshReference->m_Instance = std::dynamic_pointer_cast<SkinnedMeshInstance>(newNode->m_Leaf);
                    else
                        meshReference->m_Instance.reset();
                }
            }

            walker.Next(true);
        }
    }
    else
    {
        // attaching a subgraph that has been detached from another graph (or never attached)

        SceneGraphWalker walker(child.get());
        while (walker)
        {
            walker->m_Graph = weak_from_this();
            auto leaf = walker->GetLeaf();
            if (leaf)
                RegisterLeaf(leaf);
            walker.Next(true);
        }

        child->m_Parent = parent.get();

        if (parent)
        {
            child->m_NextSibling = parent->m_FirstChild;
            parent->m_FirstChild = child;
        }
        else
        {
            m_Root = child;
        }

        attachedChild = child;
    }

    attachedChild->PropagateDirtyFlags(Node::DirtyFlags::SubgraphStructure
        | (child->m_Dirty & Node::DirtyFlags::SubgraphMask));

    return attachedChild;
    }
    std::shared_ptr<Node> SceneGraph::AttachLeafNode(const std::shared_ptr<Node>& parent, const std::shared_ptr<Node>& leaf)
    {
        return std::shared_ptr<Node>();
    }
    std::shared_ptr<Node> SceneGraph::Detach(const std::shared_ptr<Node>& node)
    {
        return std::shared_ptr<Node>();
    }
    std::shared_ptr<Node> SceneGraph::FindNode(const std::filesystem::path& path, Node* context) const
    {
        return std::shared_ptr<Node>();
    }
    void SceneGraph::Refresh(uint32_t frameIndex)
    {
    }
    int SceneGraphWalker::Next(bool allowChildren)
    {
        if (!m_Current)
            return 0;

        if (allowChildren)
        {
            auto firstChild = m_Current->GetFirstChild();
            if (firstChild)
            {
                m_Current = firstChild;
                return 1;
            }
        }

        int depth = 0;

        while (m_Current)
        {
            if (m_Current == m_Scope)
            {
                m_Current = nullptr;
                return depth;
            }

            auto nextSibling = m_Current->GetNextSibling();
            if (nextSibling)
            {
                m_Current = nextSibling;
                return depth;
            }

            m_Current = m_Current->GetParent();
            --depth;
        }

        return depth;
    }
    int SceneGraphWalker::Up()
    {
        if (!m_Current)
            return 0;

        if (m_Current == m_Scope)
        {
            m_Current = nullptr;
            return 0;
        }

        m_Current = m_Current->GetParent();
        return -1;
    }
}