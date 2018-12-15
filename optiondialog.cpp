#include "optiondialog.h"
#include "ui_optiondialog.h"

/**
 * @brief OptionDialog::OptionDialog
 * @param parent
 * @param title
 * @param current
 */
OptionDialog::OptionDialog(QWidget *parent, const QString title, const QSize current) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);    
    ui->widthNum->setValue(current.width());
    ui->heightNum->setValue(current.height());
}

/**
 * @brief OptionDialog::~OptionDialog
 */
OptionDialog::~OptionDialog()
{
    delete ui;
}

/**
 * @brief OptionDialog::on_buttonBox_accepted
 */
void OptionDialog::on_buttonBox_accepted()
{
    const int widthVal = ui->widthNum->value();
    const int heightVal = ui->heightNum->value();
    if(widthVal > 0 && heightVal > 0){
        emit sizeChanged(QSize(widthVal, heightVal));
    }        
}
