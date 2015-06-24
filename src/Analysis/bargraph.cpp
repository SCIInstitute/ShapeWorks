#include "bargraph.h"

#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QRect>
#include <QTextItem>
#include <sstream>

BarGraph::BarGraph(QWidget *parent) :
    QWidget(parent),
    max_val(100.),
    min_val(0.),
    barwidth(10),
    margin(5)
{}

BarGraph::~BarGraph() {}

int BarGraph::getMargin() const
{
    return margin;
}

void BarGraph::setMargin(int value)
{
    margin = value;
}

void BarGraph::setData(std::vector<double> val)
{
    min_val = *std::min_element(val.begin(),val.end());
    max_val = *std::max_element(val.begin(),val.end());
    values.clear();
    for (size_t i = 0; i < val.size(); i++)
        values.push_back(val[i] - min_val);
    recalcBasicValues();
    setMinimumSize( (int)(margin*values.size()*2),200+margin*5);
}

void BarGraph::paintBargraph(QPainter &painter)
{
    QPen p(Qt::black);
    p.setWidth(2);
    painter.setPen(p);
    painter.setBrush(Qt::blue);

    for(size_t i=0, s = values.size(); i < s; ++i)
    {
        painter.drawRect(bars[i]);
        //numbered eigen value on x axis
        painter.drawText(barwidth*(i+0.5)+margin*(i+1),height()- 20,QString::number(i));
    }
    //X label
    painter.drawText(2*margin,height()- 5,"Eigenvalues (log scale)");
    // Y Label
    int num_steps = static_cast<int>(max_val - min_val + 1);
    int start = static_cast<int>(min_val);
    int separation = (height() - 45) / num_steps;
    for (int i = 0; i < num_steps; i++) {
        std::stringstream ss;
        ss << "1e" << (start+i) << "_";
        painter.drawText(width()  - 45,
                         height() - 75 - separation * i,QString(ss.str().c_str()));
    }
}

void BarGraph::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    paintBargraph(painter);
}


void BarGraph::resizeEvent(QResizeEvent *event)
{
    recalcBasicValues();
}

void BarGraph::recalcBasicValues()
{
    int sz = std::max(1,static_cast<int>(values.size()));
    barwidth = std::max(margin, (int)((width()-(margin*values.size()))/sz));
    int h = height() - 45;
    if(bars.size() != values.size())
        bars.resize(values.size());
    int x = margin;
    for(size_t i=0, s = values.size(); i < s; ++i)
    {
        int barheight = static_cast<int>(values[i] * h / (max_val - min_val));
        bars[i].setRect(x,5 + h - barheight,barwidth, barheight);
        x += margin + barwidth;
    }
}

QString BarGraph::getLabel(size_t i)
{
    if(values.size() > i)
    {
        return QString::number(values[i]+min_val);
    }
    return QString::number(i);
}
