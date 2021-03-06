/*
    Copyright (c) 2016-2017 Xavier Leclercq and the wxCharts contributors.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

/*
    Part of this file were copied from the Chart.js project (http://chartjs.org/)
    and translated into C++.

    The files of the Chart.js project have the following copyright and license.

    Copyright (c) 2013-2017 Nick Downie
    Released under the MIT license
    https://github.com/nnnick/Chart.js/blob/master/LICENSE.md
*/

#include "wxmath2dplot.h"
#include <wx/dcmemory.h>
#include <sstream>

wxMath2DPlotDataset::wxMath2DPlotDataset(
    const wxColor &dotColor,
    const wxColor &dotStrokeColor,
    wxVector<wxPoint2DDouble> &data,
    const wxChartType &chartType)
    : m_showDots(true), m_dotColor(dotColor),
      m_dotStrokeColor(dotStrokeColor), m_showLine(true),
      m_lineColor(dotColor),m_data(data),m_type(chartType)
{
}

bool wxMath2DPlotDataset::ShowDots() const
{
    return m_showDots;
}

const wxColor& wxMath2DPlotDataset::GetDotColor() const
{
    return m_dotColor;
}

const wxColor& wxMath2DPlotDataset::GetDotStrokeColor() const
{
    return m_dotStrokeColor;
}

const wxChartType& wxMath2DPlotDataset::GetType() const
{
    return m_type;
}

bool wxMath2DPlotDataset::ShowLine() const
{
    return m_showLine;
}

const wxColor& wxMath2DPlotDataset::GetLineColor() const
{
    return m_lineColor;
}

const wxVector<wxPoint2DDouble>& wxMath2DPlotDataset::GetData() const
{
    return m_data;
}

wxMath2DPlotData::wxMath2DPlotData()
{
}

void wxMath2DPlotData::AddDataset(wxMath2DPlotDataset::ptr dataset)
{
    m_datasets.push_back(dataset);
}

const wxVector<wxMath2DPlotDataset::ptr>& wxMath2DPlotData::GetDatasets() const
{
    return m_datasets;
}

wxMath2DPlot::Point::Point(
    wxPoint2DDouble value,
    const wxChartTooltipProvider::ptr tooltipProvider,
    wxDouble x,
    wxDouble y,
    wxDouble radius,
    unsigned int strokeWidth,
    const wxColor &strokeColor,
    const wxColor &fillColor,
    wxDouble hitDetectionRange)
    : wxChartPoint(x, y, radius, tooltipProvider, wxChartPointOptions(strokeWidth, strokeColor, fillColor)),
      m_value(value), m_hitDetectionRange(hitDetectionRange)
{
}

wxPoint2DDouble wxMath2DPlot::Point::GetTooltipPosition() const
{
    wxPoint2DDouble position = wxChartPoint::GetTooltipPosition();
    position.m_y -= 10;
    return position;
}

bool wxMath2DPlot::Point::HitTest(const wxPoint &point) const
{
    wxDouble distance = (point.x - GetPosition().m_x);
    if (distance < 0)
    {
        distance = -distance;
    }
    return (distance < m_hitDetectionRange);
}

wxPoint2DDouble wxMath2DPlot::Point::GetValue() const
{
    return m_value;
}

wxMath2DPlot::Dataset::Dataset(bool showDots,
                               bool showLine,
                               const wxColor &lineColor,
                               const wxChartType &chartType)
    : m_showDots(showDots), m_showLine(showLine),
      m_lineColor(lineColor), m_type(chartType)
{
}

bool wxMath2DPlot::Dataset::ShowDots() const
{
    return m_showDots;
}

bool wxMath2DPlot::Dataset::ShowLine() const
{
    return m_showLine;
}

const wxColor& wxMath2DPlot::Dataset::GetLineColor() const
{
    return m_lineColor;
}

const wxChartType& wxMath2DPlot::Dataset::GetType() const
{
    return m_type;
}

const wxVector<wxMath2DPlot::Point::ptr>& wxMath2DPlot::Dataset::GetPoints() const
{
    return m_points;
}

void wxMath2DPlot::Dataset::AppendPoint(Point::ptr point)
{
    m_points.push_back(point);
}

wxMath2DPlot::wxMath2DPlot(const wxMath2DPlotData &data,
                           const wxSize &size)
    : m_grid(
          wxPoint2DDouble(m_options.GetPadding().GetLeft(), m_options.GetPadding().GetRight()),
          size,
          GetMinXValue(data.GetDatasets()), GetMaxXValue(data.GetDatasets()),
          GetMinYValue(data.GetDatasets()), GetMaxYValue(data.GetDatasets()),
          m_options.GetGridOptions())
{
    Initialize(data);
}

