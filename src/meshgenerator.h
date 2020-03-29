#ifndef DUST3D_MESH_GENERATOR_H
#define DUST3D_MESH_GENERATOR_H
#include <QObject>
#include <set>
#include <QColor>
#include <tuple>
#include "meshcombiner.h"
#include "positionkey.h"
#include "strokemeshbuilder.h"
#include "outcome.h"
#include "snapshot.h"
#include "combinemode.h"
#include "meshloader.h"
#include "componentlayer.h"
#include "clothforce.h"

class GeneratedPart
{
public:
    ~GeneratedPart()
    {
        delete mesh;
    };
    MeshCombiner::Mesh *mesh = nullptr;
    std::vector<QVector3D> vertices;
    std::vector<std::vector<size_t>> faces;
    std::vector<OutcomeNode> outcomeNodes;
    std::vector<std::pair<std::pair<QUuid, QUuid>, std::pair<QUuid, QUuid>>> outcomeEdges;
    std::vector<std::pair<QVector3D, std::pair<QUuid, QUuid>>> outcomeNodeVertices;
    std::vector<QVector3D> previewVertices;
    std::vector<std::vector<size_t>> previewTriangles;
    OutcomePaintMap outcomePaintMap;
    bool isSucceed = false;
    bool joined = true;
};

class GeneratedComponent
{
public:
    ~GeneratedComponent()
    {
        delete mesh;
    };
    MeshCombiner::Mesh *mesh = nullptr;
    std::set<std::pair<PositionKey, PositionKey>> sharedQuadEdges;
    std::set<PositionKey> noneSeamVertices;
    std::vector<OutcomeNode> outcomeNodes;
    std::vector<std::pair<std::pair<QUuid, QUuid>, std::pair<QUuid, QUuid>>> outcomeEdges;
    std::vector<std::pair<QVector3D, std::pair<QUuid, QUuid>>> outcomeNodeVertices;
    std::vector<OutcomePaintMap> outcomePaintMaps;
};

class GeneratedCacheContext
{
public:
    std::map<QString, GeneratedComponent> components;
    std::map<QString, GeneratedPart> parts;
    std::map<QString, QString> partMirrorIdMap;
    std::map<QString, MeshCombiner::Mesh *> cachedCombination;
};

class MeshGenerator : public QObject
{
    Q_OBJECT
public:
    MeshGenerator(Snapshot *snapshot);
    ~MeshGenerator();
    bool isSucceed();
    MeshLoader *takeResultMesh();
    MeshLoader *takePartPreviewMesh(const QUuid &partId);
    const std::set<QUuid> &generatedPreviewPartIds();
    Outcome *takeOutcome();
    std::map<QUuid, StrokeMeshBuilder::CutFaceTransform> *takeCutFaceTransforms();
    std::map<QUuid, std::map<QString, QVector2D>> *takeNodesCutFaces();
    void generate();
    void setGeneratedCacheContext(GeneratedCacheContext *cacheContext);
    void setSmoothShadingThresholdAngleDegrees(float degrees);
    void setDefaultPartColor(const QColor &color);
    void setId(quint64 id);
    quint64 id();
signals:
    void finished();
public slots:
    void process();
    
private:
    QColor m_defaultPartColor = Qt::white;
    Snapshot *m_snapshot = nullptr;
    GeneratedCacheContext *m_cacheContext = nullptr;
    std::set<QString> m_dirtyComponentIds;
    std::set<QString> m_dirtyPartIds;
    float m_mainProfileMiddleX = 0;
    float m_sideProfileMiddleX = 0;
    float m_mainProfileMiddleY = 0;
    Outcome *m_outcome = nullptr;
    std::map<QString, std::set<QString>> m_partNodeIds;
    std::map<QString, std::set<QString>> m_partEdgeIds;
    std::set<QUuid> m_generatedPreviewPartIds;
    MeshLoader *m_resultMesh = nullptr;
    std::map<QUuid, MeshLoader *> m_partPreviewMeshes;
    bool m_isSucceed = false;
    bool m_cacheEnabled = false;
    float m_smoothShadingThresholdAngleDegrees = 60;
    std::map<QUuid, StrokeMeshBuilder::CutFaceTransform> *m_cutFaceTransforms = nullptr;
    std::map<QUuid, std::map<QString, QVector2D>> *m_nodesCutFaces = nullptr;
    quint64 m_id = 0;
    std::vector<QVector3D> m_clothCollisionVertices;
    std::vector<std::vector<size_t>> m_clothCollisionTriangles;
    
