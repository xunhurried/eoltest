#include "hallcurrentvector.h"
#include <algorithm>
#include <numeric>
#include <QDebug>
#include <QVector>

HallCurrentVector::HallCurrentVector()
{

}

void HallCurrentVector::push_back(float current)
{
    hallCurrents.push_back(current);
}

float HallCurrentVector::getavg()
{
    if (hallCurrents.size() <= 2)
    {
        qDebug() << "不足两个，无法滤除最大最小值";
        return 0.0f;
    }

    std::vector<float> sorted = hallCurrents; // 拷贝
    std::sort(sorted.begin(), sorted.end());

    QStringList valueList;
    for (float v : sorted)
    {
        valueList << QString::number(v, 'f', 2);
    }

    qDebug() << "所有值:" << valueList.join(", ");
    qDebug() << "最小值：" << sorted.front() << ",最大值:" << sorted.back();

    // 去除首尾各一个
    float sum = std::accumulate(sorted.begin() + 1, sorted.end() - 1, 0.0f);
    float avg = sum / static_cast<float>(sorted.size() - 2);

    // 输出平均值
    qDebug() << "滤除最大最小值后的平均值为:" << avg;

    return avg;
}

void HallCurrentVector::clear()
{
    hallCurrents.clear();
}
