/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/CQExpressionWidget.h,v $
   $Revision: 1.2 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:27:41 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef CQEXPRESSION
#define CQEXPRESSION

#include <qtextedit.h>
#include <qsyntaxhighlighter.h>

class CQExpressionWidget;

class CQExpressionHighlighter: public QSyntaxHighlighter
  {
  public:
    CQExpressionHighlighter(CQExpressionWidget* ew);
    ~CQExpressionHighlighter() {};

    virtual int highlightParagraph (const QString & text, int endStateOfLastPara);
  };

class CQExpressionWidget: public QTextEdit
  {
    Q_OBJECT
  public:
    CQExpressionWidget(QWidget * parent = 0, const char * name = 0);
    //CQExpressionWidget (const QString & contents, QWidget * parent, const char * name = 0);

  protected:
    int mOldPar;
    int mOldPos;
    //QColor mOldColor;
    //QColor mNewColor;
    //QColor mErrorColor;

    //void process();
    //void setupWidget();

    //virtual void updateColor();
    virtual void keyPressEvent (QKeyEvent * e);

    bool isInObject();

    /**
     * returns true if (par/pos) is right of (parold/posold)
     */
    bool compareCursorPositions(int parold, int posold, int par, int pos);

  protected slots:
    void slotCursorPositionChanged(int para, int pos);
    void slotSelectionChanged();
    //void slotLostFocus();
    //void slotReturnPressed();
    //void slotTextChanged(const QString & text);

  public slots:
    void doKeyboardAction (QTextEdit::KeyboardAction action);
    //void slotForceUpdate();

    //virtual void setText(const QString & text);

  signals:
    //void edited();
  };

#endif
