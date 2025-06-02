#ifndef GAMEINPUTDIALOG_H
#define GAMEINPUTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QLineEdit>

#include "Windows/Custom/customwindow.h"


namespace Ui {
class GameInputDialog;
}

class GameInputDialog : public QDialog
{
    Q_OBJECT

public:
    GameInputDialog(bool isEditMode = false, QWidget *parent = nullptr);
    ~GameInputDialog();

    QString getName();
    QString getIconPath();
    QString getFilePath();

    void setName(const QString& name);
    void setIconPath(const QString& path);
    void setFilePath(const QString& path);

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

    const int MIN_WIDTH_ICON = 100;
    const int MIN_HEIGHT_ICON = 100;

};

#endif // GAMEINPUTDIALOG_H
