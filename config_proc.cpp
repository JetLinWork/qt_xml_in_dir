#include "config_proc.h"

static const char encode[] = "GB2312";

bool ConfigProc::LoadHmi(const QString &file_path)
{
    bool flag = true;
    if(file_path.size())
    {
        QFile file(file_path);
        QDomDocument xml_doc;
        if(!file.open(QFile::ReadOnly))
        {
            QMessageBox::warning(qobject_cast<QWidget *>(winodow), "界面警告", file_path + " : 界面配置不存在");
            qDebug() << "[LoadHmi] open file faild : " << file_path << endl;
            flag = false;
        }
        else
        {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName(encode));
            QString content = stream.readAll();
            QString xml_err;
            int err_line = 0;
            if(!xml_doc.setContent(content, &xml_err, &err_line, nullptr))
            {
                flag = false;
                QMessageBox::warning(qobject_cast<QWidget *>(winodow), "警告", file_path + ":" + xml_err + " line:" +QString::number(err_line));
            }
            else
            {
                hmi_config_ = xml_doc;
                qDebug() << hmi_config_.toString();
            }
        }
        file.close();
    }
    return flag;
}

bool ConfigProc::LoadDir(const QString dir)
{
    if (dir.size())
    {
        // clear buffer
        qint32 list_num = kFileTypeMax;
        while(--list_num >= 0)
        {
            file_path_list_[list_num].clear();
            file_name_list_[list_num].clear();
        }

        // load file to buffer
        QStringList filters;
        filters.push_back("*.xml");
        ScanFile(dir, filters, kXmlFileType);
        filters.pop_back();
        filters.push_back("*.mk");
        ScanFile(dir, filters, kMkFileType);
        filters.pop_back();
        filters.push_back("*.txt");
        ScanFile(dir, filters, kTxtFileType);
        filters.pop_back();
        filters.push_back("*.cfg");
        ScanFile(dir, filters, kCfgFileType);
        filters.pop_back();
        filters.push_back("*.ini");
        ScanFile(dir, filters, kIniFileType);
        filters.pop_back();
    }
    else
    {
        QMessageBox::information(qobject_cast<QWidget *>(winodow), "提示信息", "文件夹内容为空");
        return false;
    }

    // XML file load
    if(!LoadXmlDoc(file_path_list_[kXmlFileType]))
    {
        QMessageBox::warning(qobject_cast<QWidget *>(winodow), "警告", "配置文件存在错误");
    }
    // MK file load
    if(!LoadMkDoc(file_path_list_[kMkFileType]))
    {
        QMessageBox::warning(qobject_cast<QWidget *>(winodow), "警告", "配置文件存在错误");
    }
    return true;
}

bool ConfigProc::FindElement(const QString &file, const QString &path, QDomElement &target_element)
{
    QDomDocument *xml_dom = XmlDom(file);
    if(xml_dom)
    {
        QDomElement element = xml_dom->documentElement();
        QList<QString> nodes = path.split('/', QString::SkipEmptyParts);
        QString target = nodes.size()? nodes.back(): QString("");
        while(nodes.size() && element.tagName() == nodes[0])
        {
            nodes.pop_front();
            if(nodes.size())
            {
                for(int i = 0; i < element.childNodes().size(); i++)
                {
                    if(element.childNodes().at(i).toElement().tagName() == nodes[0])
                    {
                        element = element.childNodes().at(i).toElement();
                    }
                }
                if(element.tagName() != nodes[0])
                {
                    qDebug() << "[FindElement] find node " << nodes[0] << "err" << endl;
                    break;
                }
            }
        }

        if(target.size() && element.tagName() == target)
        {
            target_element = element;
            return true;
        }
    }
    return false;
}

QString ConfigProc::QueryElement(const QString &file, const QString &path, const QString attr)
{
    QString result;
    QDomElement element;
    if(FindElement(file, path, element))
        result = element.attribute(attr, "");
            
    return result;
}

