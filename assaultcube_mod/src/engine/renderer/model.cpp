// #include "model.h"
#include "lib/glad/glad.h"

// Model::Model()
// {
//     m_meshs.reserve(1);
// }

// Model::~Model()
// {

// }

// bool Model::addMesh(Mesh& mesh)
// {
//     if (!mesh.isValidMesh()) {
//         return false;
//     }

//     m_meshs.push_back(std::move(mesh));
//     return true;
// }

// std::vector<Mesh>& Model::getMesh()
// {
//     return m_meshs;
// }

// const std::vector<Mesh>& Model::getMesh() const
// {
//     return m_meshs;
// }

// Mesh* Model::getMesh(unsigned int index)
// {
//     if (m_meshs.empty() == false) {
//         if (index <= m_meshs.size() - 1) {
//             return &m_meshs[index];
//         }
//     }
//     return nullptr;
// }

// const Mesh* Model::getMesh(unsigned int index) const
// {
//     if (m_meshs.empty() == false) {
//         if (index <= m_meshs.size() - 1) {
//             return &m_meshs[index];
//         }
//     }
// }

// /*
// * ==========
// *
// * ==========
// */
// ModelManager::ModelManager()
// {

// }

// ModelManager::~ModelManager()
// {

// }

// void ModelManager::addModel(const Model& mdl, const char* name)
// {
//     if (mdl.getMesh().empty()) {
//         return;
//     }
//     modelInfo_t minfo;
//     minfo.id = (int)((time(NULL) * 3) / 0.003f);
//     minfo.mdl = mdl;
    
//     if (name[0] != '\0') {
//         minfo.name = name;
//     }
//     else {
//         minfo.name = "mdl_name";
//     }
//     models.push_back(std::move(minfo));
// }

// void ModelManager::addModel(modelInfo_t mdlInfo)
// {
//     if (mdlInfo.mdl.getMesh().empty()) {
//         return;
//     }
//     models.push_back(std::move(mdlInfo));
// }

// // create a VBO+VAO & IBO fore this entire model. all in one
// void ModelManager::registerModelGLBuffers(Model& mdlReg)
// {
//     unsigned int vbo = 0;
    
    
// }

// //
// class iModelHandle {
// private:
//     bool m_valid;
//     Model* m_pModel;
//     unsigned int m_id;
//     class ModelManager* m_modelManager;

// public:
//     iModelHandle()
//         : m_pModel(nullptr)
//     {
//         m_valid = false;
//     }
//     ~iModelHandle() {
//         destroy();
//     }

//     // create this handle
//     bool create(Model* model_pointer) {
//         if (!model_pointer) {
//             return false;
//         }

//         m_pModel = model_pointer;
//         return false;
//     }

//     // destroy this handle
//     void destroy() {
//         if (m_pModel) {
//             // m_pModel->destroy();
//             m_pModel = nullptr;
//         }
//     }

//     bool isValid() const {
//         return m_valid;
//     }
//     Model* get() {
//         if (!m_pModel) {
//             return nullptr;
//         }
//         return m_pModel;
//     }
//     const Model* get() const{
//         if (!m_pModel) {
//             return nullptr;
//         }
//         return m_pModel;
//     }
//     int getId() const {
//         return m_id;
//     }
// };

// Model* ModelManager::GetModelFromHandle(iModelHandle& handle)
// {
//     return handle.get();
// }

// unsigned int ModelManager::GetIDFromHandle(iModelHandle& handle)
// {
//     return handle.getId();
// }