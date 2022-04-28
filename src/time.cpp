/**
 * @file time.cpp
 * @author Haolin Yang
 * @brief 时间和时间管理类的实现
 * @version 0.1
 * @date 2022-04-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "../include/time.h"
#include "../include/user.h"

#include <ctime>
#include <QDebug>

int Time::curYear = 0;
int Time::curMonth = 0;
int Time::curDay = 0;

void Time::init()
{
    time_t rawTime;
    time(&rawTime);
    struct tm *tm_curTime = localtime(&rawTime);
    curYear = tm_curTime->tm_year + 1900;
    curMonth = tm_curTime->tm_mon + 1;
    curDay = tm_curTime->tm_mday;
    qInfo() << "当前物流系统时间为" << curYear << "/" << curMonth << "/" << curDay;
}

QString Time::addDays(int dayNum)
{
    if (dayNum <= 0)
        return "要加快的天数应该为正数";
    curDay += dayNum;
    while (curDay > 31)
    {
        curDay -= 31;
        curMonth++;
    }
    while (curMonth > 12)
    {
        curMonth -= 12;
        curYear++;
    }
    qDebug() << "物流系统时间增加" << dayNum << "天，当前物流系统时间为" << curYear << "/" << curMonth << "/" << curDay;
    return "";
}

QString Time::getTime(QJsonObject &ret)
{
    qDebug() << "获取物流系统时间信息";
    ret.insert("year", Time::getCurYear());
    ret.insert("month", Time::getCurMonth());
    ret.insert("day", Time::getCurDay());
    return {};
}

bool Time::isDue() const
{
    return ((year < curYear) || (year == curYear && month < curMonth) || (year == curYear && month == curMonth && day <= curDay));
}

bool Time::isFuture() const
{
    return ((year > curYear) || (year == curYear && month > curMonth) || (year == curYear && month == curMonth && day >= curDay));

}