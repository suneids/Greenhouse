#ifndef GREENHOUSE_H
#define GREENHOUSE_H

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

namespace Ui { class Greenhouse; }

class Greenhouse : public QWidget
{
    Q_OBJECT

public:
    explicit Greenhouse(QWidget *parent = nullptr);
    ~Greenhouse();
    QTimer pollTimer;

signals:
    void sendSingleParam(uint16_t cmd, uint16_t value);

public slots:

    void updateStatus(float air_temp, float air_hum, uint16_t soilRaw, uint8_t waterState);
private:
    Ui::Greenhouse *ui;
    QChart *chartAir = nullptr;
    QChartView *chartViewAir = nullptr;
    QLineSeries *seriesTemp = nullptr;
    QLineSeries *seriesAirHum = nullptr;
    QValueAxis *axisTimeAir = nullptr;
    QValueAxis *axisTemp = nullptr;
    QValueAxis *axisAirHum = nullptr;
    QChart *chartSoil = nullptr;
    QChartView *chartViewSoil = nullptr;
    QLineSeries *seriesSoilHum = nullptr;
    QValueAxis *axisTimeSoil = nullptr;
    QValueAxis *axisSoilHum = nullptr;
    qint64 chartStartMs = 0;


private:
    void chartsInit();
    void trimSeries(QLineSeries *series, double minX);
    void chartsAddNewPoint(float tempC, float airHum, float soilHum);
};

#endif // GREENHOUSE_H
