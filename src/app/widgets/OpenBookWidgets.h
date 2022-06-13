#ifndef BabelWidgets_h
#define BabelWidgets_h

#include "Focus.h"
#include "FocusWidgets.h"

class OpenBookButton : public Button {
public:
    OpenBookButton(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

class OpenBookLabel : public Label {
public:
    OpenBookLabel(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

typedef enum {
    CellSelectionStyleNone,
    CellSelectionStyleInvert,
    CellSelectionStyleIndicatorLeading,
    CellSelectionStyleIndicatorTrailing,
    CellSelectionStyleIndicatorAbove,
    CellSelectionStyleIndicatorBelow
} OpenBookCellSelectionStyle;

class OpenBookCell : public Control {
public:
    OpenBookCell(int16_t x, int16_t y, int16_t width, int16_t height, std::string text, OpenBookCellSelectionStyle selectionStyle);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
    // TODO: implement these for the indicator style
    // void willBecomeFocused() override;
    // void willResignFocus() override;
protected:
    std::string text;
    OpenBookCellSelectionStyle selectionStyle;
};

class OpenBookTable : public Control {
public:
    OpenBookTable(int16_t x, int16_t y, int16_t width, int16_t height, int16_t cellHeight, OpenBookCellSelectionStyle selectionStyle);
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
    OpenBookCellSelectionStyle selectionStyle;
};

#endif // BabelWidgets_h
