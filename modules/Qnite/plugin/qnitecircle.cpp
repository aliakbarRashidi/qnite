#include "qnitecircle.h"
#include "qniteclipper.h"
#include "qnitemapper.h"
#include "qnitecirclenode.h"
#include "qniteaxes.h"
#include "qniteaxis.h"

#include <QDebug>
#include <QPolygon>


constexpr auto SELECTION_TOLERANCE = 20;


QniteCircle::QniteCircle(QQuickItem *parent):
  QniteXYArtist(parent),
  m_radius{10}
{
  setFlag(ItemHasContents, true);
  setClipper(new QniteClipper(this));
}

QniteCircle::~QniteCircle()
{
}

qreal QniteCircle::radius() const
{
  return m_radius;
}

void QniteCircle::setRadius(qreal radius)
{
  if (m_radius != radius) {
    m_radius = radius;
    emit radiusChanged();
  }
}

bool QniteCircle::select(const QList<QPoint>& path)
{
    // nothing to select
    if (path.isEmpty()) {
        return false;
    }

    // when there are less than 3 point we use the point selection
    // algorithm which should select the nearest point
    if (path.size() < 3) {
        return select(path.first());
    }

    // create a polygon with the path so it is easier to check
    // if a point is in the path or not
    auto polygonPath = QPolygon(QVector<QPoint>::fromList(path));

    // cycle through all the points and save those contained
    // in the polygon
    auto dataSize = xMapped().size();
    for(auto i = 0; i < dataSize; ++i) {
        QPoint cp(xMapped().at(i), yMapped().at(i));

        if (polygonPath.containsPoint(cp, Qt::OddEvenFill)) {
            m_selectedPoints.append(i);
        }
    }

    // no points have been selected
    if (m_selectedPoints.isEmpty()) {
        return false;
    }

    emit selectedChanged();
    update();
    return true;
}

bool QniteCircle::select(const QPoint p)
{
    // these variables are needed to keep
    // track of the nearest point during the search
    auto nearestIndex = -1;
    // we only evaluate points whose distance is below a
    // predefined tolerance
    auto nearestDistance = SELECTION_TOLERANCE;

    auto dataSize = xMapped().size();
    for(auto i = 0; i < dataSize; ++i) {
        QPoint cp(xMapped().at(i), yMapped().at(i));
        QPoint d = p - cp;

        auto distance = d.manhattanLength();
        if (distance < nearestDistance) {
            nearestIndex = i;
            nearestDistance = distance;
        }
    }

    // add the index to the selected points pool
    if (nearestIndex >= 0) {
        m_selectedPoints.append(nearestIndex);
        emit selectedChanged();
        update();
        return true;
    }

    return false;
}

void QniteCircle::clearSelection()
{
    m_selectedPoints.clear();
    emit selectedChanged();
    update();
}

QSGNode* QniteCircle::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
  if (!node) {
    node = new QSGNode;
  }

  // TODO: processdata should be triggered only when data changes
  processData();
  int dataSize = xMapped().size();

  // TODO: find a better approach. removing and creating all nodes is bad
  node->removeAllChildNodes();
  for(int i = 0; i < dataSize; ++i) {
    qreal cx = xMapped().at(i);
    qreal cy = yMapped().at(i);

    // choose the color based on the selection status
    auto pointColor = m_selectedPoints.contains(i) ? selectionColor() : color();

    // TODO: optimal number of segments should be  computed runtime
    node->appendChildNode(new QniteCircleNode(cx, cy, m_radius, 32, pointColor));
  }

  return node;
}

