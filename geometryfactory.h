#ifndef GEOMETRYFACTORY_H
#define GEOMETRYFACTORY_H

#include <QColor>
#include <QVector3D>
#include "boundingbox.h"
#include <QMatrix4x4>

class GeometryData;

namespace GeometryFactory
{
    // Shader overrides are not set by any of these functions.
    GeometryData* cube(float radius);
    GeometryData* cubeSolidColor(float radius, const QColor &col);
    GeometryData* cubeFullAtts(float radius);
    GeometryData* triangleQuad(float radius);
    GeometryData* lineRect(float radius, const QColor &col);
    GeometryData* lineCuboid(const QVector3D &min, const QVector3D &max, const QColor &col);
    GeometryData* lineCuboid(const BoundingBox &bbox, const QColor &col);

    GeometryData* fromObjFile(const QString &filename, float scale = 1.0f);
}

#endif // GEOMETRYFACTORY_H
