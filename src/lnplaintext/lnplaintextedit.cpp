#include "lnplaintext/lnplaintextedit.h"
#include "labelled/labelledlineedit.h"
#include "labelled/labelledspinbox.h"
#include "utilities/ContainerUtil.h"
#include "utilities/MathUtilities.h"
#include "utilities/characters.h"
#include "utilities/paths.h"
#include "utilities/x11colors.h"
#include "lnplaintext/hoverwidget.h"
#include "lnplaintext/searchwidget.h"

//#include <CuteLogger/Logger.h>

//====================================================================
//=== GoToBookmarkDialog
//====================================================================
GoToBookmarkDialog::GoToBookmarkDialog(QMap<int, BookmarkData*>* bookmarks,
                                       QWidget* parent)
  : QDialog(parent)
  , m_bookmarkIndex(-1)
  , m_text(QString())
{
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);

  m_group = new QGroupBox(tr("Bookmarks"), this);
  //  m_group->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_group);

  QVBoxLayout* grpLayout = new QVBoxLayout;
  grpLayout->setContentsMargins(0, 0, 0, 0);
  m_group->setLayout(grpLayout);

  //  auto bookmarkModel = new BookmarkModel(bookmarks);
  //  m_bookmarkView = new QTableView(this);
  //  m_bookmarkView->setModel(bookmarkModel);
  //  m_bookmarkView->verticalHeader()->hide();
  //  m_bookmarkView->horizontalHeader()->setVisible(true);
  //  m_bookmarkView->horizontalHeader()->setStretchLastSection(true);
  //  grpLayout->addWidget(m_bookmarkView);
  //  connect(m_bookmarkView,
  //          &QTableView::clicked,
  //          this,
  //          &GoToBookmarkDialog::handleClicked);

  m_buttonBox =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(m_buttonBox);

  connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int
GoToBookmarkDialog::bookmark()
{
  return m_bookmarkIndex;
}

QString
GoToBookmarkDialog::text()
{
  return m_text;
}

void
GoToBookmarkDialog::handleClicked(const QModelIndex& index)
{
  QModelIndex i0 = m_bookmarkView->model()->index(index.row(), 0);
  QModelIndex i1 = m_bookmarkView->model()->index(index.row(), 1);

  if (i0.isValid()) {
    m_bookmarkIndex = m_bookmarkView->model()->data(i0).toInt();
  }

  if (i1.isValid()) {
    m_text = m_bookmarkView->model()->data(i1).toString();
  }
}

//====================================================================
//=== BookmarkEditDialog
//====================================================================
BookmarkEditDialog::BookmarkEditDialog(QWidget* parent)
  : QDialog(parent)
{
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);

  m_textEdit = new LabelledLineEdit(tr("Note Text:"), this);
  layout->addWidget(m_textEdit);
  m_linenumberEdit = new LabelledSpinBox(tr("Line Number"), this);
  layout->addWidget(m_linenumberEdit);

  m_buttonBox =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(m_buttonBox);

  connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void
BookmarkEditDialog::setText(const QString& text)
{
  m_textEdit->setText(text);
}

QString
BookmarkEditDialog::text()
{
  return m_textEdit->text();
}

void
BookmarkEditDialog::setLineNumber(int linenumber)
{
  m_linenumberEdit->setValue(linenumber);
}

int
BookmarkEditDialog::lineNumber()
{
  return m_linenumberEdit->value();
}

//====================================================================
//=== LNPlainTextEdit
//====================================================================
int LNPlainTextEdit::m_customPixmap = -1;
QMap<int, QPixmap> LNPlainTextEdit::m_pixmaps = QMap<int, QPixmap>();

LNPlainTextEdit::LNPlainTextEdit(QWidget* parent)
  : QPlainTextEdit(parent)
  , m_lnAreaBackColor(QColorConstants::X11::grey90)
  , m_lnAreaTextColor(QColorConstants::X11::grey40)
  , m_lnAreaSelectedBackColor(QColorConstants::X11::grey80)
  , m_lnAreaSelectedTextColor(QColorConstants::X11::greenyellow)
  , m_keyMap(new KeyEventMap(this))

{
  initSettings(new LNPlainTextEditSettings(this));
  initialise();
}

LNPlainTextEdit::LNPlainTextEdit(BaseConfig* config, QWidget* parent)
  : QPlainTextEdit(parent)
  , m_config(config)
  , m_lnAreaBackColor(QColorConstants::X11::grey90)
  , m_lnAreaTextColor(QColorConstants::X11::grey40)
  , m_lnAreaSelectedBackColor(QColorConstants::X11::grey80)
  , m_lnAreaSelectedTextColor(QColorConstants::X11::greenyellow)
{
  initSettings(new LNPlainTextEditSettings(config, this));
  initialise();
}

// LNPlainTextEdit::LNPlainTextEdit(LNPlainTextEditSettings* settings,
//                                  QWidget* parent)
//   : QPlainTextEdit(parent)
//   , m_lnAreaBackColor(QColorConstants::X11::grey90)
//   , m_lnAreaTextColor(QColorConstants::X11::grey40)
//   , m_lnAreaSelectedBackColor(QColorConstants::X11::grey80)
//   , m_lnAreaSelectedTextColor(QColorConstants::X11::greenyellow)
//{
//   initialise();
// }

// LNPlainTextEdit::LNPlainTextEdit(LNPlainTextEditSettings* settings,
//                                  BaseConfig* config,
//                                  QWidget* parent)
//   : QPlainTextEdit(parent)
//   , m_config(config)
//   , m_settings(settings)
//   , m_lnAreaBackColor(QColorConstants::X11::grey90)
//   , m_lnAreaTextColor(QColorConstants::X11::grey40)
//   , m_lnAreaSelectedBackColor(QColorConstants::X11::grey80)
//   , m_lnAreaSelectedTextColor(QColorConstants::X11::greenyellow)
//{
//   initialise();
// }

// LNPlainTextEdit::LNPlainTextEdit(QSyntaxHighlighter *highlighter, BaseConfig
// *config, QWidget *parent)
//{
//   , m_settings(new LNPlainTextEditSettings(highlighter,config, this))
//   , m_lnAreaBackColor(QColorConstants::X11::grey90)
//   , m_lnAreaTextColor(QColorConstants::X11::grey40)
//   , m_lnAreaSelectedBackColor(QColorConstants::X11::grey80)
//   , m_lnAreaSelectedTextColor(QColorConstants::X11::greenyellow)
// }

void
LNPlainTextEdit::initSettings(LNPlainTextEditSettings* settings)
{
  m_settings = settings;
  if (m_config) {
    m_settings->setId(m_config->nextItemId());
    m_config->addSetting(m_settings);
  }

  QFontMetrics metrics(font());
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
  setTabStopWidth(m_settings->tabWidth() * metrics.width(' '));
#else
  setTabStopWidth(m_settings->tabStopDistance() *
                  metrics.horizontalAdvance(QLatin1Char(' ')));
#endif
}

void
LNPlainTextEdit::initialise()
{
  setMouseTracking(true);
  setFontToSourceCodePro();
  loadStandardPixmaps();

  m_keyMap = new KeyEventMap(this);
  m_keyMap->addAction(PreviousBookmark,
                      tr("Previous Bookmark"),
                      Qt::Key_Comma,
                      Qt::ControlModifier);
  m_keyMap->addAction(
    NextBookmark, tr("Next Bookmark"), Qt::Key_Stop, Qt::ControlModifier);
  m_keyMap->addAction(
    AddBookmark, tr("Add Bookmark"), Qt::Key_M, Qt::ControlModifier);
  m_keyMap->addAction(ToggleBookmark,
                      tr("Toggle Bookmark"),
                      Qt::Key_M,
                      Qt::ControlModifier | Qt::AltModifier);
  m_keyMap->addAction(EditBookmark,
                      tr("Edit Bookmark"),
                      Qt::Key_M,
                      Qt::ControlModifier | Qt::ShiftModifier);
  m_keyMap->addAction(ClearBookmarks,
                      tr("Clear Bookmarks"),
                      Qt::Key_C,
                      Qt::ControlModifier | Qt::AltModifier);

  m_keyMap->addAction(Search, tr("Find"), Qt::Key_F, Qt::ControlModifier);
  m_keyMap->addAction(
    SearchNext, tr("Find Next"), Qt::Key_G, Qt::ControlModifier);
  m_keyMap->addAction(SearchPrevious,
                      tr("Find Previous"),
                      Qt::Key_G,
                      Qt::ControlModifier | Qt::ShiftModifier);

  m_keyMap->addAction(
    Preferences, tr("Preferences"), Qt::Key_Comma, Qt::ControlModifier);

  connect(this,
          &LNPlainTextEdit::blockCountChanged,
          this,
          &LNPlainTextEdit::updateViewableAreaWidth);
  connect(this,
          &LNPlainTextEdit::updateRequest,
          this,
          &LNPlainTextEdit::updateBookmarkArea);
  connect(this,
          &LNPlainTextEdit::updateRequest,
          this,
          &LNPlainTextEdit::updateLineNumberArea);
  connect(this,
          &LNPlainTextEdit::updateRequest,
          this,
          &LNPlainTextEdit::updateFoldArea);

}

