#include "NetworkAPI.h"
#include <QJsonObject>
#include <QDebug>

NetworkAPI::NetworkAPI(QObject *parent) : QObject(parent)
  , m_netManager(new QNetworkAccessManager(this))
  , m_websocket(new QWebSocket())
  , m_status(logged_out)
  , m_allowReconnect(false)
  , m_wsConnectionCheckTimer(new QTimer(this))
  , m_wsReconnectTimer(new QTimer(this))
{
    // config websocket
    connect(this, &NetworkAPI::wsInfoReady, this, &NetworkAPI::wsConnect);
    connect(m_websocket, &QWebSocket::connected, this, &NetworkAPI::on_wsConnected);
    connect(m_websocket, &QWebSocket::disconnected, this, &NetworkAPI::on_wsDisConnected);
    connect(m_websocket, &QWebSocket::binaryMessageReceived, this, &NetworkAPI::on_wsRecieveBinaryMessage);
    connect(m_websocket, &QWebSocket::textMessageReceived, this, &NetworkAPI::on_wsRecieveTextMessage);
    connect(m_websocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        this, &NetworkAPI::on_wsError);
    connect(m_wsConnectionCheckTimer, &QTimer::timeout, this, &NetworkAPI::on_wsConnectionCheck);
    connect(m_wsReconnectTimer, &QTimer::timeout, this, &NetworkAPI::on_wsReconnect);
    //m_wsReconnectTimer->setSingleShot(true);
}

NetworkAPI::~NetworkAPI()
{
    delete m_websocket;
}

QString NetworkAPI::getWebsocketURL()
{
    return DANMAKU_SCHEMA + "://" + danmaku_domain
            + "/websocket/consumer/persistent/public/default/"
            + m_roomID + "/"
            + QUrl::toPercentEncoding(QHostInfo::localHostName()) + "~"
            + QString::number(QRandomGenerator::global()->generate64(), 16)
            + "?token=" + m_roomToken;
}

void NetworkAPI::connectionAborted()
{
    m_allowReconnect = false;
    emit ConnectionAborted(tr("连接意外断开"));
    emit wsReconnecting();
    m_websocket->abort();
    m_websocket->open(QUrl(getWebsocketURL()));
    m_status = reconnecting;
    qDebug()<<"Try reconnect...";
    m_reconnect_countdown = 4;
    emit wsReconnectCountdown(m_reconnect_countdown);
    m_wsReconnectTimer->start(1000);
}

