#include "customstyle.h"

QString CustomStyle::getLightThemeStyle() {
  return "QMainWindow { background-color: #ffffff; color: #000000; }"
         "QDialog { background-color: #ffffff; color: #000000; }"

         "QLabel { color: #000000; }"
         "QPushButton { background-color: #ffffff; color: #000000; border: 1px solid #cccccc; }"
         "QTextEdit { background-color: #ffffff; color: #000000; border: 1px solid #cccccc; }"

         "QTabWidget { background-color: #ffffff; color: #000000; border: none; }"
         "QTabWidget QTextEdit { background-color: #ffffff; color: #000000; }"

         // Вкладки 
         "QTabWidget::pane {"
         "    border: 1px solid #e0e0e0;"
         "    background: #ffffff;"
         "    border-top-left-radius: 0px;"
         "    border-top-right-radius: 8px;"
         "    border-bottom-right-radius: 8px;"
         "    border-bottom-left-radius: 8px;"
         "}"

         "QTabBar::tab {"
         "    background: #f0f0f0;"
         "    color: #000000;"
         "    border: 1px solid #e0e0e0;"
         "    border-bottom: none;"
         "    border-top-left-radius: 8px;"
         "    border-top-right-radius: 8px;"
         "    padding: 8px 16px;"
         "    margin-right: 5px;"
         "}"

         "QTabBar::tab:selected {"
         "    background: #ffffff;"
         "}"

         "QTabBar::tab:hover {"
         "    background: #e0e0e0;"
         "}"

           // Области прокрутки 
           "QScrollArea {"
           "    background-color: #ffffff;"
           "    border: none;"
           "}"

           "QScrollArea QWidget {"
           "    background-color: #ffffff;"
           "}"

           "QScrollArea QScrollBar:vertical {"
           "    width: 14px;"
           "    border-radius: 7px;"               
           "    margin: 0px;"
           "}"

           "QScrollArea QScrollBar::handle:vertical {"
           "    background-color: #cccccc;" 
           "    min-height: 30px;"          
           "    border-radius: 7px;"        
           "    margin: 2px 0;"             
           "}"

           "QScrollArea QScrollBar::handle:vertical:hover {"
           "    background-color: #bbbbbb;" 
           "}"

           "QScrollArea QScrollBar::add-page:vertical,"
           "QScrollArea QScrollBar::sub-page:vertical {"
           "    background: transparent;"   
           "}"
           "QScrollArea QScrollBar::add-line:vertical,"
           "QScrollArea QScrollBar::sub-line:vertical {"
           "    height: 0px;"               
           "}"

           "QLineEdit {"
            "border: 1px solid dark-gray;"
            "background: white;"
            "color: dark-gray"
            "}"

            // Стиль для меню
            "QMenu {"
            "    background-color: #ffffff;"
            "    border: 1px solid #e0e0e0;"
            "    border-radius: 8px;"
            "}"

            "QMenu::item {"
            "    padding: 8px 16px 8px 16px;"
            "    color: #000000;"
            "    background-color: transparent;"
            "}"

            "QMenu::item:selected {"
            "    background-color: #f0f0f0;"
            "    border-radius: 4px;"
            "}"

            "QMenu::icon {"
            "    margin-left: 8px;"
            "    position: absolute;"
            "    left: 8px;"
            "}";
}


