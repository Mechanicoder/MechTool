#include "DocumentCommon.h"

#include "ApplicationCommon.h"
#include "Transparency.h"
#include "Material.h"

#include "../test/SurfaceProperty.h" // 分析叶片法向
#include "../Draw/DrawProxy.h"
#include "../test/SurfaceTools.h"

#include "DrawNormalRate.h"

#include <QStatusBar>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QInputDialog> // 输入对话框

#include <Aspect_DisplayConnection.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS.hxx> // 转换
#include <TopoDS_Shape.hxx>

#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>

#include <iostream>
// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
Handle(V3d_Viewer) DocumentCommon::Viewer (const Standard_ExtString ,
                                           const Standard_CString ,
                                           const Standard_Real theViewSize,
                                           const V3d_TypeOfOrientation theViewProj,
                                           const Standard_Boolean theComputedMode,
                                           const Standard_Boolean theDefaultComputedMode )
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;

  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    aDisplayConnection = new Aspect_DisplayConnection (qgetenv ("DISPLAY").constData());
#endif
    aGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);
  }

  Handle(V3d_Viewer) aViewer = new V3d_Viewer (aGraphicDriver);
  aViewer->SetDefaultViewSize (theViewSize);
  aViewer->SetDefaultViewProj (theViewProj);
  aViewer->SetComputedMode (theComputedMode);
  aViewer->SetDefaultComputedMode (theDefaultComputedMode);
  return aViewer;
}

DocumentCommon::DocumentCommon(const int theIndex, ApplicationCommonWindow* app)
    : QObject(app),
    myApp(app),
    myIndex(theIndex),
    myNbViews(0), drawImage_(nullptr)
{
	TCollection_ExtendedString a3DName("Visu3D");

	myViewer = Viewer(a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True);

	myViewer->SetDefaultLights();
	myViewer->SetLightOn();

	myContext = new AIS_InteractiveContext(myViewer);

	myContext->SetAutoActivateSelection(Standard_True);
	myContext->SetAutomaticHilight(Standard_True);
	myContext->SetDisplayMode(AIS_Shaded, false);

    myContext->SetDeviationCoefficient(0.001 * 0.1);


	//myContext->SetHilightColor(Quantity_NOC_WHITE);
	//myContext->SelectionColor(Quantity_NOC_GREEN);
    drawer_ = new DrawProxy(myContext);
}

DocumentCommon::~DocumentCommon()
{
}

ApplicationCommonWindow* DocumentCommon::getApplication()
{
  return myApp;
}

MDIWindow* DocumentCommon::createNewMDIWindow()
{
  QMdiArea* ws = myApp->getWorkspace();
  return new MDIWindow (this, ws, 0);
}

void DocumentCommon::onCreateNewView()
{
  QMdiArea* ws = myApp->getWorkspace();
  MDIWindow* w = createNewMDIWindow();
  
  if (!w)
    return;

  ws->addSubWindow (w);
  myViews.append (w);

  connect( w,    SIGNAL( selectionChanged() ),
           this, SLOT( onSelectionChanged() ) );
  connect( w, SIGNAL( message( const QString&, int ) ),
           myApp->statusBar(), SLOT( showMessage( const QString&, int ) ) );
  connect( w, SIGNAL( sendCloseView( MDIWindow* ) ),
           this, SLOT( onCloseView( MDIWindow* ) ) );

  QString aName;
  w->setWindowTitle( aName.sprintf( "Document %d:%d", myIndex, ++myNbViews ) );
  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  
  w->setWindowIcon( QPixmap( dir + QObject::tr("ICON_DOC") ) );

  if ( ws->subWindowList().isEmpty() )
  {
    // Due to strange Qt4.2.3 feature the child window icon is not drawn
    // in the main menu if showMaximized() is called for a non-visible child window
    // Therefore calling show() first...
    w->show();
    w->showMaximized();
  }
  else
    w->show();

  w->setFocus();

  getApplication()->onSelectionChanged();
}

void DocumentCommon::onCloseView(MDIWindow* theView)
{
    removeView(theView);
    if( countOfWindow() == 0 )
        emit sendCloseDocument( this );
}

void DocumentCommon::removeView(MDIWindow* theView)
{
    if ( myViews.count( theView ) )
    {
    myViews.removeAll(theView);
    delete theView;
  }
}
void DocumentCommon::removeViews()
{
  while( myViews.count() )
  {
    removeView( myViews.first() );
  }
}

int DocumentCommon::countOfWindow()
{
  return myViews.count();
}

Handle(AIS_InteractiveContext) DocumentCommon::getContext()
{
  return myContext;
}

void DocumentCommon::fitAll()
{
  QList<MDIWindow*>::iterator i;
  for ( i = myViews.begin(); i != myViews.end(); i++ )
    (*i)->fitAll();
}

void DocumentCommon::onWireframe()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextCurrent() )
        myContext->SetDisplayMode( myContext->Current(), 0, false );
    myContext->UpdateCurrentViewer();
    getApplication()->onSelectionChanged();
    QApplication::restoreOverrideCursor();
}

