#include <BRep_Tool.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>

#include <BRepLib.hxx>

#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <GC_MakeCircle.hxx>

#include <GCE2d_MakeSegment.hxx>

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>

#include <TopTools_ListOfShape.hxx>

#define MC 1
#if (MC == 1)

TopoDS_Shape
MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight,
	const Standard_Real myThickness)
{
	// Profile : Define Support Points
	gp_Pnt aPnt1(-myWidth / 2., 0, 0);
	gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
	gp_Pnt aPnt3(0, -myThickness / 2., 0);
	gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
	gp_Pnt aPnt5(myWidth / 2., 0, 0);

	// Profile : Define the Geometry
	Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
	Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
	Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

	// Profile : Define the Topology
	TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
	TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
	TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
	TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

	// Complete Profile
	gp_Ax1 xAxis = gp::OX();
	gp_Trsf aTrsf;

	aTrsf.SetMirror(xAxis);
	BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
	TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
	TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

	BRepBuilderAPI_MakeWire mkWire;
	mkWire.Add(aWire);
	mkWire.Add(aMirroredWire);
	TopoDS_Wire myWireProfile = mkWire.Wire();

	// Body : Prism the Profile
	TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
	gp_Vec aPrismVec(0, 0, myHeight);
	TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

	// Body : Apply Fillets
	BRepFilletAPI_MakeFillet mkFillet(myBody);
	TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
	while (anEdgeExplorer.More()) {
		TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
		//Add edge to fillet algorithm
		mkFillet.Add(myThickness / 12., anEdge);
		anEdgeExplorer.Next();
	}

	myBody = mkFillet.Shape();

	// Body : Add the Neck	
	gp_Pnt neckLocation(0, 0, myHeight);
	gp_Dir neckAxis = gp::DZ();
	gp_Ax2 neckAx2(neckLocation, neckAxis);

	Standard_Real myNeckRadius = myThickness / 4.;
	Standard_Real myNeckHeight = myHeight / 10.;

	BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
	TopoDS_Shape myNeck = MKCylinder.Shape();

	myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

	// Body : Create a Hollowed Solid
	TopoDS_Face   faceToRemove;
	Standard_Real zMax = -1;

	for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()) {
		TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
		// Check if <aFace> is the top face of the bottle’s neck 
		Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
		if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
			Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
			gp_Pnt aPnt = aPlane->Location();
			Standard_Real aZ = aPnt.Z();
			if (aZ > zMax) {
				zMax = aZ;
				faceToRemove = aFace;
			}
		}
	}

	TopTools_ListOfShape facesToRemove;
	facesToRemove.Append(faceToRemove);
	myBody = BRepOffsetAPI_MakeThickSolid(myBody, facesToRemove, -myThickness / 50, 1.e-3);
	// Threading : Create Surfaces
	Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
	Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

	// Threading : Define 2D Curves
	gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
	gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
	gp_Ax2d anAx2d(aPnt, aDir);

	Standard_Real aMajor = 2. * M_PI;
	Standard_Real aMinor = myNeckHeight / 10;

	Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
	Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
	Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
	Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
	gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
	gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

	Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
	// Threading : Build Edges and Wires
	TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
	TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
	TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
	TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
	TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
	TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
	BRepLib::BuildCurves3d(threadingWire1);
	BRepLib::BuildCurves3d(threadingWire2);

	// Create Threading 
	BRepOffsetAPI_ThruSections aTool(Standard_True);
	aTool.AddWire(threadingWire1);
	aTool.AddWire(threadingWire2);
	aTool.CheckCompatibility(Standard_False);

	TopoDS_Shape myThreading = aTool.Shape();

	// Building the Resulting Compound 
	TopoDS_Compound aRes;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(aRes);
	aBuilder.Add(aRes, myBody);
	aBuilder.Add(aRes, myThreading);

	return aRes;
}


#elif (MC == 2)
#include <Geom_BezierCurve.hxx>

TopoDS_Shape
MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight,
	const Standard_Real myThickness)
{
	gp_Pnt pnt1(0, 0, 0);
	gp_Pnt pnt2(myWidth / 6., myHeight, 0);
	gp_Pnt pnt3(myWidth / 3., myHeight, 0);
	gp_Pnt pnt4(myWidth / 2., myHeight / 10., 0);
	gp_Pnt pnt5(2. * myWidth / 3., myHeight, 0);
	gp_Pnt pnt6(5. * myWidth / 6. , myHeight / 2., 0);
	gp_Pnt pnt7(myWidth, 0, 0);
	
	// Building the Resulting Compound 
	Handle(Geom_TrimmedCurve) cir1 = GC_MakeArcOfCircle(pnt1, gp_Pnt(0, myHeight, 0), pnt2);
	Handle(Geom_TrimmedCurve) seg1 = GC_MakeSegment(pnt1, pnt2);
	Handle(Geom_TrimmedCurve) seg2 = GC_MakeSegment(pnt2, pnt3);
	Handle(Geom_TrimmedCurve) seg3 = GC_MakeSegment(pnt3, pnt4);
	Handle(Geom_TrimmedCurve) seg4 = GC_MakeSegment(pnt4, pnt5);
	Handle(Geom_TrimmedCurve) seg5 = GC_MakeSegment(pnt5, pnt7);
	Handle(Geom_TrimmedCurve) seg6 = GC_MakeSegment(pnt7, pnt1);

	TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(cir1);
	TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(seg2);
	TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(seg3);
	TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(seg4);
	TopoDS_Edge edge5 = BRepBuilderAPI_MakeEdge(seg5);
	TopoDS_Edge edge6 = BRepBuilderAPI_MakeEdge(seg6);

	TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge1, edge2, edge3, edge4);
	TopoDS_Wire clsWire = BRepBuilderAPI_MakeWire(wire, edge5);
	TopoDS_Wire clsWire1 = BRepBuilderAPI_MakeWire(clsWire, edge6);

	TopoDS_Face face = BRepBuilderAPI_MakeFace(clsWire1);

	TopoDS_Shape shape = BRepPrimAPI_MakeRevol(face, gp_Ax1(gp_Pnt(0,0,0), gp::DX()), 1.);
	return shape;


	/*
	TopoDS_Compound aRes;
	TColgp_Array1OfPnt vec_pnt(0,7);
	for (int i = 0; i < 7; i++)
	{
		vec_pnt.SetValue(i, gp_Pnt(i, i+2,i*i ));
	}

	Geom_BezierCurve curve(vec_pnt);
	BRepBuilderAPI_MakeEdge edge(&curve);
	if (edge.IsDone())
	{
		TopoDS_Edge myEdge; edge.Edge();

		return myEdge;
	}
	TopoDS_Edge myEdge = BRepBuilderAPI_MakeEdge(&curve);
	return myEdge;*/
}
#endif