QString CustomStyle::getDarkThemeStyle() {
  return "QMainWindow { background-color: #1e1e1e; color: #ffffff; }"
         "QDialog { background-color: #1e1e1e; color: #ffffff; }"

         "QLabel { color: #ffffff; }"
         "QPushButton { background-color: #3d3d3d; color: #ffffff; border: 1px solid #4d4d4d; }"
         "QTextEdit { background-color: #1e1e1e; color: #ffffff; border: 1px solid #4d4d4d; }"

         "QTabWidget { background-color: #1e1e1e; color: #ffffff; border: none; }"
         "QTabWidget QTextEdit { background-color: #1e1e1e; color: #ffffff; }"

         // Вкладки 
         "QTabWidget::pane {"
         "    border: 1px solid #4d4d4d;"
         "    background: #1e1e1e;"
         "    border-top-left-radius: 0px;"
         "    border-top-right-radius: 8px;"
         "    border-bottom-right-radius: 8px;"
         "    border-bottom-left-radius: 8px;"
         "}"

         "QTabBar::tab {"
         "    background: #3d3d3d;"
         "    color: #ffffff;"
         "    border: 1px solid #4d4d4d;"
         "    border-bottom: none;"
         "    border-top-left-radius: 8px;"
         "    border-top-right-radius: 8px;"
         "    padding: 8px 16px;"
         "    margin-right: 5px;"
         "}"

         "QTabBar::tab:selected {"
         "    background: #4d4d4d;"
         "}"

         "QTabBar::tab:hover {"
         "    background: #5d5d5d;"
         "}"

           // Области прокрутки 
           "QScrollArea {"
           "    background-color: #1e1e1e;"
           "    border: none;"
           "}"

           "QScrollArea QWidget {"
           "    background-color: #1e1e1e;"
           "}"

           "QScrollArea QScrollBar:vertical {"
           "    width: 14px;"              
           "    margin: 0px;"
           "    border-radius: 7px;" 
           "}"

           "QScrollArea QScrollBar::handle:vertical {"
           "    background-color: #4d4d4d;" 
           "    min-height: 30px;"          
           "    border-radius: 7px;"        
           "    margin: 2px 0;"             
           "}"

           "QScrollArea QScrollBar::handle:vertical:hover {"
           "    background-color: #5d5d5d;" 
           "}"

           "QScrollArea QScrollBar::add-page:vertical,"
           "QScrollArea QScrollBar::sub-page:vertical {"
           "    background: transparent;"  
           "}"
           "QScrollArea QScrollBar::add-line:vertical,"
           "QScrollArea QScrollBar::sub-line:vertical {"
           "    height: 0px;"              
           "}"

           "QLineEdit {"
            "border: 1px solid dark-gray;"
            "background: #5d5d5d;"
            "color: white;"
            "}"

            // Стиль для меню
            "QMenu {"
            "    background-color: #3d3d3d;"
            "    border: 1px solid #4d4d4d;"
            "    border-radius: 8px;"
            "}"

            "QMenu::item {"
            "    padding: 8px 16px 8px 16px;"
            "    color: #ffffff;"
            "    background-color: transparent;"
            "}"

            "QMenu::item:selected {"
            "    background-color: #5d5d5d;"
            "    border-radius: 4px;"
            "}"

            "QMenu::icon {"
            "    margin-left: 8px;"
            "    position: absolute;"
            "    left: 8px;"
            "}";
}


QString CustomStyle::getActionBonusButtonStyle() {
    return "QPushButton#getBonusButton {"
    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
    "                              stop:0 #4CAF50,"
    "                              stop:%1 #4CAF50,"
    "                              stop:%2 #808080);"
    "    color: #ffffff;"
    "    border: 2px solid #45a049;"
    "    border-radius: 25px;"
    "    padding: 15px;"
    "    font-size: 14px;"
    "    font-weight: bold;"
    "}";
}

QString CustomStyle::getEnableBonusButtonStyle() {
        return "QPushButton#getBonusButton {"
                   "    background-color: #4CAF50;"
                   "    color: #ffffff;"
                   "    border: 2px solid #45a049;"
                   "    border-radius: 25px;"
                   "    padding: 15px;"
                   "    font-size: 14px;"
                   "    font-weight: bold;"
                   "}"
                   "QPushButton#getBonusButton:hover {"
                   "    background-color: #45a049;"
                   "}"
                   "QPushButton#getBonusButton:pressed {"
                   "    background-color: #3d8b40;"
                   "}";
}
