#ifndef TRANSLATE_H
#define TRANSLATE_H
#include "IESample.h"

#include <QObject>

#include <AIS_InteractiveContext.hxx>
#include <TopTools_HSequenceOfShape.hxx>

class TranslateDlg;

class IESAMPLE_EXPORT Translate: public QObject
{
	Q_OBJECT

public:
    enum { FormatBREP, FormatIGES, FormatSTEP, FormatVRML, FormatSTL };

	Translate( QObject* );
	~Translate();

    bool                                  importModel( const int format, const Handle(AIS_InteractiveContext)& ic);
    bool                                  exportModel( const int format, const Handle(AIS_InteractiveContext)& ic);

    QString                               info() const;

protected:
    virtual Handle(TopTools_HSequenceOfShape) importModel( const int, const QString& );
    virtual bool                              exportModel( const int format, const QString& filename,
                                                    const Handle(TopTools_HSequenceOfShape)& ic);
    virtual bool                              displayShSequence(const Handle(AIS_InteractiveContext)&,
                                                    const Handle(TopTools_HSequenceOfShape)& );

private:
    QString                                   selectFileName( const int, const bool );
    TranslateDlg*                             getDialog( const int, const bool );
    Handle(TopTools_HSequenceOfShape)         getShapes( const Handle(AIS_InteractiveContext)& );

	  Handle(TopTools_HSequenceOfShape)         importBREP( const QString& );
	  Handle(TopTools_HSequenceOfShape)         importIGES( const QString& );
	  Handle(TopTools_HSequenceOfShape)         importSTEP( const QString& );

	  bool exportBREP( const QString&, const Handle(TopTools_HSequenceOfShape)& );
	  bool exportIGES( const QString&, const Handle(TopTools_HSequenceOfShape)& );
	  bool exportSTEP( const QString&, const Handle(TopTools_HSequenceOfShape)& );
  	bool exportSTL( const QString&, const Handle(TopTools_HSequenceOfShape)& );
  	bool exportVRML( const QString&, const Handle(TopTools_HSequenceOfShape)& );

    bool checkFacetedBrep( const Handle(TopTools_HSequenceOfShape)& );

protected:
    TranslateDlg*                     myDlg;
    QString                           myInfo;
};

#endif

