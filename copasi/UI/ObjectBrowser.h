/****************************************************************************
 ** Form interface generated from reading ui file '.\ObjectBrowser.ui'
 **
 ** Created: Tue Apr 22 13:27:37 2003
 **      by:  The User Interface Compiler (uic)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/
#ifndef OBJECTBROWSER_H
#define OBJECTBROWSER_H

#include <qvariant.h>
#include <qwidget.h>
#include <vector>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QListView;
class ObjectBrowserItem;
class objectList;
class QListViewItem;
class CCopasiContainer;
class CCopasiObject;
class ObjectBrowser : public QWidget
  {
    Q_OBJECT

  public:
    objectList* objectItemList;
    objectList* refreshList;
    ObjectBrowser(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~ObjectBrowser();
    std::vector<QString*> strBuffer;

    QPushButton* cancelButton;
    QPushButton* nextButton;
    QPushButton* backButton;
    QListView* ObjectListView;

    void setCheck(ObjectBrowserItem* pCurrent);
    void setUncheck(ObjectBrowserItem* pCurrent);
    void clickToReverseCheck(ObjectBrowserItem* pCurrent);
    void setCheckMark(ObjectBrowserItem* pCurrent);

    void loadData();
    void loadChild(ObjectBrowserItem* parent, CCopasiContainer * copaParent, bool nField);
    void loadField(ObjectBrowserItem* parent, CCopasiContainer * copaParent);
    // void loadVectors(ObjectBrowserItem* parent, CCopasiContainer * copaParent);

    CCopasiObject* getFieldCopasiObject(CCopasiObject* pCurrent, const char* name);
    void updateUI();
    void loadUI();
  public slots:
    virtual void cancelClicked();
    virtual void listviewChecked(QListViewItem*);
    virtual void backClicked();
    virtual void nextClicked();

  protected:
    QGridLayout* ObjectBrowserLayout;
  };
#endif // OBJECTBROWSER_H
