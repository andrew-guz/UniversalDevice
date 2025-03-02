#pragma once

#include <memory>

#include <Wt/Chart/WCartesianChart.h>
#include <Wt/Chart/WChartGlobal.h>
#include <Wt/WColor.h>
#include <Wt/WPen.h>
#include <Wt/WShadow.h>

class ChartFactory {
public:
    template<typename ModelType>
    static Wt::Chart::WCartesianChart*
    CreateChart(const std::shared_ptr<ModelType>& model, const bool drawGridLines, Wt::Chart::SeriesType seriesType, Wt::WColor color) {
        auto chart = new Wt::Chart::WCartesianChart();
        chart->setModel(model);
        chart->setXSeriesColumn(0);
        chart->setType(Wt::Chart::ChartType::Scatter);
        chart->axis(Wt::Chart::Axis::X).setScale(Wt::Chart::AxisScale::DateTime);
        chart->axis(Wt::Chart::Axis::Y).autoLimits();
        chart->axis(Wt::Chart::Axis::Y).setGridLinesEnabled(drawGridLines);
        auto series = std::make_unique<Wt::Chart::WDataSeries>(1, seriesType, Wt::Chart::Axis::Y);
        series->setPen(Wt::WPen(color));
        series->setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
        chart->addSeries(std::move(series));
        return chart;
    }
};
