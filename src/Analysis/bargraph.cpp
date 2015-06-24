#include "bargraph.h"

#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QRect>
#include <QTextItem>
#include <sstream>
#include <cmath>
#include <iostream>

BarGraph::BarGraph(QWidget *parent) :
    QWidget(parent),
    max_val(100.),
    min_val(0.),
    barwidth(10),
    margin(5),
    use_log_(true)
{}

BarGraph::~BarGraph() {}

void BarGraph::setLogScale(bool b) {
    use_log_ = b;
    recalcBasicValues();
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
    int num_steps = static_cast<int>(use_log_?(log10(max_val) -
                                               log10(min_val)):5);
    num_steps = std::max(1,num_steps);
    std::cout << num_steps << std::endl;
    int start = static_cast<int>(use_log_?log10(min_val):0);
    int separation = (height() - 45) / num_steps;
    for (int i = 0; i < num_steps; i++) {
        std::stringstream ss;
        if (use_log_) {
            ss << "1e" << (start+i) << "_";
            painter.drawText(width()  - 45,
                             height() - 55 - separation * i,QString(ss.str().c_str()));
        } else {
            ss << (start + (max_val / num_steps) * i) << "_";
            painter.drawText(width()  - 45,
                             height() - 75 - separation * i,QString(ss.str().c_str()));
        }
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
        double val = use_log_?log10(values[i]):values[i];
        double range = use_log_?(log10(max_val) - log10(min_val)):(max_val - min_val);
        int barheight = static_cast<int>(val * h / (range));
        bars[i].setRect(x,5 + h - barheight,barwidth, barheight);
        x += margin + barwidth;
    }
}
