#ifndef QNITE_SELECT_TOOL_H
#define QNITE_SELECT_TOOL_H

#include "qniteselectiontool.h"

class QniteArtist;
class QnitePathSelectionTool: public QniteSelectionTool
{
  Q_OBJECT
  Q_PROPERTY(QVariantList selectionPath READ selectionPath NOTIFY selectionPathChanged)

public:
  explicit QnitePathSelectionTool(QQuickItem* parent = 0);
  virtual ~QnitePathSelectionTool() {}

  QVariantList selectionPath() const;

  virtual void begin(const QPoint &point);
  virtual void append(const QPoint &point);
  virtual void end();

Q_SIGNALS:
  void selectionPathChanged();

protected:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual bool doSelect(QniteArtist*);

  QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

  QList<QPoint> m_selection;
};

#endif // QNITE_SELECT_TOOL_H