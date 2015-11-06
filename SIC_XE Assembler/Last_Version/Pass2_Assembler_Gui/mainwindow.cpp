/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"
#include "QTextStream"
#include "Conversions.h"
#include "SICXEPass1.h"
#include <QtWidgets>
#include "mainwindow.h"
#include "textedit.h"
#include <QToolBar>
#include "SICXEPass2.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    statusBar()->showMessage("Start Coding...");
    setupFileMenu();//2e2felooooooo
    setupHelpMenu();
    setupEditor();
    setupAssemblingMenu();
    free = false;
   // setCentralWidget(editor);
    setWindowTitle(tr("SIC/XE Assembler"));
    //completingTextEdit = new TextEdit;
//   QToolBar *toolbar = new QToolBar();
//        addToolBar(toolbar);
//        QAction* check = new QAction(this);
//        check->setCheckable(true);
//        check->setIconText("Free Format");
//        check->connect(check,SIGNAL(changed()),this,SLOT(setFreeFormat()));
//        toolbar->addAction(check);
    completer = new QCompleter(this);
    completer->setModel(modelFromFile("wordlist.txt"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    completingTextEdit->setCompleter(completer);
    setCentralWidget(completingTextEdit);
    resize(1200, 800);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SIC/XE Assembler"),
                tr("<p>The <b>SIC/XE Assembler</b> is used to assemble SIC/XE architecture assembly code.</p>" \
                   "<p>Implemented by</p>" \
                   "<p><b>Ramy Wagdy Sobhy</b></p>"\
                   "<p><b>Rofael Emil Fayez</b></p>"\
                   "<p><b>Remon Hanna Wadie</b></p>"\
                   "<p><b>Mohamed Abd ELRahman ElFeki</b></p>"\
                   "<p><b>Mohamed Ahmed Taher Mohamed</b></p>"));
}

void MainWindow::newFile()
{
    completingTextEdit->clear();
}

void MainWindow::openFile(const QString &path)
{
    QString fileName = path;
    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), "", "Text Files (*.txt)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
            completingTextEdit->setPlainText(file.readAll());
    }
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(15);

    completingTextEdit = new TextEdit;
    completingTextEdit->setFont(font);

    highlighter = new Highlighter(completingTextEdit->document());

    QFile file("mainwindow.h");
    if (file.open(QFile::ReadOnly | QFile::Text))
        completingTextEdit->setPlainText(file.readAll());
}

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);

    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
                        QKeySequence::New);

    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
                        QKeySequence::Open);
    fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()),
                        QKeySequence::Save);


    fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()),
                        QKeySequence::Quit);

}


void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Info"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, SLOT(about()));
}

void MainWindow::setupAssemblingMenu()
{
    QMenu *assemble =new QMenu(tr("&Assembling"),this);
    menuBar()->addMenu(assemble);
    assemble->addAction(tr("Assemble"),this,SLOT(assemble()));
    assemble->addAction(tr("List File"),this,SLOT(openListFile()));
    assemble->addAction(tr("Object File"),this,SLOT(openObjectFile()));
}

void MainWindow::openListFile()
{
    QFile myFile("Intermediate_File.txt");
        myFile.open(QIODevice::ReadOnly);
        QTextStream textStream(&myFile);
        completingTextEdit->setText(textStream.readAll());
        myFile.close();
}

void MainWindow::openObjectFile()
{
    QFile myFile("OBJFILE.o");
        myFile.open(QIODevice::ReadOnly);
        QTextStream textStream(&myFile);
        completingTextEdit->setText(textStream.readAll());
        myFile.close();
}

void MainWindow::assemble()
{
//      parent()->statusBar->showMessage("Assembled");
      QString text=completingTextEdit->toPlainText();
      QFile inputFile("input.txt");
      inputFile.open(QIODevice::WriteOnly);
      if(!inputFile.isOpen()){
          QMessageBox::information(0,"error",inputFile.errorString());
      }else{
          QTextStream outStream(&inputFile);
          outStream << text;
          inputFile.close();
          Conversions * conve = new Conversions();
          try{
          SICXEPass1 *pass1=new SICXEPass1("input.txt");
          if(!pass1->pass1())
              statusBar()->showMessage("Incomplete Assembly...");
          else statusBar()->showMessage("Complete Assembly...");

          }catch(exception e){
              statusBar()->showMessage("Incomplete Assembly...");
          }
          SICXEPass2 * pass_2 = new SICXEPass2("input.txt");
          pass_2->pass2();

      }
}



void MainWindow::saveFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getSaveFileName(this,
            tr("Save"), "", "Text Files (*.txt)");

        QFile file(fileName);
        file.open(QIODevice::ReadWrite);
        QTextStream stream(&file);
               stream << completingTextEdit->toPlainText() << endl;

}

void MainWindow::setFreeFormat(){
    if(free)
    {
        free= false;
    }
    else {
        free = true;
    }
}

QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}

