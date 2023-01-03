#include "config/baseconfig.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <yaml-cpp/yaml.h>

//====================================================================
//=== ISettingItem
//====================================================================
ISettingItem::ISettingItem(QObject* parent)
  : QObject(parent)
{
}

YAML::Node
ISettingItem::createNode(YAML::Node root, YAML::Node parent)
{
  return YAML::Node();
}

int
ISettingItem::id() const
{
  return m_id;
}

void
ISettingItem::setId(int id)
{
  m_id = id;
}

//====================================================================
//=== ISettingItems
//====================================================================
int ISettingItems::HIGHEST_ITEM_ID = 0;

ISettingItems::ISettingItems(QObject* parent)
  : QObject{ parent }
{
}

void
ISettingItems::addSetting(ISettingItem* widget)
{
  m_settingList.append(widget);
}

QList<ISettingItem*>
ISettingItems::SettingItem()
{
  return m_settingList;
}

int
ISettingItems::nextItemId()
{
  return HIGHEST_ITEM_ID++;
}

void
ISettingItems::setNextItemId(int id)
{
  HIGHEST_ITEM_ID = (id > HIGHEST_ITEM_ID ? HIGHEST_ITEM_ID : id);
}

//====================================================================
//=== BaseConfig
//====================================================================
BaseConfig::BaseConfig(QObject* parent)
  : ISettingItems(parent)
{
}

BaseConfig::BaseConfig(const QString& filename, QObject* parent)
  : ISettingItems{ parent }
{
  loadFile(filename);
}

bool
BaseConfig::isValid()
{
  return !m_rootNode.IsNull();
}

YAML::Node
BaseConfig::root()
{
  return m_rootNode;
}

bool
BaseConfig::save(const QString& filename)
{
  // So far does nothing.
  return true;
}

bool
BaseConfig::load(const QString& filename)
{
  return true;
}

void
BaseConfig::saveFile(const QString& filename)
{
  QFile file(filename);
  QFileInfo info(filename);
  QDir dir;
  dir.mkdir(info.absoluteFilePath());
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    YAML::Emitter emitter;
    emitter << m_rootNode;

    QTextStream out(&file);
    out << emitter.c_str();
    file.close();
  }
}

void
BaseConfig::loadFile(const QString& filename)
{
  QFile file(filename);
  if (file.open(QFile::ReadWrite)) {
    m_rootNode = YAML::LoadFile(filename.toStdString());
  }
}