wxMath2DPlot::wxMath2DPlot(const wxMath2DPlotData &data,
                           const wxMath2DPlotOptions &options,
                           const wxSize &size)
    : m_options(options),
      m_grid(
          wxPoint2DDouble(m_options.GetPadding().GetLeft(), m_options.GetPadding().GetRight()),
          size,
          GetMinXValue(data.GetDatasets()), GetMaxXValue(data.GetDatasets()),
          GetMinYValue(data.GetDatasets()), GetMaxYValue(data.GetDatasets()),
          m_options.GetGridOptions())
{
    Initialize(data);
}

const wxMath2DPlotOptions& wxMath2DPlot::GetOptions() const
{
    return m_options;
}

void wxMath2DPlot::Save(const wxString &filename,
                        const wxBitmapType &type,
                        const wxSize &size)
{
    wxBitmap bmp(size.GetWidth(), size.GetHeight());
    wxMemoryDC mdc(bmp);
    mdc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(mdc);
    if (gc)
    {
        DoDraw(*gc);
        bmp.SaveFile(filename, type);
        delete gc;
    }
}

bool wxMath2DPlot::Scale(int coeff)
{
    return m_grid.Scale(coeff);
}

void wxMath2DPlot::Shift(double dx,double dy)
{
    m_grid.Shift(dx,-dy);
}

void wxMath2DPlot::Initialize(const wxMath2DPlotData &data)
{

    const wxVector<wxMath2DPlotDataset::ptr>& datasets = data.GetDatasets();
    for (size_t i = 0; i < datasets.size(); ++i)
    {

        Dataset::ptr newDataset(new Dataset(datasets[i]->ShowDots(),datasets[i]->ShowLine(),
                                            datasets[i]->GetLineColor(),datasets[i]->GetType()));

        const wxVector<wxPoint2DDouble>& datasetData = datasets[i]->GetData();
        for (size_t j = 0; j < datasetData.size(); ++j)
        {
            std::stringstream tooltip;
            tooltip << "(" << datasetData[j].m_x << "," << datasetData[j].m_y << ")";
            wxChartTooltipProvider::ptr tooltipProvider(
                new wxChartTooltipProviderStatic("", tooltip.str(), datasets[i]->GetLineColor())
            );

            Point::ptr point(
                new Point(datasetData[j], tooltipProvider, 20 + j * 10, 0,
                          m_options.GetDotRadius(), m_options.GetDotStrokeWidth(),
                          datasets[i]->GetDotStrokeColor(), datasets[i]->GetDotColor(),
                          m_options.GetHitDetectionRange()));

            newDataset->AppendPoint(point);
        }

        m_datasets.push_back(newDataset);
    }
}

wxDouble wxMath2DPlot::GetMinXValue(const wxVector<wxMath2DPlotDataset::ptr>& datasets)
{
    wxDouble result = 0;
    bool foundValue = false;

    for (size_t i = 0; i < datasets.size(); ++i)
    {
        const wxVector<wxPoint2DDouble>& values = datasets[i]->GetData();
        for (size_t j = 0; j < values.size(); ++j)
        {
            if (!foundValue)
            {
                result = values[j].m_x;
                foundValue = true;
            }
            else if (result > values[j].m_x)
            {
                result = values[j].m_x;
            }
        }
    }

    return result;
}

wxDouble wxMath2DPlot::GetMaxXValue(const wxVector<wxMath2DPlotDataset::ptr>& datasets)
{
    wxDouble result = 0;
    bool foundValue = false;

    for (size_t i = 0; i < datasets.size(); ++i)
    {
        const wxVector<wxPoint2DDouble>& values = datasets[i]->GetData();
        for (size_t j = 0; j < values.size(); ++j)
        {
            if (!foundValue)
            {
                result = values[j].m_x;
                foundValue = true;
            }
            else if (result < values[j].m_x)
            {
                result = values[j].m_x;
            }
        }
    }

    return result;
}

wxDouble wxMath2DPlot::GetMinYValue(const wxVector<wxMath2DPlotDataset::ptr>& datasets)
{
    wxDouble result = 0;
    bool foundValue = false;

    for (size_t i = 0; i < datasets.size(); ++i)
    {
        const wxVector<wxPoint2DDouble>& values = datasets[i]->GetData();
        for (size_t j = 0; j < values.size(); ++j)
        {
            if (!foundValue)
            {
                result = values[j].m_y;
                foundValue = true;
            }
            else if (result > values[j].m_y)
            {
                result = values[j].m_y;
            }
        }
    }

    return result;
}

