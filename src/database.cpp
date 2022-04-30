/**
 * @file database.cpp
 * @author Haolin Yang
 * @brief 数据库类的实现
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "../include/database.h"
#include <QDebug>

using namespace std;

void Database::exec(const QSqlQuery &sqlQuery)
{
    qDebug() << "执行SQL语句" << sqlQuery.lastQuery();
    QMap<QString, QVariant> sqlIter(sqlQuery.boundValues());
    for (auto i = sqlIter.begin(); i != sqlIter.end(); i++)
        qDebug() << i.key().toUtf8().data() << ":" << i.value().toString().toUtf8().data();
}

const QString &Database::getPrimaryKeyByTableName(const QString &tableName)
{
    static QString username("username");
    static QString id("id");
    if (tableName == "user")
        return username;
    else
        return id;
}

Database::Database(const QString &connectionName, const QString &fileName) : userFile(fileName), usernameSet()
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("MyDataBase.sqlite");
    db.open();

    if (!db.tables().contains("user")) //若不包含user，则创建。
    {
        QSqlQuery sqlQuery(db);
        sqlQuery.prepare("CREATE TABLE user( username TEXT PRIMARY KEY NOT NULL,"
                         "password TEXT NOT NULL,"
                         "type INT NOT NULL,"
                         "balance INT NOT NULL,"
                         "name TEXT NOT NULL,"
                         "phonenumber TEXT NOT NULL,"
                         "address TEXT NOT NULL) ");

        exec(sqlQuery);
        if (!sqlQuery.exec())
            qCritical() << "user表创建失败" << sqlQuery.lastError();
        else
            qDebug() << "user表创建成功";
    }
    else
        qDebug() << "user表已存在";

    if (!db.tables().contains("item")) //若不包含item，则创建。
    {
        QSqlQuery sqlQuery(db);
        sqlQuery.prepare("CREATE TABLE item( id INT PRIMARY KEY NOT NULL,"
                         "cost INT NOT NULL,"
                         "type INT NOT NULL,"
                         "state INT NOT NULL,"
                         "sendingTime_Year INT NOT NULL,"
                         "sendingTime_Month INT NOT NULL,"
                         "sendingTime_Day INT NOT NULL,"
                         "receivingTime_Year INT NOT NULL,"
                         "receivingTime_Month INT NOT NULL,"
                         "receivingTime_Day INT NOT NULL,"
                         "srcName TEXT NOT NULL,"
                         "dstName TEXT NOT NULL,"
                         "expressman TEXT NOT NULL,"
                         "description TEXT NOT NULL) ");

        exec(sqlQuery);
        if (!sqlQuery.exec())
            qCritical() << "item表创建失败" << sqlQuery.lastError();
        else
            qDebug() << "item表创建成功";
    }
    else
        qDebug() << "item表已存在";

    if (!userFile.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    bool isAdministratorExist = false;
    stream.setDevice(&userFile);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        usernameSet.insert(line);
        if (line == "ADMINISTRATOR")
            isAdministratorExist = true;
    }
    if (!isAdministratorExist)
        insertUser("ADMINISTRATOR", "123", ADMINISTRATOR, 0, "Administrator1", "88888888", "环宇物流大厦");
}

bool Database::modifyData(const QString &tableName, const QString &primaryKey, const QString &key, int value) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("UPDATE " + tableName + " SET " + key + " = :value WHERE " + getPrimaryKeyByTableName(tableName) + " = :primaryKey");
    sqlQuery.bindValue(":value", value);
    sqlQuery.bindValue(":primaryKey", primaryKey);

    exec(sqlQuery);
    if (sqlQuery.exec())
    {
        qDebug() << "数据库: " << key << " : "
                 << value
                 << " 修改成功";
        return true;
    }
    else
    {
        qCritical() << "数据库: " << key << " : "
                    << value
                    << " 修改失败" << sqlQuery.lastError();
        return false;
    }
}

bool Database::modifyData(const QString &tableName, const QString &primaryKey, const QString &key, const QString value) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("UPDATE " + tableName + " SET " + key + " = :value WHERE " + getPrimaryKeyByTableName(tableName) + " = :primaryKey");
    sqlQuery.bindValue(":value", value);
    sqlQuery.bindValue(":primaryKey", primaryKey);

    exec(sqlQuery);
    if (sqlQuery.exec())
    {
        qDebug() << "数据库: " << key << " : "
                 << value
                 << " 修改成功";
        return true;
    }
    else
    {
        qCritical() << "数据库: " << key << " : "
                    << value
                    << " 修改失败" << sqlQuery.lastError();
        return false;
    }
}

void Database::insertUser(const QString &username, const QString &password, int type, int balance, const QString &name, const QString &phoneNumber, const QString &address)
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("INSERT INTO user VALUES(:username, :password, :type, :balance, :name, :phonenumber, :address)");
    sqlQuery.bindValue(":username", username);
    sqlQuery.bindValue(":password", password);
    sqlQuery.bindValue(":type", type);
    sqlQuery.bindValue(":balance", balance);
    sqlQuery.bindValue(":name", name);
    sqlQuery.bindValue(":phonenumber", phoneNumber);
    sqlQuery.bindValue(":address", address);

    exec(sqlQuery);
    if (sqlQuery.exec())
    {
        qDebug() << "数据库:插入user " << username << " 成功";
        usernameSet.insert(username);
        stream << username << endl; // 向文件中写入用户名
    }
    else
        qCritical() << "数据库:插入user " << username << " 失败" << sqlQuery.lastError();
}

bool Database::queryUserByName(const QString &username) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("SELECT * FROM user WHERE username = :username");
    sqlQuery.bindValue(":username", username);

    exec(sqlQuery);
    if (sqlQuery.exec())
    {
        qDebug() << "数据库:查找user" << username << "成功";
        if (sqlQuery.next())
        {
            if (usernameSet.find(username) != usernameSet.constEnd())
                qDebug() << "文件中查找到用户 " << username;
            else
                qFatal("数据库和文件不同步");
            return true;
        }
        qDebug() << "数据库:" << username << "在数据库中不存在";
        return false;
    }
    else
    {
        qCritical() << "数据库:没有username为" << username << "的记录" << sqlQuery.lastError();
        return false;
    }
}

bool Database::queryUserByName(const QString &username, QString &retPassword, int &retType, int &retBalance, QString &retName, QString &retPhoneNumber, QString &retAddress) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("SELECT password, type, balance, name, phonenumber, address FROM user WHERE username = :username");
    sqlQuery.bindValue(":username", username);

    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库: 用户 " << username << " 查找失败" << sqlQuery.lastError();
        return false;
    }
    else
    {
        if (sqlQuery.next())
        {
            retPassword = sqlQuery.value(0).toString();
            retType = sqlQuery.value(1).toInt();
            retBalance = sqlQuery.value(2).toInt();
            retName = sqlQuery.value(3).toString();
            retPhoneNumber = sqlQuery.value(4).toString();
            retAddress = sqlQuery.value(5).toString();

            qDebug() << "数据库: 用户 " << username << "查找成功";
            return true;
        }
        else
        {
            qCritical() << "数据库: 用户 " << username << " 查找失败" << sqlQuery.lastError();
            return false;
        }
    }
}

int Database::queryBalanceByName(const QString &username) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("SELECT balance FROM user WHERE username = :username");
    sqlQuery.bindValue(":username", username);

    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库:查找user " << username << " 失败" << sqlQuery.lastError();
        return 0;
    }
    else
    {
        qDebug() << "数据库:查找user " << username << " 成功";
        if (sqlQuery.next())
            return sqlQuery.value(0).toInt();
        else
        {
            qFatal("数据库: user 没有balance项"); // todo: 增加username的值进Fatal提示中
            return 0;
        }
    }
}

bool Database::modifyUserPassword(const QString &username, const QString &password) const
{
    return modifyData("user", username, "password", password);
}

bool Database::modifyUserBalance(const QString &username, int balance) const
{
    return modifyData("user", username, "balance", balance);
}

int Database::getDBMaxId(const QString &tableName) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("SELECT MAX(id) FROM " + tableName);

    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库:获得表 " << tableName << " 中主键的最大ID失败";
        return 0;
    }
    else
    {
        qDebug() << "数据库:获得表 " << tableName << " 中主键的最大ID成功.";
        if (sqlQuery.next())
            return sqlQuery.value(0).toInt();
        return 0;
    }
}

void Database::insertItem(int id, int cost, int type, int state, const Time &sendingTime, const Time &receivingTime, const QString &srcName, const QString &dstName, const QString &expressman, const QString &description)
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("INSERT INTO item VALUES(:id, :cost, :type, :state,"
                     " :sendingTime_Year, :sendingTime_Month, :sendingTime_Day,"
                     " :receivingTime_Year, :receivingTime_Month, :receivingTime_Day,"
                     " :srcName, :dstName, :description)"); // phase2开始添加type
    sqlQuery.bindValue(":id", id);
    sqlQuery.bindValue(":cost", cost);
    sqlQuery.bindValue(":type", type);
    sqlQuery.bindValue(":state", state);
    sqlQuery.bindValue(":sendingTime_Year", sendingTime.year);
    sqlQuery.bindValue(":sendingTime_Month", sendingTime.month);
    sqlQuery.bindValue(":sendingTime_Day", sendingTime.day);
    sqlQuery.bindValue(":receivingTime_Year", receivingTime.year);
    sqlQuery.bindValue(":receivingTime_Month", receivingTime.month);
    sqlQuery.bindValue(":receivingTime_Day", receivingTime.day);
    sqlQuery.bindValue(":srcName", srcName);
    sqlQuery.bindValue(":dstName", dstName);
    sqlQuery.bindValue(":expressman", expressman);
    sqlQuery.bindValue(":description", description);
    exec(sqlQuery);
    if (!sqlQuery.exec())
        qCritical() << "数据库:插入id为 " << id << " 的物品项失败 " << sqlQuery.lastError();
    else
        qDebug() << "数据库:插入id为 " << id << " 的物品项成功 ";
}

QSharedPointer<User> Database::query2User(const QSqlQuery &sqlQuery) const
{
    QSharedPointer<User> result;
    switch (sqlQuery.value(2).toInt())
    {
    case CUSTOMER:
        result = QSharedPointer<Customer>::create(sqlQuery.value(0).toString(), sqlQuery.value(1).toString(), sqlQuery.value(3).toInt(), sqlQuery.value(4).toString(), sqlQuery.value(5).toString(), sqlQuery.value(6).toString());
        break;
    case ADMINISTRATOR:
        result = QSharedPointer<Administrator>::create(sqlQuery.value(0).toString(), sqlQuery.value(1).toString(), sqlQuery.value(3).toInt(), sqlQuery.value(4).toString(), sqlQuery.value(5).toString(), sqlQuery.value(6).toString());
        break;
    case EXPRESSMAN:
        result = QSharedPointer<Expressman>::create(sqlQuery.value(0).toString(), sqlQuery.value(1).toString(), sqlQuery.value(3).toInt(), sqlQuery.value(4).toString(), sqlQuery.value(5).toString(), sqlQuery.value(6).toString());
        break;
    }

    return result;
}

QSharedPointer<Item> Database::query2Item(const QSqlQuery &sqlQuery) const
{
    Time sendingTime{sqlQuery.value(4).toInt(), sqlQuery.value(5).toInt(), sqlQuery.value(6).toInt()};
    Time receivingTime{sqlQuery.value(7).toInt(), sqlQuery.value(8).toInt(), sqlQuery.value(9).toInt()};

    QSharedPointer<Item> result;
    switch (sqlQuery.value(2).toInt())
    {
    case FRAGILE:
        result = QSharedPointer<FragileItem>::create(sqlQuery.value(0).toInt(), sqlQuery.value(1).toInt(), sqlQuery.value(3).toInt(), sendingTime, receivingTime, sqlQuery.value(10).toString(), sqlQuery.value(11).toString(), sqlQuery.value(12).toString(), sqlQuery.value(13).toString());
        break;
    case BOOK:
        result = QSharedPointer<Book>::create(sqlQuery.value(0).toInt(), sqlQuery.value(1).toInt(), sqlQuery.value(3).toInt(), sendingTime, receivingTime, sqlQuery.value(10).toString(), sqlQuery.value(11).toString(), sqlQuery.value(12).toString(), sqlQuery.value(13).toString());
        break;
    case NORMAL:
        result = QSharedPointer<NormalItem>::create(sqlQuery.value(0).toInt(), sqlQuery.value(1).toInt(), sqlQuery.value(3).toInt(), sendingTime, receivingTime, sqlQuery.value(10).toString(), sqlQuery.value(11).toString(), sqlQuery.value(12).toString(), sqlQuery.value(13).toString());
        break;
    }

    return result;
}

int Database::queryAllUser(QList<QSharedPointer<User>> &result)
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("SELECT * FROM user");

    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库:查找用户失败" << sqlQuery.lastError();
        return 0;
    }
    else
    {
        int cnt = 0;
        while (sqlQuery.next())
        {
            result.append(query2User(sqlQuery)); //将查找结果转换为临时User对象
            cnt++;
        }
        qDebug() << "数据库:查找用户成功，共" << cnt << "条";
        return cnt;
    }
}

int Database::queryItemByFilter(QList<QSharedPointer<Item>> &result, int id, const Time &sendingTime, const Time &receivingTime, const QString &srcName, const QString &expressman, const QString &dstName) const
{
    QSqlQuery sqlQuery(db);
    QString queryString("SELECT * FROM item");
    bool flag = false;
    if (id != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "id = :id";
        flag = true;
    }
    if (sendingTime.day != -1 || sendingTime.month != -1 || sendingTime.day != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "sendingTime_Year = :sendingTime_Year";
        flag = true;
        queryString += QString(flag ? " AND " : " WHERE ") + "sendingTime_Month = :sendingTime_Month";
        queryString += QString(flag ? " AND " : " WHERE ") + "sendingTime_Day = :sendingTime_Day";
    }
    if (receivingTime.day != -1 || receivingTime.month != -1 || receivingTime.day != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "receivingTime_Year = :receivingTime_Year";
        flag = true;
        queryString += QString(flag ? " AND " : " WHERE ") + "receivingTime_Month = :receivingTime_Month";
        queryString += QString(flag ? " AND " : " WHERE ") + "receivingTime_Day = :receivingTime_Day";
    }
    if (!srcName.isEmpty())
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "srcName = :srcName";
        flag = true;
    }
    if (!dstName.isEmpty())
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "dstName = :dstName";
        flag = true;
    }
    if (!expressman.isEmpty())
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "expressman = :expressman";
        flag = true;
    }
    sqlQuery.prepare(queryString);

    if (id != -1)
        sqlQuery.bindValue(":id", id);
    if (sendingTime.day != -1 || sendingTime.month != -1 || sendingTime.day != -1)
    {
        sqlQuery.bindValue(":sendingTime_Year", sendingTime.year);
        sqlQuery.bindValue(":sendingTime_Month", sendingTime.month);
        sqlQuery.bindValue(":sendingTime_day", sendingTime.day);
    }
    if (receivingTime.day != -1 || receivingTime.month != -1 || receivingTime.day != -1)
    {
        sqlQuery.bindValue(":receivingTime_Year", receivingTime.year);
        sqlQuery.bindValue(":receivingTime_Month", receivingTime.month);
        sqlQuery.bindValue(":receivingTime_day", receivingTime.day);
    }
    if (!srcName.isEmpty())
        sqlQuery.bindValue(":srcName", srcName);
    if (!dstName.isEmpty())
        sqlQuery.bindValue(":dstName", dstName);
    if (!expressman.isEmpty())
        sqlQuery.bindValue(":expressman", expressman);

    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库:查找物品失败" << sqlQuery.lastError();
        return 0;
    }
    else
    {
        int cnt = 0;
        while (sqlQuery.next())
        {
            result.append(query2Item(sqlQuery)); //将查找结果转换为临时Item对象
            cnt++;
        }
        qDebug() << "数据库:查找物品成功，共" << cnt << "条";
        return cnt;
    }
}

bool Database::modifyItemState(const int id, const int state)
{
    return modifyData("item", QString::number(id), "state", state);
}

bool Database::modifyItemExpressman(const int id, const QString &expressman)
{
    return modifyData("item", QString::number(id), "expressman", expressman);
}

bool Database::modifyItemReceivingTime(const int id, const Time receivingTime)
{
    bool flag1 = false, flag2 = false, flag3 = false;
    flag1 = modifyData("item", QString::number(id), "receivingTime_Year", receivingTime.year);
    flag2 = modifyData("item", QString::number(id), "receivingTime_Month", receivingTime.month);
    flag2 = modifyData("item", QString::number(id), "receivingTime_Day", receivingTime.day);
    return flag1 && flag2 && flag3;
}

bool Database::deleteItem(const int id) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("DELETE FROM item WHERE id = :id");
    sqlQuery.bindValue(":id", id);
    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库删除id为 " << id << " 的项失败";
        return true;
    }
    else
    {
        qDebug() << "数据库删除id为 " << id << " 的项成功";
        return false;
    }
}

bool Database::deleteUser(const QString username) const
{
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("DELETE FROM user WHERE username = :username");
    sqlQuery.bindValue(":username", username);
    exec(sqlQuery);
    if (!sqlQuery.exec())
    {
        qCritical() << "数据库删除username为 " << username << " 的项失败";
        return true;
    }
    else
    {
        qDebug() << "数据库删除username为 " << username << " 的项成功";
        return false;
    }
}
