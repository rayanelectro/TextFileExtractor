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
    this->setFixedSize(this->geometry().width(),this->geometry().height()); //Make un-resizable


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnBrowse_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,"Open Image", "", "All Files (*.*)");
    fileName = QDir::toNativeSeparators(fileName);

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
    QString NewFileName,OldFileName;
    QString FilePath;

    CancelLoop=false;

    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) //If file selected is opened correctly.
    {
       QTextStream in(&inputFile);
       QString EOL;

       ui->btnSplit->setEnabled(false);
       ui->btnStop->setEnabled(true);


       if(ui->chkUnixEOL->isChecked()) //Based on the checkbox select LF or CR-LF for the end of line charcter.
           EOL="\n";
       else
           EOL="\r\n";

       FilePath=ui->txtFileName->text().section("\\",0,-2);


       ui->progressBar->setVisible(true);
       ui->groupBoxFileName->setEnabled(false);

       while (!in.atEnd() && CancelLoop==false) //Loop untill we reach the end of file, or the user cancels the operation.
       {
          QString line = in.readLine(); //Read one line at a time

          if(line.startsWith(ui->txtHeader->text()))
          {
              changed=true;
              HeaderCnt++;
              HeaderFound=true;

              SplitFileNumber++;

              if(outputFile.isOpen())outputFile.close(); //If by any chance the file hase laready been open, close it.

              OldFileName =ui->txtFileName->text()+ QString("_split%1.txt").arg(SplitFileNumber,5, 10, QChar('0')); //Make a new numbered file.
              outputFile.setFileName(OldFileName);
              outputFile.open(QIODevice::WriteOnly| QIODevice::Text); //Open the file for writing.


          }


          if(ui->radFileNameFromFile->isChecked())
          {
              if(line.contains(ui->txtFileNameTextTemplate->text()))
              {
                  QString lineText=line;
                  NewFileName=lineText.replace(ui->txtFileNameTextTemplate->text(),"").trimmed()+".txt";
              }
          }


          if(HeaderFound==true)
          {
              if(ui->chkIncludeHeaderFooter->isChecked()==false) //Add header and footer only if checkbox is selected by the user
              {
                  if(!line.startsWith(ui->txtHeader->text()) && !line.startsWith(ui->txtFooter->text()))
                      outputFile.write((line+EOL).toLocal8Bit().data());
              }
              else
                  outputFile.write((line+EOL).toLocal8Bit().data());

          }

          if(line.startsWith(ui->txtFooter->text()))
          {
              if(outputFile.isOpen())
              {
                  outputFile.close();
                  if(NewFileName!="")
                    QFile::rename(OldFileName,FilePath+"\\"+NewFileName);
                  NewFileName="";
              }
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
       ui->groupBoxFileName->setEnabled(true);

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
    msgBox.setText("Text File Extractor v1.2\r\nMorteza Zafari\r\nzafari@gmail.com");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setMinimumWidth(500);
    msgBox.exec();
}
