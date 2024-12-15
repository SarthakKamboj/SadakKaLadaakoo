//
//  geometry.cpp
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/12/24.
//

#include "geometry.h"
#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Foundation/NSBundle.h>

#include "defines.h"

model_t load_mesh(const aiNode* node, aiMesh** const ai_meshes) {
    model_t model;
    if (node->mNumMeshes != 1) {
        SKL_LOG("only loading models with a singular mesh");
        return model;
    }
    int index = node->mMeshes[0];
    const aiMesh* ai_mesh = ai_meshes[index];
    
    int num_verts = ai_mesh->mNumFaces*3;
    skl_vert_t* verts = (skl_vert_t*)malloc(num_verts * sizeof(skl_vert_t));
    SKL_LOG("loading %i verts", num_verts);
    int k = 0;
    for (int i = 0; i < ai_mesh->mNumFaces; i++) {
        aiFace face = ai_mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            int vert_idx = face.mIndices[j];
            aiVector3D pos = ai_mesh->mVertices[vert_idx];
            skl_vert_t& vert = verts[k];
            vert.pos = {pos.x, pos.y, pos.z};
            // vert.pos = {pos.x * 0.5f, pos.y * 0.5f, (pos.z * 0.25f) + 0.5f};
            // vert.pos = {pos.x * 0.5f, pos.y * 0.5f, 0};
            SKL_LOG("pos at index %i is (%f, %f, %f)", k, vert.pos.x, vert.pos.y, vert.pos.z);
            // vert.color = {0.25f, 0.41f, 0.88f};
            vert.color = {vert.pos.x, vert.pos.y, vert.pos.z};
            k++;
        }
    }
    
    model.verts = verts;
    model.num_verts = num_verts;
    
    return model;
}

model_t load_model(const char* model_path) {
    Assimp::Importer importer;
    
    const aiScene* main_scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipWindingOrder);
    SKL_LOG("main scene is %p", main_scene);
    
    model_t model = load_mesh(main_scene->mRootNode, main_scene->mMeshes);
    if (model.num_verts != -1) {
        SKL_LOG("loaded mesh");
    } else {
        SKL_LOG("did not load mesh");
    }
    
    return model;
}
