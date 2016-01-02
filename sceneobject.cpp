#include "sceneobject.h"
#include "callipermath.h"
#include <cmath>
#include "scene.h"
#include "shaderstack.h"
#include "resourcemanager.h"
#include <QOpenGLTexture>

SceneObject::SceneObject(SceneObject *parent) : QObject(parent)
{
    m_pScene = parent ? parent->m_pScene : NULL;
    m_pGeometry.reset(new GeometryData());
    m_vecPosition = QVector3D(0,0,0);
    m_angAngles = EulerAngle(0,0,0);
    m_bMatricesStale = true;
}

SceneObject::~SceneObject()
{
}

GeometryData* SceneObject::geometry() const
{
    return m_pGeometry.data();
}

void SceneObject::setGeometry(GeometryData *data)
{
    if ( !data || m_pGeometry.data() == data ) return;

    m_pGeometry.reset(data);
}

QVector3D SceneObject::position() const
{
    return m_vecPosition;
}

void SceneObject::setPosition(const QVector3D &pos)
{
    if ( m_vecPosition == pos ) return;

    m_vecPosition = pos;
    m_bMatricesStale = true;
}

void SceneObject::rebuildMatrices() const
{
    rebuildLocalToParent();
    m_matParentToLocal = m_matLocalToParent.inverted();
    m_bMatricesStale = false;
}

void SceneObject::rebuildLocalToParent() const
{
    // To get from local (model) space to world space,
    // we perform transforms forward.
    // To get from world space to camera space we must
    // perform the camera transforms backward - see Camera class.
    m_matLocalToParent = Math::matrixTranslate(m_vecPosition)
            * Math::matrixOrientation(m_angAngles);
}

QMatrix4x4 SceneObject::parentToLocal() const
{
    if ( m_bMatricesStale )
        rebuildMatrices();

    return m_matParentToLocal;
}

QMatrix4x4 SceneObject::localToParent() const
{
    if ( m_bMatricesStale )
        rebuildMatrices();

    return m_matLocalToParent;
}

bool SceneObject::isEmpty() const
{
    return m_pGeometry->isEmpty();
}

EulerAngle SceneObject::angles() const
{
    return m_angAngles;
}

void SceneObject::setAngles(const EulerAngle &angle)
{
    if ( m_angAngles == angle ) return;

    m_angAngles = angle;
    clampAngles();
    m_bMatricesStale = true;
}

void SceneObject::clampAngles()
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

void SceneObject::translate(const QVector3D &trans)
{
    setPosition(position() + (Math::matrixOrientation(m_angAngles)*trans));
}

SceneObject* SceneObject::parentObject() const
{
    return qobject_cast<SceneObject*>(parent());
}

QMatrix4x4 SceneObject::rootToLocal() const
{
    QMatrix4x4 mat = parentToLocal();
    SceneObject* obj = parentObject();
    while (obj)
    {
        mat = mat * obj->parentToLocal();
        obj = obj->parentObject();
    }

    return mat;
}

QList<SceneObject*> SceneObject::children() const
{
    return findChildren<SceneObject*>(QString(), Qt::FindDirectChildrenOnly);
}

void SceneObject::lookAt(const QVector3D &pos)
{
    setAngles(Math::vectorToAngleSimple(pos - m_vecPosition));
}

Scene* SceneObject::scene() const
{
    return m_pScene;
}

bool SceneObject::editable() const
{
    return true;
}

void SceneObject::draw(ShaderStack *stack)
{
    bool shaderOverridden = false;
    if ( !geometry()->isEmpty() )
    {
        // If we have a shader override, push the override.
        QString shaderOverrideName = geometry()->shaderOverride();
        if ( !shaderOverrideName.isNull() )
        {
            ShaderProgram* program = resourceManager()->shader(shaderOverrideName);
            if ( program )
            {
                shaderOverridden = true;
                stack->shaderPush(program);
            }
        }
    }

    // Multiply the modelWorld matrix by our current one.
    // This updates the shader uniform too.
    // We do this even if the geometry is empty, so that the
    // transformation will apply recursively.
    // It is the caller's responsibility to manage pushing
    // and popping of the m2w matrix.
    stack->modelToWorldPostMultiply(localToParent());

    if ( !geometry()->isEmpty() )
    {
        // Upload and bind the geometry.
        geometry()->upload();
        geometry()->bindVertices(true);
        geometry()->bindIndices(true);

        // Apply the data format.
        geometry()->applyDataFormat(stack->shaderTop());

        // Apply the texture.
        QOpenGLTexture* tex = resourceManager()->texture(geometry()->texture(0));
        tex->bind(0);

        // Draw.
        geometry()->draw();

        // Pop the shader if we pushed one earlier.
        if ( shaderOverridden )
        {
            stack->shaderPop();
        }
    }
}
