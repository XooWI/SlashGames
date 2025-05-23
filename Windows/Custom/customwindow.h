#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include <QDialog>
#include <QString>

namespace Ui {
class CustomWindow;
}

class CustomWindow: public QDialog
{
    Q_OBJECT

public:
    enum WindowType {
        DeleteConfirmation,
        UserInfo,
        GeneralInfo
    };

    explicit CustomWindow(WindowType type,
                          const QString& mainText,
                          const QString& windowTitle = QString(),
                          QWidget *parent = nullptr);
    ~CustomWindow();

private slots:
    void on_deleteButton_clicked();
    void on_cancelButton_clicked();
    void on_contunieButton_clicked();


private:
    void setupWindow(WindowType type, const QString& mainText, const QString& windowTitle);

    Ui::CustomWindow *ui;
    WindowType m_windowType;
};

#endif // CUSTOMWINDOW_H
