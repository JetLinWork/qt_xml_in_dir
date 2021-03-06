#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTime>
#include <config_proc.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_About_triggered();
    void on_opendir_triggered();
    void on_pushButtonQuery_clicked();
    void on_pushButtonSet_clicked();

private:
    void SaveAndRefresh(bool ret);
    void FreshHmi();
    void ReadHmi1();
    
private:
    Ui::MainWindow *ui;

    ConfigProc *config_;
};


#endif // MAINWINDOW_H
