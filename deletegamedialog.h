#ifndef DELETEGAMEDIALOG_H
#define DELETEGAMEDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteGameDialog;
}

class DeleteGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteGameDialog(QWidget *parent = nullptr);
    ~DeleteGameDialog();

private slots:
    void on_deleteButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::DeleteGameDialog *ui;
    QString gameName;
};

#endif // DELETEGAMEDIALOG_H
