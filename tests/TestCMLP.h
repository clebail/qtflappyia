#ifndef TESTCMLP_H
#define TESTCMLP_H

#include <QObject>
#include <QtTest>
#include <CMLP.h>

class TestCMLP : public QObject
{
    Q_OBJECT
private:
    CMLP *cmlp;
private slots:
    void initTestCase(void);
    void cleanupTestCase(void);
    void testGradient(void);
};

#endif // TESTCMLP_H
