#include "customstyle.h"

QString CustomStyle::getLightThemeStyle() {
    return R"(
            QMainWindow { background-color: #ffffff; color: #000000; }
            QDialog { background-color: #ffffff; color: #000000; }
            QLabel { color: #000000; }
            #balanceLabel { color: #000000; }
            QTextEdit { background-color: #ffffff; color: #000000; border: 1px solid #cccccc; }

            QTabWidget { background-color: #ffffff; color: #000000; border: none; }
            QTabWidget QTextEdit { background-color: #ffffff; color: #000000; }

            QTabWidget::pane {
                border: 1px solid #e0e0e0;
                background: #ffffff;
                border-top-left-radius: 0px;
                border-top-right-radius: 8px;
                border-bottom-right-radius: 8px;
                border-bottom-left-radius: 8px;
            }

            QTabBar::tab {
                background: #f0f0f0;
                color: #000000;
                border: 1px solid #e0e0e0;
                border-bottom: none;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                padding: 8px 16px;
                margin-right: 5px;
            }

            QTabBar::tab:selected {
                background: #ffffff;
            }

            QTabBar::tab:hover {
                background: #e0e0e0;
            }

            QScrollArea {
                background-color: #ffffff;
                border: none;
            }

            QScrollArea QWidget {
                background-color: #ffffff;
            }

            QScrollArea QScrollBar:vertical {
                width: 14px;
                border-radius: 7px;
                margin: 0px;
            }

            QScrollArea QScrollBar::handle:vertical {
                background-color: #cccccc;
                min-height: 30px;
                border-radius: 7px;
                margin: 2px 0;
            }

            QScrollArea QScrollBar::handle:vertical:hover {
                background-color: #bbbbbb;
            }

            QScrollArea QScrollBar::add-page:vertical,
            QScrollArea QScrollBar::sub-page:vertical {
                background: transparent;
            }
            QScrollArea QScrollBar::add-line:vertical,
            QScrollArea QScrollBar::sub-line:vertical {
                height: 0px;
            }

            QLineEdit {
                border: 1px solid dark-gray;
                background: white;
                color: dark-gray
            }


            QMenu {
                background-color: #ffffff;
                border: 1px solid #e0e0e0;
            }

            QMenu::item {
                padding: 8px 16px 8px 16px;
                color: #000000;
                background-color: transparent;
            }

            QMenu::item:selected {
                background-color: #f0f0f0;
                border-radius: 4px;
            }

            QMenu::icon {
                margin-left: 8px;
            }
            )";
}


QString CustomStyle::getDarkThemeStyle() {
    return R"(
            QMainWindow { background-color: #1e1e1e; color: #ffffff; }
            QDialog { background-color: #1e1e1e; color: #ffffff; }
            QLabel { color: #ffffff; }
            #balanceLabel { color: #ffffff; }
            QTextEdit { background-color: #1e1e1e; color: #ffffff; border: 1px solid #4d4d4d; }

            QTabWidget { background-color: #1e1e1e; color: #ffffff; border: none; }
            QTabWidget QTextEdit { background-color: #1e1e1e; color: #ffffff; }


            QTabWidget::pane {
                border: 1px solid #4d4d4d;
                background: #1e1e1e;
                border-top-left-radius: 0px;
                border-top-right-radius: 8px;
                border-bottom-right-radius: 8px;
                border-bottom-left-radius: 8px;
            }

            QTabBar::tab {
                background: #3d3d3d;
                color: #ffffff;
                border: 1px solid #4d4d4d;
                border-bottom: none;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                padding: 8px 16px;
                margin-right: 5px;
            }

            QTabBar::tab:selected {
                background: #4d4d4d;
            }

            QTabBar::tab:hover {
                background: #5d5d5d;
            }


            QScrollArea {
                background-color: #1e1e1e;
                border: none;
            }

            QScrollArea QWidget {
                background-color: #1e1e1e;
            }

            QScrollArea QScrollBar:vertical {
                width: 14px;
                margin: 0px;
                border-radius: 7px;
            }

            QScrollArea QScrollBar::handle:vertical {
                background-color: #4d4d4d;
                min-height: 30px;
                border-radius: 7px;
                margin: 2px 0;
            }

            QScrollArea QScrollBar::handle:vertical:hover {
                background-color: #5d5d5d;
            }

            QScrollArea QScrollBar::add-page:vertical,
            QScrollArea QScrollBar::sub-page:vertical {
                background: transparent;
            }
            QScrollArea QScrollBar::add-line:vertical,
            QScrollArea QScrollBar::sub-line:vertical {
                height: 0px;
            }

            QLineEdit {
                border: 1px solid dark-gray;
                background: #5d5d5d;
                color: white;
            }

            QMenu {
                background-color: #3d3d3d;
                border: 1px solid #4d4d4d;
            }

            QMenu::item {
                padding: 8px 16px 8px 16px;
                color: #ffffff;
                background-color: transparent;
            }

            QMenu::item:selected {
                background-color: #5d5d5d;
                border-radius: 4px;
            }

            QMenu::icon {
                margin-left: 8px;
            }
            )";
}


QString CustomStyle::getActionBonusButtonStyle() {
    return R"(
            QPushButton#getBonusButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4CAF50,
                stop:%1 #4CAF50,
                stop:%2 #808080);
                color: #ffffff;
                border: 2px solid #45a049;
                border-radius: 25px;
                padding: 15px;
                font-size: 14px;
                font-weight: bold;
            })";
}

QString CustomStyle::getEnableBonusButtonStyle() {
    return R"(
            QPushButton#getBonusButton {
                background-color: #4CAF50;
                color: #ffffff;
                border: 2px solid #45a049;
                border-radius: 25px;
                padding: 15px;
                font-size: 14px;
                font-weight: bold;
            }

            QPushButton#getBonusButton:hover {
                background-color: #45a049;
            }

            QPushButton#getBonusButton:pressed {
                background-color: #3d8b40;
            }
            )";
}

QString CustomStyle::getCustomNewButton() {
    return R"(
            QPushButton {
                background-image: url(%1);
                background-color: #4CAF50;
                background-repeat: no-repeat;
                background-position: center 20px;
                border-radius: 25px;
                color: white;
                font-size: 50px;
                font-weight: bold;
                padding-top: 240px;
            }
            QPushButton:hover {
                background-color: #45a049;
            }
            QPushButton:pressed {
                background-color: #3d8b40;
            }
            )";
}


QString CustomStyle::getTextRules() {
    return R"(
            <h2>📋 Основные правила</h2>
            <p><b style='color: #3498db;'>1.</b> Минимальная ставка: <b>100 монет</b></p>
            <p><b style='color: #3498db;'>2.</b> Максимальная ставка: <b>10 000 000 монет</b></p>
            <p><b style='color: #3498db;'>3.</b> Время на игру: <b>3 минуты</b></p>
            <p><b style='color: #3498db;'>4.</b> При проигрыше ставка <span style='color: #e74c3c;'>не возвращается</span></p>
            <p><b style='color: #3498db;'>5.</b> При выигрыше ставка <span style='color: #2ecc71;'>удваивается</span></p>
            <p><b style='color: #3498db;'>6.</b> Бонус можно получать каждые <b>3 часа</b></p>
            <p><b style='color: #3498db;'>7.</b> Запрещено использование скриптов и ботов</p>

            <p><b style='color: #3498db;'>8.</b> Минимальная сумма вывода: <b>100 рублей</b></p>
            <p><b style='color: #3498db;'>9.</b> Максимальная сумма вывода: <b>100 000 000 рублей</b></p>

            <p><b style='color: #3498db;'>10.</b> Комиссия при выводе:</p>
            <p style='margin-left: 10px;'>- На банковскую карту: <b>10%</b></p>
            <p style='margin-left: 10px;'>- На номер телефона: <b>5%</b></p>
            <p style='margin-left: 10px;'>- На криптокошелек: <b>0%</b></p>
           )";
}

QString CustomStyle::getTextFAQ() {
    return R"(
            <h2>❓ Часто задаваемые вопросы</h2>
            <p><b style='color: #3498db;'>Q: Как начать игру?</b></p>
            <p style='margin-left: 10px;'>A: Выберите игру из списка, установите желаемую ставку (от 100 до 10 000 000 монет) и нажмите кнопку <b>"Играть"</b>.</p>

            <p><b style='color: #3498db;'>Q: Как получить бонус?</b></p>
            <p style='margin-left: 10px;'>A: Нажмите кнопку <b>"Получить Бонус!"</b> в главном меню. Бонус доступен каждые 3 часа.</p>

            <p><b style='color: #3498db;'>Q: Что делать при технических проблемах?</b></p>
            <p style='margin-left: 10px;'>A: Обратитесь в поддержку через Telegram (@SlashGames_support_bot)</p>

            <p><b style='color: #3498db;'>Q: Как предложить новую игру?</b></p>
            <p style='margin-left: 10px;'>A: Напишите нам в поддержку с описанием вашей идеи. Лучшие предложения будут реализованы!</p>

            <p><b style='color: #3498db;'>Q: Где мои выигрыши?</b></p>
            <p style='margin-left: 10px;'>A: Все выигрыши автоматически зачисляются на ваш баланс. Вы можете увидеть изменение суммы в верхней части экрана.</p>

            <p><b style='color: #3498db;'>Q: Что делать, если возникли проблемы с пополнением баланса?</b></p>
            <p style='margin-left: 10px;'>A: Если у вас возникли трудности с оплатой, пожалуйста, свяжитесь с нашей службой поддержки в Telegram (@SlashGames_support_bot).</p>

            <p><b style='color: #3498db;'>Q: Я забыл пароль. Как его сменить?</b></p>
            <p style='margin-left: 10px;'>A: Для смены пароля перейдите в <b>"Мой Профиль"</b> в главном меню, затем выберите раздел <b>"Редактировать"</b>. Если вы не можете войти, напишите нам в Telegram (@SlashGames_support_bot).</p>

            <p><b style='color: #3498db;'>Q: Как вывести деньги?</b></p>
            <p style='margin-left: 10px;'>A: Чтобы вывести средства, нажмите на свой <b>баланс</b> в главном меню. Появится окно с опциями вывода.</p>

            <p><b style='color: #3498db;'>Q: Как пополнить баланс?</b></p>
            <p style='margin-left: 10px;'>A: Для пополнения баланса нажмите на свой <b>баланс</b> в главном меню, и вы увидите опции для пополнения.</p>
           )";
}




QString CustomStyle::getTextSupportLabel() {
    return "Нет нужной игры?"
            "<a href='https://t.me/SlashGames_support_bot' style='color: #2a7ae9; text-decoration: none;'> "
            "Напишите нам";
}

QString CustomStyle::userInfoText(){
    return R"(
            <table width='100%' style='font-size: 12pt; border-collapse: collapse;'>
            <tr><td style='padding: 4px;'><b style='color: #3465A4;'>Имя:</b></td><td style='padding: 4px;'>%1</td></tr>
            <tr><td style='padding: 4px;'><b style='color: #3465A4;'>Баланс:</b></td><td style='padding: 4px;'>%2</td></tr>
            <tr><td style='padding: 4px;'><b style='color: #3465A4;'>Дата регистрации:</b></td><td style='padding: 4px;'>%3</td></tr>
            <tr><td style='padding: 4px;'><b style='color: #3465A4;'>Токен до:</b></td><td style='padding: 4px;'><span style='%6'>%4</span> %5</td></tr>
            </table>
            )";
}

