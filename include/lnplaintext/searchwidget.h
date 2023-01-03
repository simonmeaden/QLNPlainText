#pragma once

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QWidget>

class SearchWidget : public QDialog
{
  Q_OBJECT
public:
  SearchWidget(QWidget* parent = nullptr);

signals:
  void doSearch(const QString& text,
                bool searchDown = true,
                bool allowRestartAtTop = true);
  void doSearchUp(const QString& text);
  void doSearchDown(const QString& text);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private:
  QString m_searchText;
  QString m_replaceText;
  bool m_searchDown = true;

  void setSearchText(const QString& text);
  void setReplaceText(const QString& text);
  void setSearchDown(bool direction);
  void help();
  void prev();
  void next();
  void replace();
  void replaceAll();
  void deactivate() {}
  void actiavte() {}
};
