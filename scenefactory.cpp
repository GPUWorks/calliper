#include "scenefactory.h"
#include "scene.h"
#include "originmarker.h"
#include "camera.h"
#include <QVector3D>
#include "mapdocument.h"
#include "sceneobject.h"
#include "geometryfactory.h"

namespace SceneFactory
{
    Scene* defaultScene(MapDocument* document)
    {
        Scene* scene = new Scene(document);

        OriginMarker* o = new OriginMarker(scene->root());
        o->setObjectName("origin");

        Camera* c = new Camera(scene->root());
        c->setObjectName("camera");
        c->setPosition(QVector3D(128, 128, 80));
        c->lookAt(QVector3D(0,0,0));

        return scene;
    }
}