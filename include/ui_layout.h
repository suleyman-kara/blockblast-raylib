#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "defs.h"

typedef struct {
    int cardX;
    int cardY;
    int iconAreaY;
    Rectangle sfxIcon;
    Rectangle musicIcon;
    Rectangle firstButton;
    Rectangle secondButton;
    Rectangle thirdButton;
} SettingsLayout;

static inline SettingsLayout GetSettingsLayout(bool hasThreeButtons)
{
    SettingsLayout layout;
    int iconSpacing;
    int iconStartX;
    int firstButtonY;
    int secondButtonY;
    int thirdButtonY;

    layout.cardX = (SCREEN_WIDTH - SETTINGS_CARD_WIDTH) / 2;
    layout.cardY = (SCREEN_HEIGHT - SETTINGS_CARD_HEIGHT) / 2 - 20;
    layout.iconAreaY = layout.cardY + 85;

    iconSpacing = (SETTINGS_CARD_WIDTH - 2 * SETTINGS_CARD_PADDING_X) / 2;
    iconStartX = layout.cardX + SETTINGS_CARD_PADDING_X;

    layout.sfxIcon = (Rectangle){
        (float)(iconStartX + (iconSpacing - SETTINGS_ICON_SIZE) / 2),
        (float)layout.iconAreaY,
        SETTINGS_ICON_SIZE,
        SETTINGS_ICON_SIZE
    };
    layout.musicIcon = (Rectangle){
        (float)(iconStartX + iconSpacing + (iconSpacing - SETTINGS_ICON_SIZE) / 2),
        (float)layout.iconAreaY,
        SETTINGS_ICON_SIZE,
        SETTINGS_ICON_SIZE
    };

    firstButtonY = layout.iconAreaY + SETTINGS_ICON_SIZE + 40;
    secondButtonY = firstButtonY + BTN_H + BTN_GAP;
    thirdButtonY = secondButtonY + BTN_H + BTN_GAP;

    layout.firstButton = (Rectangle){ (float)BTN_X, (float)firstButtonY, BTN_W, BTN_H };
    layout.secondButton = (Rectangle){ (float)BTN_X, (float)secondButtonY, BTN_W, BTN_H };
    layout.thirdButton = hasThreeButtons
        ? (Rectangle){ (float)BTN_X, (float)thirdButtonY, BTN_W, BTN_H }
        : (Rectangle){ 0, 0, 0, 0 };

    return layout;
}

#endif // UI_LAYOUT_H
