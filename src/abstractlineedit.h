#ifndef __ABSTRACTLINEEDIT_H
#define __ABSTRACTLINEEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QFocusEvent>

class AbstractLineEdit : public QLineEdit
{
  Q_OBJECT

  public:
    AbstractLineEdit(QWidget *parent = 0);
    AbstractLineEdit(const QString &contents, QWidget *parent = 0);

    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;

  protected slots:
    virtual void insertCompletion(const QString &completion) = 0;

  protected:
    void focusInEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);

    int startIndexOfCurrentWord() const;
    virtual QString currentPrefix() const = 0;

  private:
    QCompleter *m_completer;
};

#endif
