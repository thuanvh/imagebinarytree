
#ifndef _CUSTOM_LABEL_H_
#define _CUSTOM_LABEL_H_

#include <QLabel>
#include <QMouseEvent>

class CustomLabel : public QLabel {
  Q_OBJECT

signals:
  void mousePressed(const QPoint&);

public:
  CustomLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
  CustomLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0);

  void mousePressEvent(QMouseEvent* ev);
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
