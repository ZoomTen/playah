#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include <playahcore.h>

AboutWindow::AboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    ui->guiVersion->setText(tr("GUI version: %1").arg(QApplication::applicationVersion()));
    ui->libVersion->setText(tr("Library version: %1").arg(PlayahCore::applicationVersion()));
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::on_okButton_clicked()
{
    this->close();
}
