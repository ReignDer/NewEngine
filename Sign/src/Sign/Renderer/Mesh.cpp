#include "Mesh.h"

namespace Sign {
	Mesh::Mesh(const void* vertices, uint32_t vertexCount, const WORD* indices, uint32_t indexCount)
	{
		m_VertexArray = std::make_shared<VertexArray>();

		auto* verts = static_cast<const VertexPosColor*>(vertices);
		m_Vertices.assign(verts, verts + vertexCount);
		m_Indices.assign(indices, indices + indexCount);

		std::shared_ptr<Sign::VertexBuffer> VB;
		std::shared_ptr<Sign::IndexBuffer>  IB;

		VB = std::make_shared<Sign::VertexBuffer>(vertices, vertexCount);
		m_VertexArray->AddVertexBuffers(VB);
		IB = std::make_shared<Sign::IndexBuffer>(indices, indexCount);
		m_VertexArray->SetIndexBuffer(IB);

		for (uint32_t i = 0; i < vertexCount; ++i)
			m_FaceToVertexIndices[m_Vertices[i].FaceID].push_back(i);
	}

	void Mesh::TransformFace(uint32_t faceID, const Mat4& delta)
	{
		auto it = m_FaceToVertexIndices.find(faceID);
		if (it == m_FaceToVertexIndices.end())
			return;

		Vector3D centroid(0.0f, 0.0f, 0.0f);
		for (uint32_t idx : it->second)
			centroid += m_Vertices[idx].Position;
		centroid = centroid / (float)it->second.size();

		for (uint32_t idx : it->second)
		{
			Vector3D local = m_Vertices[idx].Position - centroid;
			Vector3D transformed = delta.TransformPoint(local);
			m_Vertices[idx].Position = centroid + Vector3D(transformed.x, transformed.y, transformed.z);
		}

		m_IsDirty = true;
	}

	void Mesh::UploadIfDirty()
	{
		if (!m_IsDirty)
			return;

		m_VertexArray->GetVertexBuffer()->SetData(m_Vertices.data(), (uint32_t)m_Vertices.size());
		m_IsDirty = false;
	}
	
}