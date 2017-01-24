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

#ifndef SIMPLE_CRYPT_H
#define SIMPLE_CRYPT_H

#include <QObject>

/**
 * Simple crypt class
 */
class SimpleCrypt : public QObject
{
    Q_OBJECT
public:
    SimpleCrypt(const QString &iv);
    /**
     * Encrypt
     * @param QString data
     * @return QString encrypted data
     */
    QString encrypt(const QString &strData);
    /**
     * Decrypt
     * @param QString data
     * @return QString decrypted data
     */
    QString decrypt(const QString &strData);

private:
    QString strKey;
    QString strIv;
};

#endif // SIMPLE_CRYPT_H
