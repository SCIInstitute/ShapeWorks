#ifndef BARGRAPH_H
#define BARGRAPH_H

#include <QWidget>
#include <utility>
#include <vector>

class BarGraph : public QWidget
{
    Q_OBJECT
    std::vector<double> values;
    double max_val,min_val;
    int barwidth,margin;
    std::vector<QRect> bars;
public:
    BarGraph(QWidget *parent = 0);
    ~BarGraph();
    void setData(std::vector<double> values);
    void paintBargraph(QPainter& painter);


    QBrush getBrush() const;
    void setBrush(const QBrush &value);

    int getMargin() const;
    void setMargin(int value);

signals:
    
public slots:
protected:

private:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    void recalcBasicValues();
    QString getLabel(size_t i);
};

#endif // BARGRAPH_H
