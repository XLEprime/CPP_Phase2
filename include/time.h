/**
 * @file time.h
 * @author Haolin Yang
 * @brief 时间类和时间管理类的声明
 * @version 0.1
 * @date 2022-04-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef TIME_H
#define TIME_H

#include <QString>
#include <QJsonValue>

// 时间类
class Time
{
public:
    int year;            //年
    int month;           //月
    int day;             //日
    static int curYear;  //物流系统当前年
    static int curMonth; //物流系统当前月
    static int curDay;   //物流系统当前日

    Time() = default;

    Time(int _year, int _month, int _day) : year(_year), month(_month), day(_day){};

    ~Time() = default;

    /**
     * @brief 初始化静态成员变量
     */
    static void init();

    static int getCurYear() { return curYear; };

    static int getCurMonth() { return curMonth; };

    static int getCurDay() { return curDay; };

    /**
     * @brief 获取物流系统时间
     *
     * @param ret 结果
     * @return QString 成功则返回空串，否则返回错误信息
     *
     * 时间信息的格式：
     * ```json
     * {
     *    "year": <字符串>,
     *    "month": <整数>,
     *    "day": <整数>
     * }
     * ```
     */
    static QString getTime(QJsonObject &ret);

    /**
     * @brief加快物流系统时间，单位:天
     *
     * @param dayNum要加快的天数
     * @return QString 成功则返回空串，否则返回错误信息
     */
    static QString addDays(int dayNum);

    /**
     * @brief 判断某时间是否到达，以物流系统时间为判据。
     *
     * @return true 达到
     * @return false 未到达
     */
    bool isDue() const;

    /**
     * @brief 判断某时间是否在将来或是今天，以物流系统时间为判据。
     *
     * @return true 是在将来或是今天
     * @return false 不是在将来或是今天
     */
    bool isFuture() const;
};

#endif