void NetworkAPI::on_loginReplyRecieve()
{
    if(m_reply->error() != QNetworkReply::NoError)
    {
        QString errorMessage;
        switch (m_reply->error())
        {
            case QNetworkReply::NoError: break;
            case 203: errorMessage += tr("无法连接房间服务器") + "\n" + tr("请检查房间号是否正确"); break;
            case 204: errorMessage += tr("房间密码错误"); break;
            default: errorMessage += tr("错误代码") + " " + QString::number(m_reply->error()) + "\n" + m_reply->errorString(); break;
        }
        emit loginFailed(m_reply->error(), errorMessage);
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(m_reply->readAll(), &error);
    if(error.error != QJsonParseError::NoError ){
        m_status = logged_out;
        emit loginFailed(-1, tr("获取令牌失败"));
        return;
    }
    QString token = doc.object().value("pulsar_jwt").toString();
    m_roomToken = token;
    qDebug() << "ws connect:" << m_roomID << " " <<token;
    emit wsInfoReady(m_roomID, m_roomToken);
}

void NetworkAPI::on_wsRecieveBinaryMessage(const QByteArray &message)
{
    QString str_message;
    str_message.prepend(message);
    str_message.replace('\'','\"');
    str_message.remove('\t');
    QByteArray processed_message = str_message.toUtf8();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(processed_message, &error);
    if(!doc.isNull()){
        QJsonObject json_obj = doc.object();
        if(json_obj.value("endOfTopic")!= QJsonValue::Undefined){
            //qDebug()<<"pong";
            return;
        }
        emit jsonMessage(json_obj);
    }
}

void NetworkAPI::on_wsRecieveTextMessage(const QString &message)
{
    on_wsRecieveBinaryMessage(message.toUtf8());
}

QString NetworkAPI::getWallUrl()
{
    if (m_status != logged_in) return "";
    return QString("https://") + danmaku_domain + "/wall/" + m_roomID + "?code=" + m_roomPass;
}

// login with id and pass as room id and password
void NetworkAPI::login(const QString &id, const QString &pass)
{
    qDebug()<<"connect request"<<id<<" "<<pass;
    m_roomID = id;
    m_roomPass = pass;
    m_status = logging_in;
    QNetworkRequest request(QUrl::fromUserInput(QString("https://") + danmaku_domain + "/api/v1/room/" + id + "/client-login"));
    request.setRawHeader(QByteArray("Authorization"), (QString("Bearer ")+pass).toLatin1());
    m_reply = m_netManager->get(request);
    connect(m_reply, &QNetworkReply::finished, this, &NetworkAPI::on_loginReplyRecieve);
}

void NetworkAPI::cancelConnect()
{
    m_reply->abort();
    m_websocket->abort();
}

void NetworkAPI::logout()
{
    m_allowReconnect = false;
    if(m_status == logged_out){
        m_wsReconnectTimer->stop();
        emit logoutSuccess();
        return;
    }
    m_status = logging_out;
    wsClose();
    //TODO
}

void NetworkAPI::wsConnect(const QString &roomid, const QString &token)
{
    m_roomID = roomid, m_roomToken = token;
    QString url = getWebsocketURL();
    qDebug()<<"[connecting]"<<url;
    m_websocket->open(QUrl(url));
}

void NetworkAPI::wsClose()
{
    m_websocket->close();
    //TODO
}

void NetworkAPI::wsCancelReconnect()
{
    m_allowReconnect = false;
    m_wsReconnectTimer ->stop();
    m_reconnect_countdown = 999;
}

void NetworkAPI::on_wsConnected()
{
    m_allowReconnect = true;
    m_status = logged_in;
    m_wsConnectionCheckTimer->start(DANMAKU_WS_PING_INTERVAL);
    m_wsReconnectTimer->stop();
    m_reconnect_countdown = -1;
    qDebug()<<"[ws connected]";
    emit loginSuccess();
}

void NetworkAPI::on_wsDisConnected()
{
    m_wsConnectionCheckTimer->stop();
    if(m_status == logged_out){
        m_allowReconnect = false;
        m_status = logged_out;
        emit logoutSuccess();
        return;
    }
    if(m_allowReconnect){
        connectionAborted();
    }
}

void NetworkAPI::on_wsError(QAbstractSocket::SocketError error)
{
    qDebug()<<"error:"<<m_websocket->errorString();
    m_wsConnectionCheckTimer->stop();
    if(m_status == reconnecting) return;
    if(m_status == logged_in){
        emit ConnectionAborted(tr("连接意外断开\n")+tr("错误信息: ") + m_websocket->errorString());
        return;
    }
    if(m_status == logging_in){
        emit loginFailed(error, tr("与服务器建立连接失败")+"\n"+tr("错误信息: ") + m_websocket->errorString());
        return;
    }
    if(m_status == logging_out){
        emit logoutFailed(tr("断开连接时发生错误\n")+tr("错误信息：")+m_websocket->errorString());
        m_websocket->abort();
    }
    m_allowReconnect = false;
    m_websocket->abort();
    m_status = logged_out;
    emit ConnectionAborted(tr("连接已断开"));
}

void NetworkAPI::on_wsConnectionCheck()
{
    m_websocket->sendTextMessage("{\"type\":\"isEndOfTopic\"}");
}

void NetworkAPI::on_wsReconnect()
{
    m_reconnect_countdown --;
    if(m_reconnect_countdown == 0){
        connectionAborted();
        emit wsReconnecting();
        return;
    }
    qDebug()<<"countdown:"<<m_reconnect_countdown;
    emit wsReconnectCountdown(m_reconnect_countdown);
}

void NetworkAPI::on_setDanmakuDomain(QString domain)
{
    danmaku_domain = domain;
}

void NetworkAPI::setDanmakuDomain(QString domain)
{
    danmaku_domain = domain;
}