void
LNPlainTextEdit::doSearch(QString& searchText)
{
}

void
LNPlainTextEdit::setFontToSourceCodePro()
{
  auto sysFamilies = QFontDatabase::families();
  if (!sysFamilies.contains("Source Code Pro")) {
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(
        QFontDatabase::addApplicationFont("://fonts/SourceCodePro-Black.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-BlackItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(
        QFontDatabase::addApplicationFont("://fonts/SourceCodePro-Bold.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-BoldItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-ExtraBold.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-ExtraBoldItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-ExtraLight.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-ExtraLightItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-Italic-VariableFont_wght.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(
        QFontDatabase::addApplicationFont("://fonts/SourceCodePro-Light.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-LightItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(
        QFontDatabase::addApplicationFont("://fonts/SourceCodePro-Medium.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-MediumItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-Regular.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-SemiBold.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-SemiBoldItalic.ttf")),
      m_fontFamilies);
    appendIfNotInList(
      QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(
        "://fonts/SourceCodePro-VariableFont_wght.ttf")),
      m_fontFamilies);
  }
  for (auto& family : sysFamilies) {
    auto fixed = QFontDatabase::isFixedPitch(family);
    if (fixed) {
      appendIfNotInList(QStringList() << family, m_fontFamilies);
    }
  }

  QFont font = QFont("Source Code Pro Medium", 9, 400, false);
  setFont(font);
}

int
LNPlainTextEdit::currentLineNumber()
{
  return m_lineNumber;
}

int
LNPlainTextEdit::lineCount()
{
  return m_lineCount;
}

void
LNPlainTextEdit::loadStandardPixmaps()
{
  m_pixmaps.insert(Bookmark, QPixmap(":/icon/BookmarkFlag"));
  m_pixmaps.insert(Cross, QPixmap(":/icon/Cross"));
  m_pixmaps.insert(Bar, QPixmap(":/icon/Dash"));
  m_pixmaps.insert(FoldedCross, QPixmap(":/icon/FoldedCross"));
  m_pixmaps.insert(UnfoldedCross, QPixmap(":/icon/UnfoldedCross"));
  m_pixmaps.insert(FoldedArrow, QPixmap(":/icon/FoldedArrow"));
  m_pixmaps.insert(UnfoldedArrow, QPixmap(":/icon/UnfoldedArrow"));
  m_pixmaps.insert(Warning, QPixmap(":/icon/Warning"));
  m_pixmaps.insert(BlackCircle, QPixmap(":/icon/BlackDot"));
  m_pixmaps.insert(WhiteCircle, QPixmap(":/icon/WhiteDot"));
  m_pixmaps.insert(BlueCircle, QPixmap(":/icon/BlueDot"));
  m_pixmaps.insert(DarkBlueCircle, QPixmap(":/icon/DarkBlueDot"));
  m_pixmaps.insert(RedCircle, QPixmap(":/icon/RedDot"));
  m_pixmaps.insert(DarkRedCircle, QPixmap(":/icon/DarkRedDot"));
  m_pixmaps.insert(GreenCircle, QPixmap(":/icon/GreenDot"));
  m_pixmaps.insert(DarkGreenCircle, QPixmap(":/icon/DarkGreenDot"));
  m_pixmaps.insert(YellowCircle, QPixmap(":/icon/YellowDot"));
  m_pixmaps.insert(OrangeCircle, QPixmap(":/icon/OrangeDot"));
  m_pixmaps.insert(PinkCircle, QPixmap(":/icon/PinkDot"));
  m_pixmaps.insert(CyanCircle, QPixmap(":/icon/CyanDot"));
}

const QStringList&
LNPlainTextEdit::getFontFamilies() const
{
  return m_fontFamilies;
}

// QList<SettingsWidget*>
// LNPlainTextEdit::settingsPages()
//{
////  auto widget = new LNPlainTextEditSettingsWidget(m_settings, this);
////  return widget;
//}

// void
// LNPlainTextEdit::setSettingsPages(QList<SettingsWidget*> widgets)
//{
////  auto settingsWidgets = LNPlainTextEditSettingsWidget();
////  if (settingsWidget && m_settings->isModified()) {
////    enableBookmarks(m_settings->isEnabledBookmarks());
////    enableLineNumbers(settingsWidget->isEnabledLineNumbers());
////    enableFolds(m_settings->isEnabledFolds());
////    setShowNewline(m_settings->isDisplayLineEnds());
////    auto f = QFont(m_settings->fontFamily(), settingsWidget->fontSize());
////    setFont(f);
////  }
//}

qreal
LNPlainTextEdit::tabStopDistance() const
{
  return m_settings->tabStopDistance();
}

void
LNPlainTextEdit::setTabStopWidth(int m_tabStop)
{
  setTabStopWidth(qreal(m_tabStop));
}

void
LNPlainTextEdit::setTabStopWidth(qreal m_tabStop)
{
  m_settings->setTabStop(m_tabStop);
  QFontMetrics metrics(font());
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
  QPlainTextEdit::setTabStopWidth(m_tabStop * metrics.width(' '));
#else
  QPlainTextEdit::setTabStopDistance(
    m_tabStop * metrics.horizontalAdvance(QLatin1Char(' ')));
#endif
}

// void
// LNPlainTextEdit::savePreferences()
//{
//   m_settings->createNode();
// }

// void
// LNPlainTextEdit::loadPreferences()
//{
//   if (m_settings) {
//     m_settings->load();
//   }
// }

void
LNPlainTextEdit::resetTabStop()
{
  m_settings->setTabStop(4);
}

const QPixmap
LNPlainTextEdit::pixmap(int id)
{
  return m_pixmaps.value(id);
}

QPixmap
LNPlainTextEdit::unfoldedPixmap(Style style, int pixmap)
{
  switch (style) {
    case ArrowStyle:
      return m_pixmaps.value(UnfoldedArrow);
    case CrossStyle:
      return m_pixmaps.value(UnfoldedCross);
    case CustomStyle:
      if (pixmap >= 0 && pixmap >= CustomPixmap && pixmap < m_pixmaps.size())
        return m_pixmaps.value(pixmap);
      else
        break;
  }
  return QPixmap();
}

QPixmap
LNPlainTextEdit::foldedPixmap(Style style, int pixmap)
{
  switch (style) {
    case ArrowStyle:
      return m_pixmaps.value(FoldedArrow);
    case CrossStyle:
      return m_pixmaps.value(FoldedCross);
    case CustomStyle:
      if (pixmap >= 0 && pixmap >= CustomPixmap && pixmap < m_pixmaps.size())
        return m_pixmaps.value(pixmap);
      else
        break;
  }
  return QPixmap();
}

bool
LNPlainTextEdit::isModified() const
{
  return m_settings->isModified();
}

const QList<int>
LNPlainTextEdit::folds()
{
  return m_foldArea->folds();
}

void
LNPlainTextEdit::setFolds(QList<int> folds)
{
  m_foldArea->setFolds(folds);
}

bool
LNPlainTextEdit::setFolds(QList<int> folds, QList<bool> folded)
{
  return m_foldArea->setFolds(folds, folded);
}

void
LNPlainTextEdit::insertFold(int lineNumber)
{
  m_foldArea->insertFold(lineNumber);
}

void
LNPlainTextEdit::insertFold(int lineNumber, bool folded)
{
  m_foldArea->insertFold(lineNumber, folded);
}

void
LNPlainTextEdit::clearFolds()
{
  m_foldArea->clearFolds();
}

void
LNPlainTextEdit::insertHoverItem(int lineNumber,
                                 int icon,
                                 const QString& title,
                                 const QString& text)
{
  m_bookmarkArea->insertHover(lineNumber, icon, title, text);
}

bool
LNPlainTextEdit::hasHoverItem(int lineNumber)
{
  return m_bookmarkArea->hasHover(lineNumber);
}

void
LNPlainTextEdit::clearHoverItems()
{
  m_bookmarkArea->clearHovers();
}

bool
LNPlainTextEdit::hasFold(int lineNumber)
{
  return m_foldArea->hasFold(lineNumber);
}

bool
LNPlainTextEdit::isFolded(int lineNumber)
{
  return m_foldArea->isFolded(lineNumber);
}

void
LNPlainTextEdit::setFold(int lineNumber, bool folded)
{
  m_foldArea->set(lineNumber, folded);
}

int
LNPlainTextEdit::addCustomPixmap(const QPixmap pixmap)
{
  if (m_customPixmap == -1) {
    m_customPixmap = int(CustomPixmap);
  }

  auto id = m_customPixmap++;
  m_pixmaps.insert(id, pixmap);
  return id;
}

void
LNPlainTextEdit::paintBookmarkArea(QPaintEvent* /*event*/)
{
  if (m_bookmarkArea) {
    auto rect = m_bookmarkArea->rect();
    QPainter painter(m_bookmarkArea);
    painter.fillRect(rect, m_lnAreaBackColor);

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    auto top =
      qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    auto bottom = top + qRound(blockBoundingRect(block).height());
    auto height = fontMetrics().height();
    auto blockHeight = blockBoundingRect(block).height();

    while (block.isValid() && top <= rect.bottom()) {
      if (block.isVisible() && bottom >= rect.top()) {
        int lineNumber = blockNumber + 1;

        if (lineNumber == m_lineNumber)
          painter.fillRect(rect.x(),
                           top,
                           rect.width(),
                           blockHeight,
                           m_lnAreaSelectedBackColor);

        if (m_bookmarkArea->hasHover(lineNumber)) {
          int pxId = m_bookmarkArea->hoverIcon(lineNumber);
          auto px = pixmap(pxId);
          painter.drawPixmap(
            Math::halfDifference(m_bookmarkArea->width(), px.width()),
            top + Math::halfDifference(height, px.height()),
            px);
        }

        if (hasBookmark(lineNumber)) {
          auto data = bookmark(lineNumber);
          auto pxId = data.pixmap;
          auto px = pixmap(pxId);
          painter.drawPixmap(
            Math::halfDifference(m_bookmarkArea->width(), px.width()),
            top + Math::halfDifference(height, px.height()),
            px);
          data.rect = QRect(0, top, width(), height);
        }
      }

      block = block.next();
      top = bottom;
      bottom = top + qRound(blockHeight);
      ++blockNumber;
    }
  }
}

void
LNPlainTextEdit::paintFoldArea(QPaintEvent* event)
{
  if (m_foldArea) {
    auto rect = event->rect();
    QPainter painter(m_foldArea);
    painter.fillRect(rect, m_lnAreaBackColor);

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    auto top =
      qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    auto bottom = top + qRound(blockBoundingRect(block).height());
    auto blockHeight = qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= rect.bottom()) {
      if (block.isVisible() && bottom >= rect.top()) {
        auto lineNumber = blockNumber + 1;

        if (lineNumber == m_lineNumber)
          painter.fillRect(rect.x(),
                           top,
                           rect.width(),
                           blockHeight,
                           m_lnAreaSelectedBackColor);

        if (m_foldArea->hasFold(lineNumber)) {
          QPixmap px;
          if (m_foldArea->isFolded(lineNumber))
            px = foldedPixmap(m_foldStyle);
          else
            px = unfoldedPixmap(m_foldStyle);
          painter.drawPixmap(
            Math::halfDifference(m_foldArea->width(), px.width()),
            top + Math::halfDifference(blockHeight, px.height()),
            px);
          m_foldArea->set(lineNumber,
                          QRect(0, top, m_foldArea->width(), blockHeight));
        }
      }

      block = block.next();
      top = bottom;
      bottom = top + blockHeight;
      ++blockNumber;
    }
  }
}

