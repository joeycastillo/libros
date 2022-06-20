#ifndef OpenBookWidgets_h
#define OpenBookWidgets_h

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
    void setWordWrap(bool value);
    void setBold(bool value);
    void setItalic(bool value);
    void setTextSize(uint16_t value);
    void setLineSpacing(uint16_t value);
    void setParagraphSpacing(uint16_t value);
protected:
    bool wrap = false;
    bool bold = false;
    bool italic = false;
    uint16_t textSize = 1;
    uint16_t lineSpacing = 0;
    uint16_t paragraphSpacing = 0;
};

typedef enum {
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
    void didBecomeFocused() override;
    void didResignFocus() override;
protected:
    Rect _indicatorRect();
    std::string text;
    OpenBookCellSelectionStyle selectionStyle;
};

class OpenBookTable : public Control {
public:
    OpenBookTable(Rect rect, int16_t cellHeight, OpenBookCellSelectionStyle selectionStyle);
    void setItems(std::vector<std::string> items);
    bool handleEvent(Event event) override;
    bool becomeFocused() override;
    int32_t getSelectedIndex();
protected:
    void updateCells();
    int32_t selectedIndex;
    int16_t cellHeight;
    int16_t cellsPerPage;
    int16_t startOffset;
    std::vector<std::string> items;
    std::string text;
    OpenBookCellSelectionStyle selectionStyle;
};

class OpenBookGrayscaleImageView : public View {
public:
    OpenBookGrayscaleImageView(Rect rect, const unsigned char *image);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
protected:
    const unsigned char *image;
};

#endif // OpenBookWidgets_h
