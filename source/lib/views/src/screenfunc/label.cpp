
#include <views/screenfunc/label.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/driver.hpp>
#include "views/screenfunc/common.h"
#include "traces.h"

/**
 * @brief sets font and scale and offset that fits to frame
 *
 * @param settings
 * @param frame
 * @param length
 */
static void fitToFrame(TextSettings& settings, const Frame& frame, size_t length);


void labelSettingsNew(LabelSettings& settings, const Frame& frame, const char* string, size_t commonLength, const display::DisplayColor& color)
{
    settings.text.color = nullptr;
    settings.color = color;
    settings.text.string = string;
    settings.text.str_len = commonLength;
    fitToFrame(settings.text, frame, commonLength);
}

void labelSettingsNew(LabelSettings& settings, const Frame& frame, const char* string, const display::DisplayColor& color)
{
    labelSettingsNew(settings, frame, string, strlen(string), color);
}

/*print string*/
void LabelDraw(const void *settings)
{
    LabelSettings *labelSettings = (LabelSettings *)settings;

    const display::DisplayColor& color = labelSettings->text.color == nullptr ?  labelSettings->color : *labelSettings->text.color;
    display::println(labelSettings->text.offsetX,
                  labelSettings->text.offsetY,
                  labelSettings->text.string,
                  labelSettings->text.font,
                  color,
                  labelSettings->text.scale);

}

uint16_t labelSettingsGetWidth(const LabelSettings& settings)
{
    return settings.text.str_len * settings.text.font->width * settings.text.scale;
}

uint16_t labelSettingsGetHeight(const LabelSettings& settings)
{
    return settings.text.font->height * settings.text.scale;
}

uint16_t textSettingsGetHeight(const TextSettings& text)
{
    return text.font->height * text.scale;
}

void labelSettingsAlignHeight(TextSettings& text, const Frame& frame, bool align_top)
{
    text.offsetY = align_top ? frame.y : frame.y + frame.height - textSettingsGetHeight(text);
}

void labelSettingsAlign(LabelSettings& settings, const Frame& frame, Align align)
{
    switch (align)
    {
    case Align::CENTER:
        {
            auto spaceLeft = frame.width - labelSettingsGetWidth(settings);
            settings.text.offsetX = frame.x + spaceLeft / 2;
            TRACE_DEBUG(3, TRACE_DISPLAY_PRINT, "Center space left = %d\n", spaceLeft);
        }
        break;
    case Align::RIGHT:
        {
            auto spaceLeft = frame.width - labelSettingsGetWidth(settings);
            settings.text.offsetX = frame.x + spaceLeft;
        }
        break;
    case Align::LEFT:
        {
            settings.text.offsetX = frame.x;
        }
        break;
    default:
        break;
    }
}

// ============= static functions ================
static void fitToFrame(TextSettings& settings, const Frame& frame, size_t length)
{
    u_int16_t widthPerChar = frame.width / length;

    getFontSizePreferBiggerFonts(widthPerChar, frame.height, &settings.font, &settings.scale);

    const auto space_left = frame.height - textSettingsGetHeight(settings);
    const auto offset_y = space_left/2;

    settings.offsetX = frame.x;
    settings.offsetY = frame.y + offset_y;
}