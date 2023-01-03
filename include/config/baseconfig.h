#pragma once

#include <QObject>

#include <yaml-cpp/yaml.h>

#include "lnplaintext_global.h"

class BaseConfig;

class ISettingItem : public QObject
{
  Q_OBJECT
public:
  explicit ISettingItem(QObject* parent);

  virtual YAML::Node createNode(YAML::Node root, YAML::Node parent);

  int id() const;
  void setId(int id);

private:
  int m_id;
};

class ISettingItems : public QObject
{
  Q_OBJECT
public:
  explicit ISettingItems(QObject* parent = nullptr);

  void addSetting(ISettingItem* widget);

  QList<ISettingItem*> SettingItem();

  int nextItemId();
  void setNextItemId(int id);

signals:

protected:
  QList<ISettingItem*> m_settingList;

private:
  static int HIGHEST_ITEM_ID;
};

class LNPLAINTEXT_SHARED_EXPORT BaseConfig : public ISettingItems
{
  Q_OBJECT
public:
  explicit BaseConfig(QObject* parent = nullptr);
  explicit BaseConfig(const QString& filename, QObject* parent = nullptr);
  ~BaseConfig() {}

  bool isValid();

  YAML::Node root();

  virtual bool save(const QString& filename = QString());
  virtual bool load(const QString& filename = QString());

signals:

protected:
  YAML::Node m_rootNode;

  void loadFile(const QString& filename);
  void saveFile(const QString& filename);
};
