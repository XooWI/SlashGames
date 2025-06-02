#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include <QIntValidator>
#include <QUrl>
#include <QButtonGroup>
#include <QFile>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTimer>
#include <QClipboard>
#include <QDebug>

#include "Style/customstyle.h"
#include "Windows/Custom/customwindow.h"
#include "Windows/Main/mainwindow.h"


namespace Ui {
class BalanceWindow;
}

struct BinInfo {
    QString bin;
    QString nameBank;
};

struct PhoneCarrierInfo {
    QString code;
    QString brand;
};

class BalanceWindow : public QDialog
{
    Q_OBJECT

public:
    BalanceWindow(QWidget *parent = nullptr, DatabaseManagement *dbManager= nullptr);
    ~BalanceWindow();

private slots:
    void on_cardMethodButton_toggled(bool checked);
    void on_phoneMethodButton_toggled(bool checked);
    void on_cryptoMethodButton_toggled(bool checked);

    void on_inputField_textChanged(const QString &text);
    void on_amountLineEdit_textChanged(const QString &text);

    void on_withdrawButton_clicked();
    void on_copyAddressButton_clicked();
    void on_qrCodeButton_clicked();


private:
    Ui::BalanceWindow *ui;
    DatabaseManagement *dbManager;

    QMap<QString, BinInfo> binData;
    void loadBinData();

    QMap<QString, PhoneCarrierInfo> phoneCarrierData;
    void loadPhoneCarrierData();

    void processCardInput(const QString &text, int originalCursorPos);
    void processPhoneInput(const QString &text, int originalCursorPos);
    void processCryptoInput(const QString &text);

    int calculateCleanTextCursorPos(const QString &currentText, const QString &cleanText, int originalCursorPos);
    int mapCleanTextCursorPosToFormattedText(const QString &formattedText, int cleanTextCursorPos);

    void updateBrandInfoLabel(const QString &cleanText);
    void updateCryptoAddressInfo(const QString &address);


    // Вспомогательные методы для обработки ввода суммы
    int calculateCleanCursorPosition(const QString &originalText, int originalPos, const QString &cleanText);

    // Переменные для хранения стилей по умолчанию
    QString defaultCardMaskPlaceholder = "0000 0000 0000 0000";
    QString defaultPhoneMaskPlaceholder = "+7 (000) 000-00-00";

    const QString CryptoPath = "https://github.com/MrAnonim114";
    const QString CryptoAdressee = "0x742d35Cc6634C05329C3aE2bE4C6BfB8CD132Fsd";
};

#endif // BALANCEWINDOW_H