void
LNPlainTextEdit::paintLNArea(QPaintEvent* event)
{
  if (m_lineNumberArea) {
    auto rect = event->rect();
    auto fm = fontMetrics();

    QPainter painter(m_lineNumberArea);
    painter.fillRect(rect, m_lnAreaBackColor);

    QTextBlock block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    auto top =
      qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    auto blockHeight = qRound(blockBoundingRect(block).height());
    auto bottom = top + blockHeight;

    while (block.isValid() && top <= event->rect().bottom()) {
      if (block.isVisible() && bottom >= event->rect().top()) {
        auto lineNumber = blockNumber + 1;
        QString number = QString::number(lineNumber);

        if (lineNumber == m_lineNumber)
          painter.fillRect(rect.x(),
                           top,
                           rect.width(),
                           blockHeight,
                           m_lnAreaSelectedBackColor);

        painter.setPen((textCursor().blockNumber() == blockNumber)
                         ? m_lnAreaSelectedTextColor
                         : m_lnAreaTextColor);
        auto width = m_lineNumberArea->width();
        painter.drawText((width - fm.horizontalAdvance(number)) - 3,
                         bottom - fm.descent(),
                         number);
      }

      block = block.next();
      top = bottom;
      bottom = top + qRound(blockBoundingRect(block).height());
      ++blockNumber;
    }
  }
}

int
LNPlainTextEdit::lineNumberAreaWidth()
{
  auto digits = 1;
  auto max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  auto space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

  return space;
}

void
LNPlainTextEdit::handleBookmarkContextMenu(QContextMenuEvent* event)
{
  QMenu menu;
  auto tc = cursorForPosition(event->pos());
  auto data = calculateLineNumber(tc);
  bool hasBmk = hasBookmark(data.first);
  m_lineNumber = data.first;

  auto action = m_keyMap->action(KeyEvent::AddBookmark);
  if (action) {
    action->setEnabled(!hasBmk);
    connect(
      action, &QAction::triggered, this, &LNPlainTextEdit::handleAddBookmark);
    menu.addAction(action);
  }

  action = m_keyMap->action(KeyEvent::EditBookmark);
  if (action) {
    action->setEnabled(hasBmk);
    connect(
      action, &QAction::triggered, this, &LNPlainTextEdit::handleEditBookmark);
    menu.addAction(action);
  }

  action = m_keyMap->action(KeyEvent::RemoveBookmark);
  if (action) {
    action->setEnabled(hasBmk);
    connect(action,
            &QAction::triggered,
            this,
            &LNPlainTextEdit::handleRemoveBookmark);
    menu.addAction(action);
  }

  menu.addSeparator();

  auto submenu = new QMenu(tr("Goto Bookmark"), this);
  for (auto [key, value] : asKeyValueRange(m_bookmarks)) {
    action = submenu->addAction(tr("Line %1").arg(key));
    action->setData(key);
    connect(action,
            &QAction::triggered,
            this,
            qOverload<bool>(&LNPlainTextEdit::goToBookmark));
  }
  menu.addMenu(submenu);

  menu.addSeparator();

  action = m_keyMap->action(KeyEvent::ClearBookmarks);
  if (action) {
    action->setEnabled(!m_bookmarks.isEmpty());
    connect(
      action, &QAction::triggered, this, &LNPlainTextEdit::clearBookmarks);
    menu.addAction(action);
  }

  menu.exec(event->globalPos());
}

void
LNPlainTextEdit::resizeEvent(QResizeEvent* event)
{
  QPlainTextEdit::resizeEvent(event);

  auto cr = contentsRect();
  auto left = cr.left();
  if (m_bookmarkArea) {
    m_bookmarkArea->setGeometry(
      QRect(left, cr.top(), m_bookmarkArea->width(), cr.height()));
    left += m_bookmarkArea->width();
  }
  if (m_lineNumberArea) {
    m_lineNumberArea->setGeometry(
      QRect(left, cr.top(), lineNumberAreaWidth(), cr.height()));
    left += m_lineNumberArea->width();
  }
  if (m_foldArea) {
    m_foldArea->setGeometry(
      QRect(left, cr.top(), m_foldArea->width(), cr.height()));
  }
}

bool
LNPlainTextEdit::isHighlightLine() const
{
  return m_highlightLine;
}

void
LNPlainTextEdit::setHighlightLine(bool highlightLine)
{
  m_highlightLine = highlightLine;
}

const QColor&
LNPlainTextEdit::lnAreaTextColor() const
{
  return m_lnAreaTextColor;
}

void
LNPlainTextEdit::setLNAreaTextColor(const QColor& color)
{
  m_lnAreaTextColor = color;
}

const QColor&
LNPlainTextEdit::lnAreaBackColor() const
{
  return m_lnAreaBackColor;
}

const QColor&
LNPlainTextEdit::lnAreaSelectedTextColor() const
{
  return m_lnAreaSelectedTextColor;
}

void
LNPlainTextEdit::setLNAreaSelectedTextColor(const QColor& color)
{
  m_lnAreaSelectedTextColor = color;
}

const QColor&
LNPlainTextEdit::lnAreaSelectedBackColor() const
{
  return m_lnAreaSelectedBackColor;
}

void
LNPlainTextEdit::setLNAreaSelectedBackColor(const QColor& color)
{
  m_lnAreaSelectedBackColor = color;
}

void
LNPlainTextEdit::setLineNumberBackColor(const QColor& color)
{
  m_lnAreaBackColor = color;
}

void
LNPlainTextEdit::updateViewableAreaWidth(int /*blockCount*/)
{
  auto width = 0;
  if (m_bookmarkArea) {
    width += m_bookmarkArea->width();
  }
  if (m_lineNumberArea) {
    width += lineNumberAreaWidth();
  }
  if (m_foldArea) {
    width += m_foldArea->width();
  }
  setViewportMargins(width, 0, 0, 0);
}

