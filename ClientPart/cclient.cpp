#include "cclient.h"

CClient::CClient(const QString& HostName,int PortId, QWidget *parent) : QWidget(parent),NextBlockSize(0)
{
    pClientSocket = new QTcpSocket(this);
    pClientSocket->connectToHost(HostName,PortId);

    connect( pClientSocket,SIGNAL(connected()),this,SLOT(slotConnected()) );
    connect( pClientSocket,SIGNAL(readyRead()),this,SLOT(slotReadyRead()) );
    connect( pClientSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slotError(QAbstractSocket::SocketError)) );

    pTextInfo = new QTextEdit;
    pTextInput = new QLineEdit;

    connect( pTextInput,SIGNAL(returnPressed()),this,SLOT(slotSendToServer()) );
    pTextInfo->setReadOnly(true);

    QPushButton * pSendButton = new QPushButton("&Send");
    connect( pSendButton,SIGNAL(clicked()),SLOT(slotSendToServer()) );

    //layout
    QVBoxLayout * pLayout = new QVBoxLayout;
    pLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pLayout->addWidget(pTextInfo);
    pLayout->addWidget(pTextInput);
    pLayout->addWidget(pSendButton);
    setLayout(pLayout);
}

void CClient::slotReadyRead()
{
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_9);
    for( ; ; )
    {
        if(!NextBlockSize) {
            if(pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> NextBlockSize;
        }
        if(pClientSocket->bytesAvailable() < NextBlockSize) {
            break;
        }
        QTime time;
        QString str;
        in >> time >> str;

        pTextInfo->append(time.toString() + " " + str);
        NextBlockSize = 0;
    }
}

void CClient::slotError(QAbstractSocket::SocketError err)
{
    QString error = "SOME SHIET HAPPEND!";
    pTextInfo->append(error);
}

void CClient::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_9);
    out << quint16(0) << QTime::currentTime() << pTextInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pClientSocket->write(arrBlock);
    pTextInput->setText("");
}

void CClient::slotConnected()
{
    pTextInfo->append("Receieved the connected() signal");
}
