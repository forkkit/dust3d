#ifndef DUST3D_TRIANGULATE_H
#define DUST3D_TRIANGULATE_H
#include <QVector3D>
#include <vector>

bool triangulateWithoutKeepVertices(std::vector<QVector3D> &vertices, const std::vector<std::vector<size_t>> &faces, std::vector<std::vector<size_t>> &triangles);

#endif
