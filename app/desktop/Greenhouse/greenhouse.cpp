#include "greenhouse.h"
#include "ui_greenhouse.h"
#include "protocol.h"
#include <QTime>

Greenhouse::Greenhouse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Greenhouse)
{
    ui->setupUi(this);
    connect(ui->btn_greenhouse_fan_on, &QPushButton::clicked, this, [this](){
        emit sendSingleParam(CMD_GREENHOUSE_FAN_SET, true);
    });
    connect(ui->btn_greenhouse_fan_off, &QPushButton::clicked, this, [this](){
        emit sendSingleParam(CMD_GREENHOUSE_FAN_SET, false);
    });

    connect(ui->btn_greenhouse_watering_20, &QPushButton::clicked, this, [this](){
        emit sendSingleParam(CMD_GREENHOUSE_PUMP_SET, 20);
    });
    connect(ui->btn_greenhouse_watering_60, &QPushButton::clicked, this, [this](){
        emit sendSingleParam(CMD_GREENHOUSE_PUMP_SET, 60);
    });

    connect(ui->chkb_greenhouse_autocooling, &QCheckBox::stateChanged, this, [this](){
        uint8_t value =  ui->chkb_greenhouse_autocooling->isChecked();
        emit sendSingleParam(CMD_GREENHOUSE_AUTOVENT_SET, value);
    });
    connect(ui->chkb_greenhouse_autowatering, &QCheckBox::stateChanged, this, [this](){
        uint8_t value =  ui->chkb_greenhouse_autowatering->isChecked();
        emit sendSingleParam(CMD_GREENHOUSE_AUTOWATER_SET, value);
    });

    connect(ui->slider_greenhouse_soil_dry_threshold, &QSlider::sliderReleased, this, [this](){
        uint16_t value = static_cast<uint16_t>(ui->slider_greenhouse_soil_dry_threshold->value());
        emit sendSingleParam(CMD_GREENHOUSE_SOIL_LIMIT_SET, value);
    });

    connect(ui->slider_greenhouse_soil_dry_threshold, &QSlider::valueChanged, this, [this](){
        ui->lbl_greenhouse_dry_soil_desc->setText(QString("Порог сухой почвы : %1").arg(ui->slider_greenhouse_soil_dry_threshold->value()));
    });
    chartsInit();
}

Greenhouse::~Greenhouse()
{
    delete ui;
}



void Greenhouse::chartsInit()
{
    chartStartMs = QDateTime::currentMSecsSinceEpoch();

    // ===== AIR CHART =====

    seriesTemp = new QLineSeries(this);
    seriesTemp->setName("Температура, °C");

    seriesAirHum = new QLineSeries(this);
    seriesAirHum->setName("Влажность, %");

    chartAir = new QChart();
    chartAir->setMargins(QMargins(0, 0, 0, 0));
    chartAir->setTitle("Воздух");
    chartAir->addSeries(seriesTemp);
    chartAir->addSeries(seriesAirHum);
    chartAir->legend()->setVisible(true);

    axisTimeAir = new QValueAxis(this);
    axisTimeAir->setTitleText("Время, сек");
    axisTimeAir->setRange(0, 300);

    axisTemp = new QValueAxis(this);
    axisTemp->setTitleText("°C");
        axisTemp->setRange(0, 50);

    axisAirHum = new QValueAxis(this);
    axisAirHum->setTitleText("%");
    axisAirHum->setRange(0, 100);

    chartAir->addAxis(axisTimeAir, Qt::AlignBottom);
    chartAir->addAxis(axisTemp, Qt::AlignLeft);
    chartAir->addAxis(axisAirHum, Qt::AlignRight);

    seriesTemp->attachAxis(axisTimeAir);
    seriesTemp->attachAxis(axisTemp);

    seriesAirHum->attachAxis(axisTimeAir);
    seriesAirHum->attachAxis(axisAirHum);

    chartViewAir = new QChartView(chartAir);
    chartViewAir->setRenderHint(QPainter::Antialiasing);

    auto *airLayout = new QVBoxLayout(ui->frame_air_chart);
    airLayout->setContentsMargins(0, 0, 0, 0);
    airLayout->addWidget(chartViewAir);


    // ===== SOIL CHART =====

    seriesSoilHum = new QLineSeries(this);
    seriesSoilHum->setName("Влажность, %");

    chartSoil = new QChart();
    chartSoil->setMargins(QMargins(0, 0, 0, 0));
    chartSoil->setTitle("Почва");
    chartSoil->addSeries(seriesSoilHum);
    chartSoil->legend()->setVisible(true);

    axisTimeSoil = new QValueAxis(this);
    axisTimeSoil->setTitleText("Время, сек");
    axisTimeSoil->setRange(0, 300);

    axisSoilHum = new QValueAxis(this);
    axisSoilHum->setTitleText("%");
    axisSoilHum->setRange(0, 100);

    chartSoil->addAxis(axisTimeSoil, Qt::AlignBottom);
    chartSoil->addAxis(axisSoilHum, Qt::AlignLeft);

    seriesSoilHum->attachAxis(axisTimeSoil);
    seriesSoilHum->attachAxis(axisSoilHum);

    chartViewSoil = new QChartView(chartSoil);
    chartViewSoil->setRenderHint(QPainter::Antialiasing);

    auto *soilLayout = new QVBoxLayout(ui->frame_soil_chart);
    soilLayout->setContentsMargins(0, 0, 0, 0);
    soilLayout->addWidget(chartViewSoil);
}


void Greenhouse::chartsAddNewPoint(float tempC, float airHum, float soilHum)
{
    qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    double t = (nowMs - chartStartMs) / 1000.0;

    seriesTemp->append(t, tempC);
    seriesAirHum->append(t, airHum);
    seriesSoilHum->append(t, soilHum);

    double windowSec = 300.0;
    double minX = qMax(0.0, t - windowSec);
    double maxX = qMax(windowSec, t);

    axisTimeAir->setRange(minX, maxX);
    axisTimeSoil->setRange(minX, maxX);

    trimSeries(seriesTemp, minX);
    trimSeries(seriesAirHum, minX);
    trimSeries(seriesSoilHum, minX);
}


void Greenhouse::trimSeries(QLineSeries *series, double minX)
{
    while(series->count() > 0 && series->at(0).x() < minX) {
        series->remove(0);
    }
}

void Greenhouse::updateStatus(float air_temp, float air_hum, uint16_t soilRaw, uint8_t waterState){
    uint16_t soil_hum = (4095 - soilRaw) * 100 / 4095;

    QString soil_status = "Нормально";
    if(soilRaw < 1500) soil_status = "Влажно";
    if(soilRaw > 2500) soil_status = "Сухо";
    ui->lbl_greenhouse_soil_status->setText(soil_status);

    ui->lbl_greenhouse_water_level->setText(
        waterState ? "Вода есть" : "Мало воды"
        );

    ui->lbl_greenhouse_connection->setText("Online");
    ui->lbl_greenhouse_last_update->setText(
        QTime::currentTime().toString("HH:mm:ss")
        );
    ui->lbl_greenhouse_air_temp->setText(QString::number(air_temp, 'f', 1) + " °C");
    ui->lbl_greenhouse_air_humidity->setText(QString::number(air_hum) + " %");
    ui->lbl_greenhouse_soil_humidity->setText(QString("%1 %").arg(QString::number(soil_hum)));
    chartsAddNewPoint(air_temp, air_hum, soil_hum);
}