bool ConfigProc::SetElement(const QString &file, const QString &path, const QString value, const QString attr)
{
    QDomElement element;
    if(FindElement(file, path, element))
    {
        element.setAttribute(attr, value);
        return true;
    }
    return false;
}

bool ConfigProc::RemoveElement(const QString &file, const QString &path)
{
    QDomElement element;
    if(FindElement(file, path, element))
    {
        if(XmlDom(file)->documentElement() == element)
        {
            qDebug() << "[RemoveElement] remove root node is not allowed." << endl;
            return false;
        }
        
        element.parentNode().removeChild(element);
        return true;
    }
    return false;

}

bool ConfigProc::RemoveElementAttr(const QString &file, const QString &path, const QString attr)
{
    QDomElement element;
    if(FindElement(file, path, element))
    {
        element.removeAttribute(attr);
        return true;
    }
    return false;
}

bool ConfigProc::AddElement(const QString &file, const QString &path, const QString &name)
{
    QDomElement element;
    if(FindElement(file, path, element) && name.size())
    {
        QDomNode child_node = XmlDom(file)->createElement(name);
        element.appendChild(child_node);
        return true;
    }
    return false;
}

bool ConfigProc::SaveXML(const QString &file)
{
    QDomDocument *xml_dom = XmlDom(file);
    if(xml_dom)
    {
        QString content = xml_dom->toString(4);
        if(XmlFilePath(file).size())
        {
            QFile qfile(XmlFilePath(file));
            if(qfile.open(QFile::WriteOnly | QFile::Truncate))
            {
                QTextStream out(&qfile);
                out.setCodec(QTextCodec::codecForName(encode));
                out << content;
                out.flush();
                // out.setGenerateByteOrderMark(false);
            }
            qfile.close();
            qDebug() << "[SaveXML] " << file << ":" << XmlFilePath(file) << " save file success! " << endl;
            return true;
        }
        else
        {
            qDebug() << "[SaveXML] " << file << " not found at " << XmlFilePath(file) << endl;
        }
    }
    else
    {
        qDebug() << "[SaveXML] no such file before : " << file << endl;
    }
    return false;
}

bool ConfigProc::ScanFile(const QString &dir_name, const QStringList &filters, TFileType f_type)
{
    QDir search_dir(dir_name);
    QFileInfoList fileInfoList = search_dir.entryInfoList(filters, QDir::AllDirs|QDir::Files);
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if (fileInfo.isDir())
        {
            if (!ScanFile(fileInfo.filePath(), filters, f_type))
            {
                // qDebug() << "[ScanFile] dir " << fileInfo.filePath() << " is empty." << endl;
                return false;
            }
        }
        else
        {
            file_path_list_[f_type].append(fileInfo.absoluteFilePath());
            file_name_list_[f_type].append(fileInfo.fileName());
        }
    }
    return true;
}

bool ConfigProc::LoadXmlDoc(QStringList &file_list)
{
    bool flag = true;
    xml_list_.clear();

    if(file_list.size())
    {
        foreach(QString file_path, file_list)
        {
            QFile file(file_path);
            QDomDocument xml_doc;
            if(!file.open(QFile::ReadOnly))
            {
                qDebug() << "[LoadXmlDoc] open file faild : " << file_path << endl;
                flag = false;
            }
            else
            {
                QTextStream stream(&file);
                stream.setCodec(QTextCodec::codecForName(encode));
                QString content = stream.readAll();
                QString xml_err;
                int err_line = 0;
                if(!xml_doc.setContent(content, &xml_err, &err_line, nullptr))
                {
                    flag = false;
                    QMessageBox::warning(qobject_cast<QWidget *>(winodow), "警告", file_path + ":" + xml_err + " line:" +QString::number(err_line));
                }
            }
            xml_list_.push_back(xml_doc);
            file.close();
        }
    }
    else
    {
        QMessageBox::information(qobject_cast<QWidget *>(winodow), "提示", "文件夹中无配置文件");
    }
    return flag;
}

