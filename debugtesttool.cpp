#include "debugtesttool.h"
#include <QKeyEvent>
#include "mapdocument.h"
#include "sceneobjectmanipulator.h"
#include "application.h"
#include "mainwindow.h"
#include "viewport.h"
#include "uiscene.h"
#include "translationhandle.h"
#include "geometryfactory.h"
#include "scenecamera.h"
#include "mapscene.h"
#include "mapgrid.h"
#include "callipermath.h"
#include "ray3d.h"

DebugTestTool::DebugTestTool(MapDocument *document) : BaseTool(DebugTestTool::staticName(), document)
{
}

DebugTestTool::~DebugTestTool()
{
    if ( m_bActive )
    {
        deactivate();
    }
}

QString DebugTestTool::staticName()
{
    return QString("DebugTestTool");
}

void DebugTestTool::vKeyPress(QKeyEvent *e)
{
    BaseTool::vKeyPress(e);
}

void DebugTestTool::vSelectedSetChanged()
{
    BaseTool::vSelectedSetChanged();
}

void DebugTestTool::vDeactivate()
{
    BaseTool::vDeactivate();
}

void DebugTestTool::vActivate()
{
    BaseTool::vActivate();
}

void DebugTestTool::vMousePress(QMouseEvent *e)
{
    BaseTool::vMousePress(e);
}

void DebugTestTool::vMouseMove(QMouseEvent *e)
{
    BaseTool::vMouseMove(e);
}

void DebugTestTool::vMouseRelease(QMouseEvent *e)
{
    BaseTool::vMouseRelease(e);
}
