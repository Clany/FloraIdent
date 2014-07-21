#ifndef MY_WIDGETS_H
#define MY_WIDGETS_H

#include "common.h"
#include <memory>
#include <QRubberBand>
#include <QLabel>
#include <QMouseEvent>


class SelectionArea : public QWidget
{
    Q_OBJECT

public:
    using Ptr = std::unique_ptr<SelectionArea>;

    explicit SelectionArea(QWidget *parent = 0);

    void addSizeGrip();

private:
    void mousePressEvent(QMouseEvent *ev)   override;
    void mouseMoveEvent(QMouseEvent *ev)    override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void resizeEvent(QResizeEvent *)        override;

    QPoint origin;
    QRubberBand* rubberband;
};

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget* parent = 0) : QLabel(parent) {};

    void setImage(const QImage& image);
    bool getSelectionArea(QRect& area);
    void clearSelection() { selection_area.reset(nullptr); }
    float getRatio() { return ratio; };

private:
    void mousePressEvent(QMouseEvent *ev)   override;
    void mouseMoveEvent(QMouseEvent *ev)    override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

    float ratio;
    QImage img;
    QPoint origin;
    SelectionArea::Ptr selection_area;
};

#endif // MY_WIDGETS_H