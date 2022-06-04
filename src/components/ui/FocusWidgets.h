#ifndef FocusWidgets_h
#define FocusWidgets_h

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include "Focus.h"
#include "Adafruit_EPD.h"

class Button : public View {
public:
    Button(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(BabelTypesetterGFX *typesetter, int16_t x, int16_t y) override;
protected:
    std::string text;
};

class HatchedView : public View {
public:
    HatchedView(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);
    void draw(BabelTypesetterGFX *typesetter, int16_t x, int16_t y) override;
protected:
    uint16_t color;
};

class Label : public View {
public:
    Label(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(BabelTypesetterGFX *typesetter, int16_t x, int16_t y) override;
    void setText(std::string text);
protected:
    std::string text;
};

typedef enum {
    CellSelectionStyleNone,
    CellSelectionStyleInvert,
    CellSelectionStyleIndicatorLeading,
    CellSelectionStyleIndicatorTrailing,
    CellSelectionStyleIndicatorAbove,
    CellSelectionStyleIndicatorBelow
} CellSelectionStyle;

class Cell : public View {
public:
    Cell(int16_t x, int16_t y, int16_t width, int16_t height, std::string text, CellSelectionStyle selectionStyle);
    void draw(BabelTypesetterGFX *typesetter, int16_t x, int16_t y) override;
    // TODO: implement these for the indicator style
    // void willBecomeFocused() override;
    // void willResignFocus() override;
protected:
    std::string text;
    CellSelectionStyle selectionStyle;
};

class Table : public View {
public:
    Table(int16_t x, int16_t y, int16_t width, int16_t height, int16_t cellHeight, CellSelectionStyle selectionStyle);
    void setItems(std::vector<std::string> items);
    void becomeFocused() override;
    bool handleEvent(Event event) override;
protected:
    void updateCells();
    int16_t cellHeight;
    int16_t cellsPerPage;
    int16_t startOffset;
    std::vector<std::string> items;
    std::string text;
    CellSelectionStyle selectionStyle;
};

#endif // FocusWidgets_h