void
LNPlainTextEdit::updateBookmarkArea(const QRect& rect, int dy)
{
  if (m_bookmarkArea) {
    if (dy) {
      m_bookmarkArea->scroll(0, dy);
    } else {
      m_bookmarkArea->update(
        0, rect.y(), m_bookmarkArea->width(), rect.height());
    }
  }

  if (rect.contains(viewport()->rect())) {
    updateViewableAreaWidth(0);
  }
}

void
LNPlainTextEdit::updateFoldArea(const QRect& rect, int dy)
{
  if (m_foldArea) {
    if (dy) {
      m_foldArea->scroll(0, dy);
    } else {
      m_foldArea->update(0, rect.y(), m_foldArea->width(), rect.height());
    }
  }

  if (rect.contains(viewport()->rect())) {
    updateViewableAreaWidth(0);
  }
}

void
LNPlainTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
  if (m_lineNumberArea) {
    if (dy) {
      m_lineNumberArea->scroll(0, dy);
    } else {
      m_lineNumberArea->update(
        rect.x(), rect.y(), m_lineNumberArea->width(), rect.height());
    }
  }

  if (rect.contains(viewport()->rect())) {
    updateViewableAreaWidth(0);
  }
}

/*
   Calculates the current text column.
*/
int LNPlainTextEdit::calculateColumn(/*QTextCursor textCursor*/)
{
  QTextCursor currentCursor = textCursor();
  QTextCursor cursor(currentCursor);
  cursor.movePosition(QTextCursor::StartOfLine);
  return currentCursor.anchor() - cursor.anchor();
}

QPair<int, int>
LNPlainTextEdit::calculateLineNumber(QTextCursor textCursor)
{
  QTextCursor cursor(textCursor);
  cursor.movePosition(QTextCursor::StartOfLine);

  int lines = 1;

  while (cursor.positionInBlock() > 0) {
    cursor.movePosition(QTextCursor::Up);
    lines++;
  }

  QTextBlock block = cursor.block().previous();

  while (block.isValid()) {
    lines += block.lineCount();
    block = block.previous();
  }

  int count = lines;

  block = cursor.block().next();

  while (block.isValid()) {
    count += block.lineCount();
    block = block.next();
  }

  return qMakePair(lines, count);
}

void
LNPlainTextEdit::goToLine(int lineNumber)
{
  m_lineNumber = lineNumber;
  gotoLineNumber();
}

void
LNPlainTextEdit::goToBookmark(bool /*triggered*/)
{
  auto a = qobject_cast<QAction*>(sender());
  if (a) {
    bool ok;
    auto lineNumber = a->data().toInt(&ok);
    if (ok) {
      goToLine(lineNumber);
    }
  }
}

void
LNPlainTextEdit::goToBookmark(int bookmark)
{
  if (hasBookmarks()) {
    auto data = this->bookmark(bookmark);
    if (data.isValid()) {
      goToLine(bookmark);
    }
  }
}

bool
LNPlainTextEdit::goToPreviousBookmark()
{
  auto lineNumberData = calculateLineNumber(textCursor());
  m_lineNumber = lineNumberData.first;
  m_lineCount = lineNumberData.second;
  int i;

  if (hasBookmarks()) {
    for (i = m_lineNumber - 1; i >= 0; i--) {
      auto data = bookmark(i);
      if (data.isValid()) {
        goToLine(i);
        return true;
      }
    }

    for (i = m_lineCount - 1; i >= m_lineNumber + 1; i--) {
      auto data = bookmark(i);
      if (data.isValid()) {
        goToLine(i);
        return true;
      }
    }
  }
  return false;
}

bool
LNPlainTextEdit::goToNextBookmark()
{
  auto lineNumberData = calculateLineNumber(textCursor());
  m_lineNumber = lineNumberData.first;
  m_lineCount = lineNumberData.second;
  int i;

  if (hasBookmarks()) {
    for (i = m_lineNumber + 1; i < m_lineCount; i++) {
      auto data = bookmark(i);
      if (data.isValid()) {
        goToLine(i);
        return true;
      }
    }

    for (i = 0; i < m_lineNumber; i++) {
      auto data = bookmark(i);
      if (data.isValid()) {
        goToLine(i);
        return true;
      }
    }
  }
  return false;
}

void
LNPlainTextEdit::handleAddBookmark(bool)
{
  if (!m_bookmarks.contains(m_lineNumber)) {
    insertBookmark(m_lineNumber);
    update();
  }
}

void
LNPlainTextEdit::handleRemoveBookmark(bool)
{
  if (m_bookmarks.contains(m_lineNumber)) {
    removeBookmark(m_lineNumber);
    update();
  }
}

void
LNPlainTextEdit::handleEditBookmark(bool)
{
  if (m_bookmarks.contains(m_lineNumber)) {
    editBookmark(m_lineNumber);
    update();
  }
}

void
LNPlainTextEdit::handleClearBookmarks(bool)
{
  clearBookmarks();
  update();
}

LNPlainTextEdit::BookmarkData
LNPlainTextEdit::bookmark(int number)
{
  if (hasBookmark(number))
    return m_bookmarks.value(number);
  return BookmarkData();
}

void
LNPlainTextEdit::bookmarkHoverFinished()
{
  if (m_hoverWidget) {
    m_hoverWidget->deleteLater();
    m_hoverWidget = nullptr;
  }
}

void
LNPlainTextEdit::setBookmarks(QMap<int, BookmarkData>* bookmarks)
{
  if (!m_bookmarks.isEmpty()) {
    m_bookmarks.clear();
  }

  auto keys = bookmarks->keys();
  for (auto& key : keys) {
    if (key <= blockCount()) {
      m_bookmarks.insert(key, bookmarks->value(key));
    }
  }
  update();
}

void
LNPlainTextEdit::insertBookmark(int bookmark, const QString& text)
{
  if (m_bookmarks.contains(bookmark) && hasBookmarkText(bookmark)) {
    m_oldBookmarks.insert(bookmark, bookmarkText(bookmark));

  } else {
    if (bookmark > 0 && bookmark <= blockCount()) {
      BookmarkData data(text);
      data.text = text;
      data.pixmap = int(Bookmark);
      m_bookmarks.insert(bookmark, data);
    }
  }

  update();
  saveBookmarks();
}

void
LNPlainTextEdit::toggleBookmark(int bookmark)
{
  if (m_bookmarks.contains(bookmark)) {
    removeBookmark(bookmark);

  } else {
    if (m_oldBookmarks.contains(bookmark)) {
      insertBookmark(bookmark, m_oldBookmarks.value(bookmark));

    } else {
      insertBookmark(bookmark);
    }
  }
  saveBookmarks();
}

void
LNPlainTextEdit::removeBookmark(int bookmark)
{
  if (m_bookmarks.contains(bookmark)) {
    m_oldBookmarks.insert(bookmark, m_bookmarks.value(bookmark).text);
    m_bookmarks.remove(bookmark);
    saveBookmarks();
    update();
  }
}

void
LNPlainTextEdit::editBookmark(int lineNumber)
{
  auto lineCount = blockCount();
  if (lineNumber > 0 && lineNumber < lineCount) {
    QString text = bookmarkText(lineNumber);

    BookmarkEditDialog dlg(this);
    dlg.setText(text);
    dlg.setLineNumber(lineNumber);
    auto result = dlg.exec();

    if (result == QDialog::Accepted) {
      auto ln = dlg.lineNumber();
      removeBookmark(lineNumber);
      insertBookmark(ln, dlg.text());
    }
    saveBookmarks();
  }
}

void
LNPlainTextEdit::clearBookmarks()
{
  m_bookmarks.clear();
  update();
  saveBookmarks();
}

bool
LNPlainTextEdit::hasBookmark(int bookmark)
{
  return m_bookmarks.contains(bookmark);
}

bool
LNPlainTextEdit::hasBookmarks()
{
  return !m_bookmarks.isEmpty();
}

bool
LNPlainTextEdit::hasBookmarkText(int bookmark)
{
  auto bm = m_bookmarks.value(bookmark);
  if (bm.isValid())
    return !bm.text.isEmpty();
  return false;
}

QString
LNPlainTextEdit::bookmarkText(int bookmark)
{
  QString text = m_bookmarks.value(bookmark).text;
  text = (text.isEmpty() ? tr("Bookmark") : text);
  return text;
}

int
LNPlainTextEdit::bookmarkCount()
{
  return m_bookmarks.size();
}

void
LNPlainTextEdit::loadBookmarks()
{
  auto filename = Paths::join(
    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),
    "bookmarks.yaml");
  QFile file(filename);

  if (file.open(QFile::ReadOnly)) {
    auto bookmarks = YAML::LoadFile(filename);

    if (bookmarks && bookmarks.IsMap()) {
      for (auto it = bookmarks.begin(); it != bookmarks.end(); ++it) {
        BookmarkData bookmark;
        auto id = it->first.as<int>();
        auto value = it->second;
        if (value.IsMap()) {
          auto text = value["text"].as<QString>();
          auto rect = value["rect"].as<QRect>();
          auto pixmap = value["pixmap"].as<int>();
          bookmark.text = text;
          bookmark.rect = rect;
          bookmark.pixmap = pixmap;
          bookmark.valid = true;
        }
        m_bookmarks.insert(id, bookmark);
      }
    }
  }
}

