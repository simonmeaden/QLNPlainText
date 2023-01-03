#include "lnplaintext/searchwidget.h"
#include "qcoreevent.h"

#include <QKeyEvent>

SearchWidget::SearchWidget(QWidget* parent)
  : QDialog(parent)
{
  auto layout = new QGridLayout;
  setLayout(layout);
  auto row = 0;

  auto lbl = new QLabel(tr("Find :"), this);
  layout->addWidget(lbl, row, 0, Qt::AlignRight);

  auto findEdit = new QLineEdit(this);
  layout->addWidget(findEdit, row, 1);
  connect(
    findEdit, &QLineEdit::textChanged, this, &SearchWidget::setSearchText);

  auto searchTypeBox = new QComboBox(this);
  layout->addWidget(searchTypeBox, row, 2);
  row++;

  lbl = new QLabel(tr("Replace :"), this);
  layout->addWidget(lbl, row, 0, Qt::AlignRight);

  auto replaceEdit = new QLineEdit(this);
  layout->addWidget(replaceEdit, row, 1);
  connect(
    replaceEdit, &QLineEdit::textChanged, this, &SearchWidget::setReplaceText);

  auto group = new QGroupBox(this);
  auto vLayout = new QVBoxLayout;
  group->setLayout(vLayout);
  auto searchDnBox = new QCheckBox(tr("Search Down"), this);
  searchDnBox->setChecked(true);
  connect(searchDnBox, &QCheckBox::clicked, this, &SearchWidget::setSearchDown);
  vLayout->addWidget(searchDnBox);
  layout->addWidget(group, row, 2);
  row++;

  auto replaceList = new QListWidget(this);
  layout->addWidget(replaceList, row, 0, 1, 2);

  auto btnBox = new QDialogButtonBox(this);
  auto helpBtn = new QPushButton(tr("Help"), this);
  btnBox->addButton(helpBtn, QDialogButtonBox::HelpRole);
  auto nextBtn = new QPushButton(tr("Next"), this);
  btnBox->addButton(nextBtn, QDialogButtonBox::ActionRole);
  auto prevBtn = new QPushButton(tr("Previous"), this);
  btnBox->addButton(prevBtn, QDialogButtonBox::ActionRole);
  auto replaceBtn = new QPushButton(tr("Replace"), this);
  btnBox->addButton(replaceBtn, QDialogButtonBox::ActionRole);
  auto replaceAllBtn = new QPushButton(tr("Replace All"), this);
  btnBox->addButton(replaceAllBtn, QDialogButtonBox::ActionRole);
  auto closeBtn = new QPushButton(tr("Close"), this);
  btnBox->addButton(closeBtn, QDialogButtonBox::RejectRole);
  layout->addWidget(btnBox, 3, 0, 1, 2);

  connect(helpBtn, &QPushButton::clicked, this, &SearchWidget::help);
  connect(nextBtn, &QPushButton::clicked, this, &SearchWidget::next);
  connect(prevBtn, &QPushButton::clicked, this, &SearchWidget::prev);
  connect(replaceBtn, &QPushButton::clicked, this, &SearchWidget::replace);
  connect(
    replaceAllBtn, &QPushButton::clicked, this, &SearchWidget::replaceAll);
  connect(closeBtn, &QPushButton::clicked, this, &SearchWidget::reject);
}

void
SearchWidget::setSearchText(const QString& text)
{
  m_searchText = text;
}

void
SearchWidget::setReplaceText(const QString& text)
{
  m_replaceText = text;
}

void
SearchWidget::setSearchDown(bool direction)
{
  m_searchDown = direction;
}

bool
SearchWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::KeyPress) {
    auto* keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_Escape) {
      deactivate();
      return true;
    } /* else if ((!_debounceTimer.isActive() &&
                 keyEvent->modifiers().testFlag(Qt::ShiftModifier) &&
                 (keyEvent->key() == Qt::Key_Return)) ||
                 (keyEvent->key() == Qt::Key_Up)) {
         emit doSearchUp();
         return true;
     }*/
      /* else if (!_debounceTimer.isActive() &&
               ((keyEvent->key() == Qt::Key_Return) ||
                (keyEvent->key() == Qt::Key_Down))) {
       emit doSearchDown();
       return true;
   }*/  /*else if (!_debounceTimer.isActive() && keyEvent->key() == Qt::Key_F3) {
      doSearch(!keyEvent->modifiers().testFlag(Qt::ShiftModifier));
      return true;
  }*/

    return false;
  }

  return QWidget::eventFilter(obj, event);
}

void
SearchWidget::help()
{
}

void
SearchWidget::prev()
{
}

void
SearchWidget::next()
{
}

void
SearchWidget::replace()
{
}

void
SearchWidget::replaceAll()
{
}
