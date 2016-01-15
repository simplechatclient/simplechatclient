/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2016 Piotr Łuczko <piotr.luczko@gmail.com>
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

#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <QObject>
#include <QStringList>

class Highlight : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Highlight)
    static Highlight *Instance;
public:
    static Highlight *instance();

    Highlight();
    void init();
    void add(const QString &strHighlight);
    void remove(const QString &strHighlight);
    void replace(const QString &strOldHighlight, const QString &strNewHighlight);
    bool contains(const QString &strHighlight);
    void clear();
    QStringList get();

private:
    QStringList lHighlight;

    void read();
    void save();
    QString fix(const QString &strHighlight);
};

#endif // HIGHLIGHT_H
