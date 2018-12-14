#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = 0, const QString title = QString("Dialog"), const QSize current = QSize(400, 400));
    ~OptionDialog();

signals:
    void sizeChanged(const QSize size);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::OptionDialog *ui;
};

#endif // OPTIONDIALOG_H
