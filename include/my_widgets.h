#ifndef MY_WIDGETS_H
#define MY_WIDGETS_H

#include <memory>
#include <QRubberBand>
#include <QLabel>
#include <QTextEdit>
#include <QMouseEvent>
#include "common.h"

class SelectionArea : public QWidget
{
    Q_OBJECT

public:
    using Ptr = std::unique_ptr<SelectionArea>;

    explicit SelectionArea(QWidget* parent = 0);

    void addSizeGrip();

private:
    void mousePressEvent(QMouseEvent* ev)   override;
    void mouseMoveEvent(QMouseEvent* ev)    override;
    void resizeEvent(QResizeEvent* ev)      override;

    QPoint origin;
    QRubberBand* rubberband = nullptr;
};

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget* parent = 0) : QLabel(parent) {};

    void setImage(const QImage& image, const QString& file_name = "");
    bool getSelectionArea(QRect& area, std::string& area_string, bool is_scaled = false);

    void clearSelection() { selection_area.reset(nullptr); }
    float getRatio() { return ratio; };

    static bool show_fn;

private:
    bool event(QEvent *event)               override;
    void mousePressEvent(QMouseEvent* ev)   override;
    void mouseMoveEvent(QMouseEvent* ev)    override;
    void mouseReleaseEvent(QMouseEvent* ev) override;

    float ratio;
    QSize org_sz;
    QPoint origin;
    QImage img;
    QString file_name;
    SelectionArea::Ptr selection_area;
};

class LogWindow : public QTextEdit
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget* parent = 0) : QTextEdit(parent) {};

protected:
    void contextMenuEvent(QContextMenuEvent * ev) override;
};

#endif // MY_WIDGETS_H