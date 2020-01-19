#ifndef _LabelEditorDialog_h_
#define _LabelEditorDialog_h_
#include "QatPlotWidgets/characterwidget.h"
#include <QScrollArea>
#include <QDialog>
#include <QTextEdit>

class LabelEditorDialog: public QDialog
 {
     Q_OBJECT

 public:

     // Constructor:
     LabelEditorDialog(QWidget *parent = 0);

     // Destructor:
     ~LabelEditorDialog();

     // Get the text editor:
     const QTextEdit *textEdit() const;
     QTextEdit *textEdit(); 

 private slots:

     void textFamily( const QString &f );
     void textSize(int i);
     void textBold();
     void textItalic();
     void textScript();
     void textColor();
     void toggleChars();
     void updateCharFormat(const QTextCharFormat &f);


 private:

     class Clockwork;
     Clockwork *c;


 };
#endif

