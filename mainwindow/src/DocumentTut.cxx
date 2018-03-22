#include "DocumentTut.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

TopoDS_Shape
MakeBottle(const Standard_Real myWidth , const Standard_Real myHeight , const Standard_Real myThickness);

DocumentTut::DocumentTut( const int theIndex, ApplicationCommonWindow* app )
: DocumentCommon( theIndex, app )
{
}

DocumentTut::~DocumentTut()
{
}

void DocumentTut::onMakeBottle()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	TopoDS_Shape aBottle = MakeBottle(50, 70, 30);
	//mc
	static int i = 0;
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Pnt(0., 0., 0.), gp_Pnt(100 * i++, 0., 0.));
	TopLoc_Location location(trsf);
	aBottle.Move(location);
	//mc-end
	Handle(AIS_Shape) AISBottle = new AIS_Shape(aBottle);
	//mc
	//Handle(AIS_InteractiveObject) aisBottleObject = AISBottle;
	//qDebug() << aisBottleObject->HilightMode();
	//aisBottleObject->SetHilightMode(1);

	Handle(AIS_InteractiveContext) context = getContext();
	context->RemoveAll();
	context->SetMaterial(AISBottle, Graphic3d_NOM_GOLD);
	context->SetDisplayMode(AISBottle, 1, Standard_False);
	//context->SetCurrentObject(AISBottle, Standard_False);
	context->Load(AISBottle);
	context->Redisplay(AISBottle);
	context->SetWidth(AISBottle,1);
	qDebug() << context->IsDisplayed(AISBottle);
	qDebug() << context->DisplayStatus(AISBottle);
	context->Display(AISBottle, Standard_True);
	//
	emit selectionChanged();
	fitAll();
	QApplication::restoreOverrideCursor();
	/*
    QApplication::setOverrideCursor( Qt::WaitCursor );
    TopoDS_Shape aBottle=MakeBottle(50,70,30);
    Handle(AIS_Shape) AISBottle=new AIS_Shape(aBottle);
    getContext()->SetMaterial(AISBottle,Graphic3d_NOM_GOLD);
    getContext()->SetDisplayMode(AISBottle,1,Standard_False);
    getContext()->Display(AISBottle, Standard_False);	
    //getContext()->SetCurrentObject(AISBottle,Standard_False);
    emit selectionChanged();
    fitAll();
    QApplication::restoreOverrideCursor();*/
}


