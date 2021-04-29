#include "Calculator.h"

#include <qdebug.h>

int main(int argc, char* argv[])
{
    qDebug() << "Result = " << Calculator::eval("2+(3*4-sin(60)*10)");
    system("pause");
    return 0;
}