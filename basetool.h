#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QString>
#include <QSet>
#include "cameracontroller.h"

class MapDocument;
class QMouseEvent;
class QKeyEvent;
class QWheelEvent;
class SceneObject;

class BaseTool : public QObject
{
    Q_OBJECT
public:
    explicit BaseTool(const QString &name, MapDocument* document);
    virtual ~BaseTool();    // Subclasses MUST handle deactivation here.
    virtual QString toolName() const;
    MapDocument* document() const;

    Qt::KeyboardModifiers keyboardModifiers() const;
    bool keyboardModifierActive(Qt::KeyboardModifier mod) const;

    // The functions below are not virtual, as they contain code that should always
    // be run regardless of what the tool is. The virtual functions are called
    // from these functions and can be re-implemented by subclasses.

    // Activation
    void activate();
    void deactivate();
    bool isActive() const;

    // Mouse input
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);

    // Keyboard input
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);


public slots:
    void selectedSetChanged();
    void update(int msec);

protected:
    // These are the basic actions that should be taken if the subclass tool does not override them.
    // This lets us default to things like toggling camera movement, changing grid size, etc.
    // and allows subclass tools to restrict these actions if required, but allow them otherwise.
    virtual void vActivate();
    virtual void vDeactivate();
    virtual void vMousePress(QMouseEvent*);
    virtual void vMouseMove(QMouseEvent*);
    virtual void vMouseRelease(QMouseEvent*);
    virtual void vWheel(QWheelEvent*);
    virtual void vKeyPress(QKeyEvent*);
    virtual void vKeyRelease(QKeyEvent*);
    virtual void vSelectedSetChanged();
    virtual void vUpdate(int msec);

    void setMouseLookEnabled(bool enabled);
    void toggleMouseLookEnabled();
    void saveFrameToDesktop();

	void addToSelectedSet(SceneObject* obj, bool clearPrevious = false);

    MapDocument*    m_pDocument;
    bool            m_bActive;

    Qt::KeyboardModifiers   m_flKBModifiers;
    CameraController        m_CameraController;
    bool                    m_bMouseLookEnabled;
};

#endif // BASETOOL_H
