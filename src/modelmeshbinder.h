#ifndef DUST3D_MODEL_MESH_BINDER_H
#define DUST3D_MODEL_MESH_BINDER_H
#include <QOpenGLVertexArrayObject>
#include <QMutex>
#include <QOpenGLBuffer>
#include <QString>
#include <QOpenGLTexture>
#include "meshloader.h"
#include "modelshaderprogram.h"

class ModelMeshBinder
{
public:
    ModelMeshBinder(bool toolEnabled=false);
    ~ModelMeshBinder();
    void updateMesh(MeshLoader *mesh);
    void initialize();
    void paint(ModelShaderProgram *program);
    void cleanup();
    void showWireframes();
    void hideWireframes();
    bool isWireframesVisible();
    void enableCheckUv();
    void disableCheckUv();
    void enableEnvironmentLight();
    bool isCheckUvEnabled();
    void reloadMesh();
private:
    MeshLoader *m_mesh = nullptr;
    MeshLoader *m_newMesh = nullptr;
    int m_renderTriangleVertexCount = 0;
    int m_renderEdgeVertexCount = 0;
    int m_renderToolVertexCount = 0;
    bool m_newMeshComing = false;
    bool m_showWireframes = false;
    bool m_hasTexture = false;
    QOpenGLTexture *m_texture = nullptr;
    bool m_hasNormalMap = false;
    QOpenGLTexture *m_normalMap = nullptr;
    bool m_hasMetalnessMap = false;
    bool m_hasRoughnessMap = false;
    bool m_hasAmbientOcclusionMap = false;
    QOpenGLTexture *m_metalnessRoughnessAmbientOcclusionMap = nullptr;
    bool m_toolEnabled = false;
    bool m_checkUvEnabled = false;
    bool m_environmentLightEnabled = false;
    QOpenGLTexture *m_environmentIrradianceMap = nullptr;
    QOpenGLTexture *m_environmentSpecularMap = nullptr;
private:
    QOpenGLVertexArrayObject m_vaoTriangle;
    QOpenGLBuffer m_vboTriangle;
    QOpenGLVertexArrayObject m_vaoEdge;
    QOpenGLBuffer m_vboEdge;
    QOpenGLVertexArrayObject m_vaoTool;
    QOpenGLBuffer m_vboTool;
    QMutex m_meshMutex;
    QMutex m_newMeshMutex;
};

#endif
