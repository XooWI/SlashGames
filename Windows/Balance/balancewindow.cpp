#include "balancewindow.h"
#include "ui_balancewindow.h"


BalanceWindow::BalanceWindow(QWidget *parent, DatabaseManagement *dbManager) :
    QDialog(parent),
    ui(new Ui::BalanceWindow),
    dbManager(dbManager)
{
    ui->setupUi(this);

    QButtonGroup *withdrawalMethodGroup = new QButtonGroup(this);
    withdrawalMethodGroup->addButton(ui->cardMethodButton);
    withdrawalMethodGroup->addButton(ui->phoneMethodButton);
    withdrawalMethodGroup->addButton(ui->cryptoMethodButton);
    withdrawalMethodGroup->setExclusive(true);

    QIntValidator *amountValidator = new QIntValidator(0, 1000000000, this);
    ui->amountLineEdit->setValidator(amountValidator);
    ui->cryptoAddressLabel->setText(CryptoAdressee);

    loadBinData();
    loadPhoneCarrierData();

}

BalanceWindow::~BalanceWindow()
{
    delete ui;
}

void BalanceWindow::loadBinData()
{
    QFile file(":/data/bin_code.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл BIN данных";
        return;
    }
    QTextStream in(&file);
    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() >= 2) {
            BinInfo info;
            info.bin = fields[0].trimmed();
            info.nameBank = fields[1].trimmed();
            binData[info.bin] = info;

        }
    }
    file.close();
}

void BalanceWindow::loadPhoneCarrierData()
{
    QFile file(":/data/phone_code.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл данных операторов связи";
        return;
    }
    QTextStream in(&file);
    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() >= 2) {
            PhoneCarrierInfo info;
            info.code = fields[0].trimmed();
            info.brand = fields[1].trimmed();
            phoneCarrierData[info.code] = info;
        }
    }
    file.close();
}

void BalanceWindow::on_copyAddressButton_clicked()
{
    QApplication::clipboard()->setText(CryptoAdressee);

    ui->copyAddressButton->setText("✓ Скопировано");
    ui->copyAddressButton->setStyleSheet(CustomStyle::activeCopyAddressButton());

    QTimer::singleShot(2000, this, [this]() {
        ui->copyAddressButton->setText("Копировать");
        ui->copyAddressButton->setStyleSheet(CustomStyle::defaultCopyAddressButton());
    });
}


void BalanceWindow::on_inputField_textChanged(const QString &text)
{
    int originalCursorPos = ui->inputField->cursorPosition();
    ui->inputField->blockSignals(true);

    if (ui->cardMethodButton->isChecked()) {
        processCardInput(text, originalCursorPos);
    } else if (ui->phoneMethodButton->isChecked()) {
        processPhoneInput(text, originalCursorPos);
    } else if (ui->cryptoMethodButton->isChecked()) {
        processCryptoInput(text);
    }

    ui->inputField->blockSignals(false);
}

void BalanceWindow::on_cardMethodButton_toggled(bool checked)
{
    if (checked) {
        ui->inputLabel->setText("Номер карты:");
        ui->inputField->clear();
        ui->brandInfoLabel->clear();
        ui->commissionSizeLabel->setText("10%");
        ui->commissionSizeLabel->setStyleSheet("color: #FF5722;");
        ui->inputField->setPlaceholderText(defaultCardMaskPlaceholder);

    }
}

void BalanceWindow::on_phoneMethodButton_toggled(bool checked)
{
    if (checked) {
        ui->inputLabel->setText("Номер телефона:");
        ui->inputField->clear();
        ui->brandInfoLabel->clear();
        ui->commissionSizeLabel->setText("5%");
        ui->commissionSizeLabel->setStyleSheet("color: #FF5722;;");
        ui->inputField->setPlaceholderText(defaultPhoneMaskPlaceholder);
    }
}

void BalanceWindow::on_cryptoMethodButton_toggled(bool checked)
{
    if (checked) {
        ui->inputLabel->setText("Адрес криптокошелька:");
        ui->inputField->clear();
        ui->brandInfoLabel->clear();
        ui->commissionSizeLabel->setText("0%");
        ui->commissionSizeLabel->setStyleSheet("color: #4CAF50;");
        ui->inputField->setPlaceholderText("0x...");
        ui->inputField->setInputMask("");
    }
}



