#ifndef MY_WIDGETS_H
#define MY_WIDGETS_H

#include <memory>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QSizeGrip>
#include <QLabel>
#include <QMouseEvent>


class SelectionArea : public QWidget
{
    Q_OBJECT

public:
    using Ptr = std::unique_ptr<SelectionArea>;

    explicit SelectionArea(QWidget *parent = 0);

    void addSizeGrip();

    void mousePressEvent(QMouseEvent *ev)   override;
    void mouseMoveEvent(QMouseEvent *ev)    override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

private:
    void resizeEvent(QResizeEvent *);

    QPoint origin;
    QRubberBand* rubberband;
};

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget* parent = 0) : QLabel(parent) {};

    void mousePressEvent(QMouseEvent *ev)   override;
    void mouseMoveEvent(QMouseEvent *ev)    override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

    void setImage(const QImage& image);

    bool getSelectionArea(QRect& area);
    float getRatio() { return ratio; };

private:
    float ratio;
    QImage img;
    QPoint origin;
    SelectionArea::Ptr selection_area;
};

#endif // MY_WIDGETS_H