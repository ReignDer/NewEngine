#include "signpch.h"
#include "MeshImporter.h"

#include "tiny_obj_loader.h"

namespace Sign {
	std::shared_ptr<Mesh> MeshImporter::ImportMesh(AssetHandle handle)
	{
		return  std::shared_ptr<Mesh>();
	}
	std::shared_ptr<Mesh> MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
        std::string inputfile = path.string();

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = path.parent_path().string(); // Path to material files

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(inputfile, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        //auto& materials = reader.GetMaterials();

        std::vector<VertexPosColor> vertices;
        std::vector<uint32_t> indices;
        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    VertexPosColor vertex{};
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    vertex.Position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.Position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.Position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];


                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        vertex.Normals.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        vertex.Normals.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        vertex.Normals.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        vertex.TexCoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        vertex.TexCoord.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    }

                    vertex.Color.x = 1.0f;
                    vertex.Color.y = 1.0f;
                    vertex.Color.z = 1.0f;
                    // Optional: vertex colors
                    // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                    vertices.push_back(vertex);
                    indices.push_back(static_cast<uint32_t>(indices.size()));
                }
                index_offset += fv;

                // per-face material
                //shapes[s].mesh.material_ids[f];
            }
        }

		return std::make_shared<Mesh>(vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size());
	}
}