void BalanceWindow::processCardInput(const QString &text, int originalCursorPos)
{
    QString cleanText = text;
    cleanText.remove(QRegularExpression("[^0-9]"));
    if (cleanText.length() > 16) {
        cleanText = cleanText.left(16);
    }

    int cleanTextCursorPos = calculateCleanTextCursorPos(text, cleanText, originalCursorPos);

    QString formattedText;
    for (int i = 0; i < cleanText.length(); ++i) {
        if (i > 0 && i % 4 == 0) {
            formattedText += " ";
        }
        formattedText += cleanText.at(i);
    }

    int newCursorPos = mapCleanTextCursorPosToFormattedText(formattedText, cleanTextCursorPos);

    ui->inputField->setText(formattedText);
    ui->inputField->setCursorPosition(qMin(newCursorPos, formattedText.length()));
    updateBrandInfoLabel(cleanText);
}

void BalanceWindow::processPhoneInput(const QString &text, int originalCursorPos)
{
    QString cleanText = text;
    cleanText.remove(QRegularExpression("[^0-9]"));

    if (!cleanText.isEmpty() && cleanText[0] == '8') {
        cleanText[0] = '7';
    }

    if (cleanText == "9" && text.length() == 1) {
        ui->inputField->setText("+7 (9");
        ui->inputField->setCursorPosition(5);
        return;
    }

    cleanText = cleanText.left(11);

    int cleanTextCursorPos = calculateCleanTextCursorPos(text, cleanText, originalCursorPos);

    QString formattedText;
    if (!cleanText.isEmpty()) {
        formattedText = "+7";

        if (cleanText.length() > 1) {
            formattedText += " (" + cleanText.mid(1, 3);
        }
        if (cleanText.length() > 4) {
            formattedText += ") " + cleanText.mid(4, 3);
        }
        if (cleanText.length() > 7) {
            formattedText += "-" + cleanText.mid(7, 2);
        }
        if (cleanText.length() > 9) {
            formattedText += "-" + cleanText.mid(9, 2);
        }
    }

    int newCursorPos = mapCleanTextCursorPosToFormattedText(formattedText, cleanTextCursorPos);

    ui->inputField->setText(formattedText);
    ui->inputField->setCursorPosition(qMin(newCursorPos, formattedText.length()));

    updateBrandInfoLabel(cleanText);
}

void BalanceWindow::processCryptoInput(const QString &text)
{
    QString address = text.trimmed();
    ui->inputField->setText(address);
    ui->inputField->setPlaceholderText("0x...");
    ui->inputField->setInputMask("");
    updateCryptoAddressInfo(address);

}


int BalanceWindow::calculateCleanTextCursorPos(const QString &currentText, const QString &cleanText, int originalCursorPos)
{
    int cleanTextCursorPos = 0;
    for (int i = 0; i < originalCursorPos && i < currentText.length(); ++i) {
        if (currentText.at(i).isDigit()) {
            cleanTextCursorPos++;
        }
    }
    return qMin(cleanTextCursorPos, cleanText.length());
}

int BalanceWindow::mapCleanTextCursorPosToFormattedText(const QString &formattedText, int cleanTextCursorPos)
{
    int newCursorPos = 0;
    int digitsInFormattedText = 0;
    for (int i = 0; i < formattedText.length(); ++i) {
        if (formattedText.at(i).isDigit()) {
            digitsInFormattedText++;
        }
        if (digitsInFormattedText == cleanTextCursorPos) {
            newCursorPos = i + 1;
            break;
        }
    }
    if (cleanTextCursorPos == formattedText.count(QRegularExpression("[0-9]"))) {
        newCursorPos = formattedText.length();
    }
    return newCursorPos;
}

