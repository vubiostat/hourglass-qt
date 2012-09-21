#include "abstractlineedit.h"
#include <QAbstractItemView>
#include <QtDebug>

AbstractLineEdit::AbstractLineEdit(QWidget *parent)
  : QLineEdit(parent), m_completer(NULL)
{
}

AbstractLineEdit::AbstractLineEdit(const QString &contents, QWidget *parent)
  : QLineEdit(contents, parent), m_completer(NULL)
{
}

void AbstractLineEdit::setCompleter(QCompleter *completer)
{
  if (m_completer) {
    QObject::disconnect(m_completer, 0, this, 0);
  }

  m_completer = completer;

  if (m_completer) {
    completer->setWidget(this);
    QObject::connect(completer, SIGNAL(activated(QString)),
        this, SLOT(insertCompletion(QString)));
  }
}

QCompleter *AbstractLineEdit::completer() const
{
  return m_completer;
}

void AbstractLineEdit::insertCompletion(const QString &completion)
{
  if (completer()->widget() != this)
    return;

  QRect r = currentWordBoundaries();
  setSelection(r.left(), cursorPosition());
  backspace();
  insert(completion);
}


void AbstractLineEdit::focusInEvent(QFocusEvent *e)
{
  if (m_completer) {
    m_completer->setWidget(this);
  }
  QLineEdit::focusInEvent(e);
}

void AbstractLineEdit::keyPressEvent(QKeyEvent *e)
{
  if (m_completer && m_completer->popup()->isVisible()) {
    // The following keys are forwarded by the completer to the widget
    switch (e->key()) {
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Escape:
      case Qt::Key_Tab:
      case Qt::Key_Backtab:
        e->ignore();
        return; // let the completer do default behavior
      default:
        break;
    }
  }

  QLineEdit::keyPressEvent(e);

  if (m_completer == NULL) {
    return;
  }

  bool isShortcut = e->key() == Qt::Key_Down;
  QString completionPrefix = currentPrefix();
  if (!isShortcut && (completionPrefix.isEmpty() || e->text().isEmpty() ||
        (e->modifiers() != Qt::NoModifier && e->modifiers() != Qt::ShiftModifier))) {
    m_completer->popup()->hide();
    return;
  }

  if (completionPrefix != m_completer->completionPrefix()) {
    m_completer->setCompletionPrefix(completionPrefix);
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
  }

  m_completer->complete(); // popup it up!
}

QString AbstractLineEdit::currentPrefix() const
{
  QRect r = currentWordBoundaries();
  return text().mid(r.left(), cursorPosition());
}
