#ifndef UIMANIPULATOR_H
#define UIMANIPULATOR_H

#include "sceneobject.h"
#include <QList>
#include <QVector3D>
#include "callipermath.h"

class UIManipulator : public SceneObject
{
	Q_OBJECT
public:
	explicit UIManipulator(SceneObject* parent = 0);

    enum ManipulationAxisFlag
    {
        AxisX = 0x1,
        AxisY = 0x2,
        AxisZ = 0x4,

        AxisXY = AxisX | AxisY,
        AxisXZ = AxisX | AxisZ,
        AxisYZ = AxisY | AxisZ,

        AxisXYZ = AxisX | AxisY | AxisZ
    };

    static QList<QVector3D> manipulationAxes(int axisFlags);
    static void clampToNearestMultiple(QVector3D &vec, qint64 multiple, int axisFlags);
	static Math::AxisIdentifier planeConstraintAxis(int axisFlags, const HierarchicalObject &camera);

    virtual SceneObject* clone() const;

    // UI elements should not be serialised.
    virtual bool serialiseToJson(QJsonObject &) const;

protected:
    explicit UIManipulator(const UIManipulator &cloneFrom);
};

#endif // UIMANIPULATOR_H