void
LNPlainTextEdit::saveBookmarks()
{
  auto filename = Paths::join(
    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),
    "bookmarks.yaml");
  QFile file(filename);
  if (file.open((QFile::WriteOnly | QFile::Truncate))) {
    YAML::Emitter emitter;

    emitter << YAML::Comment(
      tr("Bookmarks storage.\n\n"
         "Care should be taken editing this file manually\n"
         "as the wrong key-value pair could cause problems.\n"));

    emitter << YAML::Anchor("bookmarks");
    emitter << YAML::BeginMap;
    for (auto [key, value] : asKeyValueRange(m_bookmarks)) {
      emitter << YAML::Key << key << YAML::Value;
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "text" << YAML::Value << value.text;
      emitter << YAML::Key << "rect" << YAML::Value << value.rect;
      emitter << YAML::Key << "pixmap" << YAML::Value << value.pixmap;
      emitter << YAML::EndMap;
    }
    emitter << YAML::EndMap;

    emitter << YAML::EndSeq;

    QTextStream out(&file);
    out << emitter.c_str();
    file.close();
  }
}

void
LNPlainTextEdit::mousePressEvent(QMouseEvent* event)
{
  QPlainTextEdit::mousePressEvent(event);
  m_currentCursor = cursorForPosition(event->position().toPoint());

  if (event->button() == Qt::LeftButton) {
    auto data = calculateLineNumber(m_currentCursor);
    m_lineNumber = data.first;
    m_lineCount = data.second;
    gotoLineNumber();
    return;
  }
}

// void
// LNPlainTextEdit::mouseMoveEvent(QMouseEvent* event)
//{
//   auto bookmarkRect = m_bookmarkArea->rect();
//   auto pos = event->position().toPoint();
//   if (bookmarkRect.contains(pos)) {
//     m_bookmarkArea->mouseMoveEvent(event);
//     return;
//   }
//   QPlainTextEdit::mouseMoveEvent(event);
// }

bool
LNPlainTextEdit::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent*>(event);
    auto key = keyEvent->key();
    //    auto modifiers = keyEvent->modifiers();

    //    auto mapper = m_keyMap->value(KeyEventMapper::PreviousBookmark);
    //    if (mapper.isValid() && key == mapper.key &&
    //        modifiers == mapper.modifiers) {
    //      goToPreviousBookmark();
    //      return true;
    //    }

    //    mapper = m_keyMap->value(KeyEventMapper::NextBookmark);
    //    if (mapper.isValid() && key == mapper.key &&
    //        modifiers == mapper.modifiers) {
    //      goToNextBookmark();
    //      return true;
    //    }

    if (key == Qt::Key_Up && m_lineNumber > 0) {
      m_lineNumber--;
    }

    if (key == Qt::Key_Down && m_lineNumber < m_lineCount - 1) {
      m_lineNumber++;
    }
  }
  return QPlainTextEdit::eventFilter(obj, event);
}

bool
LNPlainTextEdit::event(QEvent* event)
{
  switch (event->type()) {
      //    case QEvent::HoverEnter:
      //      hoverEnter(static_cast<QHoverEvent*>(event));
      //      return true;
    case QEvent::HoverLeave:
      hoverLeave(static_cast<QHoverEvent*>(event));
      return true;
    case QEvent::HoverMove:
      hoverMove(static_cast<QHoverEvent*>(event));
      return true;
    default:
      break;
  }
  return QPlainTextEdit::event(event);
}

void
LNPlainTextEdit::paintEvent(QPaintEvent* event)
{
  QPlainTextEdit::paintEvent(event);
  // TODO fix QPaintEngine Should no longer be called error here
  QPainter painter(this);

  if (m_highlightLine) {
    auto rect = event->rect();
    auto tCursor = textCursor();
    auto cRect = cursorRect(tCursor);
    rect.setHeight(cRect.height());
    painter.fillRect(rect, m_lnAreaSelectedBackColor);
  }
}

// void
// LNPlainTextEdit::hoverEnter(QHoverEvent* event)
//{
// }

void
LNPlainTextEdit::hoverLeave(QHoverEvent* /*event*/)
{
  // should already have been deleted but to be safe
  if (m_hoverWidget) {
    m_hoverWidget->deleteLater();
    m_hoverWidget = nullptr;
  }
}

void
LNPlainTextEdit::hoverMove(QHoverEvent* event)
{
  // TODO
  if (m_bookmarkArea) {
    auto bookmarkRect = m_bookmarkArea->rect();
    auto pos = event->position().toPoint();
    if (bookmarkRect.contains(pos)) {
      auto data = calculateLineNumber(cursorForPosition(pos));
      auto lineNumber = data.first;
      if (m_bookmarkArea->hasHover(lineNumber)) {
        if (!m_hoverWidget) {
          m_hoverWidget = new HoverWidget(this);
          connect(m_hoverWidget,
                  &HoverWidget::finished,
                  this,
                  &LNPlainTextEdit::bookmarkHoverFinished);
        }
        if (!m_hoverWidget->isVisible()) {
          m_hoverWidget->setText(m_bookmarkArea->hoverText(lineNumber));
          m_hoverWidget->setTitle(m_bookmarkArea->hoverTitle(lineNumber));
          auto size = m_hoverWidget->sizeHint();
          m_hoverWidget->setGeometry(m_bookmarkArea->x(), pos.y(), 500, 200);
          m_hoverWidget->show(10000);
        }
      } else {
        // TODO not certain what needs to go here
      }
    }
  }
}

void
LNPlainTextEdit::gotoLineNumber()
{
  auto oldColumn = calculateColumn();

  QTextCursor cursorStart(document());
  cursorStart.movePosition(QTextCursor::Start);
  cursorStart.movePosition(
    QTextCursor::Down, QTextCursor::MoveAnchor, m_lineNumber - 1);
  auto start = cursorStart.position();

  QTextCursor cursorEnd(cursorStart);
  cursorEnd.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
  auto end = cursorEnd.position();

  auto length = end - start;
  if (length > oldColumn) {
    cursorStart.movePosition(
      QTextCursor::Right, QTextCursor::MoveAnchor, oldColumn);
    setTextCursor(cursorStart);
    auto data = calculateLineNumber(cursorStart);
    emit locationChanged(data.first, data.second, oldColumn);
  } else {
    setTextCursor(cursorEnd);
    auto data = calculateLineNumber(cursorEnd);
    emit locationChanged(data.first, data.second, length);
  }

  setFocus(Qt::OtherFocusReason);

  //  emit locationChanged(data.first, data.second, offset);
}

KeyEventMap*
LNPlainTextEdit::keyMap()
{
  return m_keyMap;
}

void
LNPlainTextEdit::setKeyMap(KeyEventMap* keyMap)
{
  m_keyMap = keyMap;
}

// QAction*
// LNPlainTextEdit::actionForKey(const QString& text, KeyEventMapper mapper)
//{
//   auto action = new QAction(text, this);
//   if (m_keyMap.contains(mapper)) {
//     auto k = m_keyMap.value(mapper).keySequence();
//     action->setShortcut(k);
//   }
//   return action;
// }

void
LNPlainTextEdit::displayTabs()
{
  m_settings->setDisplayTabs(true);
  auto text = toPlainText();
  QList<int> positions;
  for (int i = 0; i < text.length(); i++) {
    auto c = text.at(i);
    if (c == Characters::TAB) {
      positions.append(i);
    }
  }
  QList<int>::const_reverse_iterator it;
  it = positions.crbegin();
  while (it != positions.crend()) {
    text.insert(*it, Characters::HORIZONTAL_TAB_SYMBOL);
    ++it;
  }
  setPlainText(text);
}

void
LNPlainTextEdit::clearTabs()
{
  m_settings->setDisplayLineEnds(false);
  auto text = toPlainText();
  text.replace(Characters::HORIZONTAL_TAB_SYMBOL, "");
  setPlainText(text);
}

void
LNPlainTextEdit::resetDisplayTabs()
{
  setDisplayTabs(false);
}

// QMenu*
// LNPlainTextEdit::createContextMenu()
//{
//   auto menu = createStandardContextMenu();
//   if (m_bookmarkArea->isEnabled()) {
//     menu->addSeparator();
//     auto action = m_keyMap->action(KeyEventMapper::PreviousBookmark);
//     if (action) {
//       connect(action,
//               &QAction::triggered,
//               this,
//               &LNPlainTextEdit::goToPreviousBookmark);
//       menu->addAction(action);
//     }

