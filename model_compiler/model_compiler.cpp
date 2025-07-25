#include "arguments.h"
#include "ofbx.h"
#include "../core/mesh/mesh_header.h"
#include "vector2/vector2.h"
#include "vector4/vector4.h"
#include "matrix4x4/matrix4x4.h"

#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <execution>

Vector3 ToVector3(const ofbx::Vec3& vec3)
{
    return Vector3(vec3.x, vec3.y, vec3.z);
}

Vector2 ToVector2(const ofbx::Vec2& vec2)
{
    return Vector2(vec2.x, vec2.y);
}

Matrix4x4 ToMatrix4x4(const ofbx::Matrix& matrix)
{
    Matrix4x4 m{};
    m.m00 = matrix.m[0];
    m.m01 = matrix.m[1];
    m.m02 = matrix.m[2];
    m.m03 = matrix.m[3];
    m.m10 = matrix.m[4];
    m.m11 = matrix.m[5];
    m.m12 = matrix.m[6];
    m.m13 = matrix.m[7];
    m.m20 = matrix.m[8];
    m.m21 = matrix.m[9];
    m.m22 = matrix.m[10];
    m.m23 = matrix.m[11];
    m.m30 = matrix.m[12];
    m.m31 = matrix.m[13];
    m.m32 = matrix.m[14];
    m.m33 = matrix.m[15];
    return m;
}

void ExtractMeshesFromFbx(const std::filesystem::path& input, const std::filesystem::path& output)
{
    std::ifstream file(input, std::ios::binary);
    if (!file)
    {
        std::cout << "No input file: " << input << std::endl;
        return;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    ofbx::IScene* scene = ofbx::load(reinterpret_cast<ofbx::u8*>(&buffer[0]), buffer.size(), static_cast<ofbx::u64>(ofbx::LoadFlags::TRIANGULATE));
    if (!scene)
    {
        std::cout << "Cannot read FBX: " << input << std::endl;
        return;
    }

    bool isRightHanded = scene->getGlobalSettings()->CoordAxis == ofbx::CoordSystem_RightHanded;

    for (int i = 0; i < scene->getMeshCount(); ++i)
    {
        const ofbx::Mesh* mesh = scene->getMesh(i);
        if (!mesh)
            continue;

        const ofbx::Geometry* geom = mesh->getGeometry();
        if (geom == nullptr)
            continue;

        bool hasUV = geom->getUVs() != nullptr;
        bool hasNormals = geom->getNormals() != nullptr;
        bool hasTangents = geom->getTangents() != nullptr;

        uint64_t posSize = sizeof(float) * 3;
        uint64_t uvSize = hasUV ? sizeof(float) * 2 : 0;
        uint64_t normalsSize = hasNormals ? sizeof(float) * 3 : 0;
        uint64_t tangentsSize = hasTangents ? sizeof(float) * 3: 0;

        Matrix4x4 localToWorld = ToMatrix4x4(geom->getGlobalTransform());
        Matrix4x4 scaleMatrix = Matrix4x4::Scale({isRightHanded ? -1.0f : 1.0f, 1.0f, 1.0f });
        Matrix4x4 worldToLocal = localToWorld.Invert();
        Matrix4x4 combinedTransformation = worldToLocal * scaleMatrix * localToWorld;

        Vector3 minPoint = combinedTransformation * ToVector3(geom->getVertices()[0]).ToVector4(1);
        Vector3 maxPoint = minPoint;

        uint64_t vertexSize = posSize + uvSize + normalsSize + tangentsSize;
        std::vector<uint8_t> vertexData(vertexSize * geom->getVertexCount());
        for (int j = 0; j < geom->getVertexCount(); ++j)
        {
            uint8_t* vertexDataPtr = vertexData.data() + j * vertexSize;

            Vector3 vertex = ToVector3(geom->getVertices()[j]);
            vertex = combinedTransformation * vertex.ToVector4(1);
            memcpy(vertexDataPtr, &vertex, posSize);

            if (hasNormals)
            {
                Vector3 normal = ToVector3(geom->getNormals()[j]);
                normal = combinedTransformation * normal.ToVector4(0);
                memcpy(vertexDataPtr + posSize, &normal, normalsSize);
            }
            if (hasUV)
            {
                Vector2 uv = ToVector2(geom->getUVs()[j]);
                memcpy(vertexDataPtr + posSize + normalsSize, &uv, uvSize);
            }
            if (hasTangents)
            {
                Vector3 tangent = ToVector3(geom->getTangents()[j]);
                tangent = combinedTransformation * tangent.ToVector4(0);
                memcpy(vertexDataPtr + posSize + normalsSize + uvSize, &tangent, tangentsSize);
            }

            minPoint = Vector3::Min(minPoint, vertex);
            maxPoint = Vector3::Max(maxPoint, vertex);
        }

        std::vector<int> indices;
        for (int j = 0; j < geom->getIndexCount() / 3; ++j)
        {
            // index with negative value marks end of polygon and is also decreased by 1 during triangulation
            int index0 = geom->getFaceIndices()[j * 3 + 0];
            int index1 = geom->getFaceIndices()[j * 3 + 1];
            int index2 = geom->getFaceIndices()[j * 3 + 2];
            if (isRightHanded)
            {
                indices.push_back(index2 < 0 ? -index2 - 1 : index2);
                indices.push_back(index1 < 0 ? -index1 - 1 : index1);
                indices.push_back(index0 < 0 ? -index0 - 1 : index0);
            }
            else
            {
                indices.push_back(index0 < 0 ? -index0 - 1 : index0);
                indices.push_back(index1 < 0 ? -index1 - 1 : index1);
                indices.push_back(index2 < 0 ? -index2 - 1 : index2);
            }
        }

        MeshHeader header{};
        memcpy(header.Name, mesh->name, sizeof(mesh->name));
        header.VertexDataSize = vertexData.size();
        header.IndicesCount = indices.size();
        header.HasUV = hasUV;
        header.HasNormals = hasNormals;
        header.HasTangents = hasTangents;
        header.MinPoint = minPoint;
        header.MaxPoint = maxPoint;

        std::filesystem::path outputPath = output / mesh->name;
        std::filesystem::create_directories(outputPath.parent_path());

        std::ofstream fout;
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(&header), sizeof(MeshHeader));
        fout.write(reinterpret_cast<char*>(&vertexData[0]), vertexData.size());
        fout.write(reinterpret_cast<char*>(&indices[0]), sizeof(int) * indices.size());
        fout.close();

        std::cout << "\tMesh successfully saved: " << outputPath << std::endl;
    }
}

int main(int argc, char** argv)
{
    Arguments::Init(argv, argc);

    if (!Arguments::Contains("-input"))
        std::cout << "No -input argument" << std::endl;
    if (!Arguments::Contains("-output"))
        std::cout << "No -output argument" << std::endl;

    std::filesystem::path inputPath = Arguments::Get("-input");
    std::filesystem::path outputPath = Arguments::Get("-output");

    std::vector<std::filesystem::path> modelFilePaths;

    for (const std::filesystem::directory_entry& entry: std::filesystem::recursive_directory_iterator(inputPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".fbx")
            modelFilePaths.push_back(entry.path());
    }

    std::for_each(std::execution::par, modelFilePaths.begin(), modelFilePaths.end(), [&inputPath, &outputPath](std::filesystem::path &path)
                  { ExtractMeshesFromFbx(path, outputPath); });

    return 0;
}