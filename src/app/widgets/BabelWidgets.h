#ifndef BabelWidgets_h
#define BabelWidgets_h

#include "Focus.h"
#include "FocusWidgets.h"

class BabelButton : public Button {
public:
    BabelButton(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

class BabelLabel : public Label {
public:
    BabelLabel(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

typedef enum {
    CellSelectionStyleNone,
    CellSelectionStyleInvert,
    CellSelectionStyleIndicatorLeading,
    CellSelectionStyleIndicatorTrailing,
    CellSelectionStyleIndicatorAbove,
    CellSelectionStyleIndicatorBelow
} CellSelectionStyle;

class BabelCell : public View {
public:
    BabelCell(int16_t x, int16_t y, int16_t width, int16_t height, std::string text, CellSelectionStyle selectionStyle);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
    // TODO: implement these for the indicator style
    // void willBecomeFocused() override;
    // void willResignFocus() override;
protected:
    std::string text;
    CellSelectionStyle selectionStyle;
};

class BabelTable : public View {
public:
    BabelTable(int16_t x, int16_t y, int16_t width, int16_t height, int16_t cellHeight, CellSelectionStyle selectionStyle);
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

#endif // BabelWidgets_h