//    action = m_keyMap->action(KeyEventMapper::NextBookmark);
//    if (action) {
//      connect(
//        action, &QAction::triggered, this,
//        &LNPlainTextEdit::goToNextBookmark);
//      menu->addAction(action);
//    }

//    auto submenu = new QMenu(tr("Goto Bookmark"), this);
//    for (auto [key, value] : asKeyValueRange(m_bookmarks)) {
//      auto action = submenu->addAction(tr("Line %1").arg(key));
//      action->setData(key);
//      connect(action,
//              &QAction::triggered,
//              this,
//              qOverload<bool>(&LNPlainTextEdit::goToBookmark));
//    }
//    menu->addMenu(submenu);
//  }
//  return menu;
//}

QMenu*
LNPlainTextEdit::modifyContextMenu(QMenu* menu)
{
  if (m_bookmarkArea->isEnabled()) {
    menu->addSeparator();
    auto action = m_keyMap->action(KeyEvent::PreviousBookmark);
    if (action) {
      connect(action,
              &QAction::triggered,
              this,
              &LNPlainTextEdit::goToPreviousBookmark);
      menu->addAction(action);
    }

    action = m_keyMap->action(KeyEvent::NextBookmark);
    if (action) {
      connect(
        action, &QAction::triggered, this, &LNPlainTextEdit::goToNextBookmark);
      menu->addAction(action);
    }

    auto submenu = new QMenu(tr("Goto Bookmark"), this);
    for (auto [key, value] : asKeyValueRange(m_bookmarks)) {
      auto action = submenu->addAction(tr("Line %1").arg(key));
      action->setData(key);
      connect(action,
              &QAction::triggered,
              this,
              qOverload<bool>(&LNPlainTextEdit::goToBookmark));
    }
    menu->addMenu(submenu);
  }

  return menu;
}

void
LNPlainTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
  auto menu = createStandardContextMenu();
  menu = modifyContextMenu(menu);

  menu->exec(event->globalPos());
  menu->deleteLater();
}

bool
LNPlainTextEdit::isDisplayLineEnds()
{
  return m_settings->isDisplayLineEnds();
}

void
LNPlainTextEdit::setDisplayLineEnds(bool showNewline)
{
  m_settings->setDisplayLineEnds(showNewline);
  auto text = toPlainText();
  if (m_settings->isDisplayLineEnds()) {
    QList<int> positions;
    for (int i = 0; i < text.length(); i++) {
      auto c = text.at(i);
      if (c == Characters::NEWLINE) {
        positions.append(i);
      }
    }
    QList<int>::const_reverse_iterator it;
    it = positions.crbegin();
    while (it != positions.crend()) {
      text.insert(*it, Characters::NEWLINE_LEFT_SYMBOL);
      ++it;
    }
  } else {
    m_settings->setDisplayLineEnds(false);
    auto text = toPlainText();
    text.replace(Characters::NEWLINE_LEFT_SYMBOL, "");
  }
  setPlainText(text);
}

void
LNPlainTextEdit::resetDisplayLineEnds()
{
  setDisplayLineEnds(false);
}

bool
LNPlainTextEdit::isDisplayTabs()
{
  return m_settings->isDisplayTabs();
}

void
LNPlainTextEdit::setDisplayTabs(bool showTabs)
{
  m_settings->setDisplayTabs(showTabs);
}

void
LNPlainTextEdit::setFoldStyle(Style foldStyle)
{
  m_foldStyle = foldStyle;
}

void
LNPlainTextEdit::enableBookmarks(bool enable)
{
  m_settings->setEnabledBookmarks(enable);
  if (enable) {
    if (!m_bookmarkArea) {
      m_bookmarkArea = new BookmarkArea(this);
      loadBookmarks();
    }
    m_bookmarkArea->setEnabled(enable);
  } else {
    saveBookmarks();
    m_bookmarkArea->deleteLater();
    m_bookmarkArea = nullptr;
  }
  repaint();
}

bool
LNPlainTextEdit::isBookmarksEnabled()
{
  return m_settings->isEnabledBookmarks();
}

void
LNPlainTextEdit::resetEnableBookmarks()
{
  enableBookmarks(false);
}

void
LNPlainTextEdit::enableFolds(bool enable)
{
  m_settings->setEnabledFolds(enable);
  if (enable) {
    if (!m_foldArea) {
      m_foldArea = new FoldArea(this);
    }
    m_foldArea->setEnabled(enable);
  } else {
    m_foldArea->deleteLater();
    m_foldArea = nullptr;
  }
  repaint();
}

bool
LNPlainTextEdit::isFoldsEnabled()
{
  return m_settings->isEnabledFolds();
}

void
LNPlainTextEdit::resetDisplayFolds()
{
  enableFolds(false);
}

void
LNPlainTextEdit::enableLineNumbers(bool enable)
{
  m_settings->setEnabledLineNumbers(enable);
  if (enable) {
    if (!m_lineNumberArea) {
      m_lineNumberArea = new LineNumberArea(this);
    }
    m_lineNumberArea->setEnabled(enable);
  } else {
    m_lineNumberArea->deleteLater();
    m_lineNumberArea = nullptr;
  }
  repaint();
}

bool
LNPlainTextEdit::isLineNumbersEnabled()
{
  return m_settings->isEnabledLineNumbers();
}

void
LNPlainTextEdit::resetEnableLineNumbers()
{
  enableLineNumbers(true);
}

//====================================================================
//=== LNPlainTextEdit::CommonArea
//====================================================================

LNPlainTextEdit::CommonArea::CommonArea(LNPlainTextEdit* editor)
  : QWidget(editor)
  , m_editor(editor)
{
  setMouseTracking(true);
}

void
LNPlainTextEdit::CommonArea::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    auto pos = event->position().toPoint();
    auto cursor = m_editor->cursorForPosition(pos);
    auto data = m_editor->calculateLineNumber(cursor);
    m_editor->goToLine(data.first);
  } else
    QWidget::mousePressEvent(event);
}

bool
LNPlainTextEdit::CommonArea::isEnabled() const
{
  return m_enabled;
}

void
LNPlainTextEdit::CommonArea::setEnabled(bool enabled)
{
  m_enabled = enabled;
}

void
LNPlainTextEdit::CommonArea::contextMenuEvent(QContextMenuEvent* event)
{
  m_editor->handleBookmarkContextMenu(event);
}

//====================================================================
//=== LNPlainTextEdit::LineNumberArea
//====================================================================
LNPlainTextEdit::LineNumberArea::LineNumberArea(LNPlainTextEdit* editor)
  : CommonArea(editor)
{
}

QSize
LNPlainTextEdit::LineNumberArea::sizeHint() const
{
  if (m_enabled)
    return QSize(m_editor->lineNumberAreaWidth(), 0);
  else
    return QSize(0, 0);
}

void
LNPlainTextEdit::LineNumberArea::paintEvent(QPaintEvent* event)
{
  m_editor->paintLNArea(event);
}

//====================================================================
//=== LNPlainTextEdit::FoldArea
//====================================================================
LNPlainTextEdit::FoldArea::FoldArea(LNPlainTextEdit* editor)
  : CommonArea(editor)
  , m_width(WIDTH)
{
  setMouseTracking(true);

  // TODO set default to off
}

void
LNPlainTextEdit::FoldArea::paintEvent(QPaintEvent* event)
{
  if (m_enabled) {
    m_rect.setLeft(0);
    m_rect.setRight(0 + event->rect().width());
    m_rect.setTop(event->rect().top());
    m_rect.setBottom(event->rect().bottom());

    m_editor->paintFoldArea(event);
  }
}

const QList<int>
LNPlainTextEdit::FoldArea::folds()
{
  return m_folds.keys();
}

void
LNPlainTextEdit::FoldArea::setFolds(QList<int> folds)
{
  for (auto fold : folds) {
    m_folds.insert(fold, qMakePair(false, QRect()));
    repaint();
  }
}

bool
LNPlainTextEdit::FoldArea::setFolds(QList<int> folds, QList<bool> folded)
{
  if (folds.size() != folded.size())
    return false;
  for (int i = 0; i < folds.size(); i++) {
    m_folds.insert(folds.at(i), qMakePair(folded.at(i), QRect()));
  }
  repaint();
  return true;
}

bool
LNPlainTextEdit::FoldArea::setFolds(QList<int> folds,
                                    QList<bool> folded,
                                    QList<QRect> rects)
{
  if (folds.size() != folded.size() != rects.size())
    return false;
  for (int i = 0; i < folds.size(); i++) {
    m_folds.insert(folds.at(i), qMakePair(folded.at(i), rects.at(i)));
  }
  repaint();
  return true;
}

void
LNPlainTextEdit::FoldArea::insertFold(int lineNumber)
{
  if (!m_folds.contains(lineNumber)) {
    m_folds.insert(lineNumber, qMakePair(false, QRect()));
    repaint();
  }
}

