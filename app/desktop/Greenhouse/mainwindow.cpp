#include "mainwindow.h"
#include "protocol.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), aggregatorClient("greenhouse", this)
{
    greenhouse_pg = new Greenhouse(this);

    setCentralWidget(greenhouse_pg);

    radioClient.connectToDaemon();

    connect(
        &radioClient,
        &RadioClient::packetReceived,
        this,
        &MainWindow::radioHandleParsedPacket
        );

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // GENERAL
    connect(greenhouse_pg, &Greenhouse::sendSingleParam, this, &MainWindow::sendGreenhouseSingleParam);
    connect(this, &MainWindow::updateGreenhouseStatus, greenhouse_pg, &Greenhouse::updateStatus);
    //LOGS перекинуть потом в отдельный демон
    // connect(ui->btn_log_clear, &QPushButton::clicked, this, [this](){
    //     ui->txt_log->clear();
    // });

    radioInitScheduler();

    aggregatorClient.connectToAggregator();
    connect(
        &aggregatorClient,
        &AggregatorClient::showRequested,
        this,
        &MainWindow::showApp
        );

    connect(
        &aggregatorClient,
        &AggregatorClient::hideRequested,
        this,
        &MainWindow::hideApp
        );
}



MainWindow::~MainWindow()
{

}



void MainWindow::radioHandleParsedPacket(uint8_t id, uint8_t cmd, const QByteArray &payload)
{
    logLine(QString("RADIO RX | id=%1 cmd=%2 len=%3 | %4")
                .arg(id, 2, 16, QChar('0'))
                .arg(cmd, 2, 16, QChar('0'))
                .arg(payload.size())
                .arg(QString(payload.toHex(' ').toUpper())));

    // 1. Сначала обработать полезные данные.
    if(id == DEV_GREENHOUSE && cmd == CMD_STATUS_RESPONSE) {
        handleGreenhouseStatus(payload);
    }
}


void MainWindow::logLine(const QString &text){
    // ui->txt_log->appendPlainText(QTime::currentTime().toString("HH:mm:ss") + " " + text);
}


void MainWindow::handleGreenhouseStatus(const QByteArray &payload){
    quint8 b0 = static_cast<quint8>(payload.at(0));
    quint8 b1 = static_cast<quint8>(payload.at(1));
    quint16 temp_x10 =
        static_cast<quint16>(b0) |
        (static_cast<quint16>(b1) << 8);
    uint8_t airHum = payload[2];
    uint16_t soilRaw =
        payload[3] |
        (static_cast<uint16_t>(payload[4]) << 8);
    uint8_t waterState = payload[6];
    float air_temp = temp_x10 / 10.0;
    emit updateGreenhouseStatus(air_temp, airHum, soilRaw, waterState);
    return;
}


void MainWindow::sendGreenhouseSingleParam(uint16_t cmd, uint16_t value){
    QByteArray payload;
    QString cmd_str;
    switch(cmd){
    case CMD_GREENHOUSE_FAN_SET:
        payload.append(static_cast<char>(value ? 1 : 0));
        cmd_str = QString("FAN MANUAL %1").arg(value? "ON" : "OFF");
        break;

    case CMD_GREENHOUSE_PUMP_SET:
        payload.append(static_cast<char>(value & 0xFF));
        cmd_str = QString("PUMP MANUAL WATERING SECONDS %1").arg(value);
        break;

    case CMD_GREENHOUSE_AUTOVENT_SET:
        payload.append(static_cast<char>(value ? 1 : 0));
        cmd_str = QString("AUTOVENT MANUAL %1").arg(value? "ON" : "OFF");
        break;

    case CMD_GREENHOUSE_AUTOWATER_SET:
        payload.append(static_cast<char>(value ? 1 : 0));
        cmd_str = QString("AUTOWATER MANUAL %1").arg(value? "ON" : "OFF");
        break;
    case CMD_GREENHOUSE_SOIL_LIMIT_SET:
        payload.append(static_cast<char>(value & 0xFF));
        payload.append(static_cast<char>((value >> 8) & 0xFF));
        cmd_str = QString("SOIL DRY THRESHOLD SET %1").arg(value);
        break;
    }

    logLine(QString("GREENHOUSE %1").arg(cmd_str));
    QByteArray pkt = makePacket(DEV_GREENHOUSE, cmd, payload);
    radioClient.send(pkt, QString("GREENHOUSE %1").arg(cmd_str), 200);
}


void MainWindow::radioInitScheduler()
{
    greenhouse_pg->pollTimer.setInterval(5000);


    connect(
        &greenhouse_pg->pollTimer,
        &QTimer::timeout,
        this,
        [this]()
        {


            QByteArray pkt =
                makePacket(
                    DEV_GREENHOUSE,
                    CMD_STATUS_REQUEST,
                    QByteArray()
                    );


            radioClient.request(
                pkt,
                "GREENHOUSE STATUS REQUEST",
                DEV_GREENHOUSE,
                CMD_STATUS_RESPONSE,
                700,
                200,
                true
                );
        }
        );




    greenhouse_pg->pollTimer.start();

}


void MainWindow::showApp()
{
    showMaximized();
    raise();
    activateWindow();
}

void MainWindow::hideApp()
{
    hide();
}
