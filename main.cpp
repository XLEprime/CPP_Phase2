/**
 * @file main.cpp
 * @author Haolin Yang
 * @brief 主程序
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <QtCore>
#include <QTextStream>
#include "include/user.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stdout, ANSI_COLOR_BLUE "[Debug]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stdout, ANSI_COLOR_YELLOW "[Info]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stdout, ANSI_COLOR_MAGENTA "[Warning]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stdout, ANSI_COLOR_RED "[Critical]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stdout, ANSI_COLOR_RED "[Fatal]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    }
}

int main()
{
    qInstallMessageHandler(messageHandler); // Qt自带的输出详细日志
    Database database("defaultConnection", "../data/users.txt");
    ItemManage itemManage(&database);
    UserManage userManage(&database, &itemManage);

    QTextStream istream(stdin);
    QTextStream ostream(stdout);

    QJsonValue token(QJsonValue::Null);

    QVector<QString> userType{"", "CUSTOMER", "ADMINISTRATOR", "EXPRESSMAN"};
    QVector<QString> itemState{"", "待揽收", "待签收", "已签收"};
    QVector<QString> itemType{"", "易碎品", "图书", "普通快递"};
    QString input;
    Time::init();

    qInfo() << "欢迎使用本物流系统，输入 help 获得帮助。";

    while (true)
    {
        istream.readLineInto(&input);
        QStringList args = input.split(" ");
        args[0] = args[0].toLower();
        bool ok = false;

        if (args[0] == "help" && args.size() == 1)
        {
            qInfo() << "系统时间: time";
            qInfo() << "加快系统时间: addtime <天数>";
            qInfo() << "注册: register <用户名> <密码> <姓名> <电话号码> <地址>";
            qInfo() << "登录: login <用户名> <密码>";
            qInfo() << "登出: logout";
            qInfo() << "修改密码: changepassword <新密码>";
            qInfo() << "查看个人信息: info";
            qInfo() << "查看所有用户信息: alluserinfo";
            qInfo() << "    注意此功能仅限管理员使用。";
            qInfo() << "添加快递员: addexpressman <用户名> <密码> <姓名> <电话号码> <地址>";
            qInfo() << "    注意此功能仅限管理员使用。";
            qInfo() << "删除快递员: deleteexpressman <用户名>";
            qInfo() << "    注意此功能仅限管理员使用。";
            qInfo() << "为一个快递指定快递员: assign <用户名> <物品单号>";
            qInfo() << "    注意此功能仅限管理员使用。";
            qInfo() << "运送一个快递: delivery <物品单号>";
            qInfo() << "    注意此功能仅限快递员使用。";
            qInfo() << "充值: addbalance <增加量>";
            qInfo() << "查询所有快递: queryallitem";
            qInfo() << "    注意此功能仅限管理员使用。";
            qInfo() << "查询所有符合条件的快递: query <物品单号> <寄送时间年> <寄送时间月> <寄送时间日> <接收时间年> <接收时间月> <接收时间日> <寄件用户的用户名> <收件用户的用户名> <快递员的用户名> <快递状态>";
            qInfo() << "    若要查询所有符合该条件的物品，则该条件用*代替。注意此功能仅限管理员使用。其中快递状态：1 待揽收 2 待签收 3 已签收。";
            qInfo() << "查询快递员所属所有符合条件的快递: queryexpress <物品单号> <寄送时间年> <寄送时间月> <寄送时间日> <接收时间年> <接收时间月> <接收时间日> <寄件用户的用户名> <收件用户的用户名> <快递状态>";
            qInfo() << "    注意此功能仅限快递员使用。若要查询所有符合该条件的物品，则该条件用*代替。若要查询全部，可以只输入queryexpress。";
            qInfo() << "查找发出的符合条件的快递: querysrc <物品单号> <寄送时间年> <寄送时间月> <寄送时间日> <接收时间年> <接收时间月> <接收时间日> <收件用户的用户名> <快递状态>";
            qInfo() << "    若要查询所有符合该条件的物品，则该条件用*代替。若要查询全部，可以只输入querysrc。其中快递状态：1 待揽收 2 待签收 3 已签收。";
            qInfo() << "查找将收到的符合条件的快递: querysrc <物品单号> <寄送时间年> <寄送时间月> <寄送时间日> <接收时间年> <接收时间月> <接收时间日> <寄件用户的用户名> <快递状态>";
            qInfo() << "    若要查询所有符合该条件的物品，则该条件用*代替。若要查询全部，可以只输入querydst。其中快递状态：1 待揽收 2 待签收 3 已签收。";
            qInfo() << "发送快递: send <收件用户的用户名> <物品类别> <数量> <描述>";
            qInfo() << "    其中<物品类别>为整数：1 易碎品 2 图书 3普通快递 <数量>为整数： 易碎品单位为斤 图书单位为本 普通快递单位为斤 若为小数则向上取整计算价格";
            qInfo() << "接收快递: receive <物品单号>";
            qInfo() << "退出系统: exit";
        }
        else if (args[0] == "time" && args.size() == 1)
        {
            QJsonObject retInfo;
            QString ret = Time::getTime(retInfo);
            if (ret.isEmpty())
                qInfo() << "查询物流系统时间成功，当前时间为" << retInfo["year"].toInt() << "/" << retInfo["month"].toInt() << "/" << retInfo["day"].toInt();
        }
        else if (args[0] == "addtime" && args.size() == 2 && args[1].toInt(&ok) && ok)
        {
            QString ret = Time::addDays(args[1].toInt());
            if (ret.isEmpty())
                qInfo() << "物流系统时间增加成功。";
        }
        else if (args[0] == "register" && args.size() == 6) //在phase2开始添加type
        {
            if (!token.isNull())
            {
                qInfo() << "当前已有用户登录，请登出后重试。";
                continue;
            }
            QString ret = userManage.registerUser(token.toObject(), args[1], args[2], CUSTOMER, args[3], args[4], args[5]);
            if (ret.isEmpty())
                qInfo() << "用户 " << args[1] << " 注册成功";
            else
                qInfo() << "用户 " << args[1] << " 注册失败" << ret;
        }
        else if (args[0] == "login" && args.size() == 3) // login 账号 密码
        {
            if (!token.isNull())
            {
                qInfo() << "当前已有用户登录，请登出后重试。";
                continue;
            }
            QJsonObject retToken;
            QString ret = userManage.login(args[1], args[2], retToken);
            if (ret.isEmpty())
            {
                qInfo() << "用户 " << args[1] << " 登录成功";
                token = retToken;
            }
            else
                qInfo() << "用户 " << args[1] << " 登录失败" << ret;
        }
        else if (args[0] == "logout" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QString ret = userManage.logout(token.toObject());
            if (ret.isEmpty())
                qInfo() << "已登出";
            else
                qInfo() << "登出失败" << ret;
            token = QJsonValue(QJsonValue::Null);
        }
        else if (args[0] == "changepassword" && args.size() == 2)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QString ret = userManage.changePassword(token.toObject(), args[1]);
            if (ret.isEmpty())
                qInfo() << "修改密码成功";
            else
                qInfo() << "修改密码失败 " << ret;
        }
        else if (args[0] == "info" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject retInfo;
            QString ret = userManage.getUserInfo(token.toObject(), retInfo);
            if (ret.isEmpty())
            {
                qInfo() << "查询用户信息成功 用户名为" << retInfo["username"].toString()
                        << "类型为" << userType[retInfo["type"].toInt()]
                        << "余额为" << retInfo["balance"].toInt()
                        << "姓名为" << retInfo["name"].toString()
                        << "电话为" << retInfo["phonenumber"].toString()
                        << "住址为" << retInfo["address"].toString();
            }
        }
        else if (args[0] == "alluserinfo" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonArray queryRet;
            QString ret = userManage.queryAllUserInfo(token.toObject(), queryRet);
            if (ret.isEmpty())
            {
                qInfo() << "查询成功";
                for (const auto &i : queryRet)
                {
                    QJsonObject user = i.toObject();
                    qInfo() << "查询用户信息成功 用户名为" << user["username"].toString()
                            << "类型为" << userType[user["type"].toInt()]
                            << "余额为" << user["balance"].toInt()
                            << "姓名为" << user["name"].toString()
                            << "电话为" << user["phonenumber"].toString()
                            << "住址为" << user["address"].toString();
                }
            }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "addexpressman" && args.size() == 6)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QString ret = userManage.registerUser(token.toObject(), args[1], args[2], EXPRESSMAN, args[3], args[4], args[5]);
            if (ret.isEmpty())
                qInfo() << "用户 " << args[1] << " 注册成功";
            else
                qInfo() << "用户 " << args[1] << " 注册失败" << ret;
        }
        else if (args[0] == "deleteexpressman" && args.size() == 2)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QString ret = userManage.deleteExpressman(token.toObject(), args[1]);
            if (ret.isEmpty())
                qInfo() << "快递员 " << args[1] << " 删除成功";
            else
                qInfo() << "快递员 " << args[1] << " 删除失败" << ret;
        }
        else if (args[0] == "assign" && args.size() == 3 && args[2].toInt(&ok) && ok)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject info;
            info.insert("expressman", args[1]);
            info.insert("itemId", args[2].toInt());
            QString ret = userManage.assignExpressman(token.toObject(), info);
            if (ret.isEmpty())
                qInfo() << "指派成功";
            else
                qInfo() << "指派失败" << ret;
        }
        else if (args[0] == "delivery" && args.size() == 2 && args[1].toInt(&ok) && ok)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject info;
            info.insert("itemId", args[1].toInt());
            QString ret = userManage.deliveryItem(token.toObject(), info);
            if (ret.isEmpty())
                qInfo()
                    << "运送成功";
            else
                qInfo() << "运送失败" << ret;
        }
        else if (args[0] == "addbalance" && args.size() == 2)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            if (!args[1].toInt(&ok) || !ok)
            {
                qInfo() << "单次余额改变量不能超过1000000000";
                continue;
            }
            QString ret = userManage.addBalance(token.toObject(), args[1].toInt());
            if (ret.isEmpty())
                qInfo() << "余额充值成功";
            else
                qInfo() << "余额充值失败 " << ret;
        }
        else if (args[0] == "queryallitem" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 0);
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt()<< "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "query" && args.size() == 12 && ((args[1] == '*') || args[1].toInt(&ok) && ok) && ((args[2] == '*') || args[2].toInt(&ok) && ok) && ((args[3] == '*') || args[3].toInt(&ok) && ok) && ((args[4] == '*') || args[4].toInt(&ok) && ok) && ((args[5] == '*') || args[5].toInt(&ok) && ok) && ((args[6] == '*') || args[6].toInt(&ok) && ok) && ((args[7] == '*') || args[7].toInt(&ok) && ok) && ((args[11] == '*') || args[11].toInt(&ok) && ok))
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 0);
            if (args[1] != "*")
                filter.insert("id", args[1].toInt());
            if (args[2] != "*")
                filter.insert("sendingTime_Year", args[2].toInt());
            if (args[3] != "*")
                filter.insert("sendingTime_Month", args[3].toInt());
            if (args[4] != "*")
                filter.insert("sendingTime_Day", args[4].toInt());
            if (args[5] != "*")
                filter.insert("receivingTime_Year", args[5].toInt());
            if (args[6] != "*")
                filter.insert("receivingTime_Month", args[6].toInt());
            if (args[7] != "*")
                filter.insert("receivingTime_Day", args[7].toInt());
            if (args[8] != "*")
                filter.insert("srcName", args[8]);
            if (args[9] != "*")
                filter.insert("dstName", args[9]);
            if (args[10] != "*")
                filter.insert("expressman", args[10]);
            if (args[11] != "*")
                filter.insert("state", args[11].toInt());
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "querysrc" && args.size() == 11 && ((args[1] == '*') || args[1].toInt(&ok) && ok) && ((args[2] == '*') || args[2].toInt(&ok) && ok) && ((args[3] == '*') || args[3].toInt(&ok) && ok) && ((args[4] == '*') || args[4].toInt(&ok) && ok) && ((args[5] == '*') || args[5].toInt(&ok) && ok) && ((args[6] == '*') || args[6].toInt(&ok) && ok) && ((args[7] == '*') || args[7].toInt(&ok) && ok) && ((args[10] == '*') || args[10].toInt(&ok) && ok))
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 1);
            if (args[1] != "*")
                filter.insert("id", args[1].toInt());
            if (args[2] != "*")
                filter.insert("sendingTime_Year", args[2].toInt());
            if (args[3] != "*")
                filter.insert("sendingTime_Month", args[3].toInt());
            if (args[4] != "*")
                filter.insert("sendingTime_Day", args[4].toInt());
            if (args[5] != "*")
                filter.insert("receivingTime_Year", args[5].toInt());
            if (args[6] != "*")
                filter.insert("receivingTime_Month", args[6].toInt());
            if (args[7] != "*")
                filter.insert("receivingTime_Day", args[7].toInt());
            if (args[8] != "*")
                filter.insert("dstName", args[8]);
            if (args[9] != "*")
                filter.insert("expressman", args[9]);
            if (args[10] != "*")
                filter.insert("state", args[10].toInt());
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "querysrc" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 1);
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "querydst" && args.size() == 11 && ((args[1] == '*') || args[1].toInt(&ok) && ok) && ((args[2] == '*') || args[2].toInt(&ok) && ok) && ((args[3] == '*') || args[3].toInt(&ok) && ok) && ((args[4] == '*') || args[4].toInt(&ok) && ok) && ((args[5] == '*') || args[5].toInt(&ok) && ok) && ((args[6] == '*') || args[6].toInt(&ok) && ok) && ((args[7] == '*') || args[7].toInt(&ok) && ok) && ((args[10] == '*') || args[10].toInt(&ok) && ok))
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 2);
            if (args[1] != "*")
                filter.insert("id", args[1].toInt());
            if (args[2] != "*")
                filter.insert("sendingTime_Year", args[2].toInt());
            if (args[3] != "*")
                filter.insert("sendingTime_Month", args[3].toInt());
            if (args[4] != "*")
                filter.insert("sendingTime_Day", args[4].toInt());
            if (args[5] != "*")
                filter.insert("receivingTime_Year", args[5].toInt());
            if (args[6] != "*")
                filter.insert("receivingTime_Month", args[6].toInt());
            if (args[7] != "*")
                filter.insert("receivingTime_Day", args[7].toInt());
            if (args[8] != "*")
                filter.insert("srcName", args[8]);
            if (args[9] != "*")
                filter.insert("expressman", args[9]);
            if (args[10] != "*")
                filter.insert("state", args[10].toInt());
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "querydst" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 2);
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "queryexpress" && args.size() == 12 && ((args[1] == '*') || args[1].toInt(&ok) && ok) && ((args[2] == '*') || args[2].toInt(&ok) && ok) && ((args[3] == '*') || args[3].toInt(&ok) && ok) && ((args[4] == '*') || args[4].toInt(&ok) && ok) && ((args[5] == '*') || args[5].toInt(&ok) && ok) && ((args[6] == '*') || args[6].toInt(&ok) && ok) && ((args[7] == '*') || args[7].toInt(&ok) && ok) && ((args[11] == '*') || args[11].toInt(&ok) && ok))
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 3);
            if (args[1] != "*")
                filter.insert("id", args[1].toInt());
            if (args[2] != "*")
                filter.insert("sendingTime_Year", args[2].toInt());
            if (args[3] != "*")
                filter.insert("sendingTime_Month", args[3].toInt());
            if (args[4] != "*")
                filter.insert("sendingTime_Day", args[4].toInt());
            if (args[5] != "*")
                filter.insert("receivingTime_Year", args[5].toInt());
            if (args[6] != "*")
                filter.insert("receivingTime_Month", args[6].toInt());
            if (args[7] != "*")
                filter.insert("receivingTime_Day", args[7].toInt());
            if (args[8] != "*")
                filter.insert("srcName", args[8]);
            if (args[9] != "*")
                filter.insert("dstName", args[9]);
            if (args[10] != "*")
                filter.insert("expressman", args[10]);
            if (args[11] != "*")
                filter.insert("state", args[11].toInt());
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "queryexpress" && args.size() == 1)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject filter;
            filter.insert("type", 3);
            QJsonArray queryRet;
            QString ret = userManage.queryItem(token.toObject(), filter, queryRet);
            if (ret.isEmpty())
                for (const auto &i : queryRet)
                {
                    QJsonObject item = i.toObject();
                    qInfo() << "物品单号为 " << item["id"].toInt() << " 花费为 " << item["cost"].toInt() << "快递类型为 " << itemType[item["type"].toInt()] << " 状态为 " << itemState[item["state"].toInt()] << " 寄送时间为 " << item["sendingTime_Year"].toInt() << "/" << item["sendingTime_Month"].toInt() << "/" << item["sendingTime_Day"].toInt()
                            << " 接收时间为 " << item["receivingTime_Year"].toInt() << "/" << item["receivingTime_Month"].toInt() << "/" << item["receivingTime_Day"].toInt() << "/"
                            << " 寄件人为 " << item["srcName"].toString() << "收件人为" << item["dstName"].toString() << "快递员为" << item["expressman"].toString() << "描述为" << item["description"].toString();
                }
            else
                qInfo() << "查询失败" << ret;
        }
        else if (args[0] == "send" && args.size() == 5 && args[2].toInt(&ok) && ok && args[3].toDouble(&ok) && ok)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }

            int amount = 0;
            if (args[2].toInt() != BOOK)
                amount = ceil(args[3].toDouble()); //向上取整
            else if (args[3].toInt(&ok) && ok)
                amount = args[3].toInt();
            else
            {
                qInfo() << "图书只支持整本寄出";
                continue;
            }

            if (amount <= 0)
            {
                qInfo() << "数量必须为正数";
                continue;
            }

            QJsonObject info;
            info.insert("dstName", args[1]);
            info.insert("type", args[2].toInt());
            info.insert("amount", amount);
            info.insert("description", args[4]);
            QString ret = userManage.sendItem(token.toObject(), info);
            if (ret.toInt(&ok) && ok)
                qInfo() << "快递发送成功，共花费" << ret.toInt() << "元";
            else
                qInfo() << "快递发送失败" << ret;
        }
        else if (args[0] == "receive" && args.size() == 2 && args[1].toInt(&ok) && ok)
        {
            if (token.isNull())
            {
                qInfo() << "当前没有用户登录，请登录后重试。";
                continue;
            }
            QJsonObject info;
            info.insert("id", args[1].toInt());
            QString ret = userManage.receiveItem(token.toObject(), info);
            if (ret.isEmpty())
                qInfo() << "物品接收成功";
            else
                qInfo() << "物品接收失败" << ret;
        }
        else if (args[0] == "exit" && args.size() == 1)
        {
            if (!token.isNull())
                userManage.logout(token.toObject());
            break;
        }
        else
            qInfo() << "指令输入有误，请输入help查看帮助";
    }

    return 0;
}