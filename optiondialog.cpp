#include "optiondialog.h"
#include "ui_optiondialog.h"

OptionDialog::OptionDialog(QWidget *parent, const QString title, const QSize current) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);    
    ui->widthNum->setValue(current.width());
    ui->heightNum->setValue(current.height());
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::on_buttonBox_accepted()
{
    const int widthVal = ui->widthNum->value();
    const int heightVal = ui->heightNum->value();
    if(widthVal > 0 && heightVal > 0){
        emit sizeChanged(QSize(widthVal, heightVal));
    }        
}
