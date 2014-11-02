#include <QCursor>
#include <QHBoxLayout>
#include <QSizeGrip>
#include <QGroupBox>
#include <iostream>
#include "my_widgets.h"

using namespace std;
using namespace cls;

//////////////////////////////////////////////////////////////////////////
// Selection area
SelectionArea::SelectionArea(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::SubWindow);
    setCursor(QCursor(Qt::CursorShape::SizeAllCursor));

    if (layout()) delete layout();

    rubberband = new QRubberBand(QRubberBand::Rectangle, this);
    rubberband->show();
}

void SelectionArea::addSizeGrip()
{
    if (layout() == nullptr) {
        setLayout(new QGridLayout(this));
        auto grid_layout = (QGridLayout*)layout();
        grid_layout->setContentsMargins(QMargins());
        grid_layout->addWidget(new QSizeGrip(this), 0, 0, Qt::AlignLeft | Qt::AlignTop);
        grid_layout->addWidget(new QSizeGrip(this), 1, 0, Qt::AlignLeft | Qt::AlignBottom);
        grid_layout->addWidget(new QSizeGrip(this), 0, 1, Qt::AlignRight | Qt::AlignTop);
        grid_layout->addWidget(new QSizeGrip(this), 1, 1, Qt::AlignRight | Qt::AlignBottom);
    }
}

void SelectionArea::mousePressEvent(QMouseEvent *ev)
{
    origin = ev->pos();
}

void SelectionArea::mouseMoveEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        // dst is the new top left corner of the selection area
        QPoint dst = pos() + (ev->pos() - origin);
        QSize lr_bound = ((ImageLabel*)parent())->size() - size();

        // Prevent to move out of canvas
        if (dst.x() < 0) dst.setX(0);
        if (dst.y() < 0) dst.setY(0);
        if (dst.x() > lr_bound.width()) dst.setX(lr_bound.width());
        if (dst.y() > lr_bound.height()) dst.setY(lr_bound.height());

        move(dst);
    }
}

void SelectionArea::resizeEvent(QResizeEvent *)
{
    rubberband->resize(size());
}

//////////////////////////////////////////////////////////////////////////
// Image label
void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
    origin = ev->pos();

    selection_area.reset(new SelectionArea(this));
}

void ImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        // Prevent to move out of canvas
        QPoint dst = ev->pos();
        if (dst.x() < 0) dst.setX(0);
        if (dst.y() < 0) dst.setY(0);
        if (dst.x() > width()  - 1) dst.setX(width()  - 1);
        if (dst.y() > height() - 1) dst.setY(height() - 1);

        selection_area->setGeometry(QRect(origin, dst).normalized());
        selection_area->show();
    }
    this->repaint();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent*)
{
    selection_area->addSizeGrip();
}

void ImageLabel::setImage(const QImage& image)
{
    if (org_sz.isEmpty()) org_sz = size();
    setMaximumSize(org_sz);
    resize(org_sz);

    float h_ratio = (float)size().height() / image.height();
    float w_ratio = (float)size().width()  / image.width();
    ratio = w_ratio < h_ratio ? w_ratio : h_ratio;

    img = image.scaled(image.size() * ratio,
                       Qt::IgnoreAspectRatio,
                       Qt::SmoothTransformation);

    setPixmap(QPixmap::fromImage(img));
}

bool ImageLabel::getSelectionArea(QRect& area, std::string& area_string, bool is_scaled)
{
    if (selection_area && selection_area->width()) {
        area = QRect(selection_area->pos(), selection_area->size());
        area_string = string("Selected image region: [x: ") +
                      to_string(area.left()) + ", y: " +
                      to_string(area.top()) + ", w: " +
                      to_string(area.width()) + ", h: " +
                      to_string(area.height()) + "]";
        if (is_scaled) {
            double scale = 1 / ratio;
            area.setRect(area.left()   * scale,
                         area.top()    * scale,
                         area.width()  * scale,
                         area.height() * scale);
        }
        return true;
    }

    area = QRect();
    return false;
}