wxDouble wxMath2DPlot::GetMaxYValue(const wxVector<wxMath2DPlotDataset::ptr>& datasets)
{
    wxDouble result = 0;
    bool foundValue = false;

    for (size_t i = 0; i < datasets.size(); ++i)
    {
        const wxVector<wxPoint2DDouble>& values = datasets[i]->GetData();
        for (size_t j = 0; j < values.size(); ++j)
        {
            if (!foundValue)
            {
                result = values[j].m_y;
                foundValue = true;
            }
            else if (result < values[j].m_y)
            {
                result = values[j].m_y;
            }
        }
    }

    return result;
}

void wxMath2DPlot::DoSetSize(const wxSize &size)
{
    wxSize newSize(
        size.GetWidth() - m_options.GetPadding().GetTotalHorizontalPadding(),
        size.GetHeight() - m_options.GetPadding().GetTotalVerticalPadding()
    );
    m_grid.Resize(newSize);
}

void wxMath2DPlot::DoFit()
{
    for (size_t i = 0; i < m_datasets.size(); ++i)
    {
        const wxVector<Point::ptr>& points = m_datasets[i]->GetPoints();
        for (size_t j = 0; j < points.size(); ++j)
        {
            const Point::ptr& point = points[j];
            point->SetPosition(m_grid.GetMapping().GetWindowPosition(point->GetValue().m_x, point->GetValue().m_y));
        }
    }
}

void wxMath2DPlot::DoDraw(wxGraphicsContext &gc)
{

    m_grid.Draw(gc);
    Fit();

    for (size_t i = 0; i < m_datasets.size(); ++i)
    {
        const wxVector<Point::ptr>& points = m_datasets[i]->GetPoints();

        wxGraphicsPath path = gc.CreatePath();

        if (points.size() > 0)
        {
            const Point::ptr& point = points[0];
            auto value = point->GetValue();

            wxPoint2DDouble firstPosition = m_grid.GetMapping().GetWindowPosition(value.m_x,value.m_y);
            path.MoveToPoint(firstPosition);

            wxPoint2DDouble lastPosition;
            for (size_t j = 1; j < points.size(); ++j)
            {
                const Point::ptr& point = points[j];
                value = point->GetValue();
                lastPosition =  m_grid.GetMapping().GetWindowPosition(value.m_x,value.m_y );

                if(m_datasets[i]->GetType()==wxCHARTTYPE_STEPPED)
                {
                    wxDouble y = value.m_y;
                    value = points[j-1]->GetValue();
                    value = m_grid.GetMapping().GetWindowPosition(value.m_x,y);
                    path.AddLineToPoint(value);
                }

                if(m_datasets[i]->GetType()==wxCHARTTYPE_STEM)
                {
                    value = points[j-1]->GetValue();
                    value = m_grid.GetMapping().GetWindowPosition(value.m_x,0);
                    path.AddLineToPoint(value);
                    path.MoveToPoint(lastPosition);
                    if(j+1==points.size())
                    {
                        value = points[j]->GetValue();
                        value = m_grid.GetMapping().GetWindowPosition(value.m_x,0);
                        path.AddLineToPoint(value);
                    }
                }
                else
                {
                    path.AddLineToPoint(lastPosition);
                }
            }

            if (m_datasets[i]->ShowLine())
            {
                wxPen pen(m_datasets[i]->GetLineColor(), m_options.GetLineWidth());
                gc.SetPen(pen);
            }
            else
            {
                // TODO : transparent pen
            }

            gc.StrokePath(path);
        }

        if (m_datasets[i]->ShowDots())
        {
            for (size_t j = 0; j < points.size(); ++j)
            {
                points[j]->Draw(gc);
            }
        }
    }
}

wxSharedPtr<wxVector<const wxChartElement*> > wxMath2DPlot::GetActiveElements(const wxPoint &point)
{
    wxSharedPtr<wxVector<const wxChartElement*> > activeElements(new wxVector<const wxChartElement*>());
    for (size_t i = 0; i < m_datasets.size(); ++i)
    {
        const wxVector<Point::ptr>& points = m_datasets[i]->GetPoints();
        for (size_t j = 0; j < points.size(); ++j)
        {
            if (points[j]->HitTest(point))
            {
                activeElements->push_back(points[j].get());
            }
        }
    }
    return activeElements;
}
