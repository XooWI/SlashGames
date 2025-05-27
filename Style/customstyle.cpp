#include "customstyle.h"

QString CustomStyle::getLightThemeStyle() {
    return R"(
            QMainWindow { background-color: #ffffff; color: #000000; }
            QDialog { background-color: #ffffff; color: #000000; }
            QLabel { color: #000000; }
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
            <p><b style='color: #3498db;'>2.</b> Максимальная ставка: <b>10 000 монет</b></p>
            <p><b style='color: #3498db;'>3.</b> Время на игру: <b>3 минуты</b></p>
            <p><b style='color: #3498db;'>4.</b> При проигрыше ставка <span style='color: #e74c3c;'>не возвращается</span></p>
            <p><b style='color: #3498db;'>5.</b> При выигрыше ставка <span style='color: #2ecc71;'>удваивается</span></p>
            <p><b style='color: #3498db;'>6.</b> Бонус можно получать каждые <b>3 часа</b></p>
            <p><b style='color: #3498db;'>7.</b> Запрещено использование скриптов и ботов</p>
           )";
}


QString CustomStyle::getTextFAQ() {
    return R"(
            <h2>❓ Часто задаваемые вопросы</h2>
            <p><b style='color: #3498db;'>Q: Как начать игру?</b></p>
            <p style='margin-left: 10px;'>A: Выберите игру из списка, установите желаемую ставку (от 100 до 10 000 монет) и нажмите кнопку <b>"Играть"</b>.</p>
            <p><b style='color: #3498db;'>Q: Как получить бонус?</b></p>
            <p style='margin-left: 10px;'>A: Нажмите кнопку <b>"Получить Бонус!"</b> в главном меню. Бонус доступен каждые 3 часа.</p>
            <p><b style='color: #3498db;'>Q: Что делать при технических проблемах?</b></p>
            <p style='margin-left: 10px;'>A: Обратитесь в поддержку через Telegram (@SlashGames_support_bot)</p>
            <p><b style='color: #3498db;'>Q: Как предложить новую игру?</b></p>
            <p style='margin-left: 10px;'>A: Напишите нам в поддержку с описанием вашей идеи. Лучшие предложения будут реализованы!</p>
            <p><b style='color: #3498db;'>Q: Где мои выигрыши?</b></p>
            <p style='margin-left: 10px;'>A: Все выигрыши автоматически зачисляются на ваш баланс. Вы можете увидеть изменение суммы в верхней части экрана.</p>

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
