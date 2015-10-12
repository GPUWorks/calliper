#ifndef CCAMERALENS_H
#define CCAMERALENS_H

#include <QObject>
#include <QMatrix4x4>

class CCameraLens : public QObject
{
    Q_OBJECT
public:
    enum LensType
    {
        Orthographic,
        Perspective
    };

    explicit CCameraLens(LensType type, QObject* parent = 0);
    QMatrix4x4 projectionMatrix() const;

    LensType type() const;
    void setType(LensType type);

    float fieldOfView() const;
    void setFieldOfView(float fov);

    float aspectRatio() const;
    void setAspectRatio(float ratio);

    float leftPlane() const;
    void setLeftPlane(float plane);

    float rightPlane() const;
    void setRightPlane(float plane);

    float topPlane() const;
    void setTopPlane(float plane);

    float bottomPlane() const;
    void setBottomPlane(float plane);

    float nearPlane() const;
    void setNearPlane(float plane);

    float farPlane() const;
    void setFarPlane(float plane);

    void setPlanes(float left, float right, float top, float bottom, float near, float far);

    static QMatrix4x4 perspectiveMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
    static QMatrix4x4 orthographicMatrix(float top, float bottom, float left, float right, float nearPlane, float farPlane);

signals:
    void lensChanged();

private:
    LensType    m_iType;

    float       m_flPFOV;
    float       m_flPAspectRatio;

    float       m_flTopPlane;
    float       m_flBottomPlane;
    float       m_flLeftPlane;
    float       m_flRightPlane;
    float       m_flNearPlane;
    float       m_flFarPlane;

    mutable bool        m_bMatrixDirty;
    mutable QMatrix4x4  m_matProjectionMatrix;

    void regenerateMatrix() const;
};

#endif // CCAMERALENS_H
