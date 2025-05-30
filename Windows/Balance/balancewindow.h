#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include <QDialog>
#include <QMap> // Используем QMap для хранения BIN данных
#include <QRegularExpression> // Для валидации
#include <QIntValidator>
#include <QUrl>
#include <QButtonGroup>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTimer>
#include <QClipboard>
#include <QDebug> // Для отладки

#include "Style/customstyle.h"
#include "Windows/Custom/customwindow.h"


namespace Ui {
class BalanceWindow;
}

// Структура для хранения информации о BIN
struct BinInfo {
    QString bin;
    QString issuer;
};

// New struct for phone carrier info
struct PhoneCarrierInfo {
    QString code;
    QString brand;
};

class BalanceWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BalanceWindow(QWidget *parent = nullptr);
    ~BalanceWindow();

private slots:
    void on_copyAddressButton_clicked();
    void on_cardMethodButton_toggled(bool checked);
    void on_phoneMethodButton_toggled(bool checked);
    void on_cryptoMethodButton_toggled(bool checked);
    void on_inputField_textChanged(const QString &text);
    void on_withdrawButton_clicked();
    void on_qrCodeButton_clicked();
    void on_amountLineEdit_textChanged(const QString &text); // Слот для суммы

private:
    Ui::BalanceWindow *ui;
    QMap<QString, BinInfo> binData;
    void loadBinData();

    QMap<QString, PhoneCarrierInfo> phoneCarrierData;
    void loadPhoneCarrierData();

    // Переменные для хранения стилей по умолчанию
    QString defaultCardMaskPlaceholder = "0000 0000 0000 0000";
    QString defaultPhoneMaskPlaceholder = "+7 (000) 000-00-00";

    const QString CryptoPath = "https://github.com/MrAnonim114";
    const QString CryptoAdressee = "0x12174343747473387hdjhdry7uhurhftt5566656b";


    void processCardInput(const QString &text, int originalCursorPos);
    void processPhoneInput(const QString &text, int originalCursorPos);
    void processCryptoInput(const QString &text);

    int calculateCleanTextCursorPos(const QString &currentText, const QString &cleanText, int originalCursorPos);
    int mapCleanTextCursorPosToFormattedText(const QString &formattedText, int cleanTextCursorPos);

    void updateBrandInfoLabel(const QString &cleanText);
    void updateCryptoAddressInfo(const QString &address);
};

#endif // BALANCEWINDOW_H

