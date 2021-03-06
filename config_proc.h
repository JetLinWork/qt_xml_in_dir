#ifndef CONFIG_PROC_H
#define CONFIG_PROC_H

# include <stdio.h>
# include <stdlib.h>

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

namespace Ui {
class ConfigProc;
class AppfsVerProc;
}

class AppfsVer
{
public:
    AppfsVer();
    ~AppfsVer();

    struct TAppfsVerTag
    {
        int major_ver;
        int minor_ver;
        int rev_ver;
        int sub_ver;
    };
    typedef struct TAppfsVerTag TAppfsVer;

public:
    bool ReadFile(QString file_path);
    bool SaveFile(TAppfsVer &version_save);
    QString &VersionStr() { return ver_string_; }
    TAppfsVer &Version() { return version_; }

private:
    QString file_path_;
    QString content_;
    QString ver_string_;
    TAppfsVer version_;
};

class MainWindow;

class ConfigProc
{
public:
    ConfigProc(MainWindow* win) : winodow((QObject*)win) {}
    ~ConfigProc() {}
public:
    enum TFileTypeTag
    {
        kXmlFileType = 0,
        kTxtFileType,
        kCfgFileType,
        kMkFileType,
        kIniFileType,
        kFileTypeMax,
    };
    typedef enum TFileTypeTag TFileType;

public:
    bool LoadHmi(const QString &file_path);
    bool LoadDir(const QString dir);
    bool FindElement(const QString &file, const QString &path, QDomElement &target_element);
    QString QueryElement(const QString &file, const QString &path, const QString attr = "value");
    bool SetElement(const QString &file, const QString &path, const QString value = "", const QString attr = "value");
    bool RemoveElement(const QString &file, const QString &path);
    bool RemoveElementAttr(const QString &file, const QString &path, const QString attr = "value");
    bool AddElement(const QString &file, const QString &path, const QString &name);
    bool SaveXML(const QString &file);
    AppfsVer &AppVer() { return appfs_ver_; }

private:
    bool ScanFile(const QString &dir_name, const QStringList &filters, TFileType f_type);
    bool LoadXmlDoc(QStringList &file_list);
    bool LoadMkDoc(QStringList &file_list);
    QString XmlFilePath(const QString &file);
    QDomDocument *XmlDom(const QString &file);

private:
    QObject* winodow;
    QStringList file_path_list_[kFileTypeMax];
    QStringList file_name_list_[kFileTypeMax];
    QList<QDomDocument> xml_list_;
    QDomDocument hmi_config_;
    AppfsVer appfs_ver_;
};


#endif // CONFIG_PROC_H
