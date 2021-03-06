#include "mainwindow.h"
#include "ui_mainwindow.h"

static QString empty_prompt = "无";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    config_(nullptr)
{
    ui->setupUi(this);
    config_ = new ConfigProc(this);

    if(!config_)
    {
        qDebug() << "config_ is null! program will not run!!!" << endl;
        exit(-1);
    }

    //  QDateTime time_tick=QDateTime::currentDateTime();
    //  QDateTime now;
    //  do{
    //  now=QDateTime::currentDateTime();
    //  } while (time_tick.secsTo(now)<=3);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_About_triggered()
{
    QString about = "author : linyp 1025339984@qq.com\r\ncompile time : " + QString(__DATE__) + " " + QString(__TIME__);
    QMessageBox::about(this, "关于", about);
//    ui->comboBoxGuiPageNum->setCurrentIndex(5);
}

void MainWindow::on_opendir_triggered()
{
    const QString directoryName = QFileDialog::getExistingDirectory(this);

    ui->main_info->setText("配置文件路径：" + directoryName);

    config_->LoadDir(directoryName);

    FreshHmi();
}

void MainWindow::on_pushButtonQuery_clicked()
{
    qDebug() << "on_pushButtonQuery_clicked" << endl;
    SaveAndRefresh(config_->SaveXML((const QString)ui->lineEditQueryfile->text()));
}

void MainWindow::on_pushButtonSet_clicked()
{
    qDebug() << "on_pushButtonSet_clicked" << endl;
    ReadHmi1();
    SaveAndRefresh(config_->SaveXML((const QString)ui->lineEditSetFile->text()));
}

void MainWindow::SaveAndRefresh(bool ret)
{
    if(ret)
        QMessageBox::information(this, "提示", "操作成功");
    else
        QMessageBox::warning(this, "警告", "失败");

    FreshHmi();
}

void MainWindow::FreshHmi()
{
    ui->labelResult->setText(config_->QueryElement( ui->lineEditQueryfile->text()
                                                   ,ui->lineEditQueryPath->text()
                                                   ,ui->lineEditQueryAttr->text()));
}

void MainWindow::ReadHmi1()
{
    config_->SetElement( ui->lineEditSetFile->text()
                         ,ui->lineEditSetPath->text()
                         ,ui->lineEditSetVal->text()
                         ,ui->lineEditSetAttr->text());
}