void
LNPlainTextEdit::FoldArea::insertFold(int lineNumber, bool folded)
{
  if (!m_folds.contains(lineNumber)) {
    m_folds.insert(lineNumber, qMakePair(folded, QRect()));
    repaint();
  }
}

void
LNPlainTextEdit::FoldArea::insertFold(int lineNumber, bool folded, QRect rect)
{
  if (!m_folds.contains(lineNumber)) {
    m_folds.insert(lineNumber, qMakePair(folded, rect));
    repaint();
  }
}

void
LNPlainTextEdit::FoldArea::clearFolds()
{
  m_folds.clear();
  repaint();
}

bool
LNPlainTextEdit::FoldArea::hasFold(int lineNumber)
{
  return m_folds.contains(lineNumber);
}

bool
LNPlainTextEdit::FoldArea::isFolded(int lineNumber)
{
  auto value = m_folds.value(lineNumber);
  // TODO value should be (false, QRect()) if no fold. Check this.
  return value.first;
}

QRect
LNPlainTextEdit::FoldArea::rect(int lineNumber)
{
  return m_folds.value(lineNumber).second;
}

void
LNPlainTextEdit::FoldArea::set(int lineNumber, bool folded)
{
  if (hasFold(lineNumber)) {
    auto pair = m_folds.value(lineNumber);
    m_folds.insert(lineNumber, qMakePair(folded, pair.second));
  } else {
    m_folds.insert(lineNumber, qMakePair(folded, QRect()));
  }
  repaint();
}

void
LNPlainTextEdit::FoldArea::set(int lineNumber, QRect rect)
{
  auto pair = m_folds.value(lineNumber);
  m_folds.insert(lineNumber, qMakePair(pair.first, rect));
}

void
LNPlainTextEdit::FoldArea::set(int lineNumber, bool folded, QRect rect)
{
  m_folds.insert(lineNumber, qMakePair(folded, rect));
  repaint();
}

int
LNPlainTextEdit::FoldArea::width() const
{
  return m_width;
}

void
LNPlainTextEdit::FoldArea::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    for (auto [lineNumber, value] : asKeyValueRange(m_folds)) {
      auto rect = value.second;
      if (rect.contains(event->position().toPoint())) {
        if (value.first) { // TODO are these the right way around?
          emit m_editor->unfold(lineNumber);
        } else {
          emit m_editor->fold(lineNumber);
        }
        set(lineNumber, !value.first, value.second);
        repaint();
      }
    }
  }
  CommonArea::mousePressEvent(event);
}

QSize
LNPlainTextEdit::FoldArea::sizeHint() const
{
  if (!m_enabled) {
    return QSize(0, 0);
  }
  return QSize(WIDTH, 0);
}

//====================================================================
//=== LNPlainTextEdit::BookmarkArea
//====================================================================
LNPlainTextEdit::BookmarkArea::BookmarkArea(LNPlainTextEdit* editor)
  : CommonArea(editor)
  , m_width(WIDTH)
{
  setMouseTracking(true);
}

LNPlainTextEdit::BookmarkArea::~BookmarkArea()
{
  clearHovers();
}

QSize
LNPlainTextEdit::BookmarkArea::sizeHint() const
{
  if (!m_enabled) {
    return QSize(0, 0);
  }
  return QSize(WIDTH, 0);
}

int
LNPlainTextEdit::BookmarkArea::width() const
{
  if (!m_enabled) {
    return 0;
  }
  return m_width;
}

void
LNPlainTextEdit::BookmarkArea::paintEvent(QPaintEvent* event)
{
  if (m_enabled) {
    m_editor->paintBookmarkArea(event);
  }
}

void
LNPlainTextEdit::BookmarkArea::resizeEvent(QResizeEvent* event)
{
  m_rect.setSize(event->size());
}

const QRect&
LNPlainTextEdit::BookmarkArea::rect() const
{
  return m_rect;
}

void
LNPlainTextEdit::BookmarkArea::insertHover(int lineNumber,
                                           int icon,
                                           const QString& title,
                                           const QString& text)
{
  if (!m_hovers.contains(lineNumber)) {
    auto item = new HoverItem;
    item->iconId = icon;
    item->title = title;
    item->text = text;
    m_hovers.insert(lineNumber, item);
    repaint();
  }
}

bool
LNPlainTextEdit::BookmarkArea::hasHover(int lineNumber)
{
  return m_hovers.contains(lineNumber);
}

void
LNPlainTextEdit::BookmarkArea::clearHovers()
{
  qDeleteAll(m_hovers);
  m_hovers.clear();
}

QString
LNPlainTextEdit::BookmarkArea::hoverTitle(int lineNumber)
{
  if (m_hovers.contains(lineNumber)) {
    auto item = m_hovers.value(lineNumber);
    if (item)
      return item->title;
  }
  return QString();
}

QString
LNPlainTextEdit::BookmarkArea::hoverText(int lineNumber)
{
  if (m_hovers.contains(lineNumber)) {
    auto item = m_hovers.value(lineNumber);
    if (item)
      return item->text;
  }
  return QString();
}

int
LNPlainTextEdit::BookmarkArea::hoverIcon(int lineNumber)
{
  if (m_hovers.contains(lineNumber)) {
    auto item = m_hovers.value(lineNumber);
    if (item)
      return item->iconId;
  }
  return -1;
}

//====================================================================
//=== LNPlainTextEdit::BookmarkModel
//====================================================================
LNPlainTextEdit::BookmarkModel::BookmarkModel(
  QMap<int, BookmarkData*>* bookmarks)
  : QAbstractTableModel()
  , m_bookmarks(bookmarks)
{
}

int
LNPlainTextEdit::BookmarkModel::columnCount(const QModelIndex&) const
{
  return 2;
}

int
LNPlainTextEdit::BookmarkModel::rowCount(const QModelIndex&) const
{
  return m_bookmarks->size();
}

QVariant
LNPlainTextEdit::BookmarkModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0: {
        auto keys = m_bookmarks->keys();
        return keys.at(index.row());
      }

      case 1: {
        auto keys = m_bookmarks->keys();
        int i = keys.at(index.row());
        return m_bookmarks->value(i)->text;
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags
LNPlainTextEdit::BookmarkModel::flags(const QModelIndex& index) const
{
  if (index.isValid()) {
    return (QAbstractTableModel::flags(index));
  }

  return Qt::NoItemFlags;
}

QVariant
LNPlainTextEdit::BookmarkModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int /*role*/) const
{
  if (orientation == Qt::Horizontal) {
    if (section == 0) {
      return tr("Bookmark");

    } else {
      return tr("Message");
    }
  }

  return QVariant();
}

//====================================================================
//=== LNPlainTextEditSettingsWidget
//====================================================================
LNPlainTextEditSettingsWidget::LNPlainTextEditSettingsWidget(
  LNPlainTextEditSettings* settings,
  LNPlainTextEdit* parent)
  : QWidget(parent)
  , m_editor(parent)
  , m_settings(settings)
{
}

