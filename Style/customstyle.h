#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QString>

class CustomStyle
{
public:
    static QString getLightThemeStyle();
    static QString getDarkThemeStyle();

    static QString getTextSupportLabel();

    static QString getActionBonusButtonStyle();
    static QString getEnableBonusButtonStyle();
    static QString getCustomNewButton();

    static QString getTextRules();
    static QString getTextFAQ();

    static QString userInfoText();

    static QString defaultCopyAddressButton();
    static QString activeCopyAddressButton();
    static QString confirmationPayWindow();
    static QString checkPayWindow();

};

#endif // CUSTOMSTYLE_H
