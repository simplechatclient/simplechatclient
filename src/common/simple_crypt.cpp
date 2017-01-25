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

SimpleCrypt::SimpleCrypt(const QString &iv)
{
#ifndef Q_OS_MAC
	#ifdef Q_OS_WIN
		char volName[256];
		char fileSysName[256];
		DWORD dwSerialNumber;
		DWORD dwMaxComponentLen;
		DWORD dwFileSysFlags;

		bool res = GetVolumeInformation("c:\\", volName, 256, &dwSerialNumber, &dwMaxComponentLen, &dwFileSysFlags, fileSysName, 256);
		if (res) {
			strKey = QString::number(dwSerialNumber,10);
        }
	#else
		long lSerialNumber = gethostid();
		strKey = QString::number(lSerialNumber, 10);
	#endif
#endif

    // iv
    strIv = iv;

    if (strKey.length() < 32) { while(strKey.length() < 32) { strKey += "x"; } }
    if (strIv.length() < 32) { while(strIv.length() < 32) { strIv += "x"; } }
}

QString SimpleCrypt::encrypt(const QString &strData)
{
    if ((strKey.isEmpty()) || (strData.isEmpty()) || (strIv.isEmpty()))
    {
        qWarning() << tr("Error: crypt: Cannot encrypt - empty argument");
        return QString::null;
    }

    if (!QCA::isSupported("aes128-cbc-pkcs7"))
    {
        qWarning() << tr("Warning: AES is not supported!");
        return strData;
    }

    QCA::Initializer init;
    QCA::SymmetricKey key = QByteArray(strKey.toLatin1());
    QCA::InitializationVector iv = QByteArray(strIv.toLatin1());

	QCA::Cipher cipher(QString("aes128"), QCA::Cipher::CBC,
						QCA::Cipher::DefaultPadding,
						QCA::Encode,
						key, iv);

    QCA::SecureArray data = QByteArray(strData.toLatin1());
    QCA::SecureArray encryptedData = cipher.process(data);

    if (!cipher.ok())
    {
        qWarning() << "Encryption failed!";
        return QString::null;
    }

    return QCA::arrayToHex(encryptedData.toByteArray());
}

QString SimpleCrypt::decrypt(const QString &strData)
{
    if ((strKey.isEmpty()) || (strData.isEmpty()) || (strIv.isEmpty()))
    {
        qWarning() << tr("Error: crypt: Cannot decrypt - empty argument");
        return QString::null;
    }

    if (!QCA::isSupported("aes128-cbc-pkcs7"))
    {
        qWarning() << tr("Warning: AES is not supported!");
        return strData;
    }

    QCA::Initializer init;
    QCA::SymmetricKey key = QByteArray(strKey.toLatin1());
    QCA::InitializationVector iv = QByteArray(strIv.toLatin1());

	QCA::Cipher cipher(QString("aes128"), QCA::Cipher::CBC,
						QCA::Cipher::DefaultPadding,
						QCA::Decode,
						key, iv);

    QCA::SecureArray data = QCA::hexToArray(strData);
    QCA::SecureArray decryptedData = cipher.process(data);

    if (!cipher.ok())
    {
        qWarning() << "Decryption failed!";
        return QString::null;
    }

    return decryptedData.data();
}
