#ifndef DOCUMENTCOMMON_H
#define DOCUMENTCOMMON_H

#include "MDIWindow.h"
//#include "IESample.h"

#include <QObject>
#include <QList>

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class ApplicationCommonWindow;
class DrawNormalRate;
class DrawProxy;

class COMMONSAMPLE_EXPORT DocumentCommon : public QObject
{
    Q_OBJECT

public:
    DocumentCommon(const int, ApplicationCommonWindow*);
    ~DocumentCommon();

    ApplicationCommonWindow*       getApplication();
    Handle(AIS_InteractiveContext) getContext();
    void                           removeView(MDIWindow*);
    void                           removeViews();
    int                            countOfWindow();
    void                           fitAll();

protected:
    virtual MDIWindow*                   createNewMDIWindow();

signals:
    void                           selectionChanged();
    void                           sendCloseDocument(DocumentCommon*);

public slots:
    virtual void                   onCloseView(MDIWindow*);
    virtual void                   onCreateNewView();
    virtual void                   onMaterial();
    virtual void                   onMaterial(int);
    virtual void                   onDelete();
    virtual void onAnalyseNormal(); // 分析曲面法向
    virtual void OnSurfaceOffset(); // 构造曲面偏置线

    void                           onWireframe();
    void                           onShading();
    void                           onColor();
    void                           onTransparency();
    void                           onTransparency(int);

    void onSelectionChanged();

private:
    Handle(V3d_Viewer)             Viewer(const Standard_ExtString theName,
        const Standard_CString theDomain,
        const Standard_Real theViewSize,
        const V3d_TypeOfOrientation theViewProj,
        const Standard_Boolean theComputedMode,
        const Standard_Boolean theDefaultComputedMode);

protected:
    ApplicationCommonWindow*       myApp;
    QList<MDIWindow*>              myViews;
    Handle(V3d_Viewer)             myViewer;
    Handle(AIS_InteractiveContext) myContext;
    int                            myIndex;
    int                            myNbViews;

    AIS_ListOfInteractive mySelectedList; // 选择的对象

    DrawNormalRate *drawImage_;

    DrawProxy *drawer_;
};

#endif


