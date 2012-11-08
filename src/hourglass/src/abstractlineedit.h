#ifndef __ABSTRACTLINEEDIT_H
#define __ABSTRACTLINEEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QRect>

class AbstractLineEdit : public QLineEdit
{
  Q_OBJECT

  public:
    AbstractLineEdit(QWidget *parent = 0);
    AbstractLineEdit(const QString &contents, QWidget *parent = 0);

    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;

  protected slots:
    virtual void insertCompletion(const QString &completion);

  protected:
    virtual void focusInEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void complete();
    void complete(const QString &prefix);

    virtual QRect currentWordBoundaries() const = 0;
    QString currentPrefix() const;

  private:
    QCompleter *m_completer;
};

#endif
