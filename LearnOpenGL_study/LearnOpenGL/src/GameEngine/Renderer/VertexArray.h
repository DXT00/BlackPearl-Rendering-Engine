#pragma once
#include"GameEngine/Renderer/Buffer.h"
class VertexArray
{
public:
	VertexArray();
	~VertexArray()=default;
	void Bind();
	void UnBind();

	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

private:
	unsigned int m_RendererID;
	std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

