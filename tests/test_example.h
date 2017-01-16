
#include <QObject>

class TestExample : public QObject
{
    Q_OBJECT

private slots:
    void toUpper_data();
    void toUpper();
};