    void collectParts();
    bool checkIsComponentDirty(const QString &componentIdString);
    bool checkIsPartDirty(const QString &partIdString);
    bool checkIsPartDependencyDirty(const QString &partIdString);
    void checkDirtyFlags();
    MeshCombiner::Mesh *combinePartMesh(const QString &partIdString, bool *hasError, bool addIntermediateNodes=true);
    MeshCombiner::Mesh *combineComponentMesh(const QString &componentIdString, CombineMode *combineMode);
    void makeXmirror(const std::vector<QVector3D> &sourceVertices, const std::vector<std::vector<size_t>> &sourceFaces,
        std::vector<QVector3D> *destVertices, std::vector<std::vector<size_t>> *destFaces);
    void collectSharedQuadEdges(const std::vector<QVector3D> &vertices, const std::vector<std::vector<size_t>> &faces,
        std::set<std::pair<PositionKey, PositionKey>> *sharedQuadEdges);
    MeshCombiner::Mesh *combineTwoMeshes(const MeshCombiner::Mesh &first, const MeshCombiner::Mesh &second,
        MeshCombiner::Method method,
        bool recombine=true);
    void generateSmoothTriangleVertexNormals(const std::vector<QVector3D> &vertices, const std::vector<std::vector<size_t>> &triangles,
        const std::vector<QVector3D> &triangleNormals,
        std::vector<std::vector<QVector3D>> *triangleVertexNormals);
    const std::map<QString, QString> *findComponent(const QString &componentIdString);
    CombineMode componentCombineMode(const std::map<QString, QString> *component);
    bool componentRemeshed(const std::map<QString, QString> *component, float *polyCountValue=nullptr);
    MeshCombiner::Mesh *combineComponentChildGroupMesh(const std::vector<QString> &componentIdStrings,
        GeneratedComponent &componentCache);
    MeshCombiner::Mesh *combineMultipleMeshes(const std::vector<std::tuple<MeshCombiner::Mesh *, CombineMode, QString>> &multipleMeshes, bool recombine=true);
    QString componentColorName(const std::map<QString, QString> *component);
    ComponentLayer componentLayer(const std::map<QString, QString> *component);
    float componentClothStiffness(const std::map<QString, QString> *component);
    size_t componentClothIteration(const std::map<QString, QString> *component);
    ClothForce componentClothForce(const std::map<QString, QString> *component);
    float componentClothOffset(const std::map<QString, QString> *component);
    void collectUncombinedComponent(const QString &componentIdString);
    void collectClothComponent(const QString &componentIdString);
    void collectClothComponentIdStrings(const QString &componentIdString,
        std::vector<QString> *componentIdStrings);
    void cutFaceStringToCutTemplate(const QString &cutFaceString, std::vector<QVector2D> &cutTemplate);
    void remesh(const std::vector<OutcomeNode> &inputNodes,
        const std::vector<std::tuple<QVector3D, float, size_t>> &interpolatedNodes,
        const std::vector<QVector3D> &inputVertices,
        const std::vector<std::vector<size_t>> &inputFaces,
        float targetVertexMultiplyFactor,
        std::vector<QVector3D> *outputVertices,
        std::vector<std::vector<size_t>> *outputQuads,
        std::vector<std::vector<size_t>> *outputTriangles,
        std::vector<std::pair<QVector3D, std::pair<QUuid, QUuid>>> *outputNodeVertices);
};

#endif
