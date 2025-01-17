#include "scenecamera.h"
#include "callipermath.h"
#include <QtMath>
#include "geometryfactory.h"
#include "shaderstack.h"
#include "pervertexcolorshader.h"
#include "resourcemanager.h"
#include <QPoint>
#include <QSize>
#include <QJsonArray>

#define MAX_PITCH_DELTA 89.0f
#define MAX_ROLL_DELTA 180.0f

SceneCamera::SceneCamera(SceneObject *parent) : SceneObject(parent)
{
    initDefaults();
    m_LocalLensBounds = m_pLens->localViewVolumeBounds();
    rebuildViewBoundsGeometry();
}

void SceneCamera::initDefaults()
{
    m_pLens.reset(new CameraLens(CameraLens::Perspective));
    m_pBoundsGeom.reset(new GeometryData());
    m_bDrawBounds = false;
}

SceneCamera::SceneCamera(const SceneCamera &cloneFrom) : SceneObject(cloneFrom),
    m_pLens(new CameraLens(*cloneFrom.m_pLens.data())), m_pBoundsGeom(new GeometryData(*cloneFrom.m_pBoundsGeom.data()))
{
    m_LocalLensBounds = cloneFrom.m_LocalLensBounds;
    m_bDrawBounds = cloneFrom.m_bDrawBounds;
}

SceneCamera::~SceneCamera()
{
}

bool SceneCamera::drawBounds() const
{
    return m_bDrawBounds;
}

void SceneCamera::setDrawBounds(bool enabled)
{
    m_bDrawBounds = enabled;
}

CameraLens* SceneCamera::lens() const
{
    return m_pLens.data();
}

void SceneCamera::rebuildLocalToParent() const
{
    // To get from local (model) space to world space,
    // we perform transforms forward.
    // To get from world space to camera space we must
    // perform the camera transforms backward.

    // Our default orientation should be looking down X,
    // but the OpenGL camera by default looks down (Hammer's) Y.
    // This means we need to apply an orientation first so that we face down
    // X before we do our other transforms.
    static const QMatrix4x4 defaultRot = Math::matrixRotateZ(qDegreesToRadians(-90.0f));

    SceneObject::rebuildLocalToParent();
    m_matLocalToParent = m_matLocalToParent * defaultRot;
}

void SceneCamera::clampAngles()
{
    // These are slightly different to normal because the camera
    // needs to avoid the pitch singularities.

    if ( m_angAngles.pitch() < -MAX_PITCH_DELTA )
        m_angAngles.setPitch(-MAX_PITCH_DELTA);

    else if ( m_angAngles.pitch() > MAX_PITCH_DELTA )
        m_angAngles.setPitch(MAX_PITCH_DELTA);

    if ( m_angAngles.roll() < -MAX_ROLL_DELTA )
        m_angAngles.setRoll(-MAX_ROLL_DELTA);

    else if ( m_angAngles.roll() > MAX_ROLL_DELTA )
        m_angAngles.setRoll(MAX_ROLL_DELTA);

    m_angAngles.setYaw(std::fmod(m_angAngles.yaw(), 360.0f));
}

void SceneCamera::rebuildViewBoundsGeometry()
{
    m_pBoundsGeom->clear();
    if ( m_LocalLensBounds.isNull() ) return;

    m_pBoundsGeom.reset(GeometryFactory::lineCuboid(m_LocalLensBounds, QColor::fromRgb(0xffff0000)));
    m_pBoundsGeom->setShaderOverride(PerVertexColorShader::staticName());
}

void SceneCamera::draw(ShaderStack *stack)
{
    stack->modelToWorldPostMultiply(localToParent());

    if ( stack->cameraParams().hierarchicalObject() != this )
    {
        if ( m_bDrawBounds )
        {
            BoundingBox bounds = lens()->localViewVolumeBounds();
            if ( bounds != m_LocalLensBounds )
            {
                m_LocalLensBounds = bounds;
                rebuildViewBoundsGeometry();
            }

            if ( !m_pBoundsGeom->isEmpty() )
            {
                stack->shaderPush(resourceManager()->shader(m_pBoundsGeom->shaderOverride()));
                stack->modelToWorldPush();
                stack->modelToWorldPostMultiply(Math::openGLToHammer());

                m_pBoundsGeom->upload();
                m_pBoundsGeom->bindVertices(true);
                m_pBoundsGeom->bindIndices(true);
                m_pBoundsGeom->applyDataFormat(stack->shaderTop());
                m_pBoundsGeom->draw();

                stack->modelToWorldPop();
                stack->shaderPop();
            }
        }
    }
}

bool SceneCamera::scalable() const
{
    // Scaling a camera doesn't make any sense, silly.
    return false;
}

