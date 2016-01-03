#include "boundingbox.h"
#include <QVector4D>

BoundingBox::BoundingBox()
{
}

BoundingBox::BoundingBox(const QVector3D &vMin, const QVector3D &vMax)
{
    m_vecMin = vMin;
    m_vecMax = vMax;
    sortVectors();
}

void BoundingBox::sortVectors()
{
    if ( m_vecMin.x() > m_vecMax.x() )
    {
        float temp = m_vecMin.x();
        m_vecMin.setX(m_vecMax.x());
        m_vecMax.setX(temp);
    }

    if ( m_vecMin.y() > m_vecMax.y() )
    {
        float temp = m_vecMin.y();
        m_vecMin.setY(m_vecMax.y());
        m_vecMax.setY(temp);
    }

    if ( m_vecMin.z() > m_vecMax.z() )
    {
        float temp = m_vecMin.z();
        m_vecMin.setZ(m_vecMax.z());
        m_vecMax.setZ(temp);
    }
}

QVector3D BoundingBox::min() const
{
    return m_vecMin;
}

void BoundingBox::setMin(const QVector3D &vec)
{
    if ( m_vecMin == vec ) return;

    m_vecMin = vec;
    sortVectors();
}

QVector3D BoundingBox::max() const
{
    return m_vecMax;
}

void BoundingBox::setMax(const QVector3D &vec)
{
    if ( m_vecMax == vec ) return;

    m_vecMax = vec;
    sortVectors();
}

bool BoundingBox::isNull() const
{
    return m_vecMin.isNull() && m_vecMax.isNull();
}

QList<QVector3D> BoundingBox::corners() const
{
    return isNull() ? QList<QVector3D>() : corners(m_vecMin, m_vecMax);
}

QList<QVector3D> BoundingBox::corners(const QVector3D &min, const QVector3D &max)
{
    QList<QVector3D> list;

    list.append(min);
    list.append(QVector3D(max.x(), min.y(), min.z()));
    list.append(QVector3D(min.x(), max.y(), min.z()));
    list.append(QVector3D(max.x(), max.y(), min.z()));
    list.append(QVector3D(min.x(), min.y(), max.z()));
    list.append(QVector3D(max.x(), min.y(), max.z()));
    list.append(QVector3D(min.x(), max.y(), max.z()));
    list.append(QVector3D(max));

    return list;
}

BoundingBox BoundingBox::transformed(const QMatrix4x4 &transform) const
{
    QList<QVector3D> list = corners();

    for ( int i = 0; i < list.count(); i++ )
    {
        list[i] = (transform * QVector4D(list[i],1)).toVector3D();
    }

    QVector3D min = list.at(0), max = list.at(0);
    for ( int i = 1; i < list.count(); i++ )
    {
        const QVector3D &v = list.at(i);

        if ( v.x() < min.x() )
            min.setX(v.x());
        if ( v.x() > max.x() )
            max.setX(v.x());

        if ( v.y() < min.y() )
            min.setY(v.y());
        if ( v.y() > max.y() )
            max.setY(v.y());

        if ( v.z() < min.z() )
            min.setZ(v.z());
        if ( v.z() > max.z() )
            max.setZ(v.z());
    }

    return BoundingBox(min,max);
}

bool BoundingBox::operator ==(const BoundingBox &other) const
{
    return m_vecMin == other.m_vecMin &&
            m_vecMax == other.m_vecMax;
}

bool BoundingBox::operator !=(const BoundingBox &other) const
{
    return !(*this == other);
}

QVector3D BoundingBox::centroid() const
{
    return m_vecMin + ((m_vecMax-m_vecMin)/2.0f);
}