void BalanceWindow::updateBrandInfoLabel(const QString &cleanText)
{
    if (ui->cardMethodButton->isChecked()) {
        if (cleanText.length() >= 6) {
            QString bin = cleanText.left(6);
            if (binData.contains(bin)) {
                BinInfo info = binData[bin];
                if (bin.startsWith("22")) {
                    ui->brandInfoLabel->setText(QString("МИР %1").arg(info.nameBank));
                    ui->brandInfoLabel->setStyleSheet("color: #4CAF50; ");
                }
            } else {
                ui->brandInfoLabel->setText("Банк не определен");
                ui->brandInfoLabel->setStyleSheet("color: #FF5722; ");
            }
        } else if (cleanText.length() >= 2 && cleanText.startsWith("22")) {
            ui->brandInfoLabel->setText("МИР");
            ui->brandInfoLabel->setStyleSheet("color: #4CAF50; ");
        }
        else {
            ui->brandInfoLabel->clear();
        }
    } else if (ui->phoneMethodButton->isChecked()) {
        if (cleanText.length() >= 4) {
            QString code = cleanText.mid(1, 3);
            if (phoneCarrierData.contains(code)) {
                PhoneCarrierInfo info = phoneCarrierData[code];
                ui->brandInfoLabel->setText(QString("%1").arg(info.brand));
                ui->brandInfoLabel->setStyleSheet("color: #4CAF50; ");
            } else {
                ui->brandInfoLabel->setText("Оператор не определен");
                ui->brandInfoLabel->setStyleSheet("color: #FF5722; ");
            }
        } else {
            ui->brandInfoLabel->clear();
        }
    }
}

void BalanceWindow::updateCryptoAddressInfo(const QString &address)
{
    QString walletType = "Неизвестный тип";
    QColor textColor = Qt::red;

    if (address.startsWith("0x")) {
            walletType = "Ethereum";
            textColor = Qt::darkGreen;
        }
    else if (address.startsWith("1") || address.startsWith("3") || address.startsWith("bc1")) {
            walletType = "Bitcoin";
            textColor = Qt::darkGreen;
    } else if (address.startsWith("T")) {
        walletType = "Tron (TRX)";
        textColor = Qt::darkGreen;
    } else if (address.startsWith("r")) {
        walletType = "Ripple (XRP)";
        textColor = Qt::darkGreen;
    }

    if (address.isEmpty()) {
        ui->brandInfoLabel->setText("");
        ui->brandInfoLabel->setStyleSheet("");
    } else {
        ui->brandInfoLabel->setText(walletType);
        ui->brandInfoLabel->setStyleSheet(QString("color: %1;").arg(textColor.name()));
    }
}