QVector3D SceneCamera::worldTranslation(const QPoint &p0, const QPoint &p1, const QSize &viewSize, float distFromCamera) const
{
//    Get the camera-space directions corresponding to the two points.
//    If the lens is perspective, these will diverge from the camera origin ((0,0,0) camera space).
//    QVector3D cDir0 = lens()->mapPoint(p0, viewSize);
//    QVector3D cDir1 = lens()->mapPoint(p1, viewSize);

//    Get the camera->world matrix.
    QMatrix4x4 cameraToWorld = rootToLocal().inverted() * Math::openGLToHammer();

//    Translate the directions to world space.
//    QVector3D wDir0 = (cameraToWorld * QVector4D(cDir0, 0)).toVector3D();
//    QVector3D wDir1 = (cameraToWorld * QVector4D(cDir1, 0)).toVector3D();

//    Calculate the new positions in world space.
//    These lie along the lines that begin at the camera origin in the world
//    and extend along each direction. If the lens is perspective, the further
//    the distance from the camera the further separated the two points will be.
//    QVector3D wPos0 = position() + (distFromCamera * wDir0);
//    QVector3D wPos1 = position() + (distFromCamera * wDir1);

//    Return the translation.
//    return wPos1 - wPos0;

//    In one line, this is:
     return (position() + (distFromCamera * ((cameraToWorld * QVector4D(lens()->mapPoint(p1, viewSize), 0)).toVector3D())))
             - (position() + (distFromCamera * ((cameraToWorld * QVector4D(lens()->mapPoint(p0, viewSize), 0)).toVector3D())));
}

QVector3D SceneCamera::frustumDirection(const QPoint &p, const QSize &viewSize) const
{
    return ((rootToLocal().inverted() * Math::openGLToHammer()) * QVector4D(lens()->mapPoint(p, viewSize), 0)).toVector3D();
}

SceneObject* SceneCamera::clone() const
{
    return new SceneCamera(*this);
}

bool SceneCamera::serialiseToJson(QJsonObject &obj) const
{
    obj.insert(ISerialisable::KEY_IDENTIFIER(), QJsonValue(SceneCamera::serialiseIdentifier()));

    // Serialise the parent object first.
    QJsonObject jsonParent;
    SceneObject::serialiseToJson(jsonParent);

    // Insert this as the superclass.
    obj.insert(ISerialisable::KEY_SUPERCLASS(), QJsonValue(jsonParent));

    obj.insert("lensType", QJsonValue(CameraLens::LensTypeNames[m_pLens->type()]));
    obj.insert("nearPlane", QJsonValue(m_pLens->nearPlane()));
    obj.insert("farPlane", QJsonValue(m_pLens->farPlane()));
    obj.insert("drawBounds", QJsonValue(m_bDrawBounds));

    if ( m_pLens->type() == CameraLens::Orthographic )
    {
        QJsonArray arrPlanes;
        arrPlanes.append(QJsonValue(m_pLens->topPlane()));
        arrPlanes.append(QJsonValue(m_pLens->bottomPlane()));
        arrPlanes.append(QJsonValue(m_pLens->leftPlane()));
        arrPlanes.append(QJsonValue(m_pLens->rightPlane()));
        obj.insert("orthoPlanes", QJsonValue(arrPlanes));
    }
    else if ( m_pLens->type() == CameraLens::Perspective )
    {
        obj.insert("fov", QJsonValue(m_pLens->fieldOfView()));
    }

    return true;
}

SceneCamera::SceneCamera(const QJsonObject &serialisedData, SceneObject *parent) : SceneObject(serialisedData.value(ISerialisable::KEY_SUPERCLASS()).toObject(), parent)
{
    initDefaults();

    if ( !validateIdentifier(serialisedData, SceneCamera::serialiseIdentifier()) )
        return;

    QString lensType = serialisedData.value("lensType").toString();
    if ( lensType == CameraLens::LensTypeNames[0] )
    {
        m_pLens->setType(CameraLens::Orthographic);
    }
    else
    {
        m_pLens->setType(CameraLens::Perspective);
    }

    QJsonValue vNearPlane = serialisedData.value("nearPlane");
    if ( vNearPlane.isDouble() )
    {
        m_pLens->setNearPlane((float)vNearPlane.toDouble());
    }

    QJsonValue vFarPlane = serialisedData.value("farPlane");
    if ( vFarPlane.isDouble() )
    {
        m_pLens->setFarPlane((float)vFarPlane.toDouble());
    }

    QJsonValue vDrawBounds = serialisedData.value("drawBounds");
    if ( vDrawBounds.isBool() )
    {
        m_bDrawBounds = vDrawBounds.toBool();
    }

    if ( m_pLens->type() == CameraLens::Orthographic )
    {
        QJsonArray arrPlanes = serialisedData.value("orthoPlanes").toArray();
        for ( int i = 0; i < arrPlanes.count(); i++ )
        {
            switch (i)
            {
            case 0:
                m_pLens->setTopPlane((float)arrPlanes.at(i).toDouble());
                break;

            case 1:
                m_pLens->setBottomPlane((float)arrPlanes.at(i).toDouble());
                break;

            case 2:
                m_pLens->setLeftPlane((float)arrPlanes.at(i).toDouble());
                break;

            case 3:
                m_pLens->setRightPlane((float)arrPlanes.at(i).toDouble());
                break;

            default:
                break;
            }
        }
    }
    else if ( m_pLens->type() == CameraLens::Perspective )
    {
        QJsonValue vFOV = serialisedData.value("fov");
        if ( vFOV.isDouble() )
        {
            m_pLens->setFieldOfView((float)vFOV.toDouble());
        }
    }
}


QString SceneCamera::serialiseIdentifier() const
{
    return staticMetaObject.className();
}
