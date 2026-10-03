#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort>
#include <QColor>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QMouseEvent>
#include <QPoint>
#include "protocol.h"
#include "aggregatorclient.h"
#include "greenhouse.h"
#include "radioclient.h"
#include <QQueue>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showApp();
    void hideApp();

public slots:
    void sendGreenhouseSingleParam(uint16_t cmd, uint16_t param);

signals:
    void updateGreenhouseStatus(float air_temp, float air_hum, uint16_t soilRaw, uint8_t waterState);

private:

    bool serialWriteChunked(const QByteArray &pkt, int chunkSize, int gapMs);

    QTimer radioTxTimer;
    int radioTxDelayMs = 150;
    bool radioTxActive = false;

    bool ledSceneSending = false;
    RadioClient radioClient;
    AggregatorClient aggregatorClient;
    Greenhouse *greenhouse_pg;


protected:

    void handleGreenhouseStatus(const QByteArray &payload);
    void logLine(const QString &text);
    void radioInitScheduler();
    void radioHandleParsedPacket(uint8_t id, uint8_t cmd, const QByteArray &payload);

};
#endif // MAINWINDOW_H
