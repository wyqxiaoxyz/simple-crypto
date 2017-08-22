#ifndef CIPHER_H
#define CIPHER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>


#define PADDING RSA_PKCS1_PADDING
#define KEYSIZE 32
#define IVSIZE 32
#define BLOCKSIZE 256
#define SALTSIZE 8


class Cipher : public QObject
{
	Q_OBJECT
public:
	explicit Cipher(QObject *parent = 0);
signals:

public slots:

};

#endif 