#ifndef GAMEINPUTDIALOG_H
#define GAMEINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

namespace Ui {
class GameInputDialog;
}

class GameInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameInputDialog(QWidget *parent = nullptr);
    ~GameInputDialog();
    QString getName();
    QString getIconPath();
    QString getExecutablePath();

private slots:
    void on_browseIconButton_clicked();
    void on_browseFileButton_clicked();
    void on_acceptButton_clicked();
    void on_cancelButton_clicked();

    void on_nameEdit_textChanged(const QString &arg1);

    void on_iconPathEdit_textChanged(const QString &arg1);

    void on_filePathEdit_textChanged(const QString &arg1);

private:

    Ui::GameInputDialog *ui;
};

#endif // GAMEINPUTDIALOG_H