void DocumentCommon::onShading()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextCurrent() )
        myContext->SetDisplayMode( myContext->Current(), AIS_Shaded, false );

    
    myContext->UpdateCurrentViewer();
    getApplication()->onSelectionChanged();
    QApplication::restoreOverrideCursor();
}

void DocumentCommon::onColor()
{
    QColor aColor ;
    Quantity_Color aShapeColor;
    myContext->InitCurrent();
    Handle(AIS_InteractiveObject) Current = myContext->Current() ;
    if ( Current->HasColor () )
    {
        aShapeColor = myContext->Color( Current );
        aColor.setRgb( aShapeColor.Red() * 255, aShapeColor.Green() * 255, aShapeColor.Blue() * 255 );
    }
    else
        aColor.setRgb( 255, 255, 255 );

    QColor aRetColor = QColorDialog::getColor( aColor );
    if ( aRetColor.isValid() )
    {
        Quantity_Color color( aRetColor.red() / 255., aRetColor.green() / 255.,
                        aRetColor.blue() / 255., Quantity_TOC_RGB );
        for (; myContext->MoreCurrent(); myContext->NextCurrent() )
            myContext->SetColor( myContext->Current(), color.Name() );
        /*AIS_ListOfInteractive::iterator it = mySelectedList.begin();
        for (it; it != mySelectedList.end(); it++)
        {
            myContext->SetColor(*it, color.Name());
        }*/
    }
}

void DocumentCommon::onMaterial( int theMaterial )
{
    for ( myContext->InitCurrent(); myContext->MoreCurrent (); myContext->NextCurrent () )
        myContext->SetMaterial( myContext->Current(), (Graphic3d_NameOfMaterial)theMaterial );
}

void DocumentCommon::onMaterial()
{
    DialogMaterial* m = new DialogMaterial();
    connect( m, SIGNAL( sendMaterialChanged( int ) ), this, SLOT( onMaterial( int ) ) );
    m->exec();
}

void DocumentCommon::onTransparency( int theTrans )
{
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextSelected() )
        myContext->SetTransparency( myContext->Current(), ((Standard_Real)theTrans) / 10.0 );
}

void DocumentCommon::onSelectionChanged()
{
    mySelectedList.Clear();

    for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
    {
        Handle(AIS_InteractiveObject) ais_obj = myContext->SelectedInteractive();
        if (ais_obj)
        {
            mySelectedList.Append(ais_obj);
        }
    }
    emit selectionChanged();
}

void DocumentCommon::onTransparency()
{
    DialogTransparency* aDialog = new DialogTransparency();
    connect( aDialog, SIGNAL( sendTransparencyChanged( int ) ), this, SLOT( onTransparency( int ) ) );
    aDialog->exec();
}

void DocumentCommon::onDelete()
{
    myContext->EraseSelected (Standard_False);
    myContext->ClearSelected();
    myContext->UpdateCurrentViewer();
    getApplication()->onSelectionChanged();
}

void DocumentCommon::onAnalyseNormal()
{
    if (mySelectedList.Size() == 1)
    {
        std::cout << "Analysis face normal\n";

        Handle(AIS_InteractiveObject) obj = mySelectedList.First();
        Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(obj);

        TopoDS_Face face = TopoDS::Face(shape->Shape());
        if (!face.IsNull())
        {
            double umin, umax, vmin, vmax;
            umin = umax = vmin = vmax = 0.0;
            // 法向变化
            /*std::vector<std::vector<double> > res = SurfaceProperty::NormalRate(
                face, umin, umax, vmin, vmax, false);*/
            // 个点曲率
            std::vector<std::vector<double> > res = SurfaceProperty::Curvature(
                face, umin, umax, vmin, vmax, false);
            /*for (size_t i = 0; i < res.size(); i++)
            {
                for (size_t j = 0; j < res[i].size(); j++)
                {
                    int first = res[i][j];
                }
            }*/
            drawImage_ = new DrawNormalRate(res, umin, umax, vmin, vmax, myViews.front());
            drawImage_->show();
        }
    }
}

void DocumentCommon::OnSurfaceOffset()
{
    if (mySelectedList.Size() == 1)
    {
        std::cout << "Draw offset curves\n";

        Handle(AIS_InteractiveObject) obj = mySelectedList.First();
        Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(obj);

        TopoDS_Face face = TopoDS::Face(shape->Shape());
        if (!face.IsNull())
        {
            std::cout << "Select a face\n";

            Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

            QInputDialog *input = new QInputDialog(nullptr);
            if (input->exec() == QDialog::Accepted)
            {
                double dist = input->textValue().toDouble();
                std::cout << "Offset distance: " + std::to_string(dist) + "\n";
                std::vector<std::vector<gp_Pnt> > pnts;
                std::vector<Handle(Geom_Curve)> curves = SurfaceTools::OffsetCurves(surf, dist, pnts);
                drawer_->DrawCurvs(curves);

                //for (size_t i = 0; i < pnts.size(); i++)
                //{
                //    drawer_->DrawPoints(pnts[i]);
                //}
            }
        }
    }
}
