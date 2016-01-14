#include "hierarchicalobject.h"
#include "callipermath.h"
#include <cmath>

HierarchicalObject::HierarchicalObject(HierarchicalObject *parent) : QObject(parent)
{
    m_vecPosition = QVector3D(0,0,0);
    m_angAngles = EulerAngle(0,0,0);
    m_bMatricesStale = true;
    m_vecScale = QVector3D(1,1,1);
}

HierarchicalObject::~HierarchicalObject()
{

}

QVector3D HierarchicalObject::position() const
{
    return m_vecPosition;
}

void HierarchicalObject::setPosition(const QVector3D &pos)
{
    if ( m_vecPosition == pos ) return;

    m_vecPosition = pos;
    m_bMatricesStale = true;
}

void HierarchicalObject::rebuildMatrices() const
{
    rebuildLocalToParent();
    m_matParentToLocal = m_matLocalToParent.inverted();
    m_bMatricesStale = false;
}

void HierarchicalObject::rebuildLocalToParent() const
{
    // To get from local (model) space to world space,
    // we perform transforms forward.
    // To get from world space to camera space we must
    // perform the camera transforms backward - see Camera class.
    m_matLocalToParent = Math::matrixTranslate(m_vecPosition)
            * Math::matrixOrientation(m_angAngles)
            * Math::matrixScale(m_vecScale);
}

QMatrix4x4 HierarchicalObject::parentToLocal() const
{
    if ( m_bMatricesStale )
        rebuildMatrices();

    return m_matParentToLocal;
}

QMatrix4x4 HierarchicalObject::localToParent() const
{
    if ( m_bMatricesStale )
        rebuildMatrices();

    return m_matLocalToParent;
}

EulerAngle HierarchicalObject::angles() const
{
    return m_angAngles;
}

void HierarchicalObject::setAngles(const EulerAngle &angle)
{
    if ( m_angAngles == angle ) return;

    m_angAngles = angle;
    clampAngles();
    m_bMatricesStale = true;
}

void HierarchicalObject::clampAngles()
{
    if ( m_angAngles.pitch() < -90.0f )
        m_angAngles.setPitch(-90.0f);

    else if ( m_angAngles.pitch() > 90.0f )
        m_angAngles.setPitch(90.0f);

    if ( m_angAngles.roll() < -180.0f )
        m_angAngles.setRoll(-180.0f);

    else if ( m_angAngles.roll() > 180.0f )
        m_angAngles.setRoll(180.0f);

    m_angAngles.setYaw(std::fmod(m_angAngles.yaw(), 360.0f));
}

void HierarchicalObject::translate(const QVector3D &trans)
{
    setPosition(position() + (Math::matrixOrientation(m_angAngles)*trans));
}

QMatrix4x4 HierarchicalObject::rootToLocal() const
{
    QMatrix4x4 mat = parentToLocal();
    HierarchicalObject* obj = qobject_cast<HierarchicalObject*>(parent());
    while (obj)
    {
        mat = mat * obj->parentToLocal();
        obj = qobject_cast<HierarchicalObject*>(obj->parent());
    }

    return mat;
}

void HierarchicalObject::lookAt(const QVector3D &pos)
{
    setAngles(Math::vectorToAngleSimple(pos - m_vecPosition));
}

QVector3D HierarchicalObject::scale() const
{
    return m_vecScale;
}

void HierarchicalObject::setScale(const QVector3D &vec)
{
    if ( !scalable() || m_vecScale == vec || m_vecScale.isNull() ) return;

    m_vecScale = vec;
    m_bMatricesStale = true;
}

bool HierarchicalObject::scalable() const
{
    return true;
}