bool ConfigProc::LoadMkDoc(QStringList &file_list)
{
    bool flag = true;

    if(file_list.size())
    {
        foreach(QString file_path, file_list)
        {
            if(file_path.contains("version.mk"))
            {
                appfs_ver_.ReadFile(file_path);
            }
        }
    }
    return flag;
}

QString ConfigProc::XmlFilePath(const QString &file)
{
    int index = -1;
    if((index = file_name_list_[kXmlFileType].indexOf(file)) != -1)
    {
        return file_path_list_[kXmlFileType][index];
    }
    if(file.size())
        qDebug() << "[XmlFilePath] xml path of " << file << "not found." << endl;
    return QString("");
}

QDomDocument *ConfigProc::XmlDom(const QString &file)
{
    int index = -1;
    if((index = file_name_list_[kXmlFileType].indexOf(file)) != -1)
    {
        if(xml_list_[index].isNull())
        {
            qDebug() << "[XmlDom] xml dom of " << file << "is null." << endl;
            return nullptr;
        }
        return &xml_list_[index];
    }
    qDebug() << "[XmlDom] xml dom of " << file << "not found." << endl;
    return nullptr;
}


AppfsVer::AppfsVer()
{
}

AppfsVer::~AppfsVer()
{
}

static inline int FindVerNum(const QString &content, const QString &pattern)
{
    int from, to;
    from = content.indexOf(pattern) + pattern.size();
    to = content.indexOf('\n', from);
    // qDebug() << "[FindVerNum] " << content.mid(from, to) << endl;
    return atoi(content.mid(from, to).toStdString().c_str());
}

bool AppfsVer::ReadFile(QString file_path)
{
    bool flag = false;
    file_path_ = file_path;
    if(file_path_.size())
    {
        QFile file(file_path_);
        if(!file.open(QFile::ReadOnly))
        {
            qDebug() << "[ReadFile] open file faild : " << file_path_ << endl;
        }
        else
        {
            flag = true;
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName(encode));
            content_ = stream.readAll();
            version_.major_ver = FindVerNum(content_, "VER_MAJOR=");
            version_.minor_ver = FindVerNum(content_, "VER_MINOR=");
            version_.rev_ver = FindVerNum(content_, "VER_REV=");
            version_.sub_ver = FindVerNum(content_, "VER_SUB=");
            ver_string_ = QString("%1.%2.%3.%4").arg(version_.major_ver)
                                                .arg(version_.minor_ver)
                                                .arg(version_.rev_ver)
                                                .arg(version_.sub_ver);
            qDebug() << "[ReadFile] Appfs = " << ver_string_ << endl;
        }
        file.close();
    }
    return flag;
}

bool AppfsVer::SaveFile(TAppfsVer &version_save)
{
    bool flag = false;

    if(file_path_.size())
    {
        QFile file(file_path_);
        if(!file.open(QFile::ReadWrite | QFile::Truncate))
        {
            qDebug() << "[ReadFile] open file faild : " << file_path_ << endl;
        }
        else
        {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName(encode));
            qDebug() << "[SaveFile] content_ = " << content_ << endl;
            stream << (content_.
                replace("VER_MAJOR=" + QString::number(version_.major_ver), "VER_MAJOR=" + QString::number(version_save.major_ver)).
                replace("VER_MINOR=" + QString::number(version_.minor_ver), "VER_MINOR=" + QString::number(version_save.minor_ver)).
                replace("VER_REV=" + QString::number(version_.rev_ver), "VER_REV=" + QString::number(version_save.rev_ver)).
                replace("VER_SUB=" + QString::number(version_.sub_ver), "VER_SUB=" + QString::number(version_save.sub_ver)));
            stream.flush();
            ver_string_ = QString("%1.%2.%3.%4").arg(version_save.major_ver)
                                                .arg(version_save.minor_ver)
                                                .arg(version_save.rev_ver)
                                                .arg(version_save.sub_ver);
            qDebug() << "[ReadFile] Appfs = " << ver_string_ << endl;
//            qDebug() << "[ReadFile] mk file : " << content_ << endl;
            version_ = version_save;
        }
        file.close();
        flag = true;
    }
    return flag;
}
