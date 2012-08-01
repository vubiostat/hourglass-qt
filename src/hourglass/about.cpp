#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include "about.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle("About Hourglass");

  QGridLayout *layout = new QGridLayout(this);

  QLabel *icon = new QLabel(this);
  icon->setPixmap(QPixmap(":/icons/hourglass.png"));
  icon->setScaledContents(true);
  layout->addWidget(icon, 0, 0, 4, 1);

  QString versionString = QString("Version %1.%2.%3%4").
    arg(HOURGLASS_MAJOR_VERSION).
    arg(HOURGLASS_MINOR_VERSION).
    arg(HOURGLASS_PATCH_VERSION).
#ifdef HOURGLASS_DEV
    arg("dev");
#else
    arg(QString());
#endif
  QLabel *version = new QLabel(versionString, this);
  layout->addWidget(version, 0, 1, Qt::AlignHCenter);

  QString timestampString = QString("Built %1 %2").arg(__DATE__).arg(__TIME__);
  QLabel *timestamp = new QLabel(timestampString, this);
  layout->addWidget(timestamp, 1, 1, Qt::AlignHCenter);

  QChar copyrightSymbol(0x00a9);
  QString copyrightString = QString("Copyright %1 2012 Vanderbilt University").
    arg(copyrightSymbol);
  QLabel *copyright = new QLabel(copyrightString, this);
  layout->addWidget(copyright, 2, 1, Qt::AlignHCenter);

  QLabel *source = new QLabel("<a href=\"https://github.com/vubiostat/hourglass-qt\">Source code</a>", this);
  source->setOpenExternalLinks(true);
  layout->addWidget(source, 3, 1, Qt::AlignHCenter);

  QPushButton *okay = new QPushButton(tr("OK"), this);
  okay->setDefault(true);
  connect(okay, SIGNAL(pressed()), this, SLOT(accept()));
  layout->addWidget(okay, 4, 1, Qt::AlignRight);
}
