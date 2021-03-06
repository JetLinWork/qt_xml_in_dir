# Tips  
## English  
### how to use  
source file : config_proc.cpp / config_proc.h   
step1. includ the head file : #include <config_proc.h>. Add xml lib to your project at *.pro like this : QT += xml   
step2. new a class in MainWindow class : ConfigProc *config_ = new ConfigProc(this);   
step3. select a dir as working dir : config_->LoadDir(directoryName); class will load each xml file into menmory (DOM XML).    
step4. query or set specified xml tag. don't forget to save file. using interface below:    
 public:    
  bool LoadDir(const QString dir);    
  bool FindElement(const QString &file, const QString &path, QDomElement &target_element);    
  QString QueryElement(const QString &file, const QString &path, const QString attr = "value");    
  bool SetElement(const QString &file, const QString &path, const QString value = "", const QString attr = "value");    
  bool RemoveElement(const QString &file, const QString &path);    
  bool RemoveElementAttr(const QString &file, const QString &path, const QString attr = "value");    
  bool AddElement(const QString &file, const QString &path, const QString &name);    
  bool SaveXML(const QString &file);    
  
the file will read and write in "GB2312", if you wanna change it. modify [ static const char encode[] = "GB2312"; ] in config_proc.h   
there is a demo shown in mainwindow.cpp/mainwindow.h   
you can even run it.  

QT pack  
windows:  
install Qt Creator->Generate exe executable file -> copy exe file to another dir -> windeployqt package dynamic library needed, -> using enigma_Virtual_Box pack to one exe
linux:  
install Qt Creator->Generate exe executable file -> copy exe file to another dir -> linuxdeployqt package dynamic library needed, -> pack dep/rpm

## 中文  
### 使用  
参见 bolg   