void
LNPlainTextEditSettingsWidget::initGui(int& row)
{
  auto layout = new QGridLayout;
  setLayout(layout);

  auto group = new QGroupBox(this);
  auto groupLayout = new QVBoxLayout;
  group->setLayout(groupLayout);
  auto chk = new QCheckBox(tr("Enable bookmarks :"), this);
  chk->setChecked(m_editor->isBookmarksEnabled());
  chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  connect(chk,
          &QCheckBox::clicked,
          this,
          &LNPlainTextEditSettingsWidget::setEnabledBookmarks);
  groupLayout->addWidget(chk);

  chk = new QCheckBox(tr("Enable line numbers :"), this);
  chk->setChecked(m_editor->isLineNumbersEnabled());
  chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  connect(chk,
          &QCheckBox::clicked,
          this,
          &LNPlainTextEditSettingsWidget::setEnabledLineNumbers);
  groupLayout->addWidget(chk);

  chk = new QCheckBox(tr("Enable folds :"), this);
  chk->setChecked(m_editor->isFoldsEnabled());
  chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  connect(chk,
          &QCheckBox::clicked,
          this,
          &LNPlainTextEditSettingsWidget::setEnabledFolds);
  groupLayout->addWidget(chk);
  layout->addLayout(groupLayout, 0, 0);

  chk = new QCheckBox(tr("Display line end :"), this);
  chk->setChecked(m_editor->isDisplayLineEnds());
  chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  connect(chk,
          &QCheckBox::clicked,
          this,
          &LNPlainTextEditSettingsWidget::setDisplayLineEnds);
  layout->addWidget(chk, row++, 0);

  auto tabGroup = new QGroupBox(tr("Tabs"), this);
  layout->addWidget(tabGroup, row++, 0);
  auto col = 0;
  auto tabLayout = new QGridLayout;
  tabGroup->setLayout(tabLayout);

  auto lbl = new QLabel(tr("Tab Width:"), this);
  tabLayout->addWidget(lbl, row, col++);
  auto tabBox = new QSpinBox(this);
  tabBox->setMinimum(1);
  tabBox->setMaximum(20);
  tabBox->setValue(m_settings->tabStopDistance());
  connect(tabBox,
          &QSpinBox::valueChanged,
          this,
          &LNPlainTextEditSettingsWidget::tabWidthChanged);

  auto fontBox = new QGroupBox(tr("Font"), this);
  layout->addWidget(fontBox, row++, 0);
  col = 0;
  auto fontLayout = new QGridLayout;
  fontBox->setLayout(fontLayout);

  lbl = new QLabel(tr("Family:"), this);
  fontLayout->addWidget(lbl, 0, col++);
  auto familyBox = new QComboBox(this);
  familyBox->addItems(m_editor->getFontFamilies());
  auto font = m_editor->font();
  familyBox->setCurrentText(font.family());
  connect(familyBox,
          &QComboBox::currentTextChanged,
          this,
          &LNPlainTextEditSettingsWidget::fontFamilyHasChanged);
  fontLayout->addWidget(familyBox, 0, col++);

  lbl = new QLabel(tr("Size:"), this);
  fontLayout->addWidget(lbl, 0, col++);
  auto sizeBox = new QComboBox(this);
  sizeBox->addItems({ "6", "7", "8", "9", "10", "11", "12", "14", "16", "18" });
  sizeBox->setCurrentText(QString::number(font.pointSize()));
  connect(sizeBox,
          &QComboBox::currentTextChanged,
          this,
          &LNPlainTextEditSettingsWidget::fontSizeHasChanged);
  fontLayout->addWidget(sizeBox, 0, col++);

  //  chk = new QCheckBox("Display tab characters :", this);
  //  chk->setChecked(m_editor->isFoldsEnabled());
  //  chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  //  connect(
  //    chk, &QCheckBox::clicked, this,
  //    &LNPlainTextEditSettings::setEnableFolds);
  //  m_layout->addWidget(chk, row++, 0);

  //  chk = new QCheckBox("Load standard icons :", this);
  //  chk->setChecked(m_editor->uses());
  //  chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  //  connect(
  //    chk, &QCheckBox::clicked, this,
  //    &LNPlainTextEditSettings::setEnableFolds);
  //  m_layout->addWidget(chk, row++, 0);
}

const QString&
LNPlainTextEditSettingsWidget::fontFamily() const
{
  return m_settings->fontFamily();
}

void
LNPlainTextEditSettingsWidget::setFontFamily(const QString& fontFamily)
{
  m_settings->setFontFamily(fontFamily);
}

int
LNPlainTextEditSettingsWidget::fontSize() const
{
  return m_settings->fontSize();
}

qreal
LNPlainTextEditSettingsWidget::fontSizeF() const
{
  return m_settings->fontSizeF();
}

void
LNPlainTextEditSettingsWidget::setFontSize(int fontSize)
{
  m_settings->setFontSize(fontSize);
}

void
LNPlainTextEditSettingsWidget::setFontSizeF(qreal fontSize)
{
  m_settings->setFontSizeF(fontSize);
}

void
LNPlainTextEditSettingsWidget::fontFamilyHasChanged(const QString& text)
{
  setFontFamily(text);
  emit fontFamilyChanged(text);
}

void
LNPlainTextEditSettingsWidget::fontSizeHasChanged(const QString& text)
{
  auto size = text.toInt();
  setFontSize(size);
  emit fontSizeChanged(size);
}

void
LNPlainTextEditSettingsWidget::tabWidthChanged(int tabs)
{
  m_settings->setTabStop(tabs);
}

bool
LNPlainTextEditSettingsWidget::isDisplayLineEnds() const
{
  return m_settings->isDisplayLineEnds();
}

void
LNPlainTextEditSettingsWidget::setDisplayLineEnds(bool displayLineEnds)
{
  m_settings->setDisplayLineEnds(displayLineEnds);
}

// bool
// LNPlainTextEditSettings::useStandardIcons() const
//{
//   return m_useStandardIcons;
// }

// voidd
// LNPlainTextEditSettings::setUseStandardIcons(bool useStandardIcons)
//{
//   if (m_useStandardIcons != useStandardIcons) {
//     m_useStandardIcons = useStandardIcons;
//     m_modified = true;
//   }
// }

bool
LNPlainTextEditSettingsWidget::isEnabledBookmarks() const
{
  return m_settings->isEnabledBookmarks();
}

void
LNPlainTextEditSettingsWidget::setEnabledBookmarks(bool enable)
{
  m_settings->setEnabledBookmarks(enable);
}

bool
LNPlainTextEditSettingsWidget::isEnabledLineNumbers() const
{
  return m_settings->isEnabledLineNumbers();
}

void
LNPlainTextEditSettingsWidget::setEnabledLineNumbers(bool enable)
{
  m_settings->setEnabledLineNumbers(enable);
}

bool
LNPlainTextEditSettingsWidget::isEnabledFolds() const
{
  return m_settings->isEnabledFolds();
}

void
LNPlainTextEditSettingsWidget::setEnabledFolds(bool enable)
{
  m_settings->setEnabledFolds(enable);
}

bool
LNPlainTextEditSettingsWidget::isModified() const
{
  return m_settings->isModified();
}

//====================================================================
//=== LNPlainTextEditSettings
//====================================================================

LNPlainTextEditSettings::LNPlainTextEditSettings(QObject* parent)
  : ISettingItem(parent)
{
}

LNPlainTextEditSettings::LNPlainTextEditSettings(BaseConfig* config,
                                                 QObject* parent)
  : ISettingItem(parent)
{
}

LNPlainTextEditSettings::~LNPlainTextEditSettings() {}

bool
LNPlainTextEditSettings::isEnabledBookmarks() const
{
  return m_enableBookmarks;
}

void
LNPlainTextEditSettings::setEnabledBookmarks(bool enable)
{
  if (m_enableBookmarks != enable) {
    m_enableBookmarks = enable;
    m_modified = true;
  }
}

bool
LNPlainTextEditSettings::isEnabledLineNumbers() const
{
  return m_enableLineNumbers;
}

void
LNPlainTextEditSettings::setEnabledLineNumbers(bool enable)
{
  if (m_enableLineNumbers != enable) {
    m_enableLineNumbers = enable;
    m_modified = true;
  }
}

bool
LNPlainTextEditSettings::isEnabledFolds() const
{
  return m_enableFolds;
}

void
LNPlainTextEditSettings::setEnabledFolds(bool enable)
{
  if (m_enableFolds != enable) {
    m_enableFolds = enable;
    m_modified = true;
  }
}

bool
LNPlainTextEditSettings::isModified() const
{
  return m_modified;
}

bool
LNPlainTextEditSettings::isDisplayLineEnds() const
{
  return m_displayLineEnds;
}

void
LNPlainTextEditSettings::setDisplayLineEnds(bool displayLineEnds)
{
  if (m_displayLineEnds != displayLineEnds) {
    m_displayLineEnds = displayLineEnds;
    m_modified = true;
  }
}

bool
LNPlainTextEditSettings::isDisplayTabs() const
{
  return m_displayTabs;
}

void
LNPlainTextEditSettings::setDisplayTabs(bool displayTabs)
{
  if (m_displayTabs != displayTabs) {
    m_displayTabs = displayTabs;
    m_modified = true;
  }
}

qreal
LNPlainTextEditSettings::tabStopDistance() const
{
  return m_tabWidth;
}

void
LNPlainTextEditSettings::setTabStop(qreal TabWidth)
{
  m_tabWidth = TabWidth;
}

YAML::Node
LNPlainTextEditSettings::createNode(YAML::Node /*root*/, YAML::Node parent)
{
  parent["enable_bookmarks"] = m_enableBookmarks;
  parent["line_numbers"] = m_enableLineNumbers;
  parent["folds"] = m_enableFolds;
  parent["line_ends"] = m_displayLineEnds;
  parent["tabs"] = m_displayTabs;
  parent["tab_width"] = m_tabWidth;
  parent["font_family"] = m_fontFamily;
  parent["font_size"] = m_fontSize;
  return parent;
}

bool
LNPlainTextEditSettings::load()
{
  return true;
}

int
LNPlainTextEditSettings::fontSize() const
{
  return int(m_fontSize);
}

qreal
LNPlainTextEditSettings::fontSizeF() const
{
  return m_fontSize;
}

void
LNPlainTextEditSettings::setFontSize(int fontSize)
{
  m_fontSize = fontSize;
  m_modified = true;
}

void
LNPlainTextEditSettings::setFontSizeF(qreal fontSize)
{
  m_fontSize = fontSize;
  m_modified = true;
}

const QString&
LNPlainTextEditSettings::fontFamily() const
{
  return m_fontFamily;
}

void
LNPlainTextEditSettings::setFontFamily(const QString& fontFamily)
{
  if (m_fontFamily != fontFamily) {
    m_fontFamily = fontFamily;
    m_modified = true;
  }
}
