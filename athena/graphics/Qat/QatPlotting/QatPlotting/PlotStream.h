#ifndef _PlotStream_h_
#define _PlotStream_h_
#include <sstream>
#include <iomanip>
#include <QtGui/QTextEdit>
#include <QtGui/QColor>
#include <string>
//  This is a simple class to be used with editable label widgets
class PlotStream {

 public:

  class Clear {
  public:
    Clear() {}
  };

  class EndP {
  public:
    EndP() {}
  };

  class Family {
  public:
    Family(const std::string & name):name(name) {}
    std::string name;
  };

  class Size {
  public:
    Size(unsigned int pointsize):pointsize(pointsize){}
    unsigned int pointsize;
  };

  class Super {
  public:
    Super(){}
  };

  class Sub {
  public:
    Sub(){}
  };

  class Normal {
  public:
    Normal(){}
  };

  class Bold {
  public:
    Bold(){}
  };

  class Regular {
  public:
    Regular(){}
  };

  class Italic {
  public:
    Italic(){}
  };

  class Oblique {
  public:
    Oblique(){}
  };

  class Left {
  public:
    Left(){}
  };

  class Right {
  public:
    Right(){}
  };

  class Center {
  public:
    Center(){}
  };

  class Color {
  public:
    Color(const QColor &color):color(color){}
    QColor color;
  };

  PlotStream (QTextEdit * textEdit):textEdit(textEdit){}

  PlotStream & operator << (const Clear &){ 
    textEdit->clear();
    stream.str("");
    return *this;
  }
  PlotStream & operator << (const EndP &){ 
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    return *this;
  }
  PlotStream & operator << (const Family & family){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setFontFamily(QString(family.name.c_str()));
    return *this;
  }
  PlotStream & operator << (const Size & sz){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setFontPointSize(sz.pointsize);
    return *this;
  }
  PlotStream & operator << (const Super &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    QTextCharFormat format=textEdit->currentCharFormat();
    format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    textEdit->setCurrentCharFormat(format);
    return *this;
  }
  PlotStream & operator << (const Sub &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    QTextCharFormat format=textEdit->currentCharFormat();
    format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    textEdit->setCurrentCharFormat(format);
    return *this;
  }

  PlotStream & operator << (const Normal &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    QTextCharFormat format=textEdit->currentCharFormat();
    format.setVerticalAlignment(QTextCharFormat::AlignNormal);
    textEdit->setCurrentCharFormat(format);
    return *this;
  }

  PlotStream & operator << (const Left &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setAlignment(Qt::AlignLeft);
    return *this;
  }

  PlotStream & operator << (const Right &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setAlignment(Qt::AlignRight);
    return *this;
  }

  PlotStream & operator << (const Center &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setAlignment(Qt::AlignCenter);
    return *this;
  }

  PlotStream & operator << (const Bold &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setFontWeight(QFont::Bold);
    return *this;
  }

  PlotStream & operator << (const Regular &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setFontWeight(QFont::Normal);
    return *this;
  }

  PlotStream & operator << (const Italic &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setFontItalic(true);
    return *this;
  }

  PlotStream & operator << (const Oblique &){
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setFontItalic(false);
    return *this;
  }

  PlotStream & operator << (const Color & color) {
    textEdit->insertPlainText(stream.str().c_str());
    stream.str("");
    textEdit->setTextColor(color.color);
    return *this;
  }

  PlotStream & operator << (const std::string & text) {
    stream << text;
    return *this;
  }

  PlotStream & operator << (const char c) {
    stream << c;
    return *this;
  }

  PlotStream & operator << (double x) {
    stream << x;
    return *this;
  }

  PlotStream & operator << (int i) {
    stream << i;
    return *this;
  }
 
  QTextEdit *textEdit;
  std::ostringstream stream;
};

#endif
