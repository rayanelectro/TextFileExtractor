#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

bool CancelLoop=false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    this->setFixedSize(this->geometry().width(),this->geometry().height());


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnBrowse_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,"Open Image", "", "All Files (*.*)");
    if(fileName!="")
        ui->txtFileName->setText(fileName);
}

void MainWindow::on_btnSplit_clicked()
{
    QFile inputFile(ui->txtFileName->text());
    QFile outputFile;
    int HeaderCnt=0,FooterCnt=0;
    int SplitFileNumber=0;
    bool changed=false;
    bool HeaderFound=false;

    CancelLoop=false;

    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QTextStream in(&inputFile);
       QString EOL;

       ui->btnSplit->setEnabled(false);
       ui->btnStop->setEnabled(true);


       if(ui->chkUnixEOL->isChecked())
           EOL="\n";
       else
           EOL="\r\n";

       ui->progressBar->setVisible(true);
       while (!in.atEnd() && CancelLoop==false)
       {
          QString line = in.readLine();

          if(line.startsWith(ui->txtHeader->text()))
          {
              changed=true;
              HeaderCnt++;
              HeaderFound=true;

              SplitFileNumber++;

              if(outputFile.isOpen())outputFile.close();

              QString SplitFileName =ui->txtFileName->text()+ QString("_split%1.txt").arg(SplitFileNumber,5, 10, QChar('0'));
              outputFile.setFileName(SplitFileName);
              outputFile.open(QIODevice::WriteOnly| QIODevice::Text);

          }

          if(HeaderFound==true)
          {
              if(ui->chkIncludeHeaderFooter->isChecked()==false)
              {
                  if(!line.startsWith(ui->txtHeader->text()) && !line.startsWith(ui->txtFooter->text()))
                      outputFile.write((line+EOL).toLocal8Bit().data());
              }
              else
                  outputFile.write((line+EOL).toLocal8Bit().data());

          }

          if(line.startsWith(ui->txtFooter->text()))
          {
              if(outputFile.isOpen())outputFile.close();
              changed=true;
              HeaderFound=false;
              FooterCnt++;
          }
          if(changed==true)
          {
            ui->txtHeaderFound->setText(QString::number(HeaderCnt));
            ui->txtFooterFound->setText(QString::number(FooterCnt));
            qApp->processEvents();
            changed=false;
          }
       }
       inputFile.close();
       ui->progressBar->setVisible(false);
       QMessageBox msgBox;
       msgBox.setText("File extraction finished!");
       msgBox.setIcon(QMessageBox::Information);
       msgBox.exec();
       ui->btnSplit->setEnabled(true);
       ui->btnStop->setEnabled(false);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Input File Error!");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}

void MainWindow::on_MainWindow_destroyed()
{
    qApp->exit();
}

void MainWindow::on_btnStop_clicked()
{
    CancelLoop=true;
    ui->btnSplit->setEnabled(true);
    ui->btnStop->setEnabled(false);

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Text File Extractor v1.0\r\nMorteza Zafari\r\nzafari@gmail.com");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setMinimumWidth(500);
    msgBox.exec();
}