QString CustomStyle::defaultCopyAddressButton(){
    return R"(
        QPushButton {
          background-color: transparent;
          border: 1px solid #4CAF50;
          color: #4CAF50;
          padding: 8px 16px;
          border-radius: 20px;
          font-size: 12pt;
          font-weight: 500;
        }
        QPushButton:hover {
          background-color: rgba(76, 175, 80, 0.1);
          border-color: #388E3C;
          color: #388E3C;
        }
        QPushButton:pressed {
          background-color: rgba(76, 175, 80, 0.2);
        }
            )";
}


QString CustomStyle::activeCopyAddressButton(){
    return R"(
        QPushButton {
          background-color: #4CAF50;
          border: 1px solid #4CAF50;
          color: white;
          padding: 8px 16px;
          border-radius: 20px;
          font-size: 11pt;
        }
            )";
}

QString CustomStyle::confirmationPayWindow() {
    return R"html(
        <style>
            body, html, div { font-family: 'Arial', sans-serif; color: black; margin: 0; padding: 0; }
            h2 { color: black; text-align: center; margin: 0 0 20px 0; font-size: 50pt; font-weight: bold; }
            table { width: 100%; border-collapse: collapse; }
            td { padding: 3px 0; font-size: 12pt; }
            td:first-child { font-weight: bold; width: 45%; color: #555; }
            .total-row { border-top: 1px solid #ddd; padding-top: 15px; font-size: 20pt; font-weight: bold; color: black; }
            .total-amount { color: #007bff; font-size: 20pt; }
        </style>
        <div>
            <h2>Подтверждение операции</h2>
            <h2></h2>
            <table>
                <tr><td>%7:</td><td>%1</td></tr>
                <tr><td>%8:</td><td>%2</td></tr>
                <tr><td>Сумма:</td><td>%3 💲</td></tr>
                <tr><td>Комиссия:</td><td>%4% (%5 💲)</td></tr>
                <tr><td colspan='2' class='total-row'>Итого: <span class='total-amount'>%6 💲</span></td></tr>
            </table>
        </div>
    )html";
}

QString CustomStyle::checkPayWindow() {
    return R"html(
        <style>
            body, html, div { font-family: 'Arial', sans-serif; color: black; margin: 0; padding: 0; }
            h2 { color: #28a745; text-align: center; margin: 0 0 20px 0; font-size: 50pt; font-weight: bold; }
            table { width: 100%; border-collapse: collapse; }
            td { padding: 3px 0; font-size: 12pt; }
            td:first-child { font-weight: bold; width: 45%; color: #555; }
            .total-row { border-top: 1px dashed #ccc; padding-top: 15px; font-size: 20pt; font-weight: bold; color: black; }
            .total-amount { color: #007bff; font-size: 20pt; }
        </style>
        <div>
            <h2>Операция успешно выполнена! &#x2714;</h2>
            <h2></h2>
            <table>
                <tr><td>Дата и время:</td><td>%1</td></tr>
                <tr><td>Тип операции:</td><td>Вывод средств</td></tr>
                <tr><td>%8:</td><td>%2</td></tr>
                <tr><td>%9:</td><td>%3</td></tr>
                <tr><td>Сумма:</td><td>%4 💲</td></tr>
                <tr><td>Комиссия:</td><td>%5% (%6 💲)</td></tr>
                <tr><td colspan='2' class='total-row'>Итого: <span class='total-amount'>%7 💲</span></td></tr>
            </table>
        </div>
    )html";
}
