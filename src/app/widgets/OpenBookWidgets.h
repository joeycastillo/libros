#ifndef BabelWidgets_h
#define BabelWidgets_h

#include "Focus.h"
#include "FocusWidgets.h"

class OpenBookButton : public Button {
public:
    OpenBookButton(Rect rect, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

class OpenBookLabel : public Label {
public:
    OpenBookLabel(Rect rect, std::string text);
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
    OpenBookCell(Rect rect, std::string text, OpenBookCellSelectionStyle selectionStyle);
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
    OpenBookTable(Rect rect, int16_t cellHeight, OpenBookCellSelectionStyle selectionStyle);
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