void BalanceWindow::on_withdrawButton_clicked()
{
    const QString formattedRecipient = ui->inputField->text();
    QString cleanRecipient = formattedRecipient;
    cleanRecipient.remove(QRegularExpression("[^0-9a-fA-F]"));

    const QString amountText = ui->amountLineEdit->text().remove(QRegularExpression("[^0-9]"));
    const int amount = amountText.toInt();

    if (formattedRecipient.isEmpty()) {
        QMessageBox::warning(this, "Ошибка вывода", "Пожалуйста, введите данные для вывода.");
        return;
    }
    if (amount < 100) {
        QMessageBox::warning(this, "Ошибка вывода", "Минимальная сумма вывода - 100$");
        return;
    }

    if (amount <= 0) {
        QMessageBox::warning(this, "Ошибка вывода", "Пожалуйста, введите корректную сумму вывода.");
        return;
    }

    if (dbManager->getBalance() < amount){
        QMessageBox::warning(this, "Ошибка вывода", "Недостаточно средств!");
        return;
    }

    bool isValidInput = false;
    double commissionRate = 0.0;
    QString validationErrorMessage = "";
    QString operatorInfo = ui->brandInfoLabel->text();
    QString recipientLabel;
    QString operatorBankPSTitle;

    if (ui->cardMethodButton->isChecked()) {
        isValidInput = cleanRecipient.length() == 16;
        validationErrorMessage = "Номер карты должен содержать 16 цифр.";
        commissionRate = 0.10;
        recipientLabel = "Номер карты";
        operatorBankPSTitle = "Банк";

    } else if (ui->phoneMethodButton->isChecked()) {
        QString validationCleanRecipient = cleanRecipient;
        if (validationCleanRecipient.startsWith('8')) {
            validationCleanRecipient[0] = '7';
        }
        isValidInput = validationCleanRecipient.length() == 11 && validationCleanRecipient.startsWith('7');
        validationErrorMessage = "Номер телефона должен содержать 11 цифр и начинаться с 7";
        commissionRate = 0.05;
        recipientLabel = "Номер телефона";
        operatorBankPSTitle = "Оператор";

    } else if (ui->cryptoMethodButton->isChecked()) {
        isValidInput = true;
        commissionRate = 0.00;
        recipientLabel = "Адрес кошелька";
        operatorBankPSTitle = "Тип кошелька";
    }

    if (!isValidInput) {
        QMessageBox::warning(this, "Ошибка ввода", validationErrorMessage);
        return;
    }

    const double commissionAmount = qRound(amount * commissionRate);
    const double totalAmount = amount + commissionAmount;

    if (dbManager->getBalance() < totalAmount){
        QMessageBox::warning(this, "Ошибка вывода", "Недостаточно средств для оплаты комиссии!");
        return;
    }

    QString displayAmount = QString::number(amount);
    QString displayCommissionAmount = QString::number(commissionAmount, 'f', 2);
    QString displayTotalAmount = QString::number(totalAmount, 'f', 2);

    QLocale locale(QLocale::Russian);
    displayAmount = locale.toString(amount);
    displayCommissionAmount = locale.toString(commissionAmount, 'f', 2);
    displayTotalAmount = locale.toString(totalAmount, 'f', 2);

    const QString confirmationHtml = CustomStyle::confirmationPayWindow().arg(formattedRecipient, operatorInfo, displayAmount,
            QString::number(commissionRate * 100, 'f', 0), displayCommissionAmount,
            displayTotalAmount, recipientLabel, operatorBankPSTitle);

    CustomWindow confirmationDialog(CustomWindow::ConfirmationPay, confirmationHtml, "Подтверждение вывода", this);

    if (confirmationDialog.exec() == QDialog::Accepted) {
        const QString receiptHtml = CustomStyle::checkPayWindow().arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"), formattedRecipient, operatorInfo,
                displayAmount, QString::number(commissionRate * 100, 'f', 0),
                displayCommissionAmount, displayTotalAmount,
                recipientLabel, operatorBankPSTitle);
        if (dbManager->getBalance() >= totalAmount){
            MainWindow *mainWin = qobject_cast<MainWindow *>(parent());
            mainWin->updateBalance(-totalAmount);
        } else{
            QMessageBox::warning(this, "Ошибка вывода", "Недостаточно средств!");
            return;
        }

        CustomWindow receiptDialog(CustomWindow::GeneralInfo, receiptHtml, "Чек об операции", this);
        receiptDialog.exec();

        ui->inputField->clear();
        ui->amountLineEdit->clear();
        ui->brandInfoLabel->clear();
    }
}

void BalanceWindow::on_amountLineEdit_textChanged(const QString &text)
{
    const int cursorPos = ui->amountLineEdit->cursorPosition();

    QString cleanText = text;
    cleanText.remove(QRegularExpression("[^0-9]"));

    if (cleanText.length() > 1) {
        cleanText.remove(QRegularExpression("^0+"));
    }

    QLocale locale(QLocale::Russian);
    QString formatted = locale.toString(cleanText.toInt());

    ui->amountLineEdit->setText(formatted);

    int newPos = calculateCleanCursorPosition(text, cursorPos, formatted);
    ui->amountLineEdit->setCursorPosition(newPos);

}

int BalanceWindow::calculateCleanCursorPosition(const QString &oldText,
                                           int oldPos,
                                           const QString &newText)
{
    int digitsBefore = 0;
    for (int i = 0; i < oldPos; ++i) {
        if (oldText[i].isDigit()) digitsBefore++;
    }

    int newPos = 0;
    int digitsCounted = 0;
    for (; newPos < newText.length(); ++newPos) {
        if (newText[newPos].isDigit()) {
            digitsCounted++;
            if (digitsCounted > digitsBefore) break;
        }
    }

    return newPos;
}


void BalanceWindow::on_qrCodeButton_clicked()
{
    QDesktopServices::openUrl(QUrl(CryptoPath));
}
