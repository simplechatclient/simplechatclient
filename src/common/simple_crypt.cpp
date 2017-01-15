/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2017 Piotr Łuczko <piotr.luczko@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include "simple_crypt.h"

#ifndef Q_OS_MAC
	#include <Qca-qt5/QtCrypto/QtCrypto>

	#ifdef Q_OS_WIN
		#include <windows.h>
	#else
		#include <unistd.h>
	#endif
#endif

SimpleCrypt::SimpleCrypt()
{
#ifndef Q_OS_MAC
	#ifdef Q_OS_WIN
		char volName[256];
		char fileSysName[256];
		DWORD dwSerialNumber;
		DWORD dwMaxComponentLen;
		DWORD dwFileSysFlags;

		bool res = GetVolumeInformation("c:\\", volName, 256, &dwSerialNumber, &dwMaxComponentLen, &dwFileSysFlags, fileSysName, 256);
		if (res)
			strIv = QString::number(dwSerialNumber,10);
	#else
		long lSerialNumber = gethostid();
		strIv = QString::number(lSerialNumber, 10);
	#endif
#endif

    if (strIv.isEmpty())
        strIv = "69193462";
}

QString SimpleCrypt::encrypt(QString strKey, const QString &strData)
{
    if ((strKey.isEmpty()) || (strData.isEmpty()) || (strIv.isEmpty()))
    {
        qWarning() << tr("Error: crypt: Cannot encrypt - empty argument");
        return QString::null;
    }

//    QCA::Initializer init;

//    if ((!QCA::isSupported("aes256-cbc-pkcs7")) && (!QCA::isSupported("blowfish-cbc")))
//    {
//        qWarning() << tr("Warning: AES and Blowfish is not supported!");
//        return strData;
//    }

//    QCA::SecureArray arg = QByteArray(strData.toLatin1());

//    if (strKey.length() < 16) { while(strKey.length() < 16) { strKey += " "; } }
//    QCA::SymmetricKey key = QByteArray(strKey.toLatin1());

//    QCA::InitializationVector iv = QByteArray(strIv.toLatin1());

//    if (QCA::isSupported("aes256-cbc-pkcs7"))
//    {
//        QCA::Cipher cipher(QString("aes256"),QCA::Cipher::CBC,
//                           QCA::Cipher::DefaultPadding,
//                           QCA::Encode,
//                           key, iv);
//        return QCA::arrayToHex(QCA::SecureArray(cipher.process(arg)).toByteArray());
//    }
//    else if (QCA::isSupported("blowfish-cbc"))
//    {
//        QCA::Cipher cipher(QString("blowfish"), QCA::Cipher::CBC,
//                           QCA::Cipher::DefaultPadding,
//                           QCA::Encode,
//                           key, iv);
//        return QCA::arrayToHex(QCA::SecureArray(cipher.process(arg)).toByteArray());
//    }
//    else

        return strData;
}

QString SimpleCrypt::decrypt(QString strKey, const QString &strData)
{
    if ((strKey.isEmpty()) || (strData.isEmpty()) || (strIv.isEmpty()))
    {
        qWarning() << tr("Error: crypt: Cannot decrypt - empty argument");
        return QString::null;
    }

//    QCA::Initializer init;

//    if ((!QCA::isSupported("aes256-cbc-pkcs7")) && (!QCA::isSupported("blowfish-cbc")))
//    {
//        qWarning() << tr("Warning: AES and Blowfish is not supported!");
//        return strData;
//    }

//    QCA::SecureArray arg = QCA::hexToArray(strData);

//    if (strKey.length() < 16) { while(strKey.length() < 16) { strKey += " "; } }
//    QCA::SymmetricKey key = QByteArray(strKey.toLatin1());

//    QCA::InitializationVector iv = QByteArray(strIv.toLatin1());

//    if (QCA::isSupported("aes256-cbc-pkcs7"))
//    {
//        QCA::Cipher cipher(QString("aes256"),QCA::Cipher::CBC,
//                           QCA::Cipher::DefaultPadding,
//                           QCA::Decode,
//                           key, iv);
//        return QCA::SecureArray(cipher.process(arg)).data();
//    }
//    else if (QCA::isSupported("blowfish-cbc"))
//    {
//        QCA::Cipher cipher(QString("blowfish"),QCA::Cipher::CBC,
//                           QCA::Cipher::DefaultPadding,
//                           QCA::Decode,
//                           key, iv);
//        return QCA::SecureArray(cipher.process(arg)).data();
//    }
//    else

        return strData;
}
