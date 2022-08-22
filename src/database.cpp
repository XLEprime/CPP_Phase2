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
#include <QDir>

using namespace std;

void Database::exec(const QSqlQuery &sqlQuery)
{
    qDebug() << "执行SQL语句" << sqlQuery.lastQuery();
    QMap<QString, QVariant> sqlIter(sqlQuery.boundValues());
    for (auto i = sqlIter.begin(); i != sqlIter.end(); i++)
        qDebug() << i.key().toUtf8().data() << ":" << i.value().toString().toUtf8().data();
}

//由于user信息改为文件存储，数据库中只有一个表了，保留以做拓展性。
const QString &Database::getPrimaryKeyByTableName(const QString &tableName)
{
    // static QString username("username");
    static QString id("id");
    // if (tableName == "user")
    //     return username;
    // else
    return id;
}

Database::Database(const QString &connectionName, const QString &fileName) : userFileName(fileName), usernameSet()
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("../data/db.sqlite");
    db.open();

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

    QFile userFile(userFileName);
    if (!userFile.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }

    QTextStream stream;
    bool isAdministratorExist = false;
    stream.setDevice(&userFile);
    int type, balance;
    QString username, password, name, phoneNumber, address;
    char ch;
    while (!stream.atEnd())
    {
        stream >> username >> password >> type >> balance >> name >> phoneNumber >> address;
        stream >> ch;
        usernameSet.insert(username);
        if (username == "admin")
            isAdministratorExist = true;
    }
    if (!isAdministratorExist)
        insertUser("admin", "123", ADMINISTRATOR, 0, "管理员", "88888888", "环宇物流大厦");
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
    if (!usernameSet.contains(username))
    {
        qDebug() << "文件：插入user " << username << " 成功";
        usernameSet.insert(username);
        QFile userFile(userFileName);
        if (!userFile.open(QIODevice::ReadWrite | QIODevice ::Text))
        {
            qCritical() << "user文件打开失败";
            exit(1);
        }
        QTextStream stream(&userFile);

        int tempType, tempBalance;
        QString tempUsername, tempPassword, tempName, tempPhoneNumber, tempAddress;
        char ch;
        while (!stream.atEnd())
        {
            stream >> tempUsername >> tempPassword >> tempType >> tempType >> tempName >> tempPhoneNumber >> tempAddress;
            stream >> ch;
        }
        qDebug() << username << password << type << balance << name << phoneNumber << address;
        stream << username << " " << password << " " << type << " " << balance << " " << name << " " << phoneNumber << " " << address << Qt::endl;
        userFile.close();
    }
    else
        qCritical() << "文件：插入user " << username << "失败"
                    << "该用户已存在文件中";
}

QSharedPointer<User> Database::queryUserByName(const QString &targetUsername) const
{
    QFile userFile(userFileName);
    if (!userFile.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    QTextStream stream(&userFile);
    int type, balance;
    QString username, password, name, phoneNumber, address;
    char ch;

    while (!stream.atEnd())
    {
        stream >> username >> password >> type >> balance >> name >> phoneNumber >> address;
        stream >> ch;
        if (username == targetUsername)
        {
            userFile.close();
            return query2User(username, password, type, balance, name, phoneNumber, address);
        }
    }
    return NULL;
}

int Database::queryBalanceByName(const QString &username) const
{
    QSharedPointer<User> user = queryUserByName(username);
    if (user)
        return user->getBalance();
    else
        return -1;
}

bool Database::modifyUserPassword(const QString &targetUsername, const QString &targetPassword) const
{
    if (!usernameSet.contains(targetUsername))
        return false;

    int type, balance;
    QString username, password, name, phoneNumber, address;
    char ch;
    QFile userFile1(userFileName), userFile2("../data/tempUsers.txt");
    if (!userFile1.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    if (!userFile2.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    QTextStream stream1(&userFile1);
    QTextStream stream2(&userFile2);

    while (!stream1.atEnd())
    {
        stream1 >> username >> password >> type >> balance >> name >> phoneNumber >> address;
        stream1 >> ch; //吃一个回车
        qDebug() << username << password << type << balance << name << phoneNumber << address;
        if (username == targetUsername)
            password = targetPassword;
        stream2 << username << " " << password << " " << type << " " << balance << " " << name << " " << phoneNumber << " " << address << Qt::endl;
    }
    userFile1.close();
    userFile2.close();
    QDir dir;
    dir.remove(userFileName);
    dir.rename("../data/tempUsers.txt", userFileName);
    return true;
}

bool Database::modifyUserBalance(const QString &targetUsername, int targetBalance) const
{
    if (!usernameSet.contains(targetUsername))
        return false;

    int type, balance;
    QString username, password, name, phoneNumber, address;
    char ch;
    QFile userFile1(userFileName), userFile2("../data/tempUsers.txt");
    if (!userFile1.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    if (!userFile2.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    QTextStream stream1(&userFile1);
    QTextStream stream2(&userFile2);

    while (!stream1.atEnd())
    {
        stream1 >> username >> password >> type >> balance >> name >> phoneNumber >> address;
        stream1 >> ch; //吃一个回车
        qDebug() << username << password << type << balance << name << phoneNumber << address;
        if (username == targetUsername)
            balance = targetBalance;
        stream2 << username << " " << password << " " << type << " " << balance << " " << name << " " << phoneNumber << " " << address << Qt::endl;
    }
    userFile1.close();
    userFile2.close();
    QDir dir;
    dir.remove(userFileName);
    dir.rename("../data/tempUsers.txt", userFileName);
    return true;
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
                     " :srcName, :dstName, :expressman, :description)");
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

QSharedPointer<User> Database::query2User(const QString &username, QString &password, int &type, int &balance, QString &name, QString &phoneNumber, QString &address) const
{
    QSharedPointer<User> result;
    switch (type)
    {
    case CUSTOMER:
        result = QSharedPointer<Customer>::create(username, password, balance, name, phoneNumber, address);
        break;
    case ADMINISTRATOR:
        result = QSharedPointer<Administrator>::create(username, password, balance, name, phoneNumber, address);
        break;
    case EXPRESSMAN:
        result = QSharedPointer<Expressman>::create(username, password, balance, name, phoneNumber, address);
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

    QFile userFile(userFileName);
    if (!userFile.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    QTextStream stream(&userFile);
    int type, balance, cnt = 0;
    QString username, password, name, phoneNumber, address;
    char ch;

    while (!stream.atEnd())
    {
        cnt++;
        stream >> username >> password >> type >> balance >> name >> phoneNumber >> address;
        stream >> ch;
        result.append(query2User(username, password, type, balance, name, phoneNumber, address));
    }

    userFile.close();
    return cnt;
}

int Database::queryItemByFilter(QList<QSharedPointer<Item>> &result, int id, int state, const Time &sendingTime, const Time &receivingTime, const QString &srcName, const QString &dstName, const QString &expressman) const
{
    QSqlQuery sqlQuery(db);
    QString queryString("SELECT * FROM item");
    bool flag = false;
    if (id != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "id = :id";
        flag = true;
    }
    if (state != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "state = :state";
        flag = true;
    }
    if (sendingTime.year != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "sendingTime_Year = :sendingTime_Year";
        flag = true;
    }
    if (sendingTime.month != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "sendingTime_Month = :sendingTime_Month";
        flag = true;
    }
    if (sendingTime.day != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "sendingTime_Day = :sendingTime_Day";
        flag = true;
    }
    if (receivingTime.year != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "receivingTime_Year = :receivingTime_Year";
        flag = true;
    }
    if (receivingTime.month != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "receivingTime_Month = :receivingTime_Month";
        flag = true;
    }
    if (receivingTime.day != -1)
    {
        queryString += QString(flag ? " AND " : " WHERE ") + "receivingTime_Day = :receivingTime_Day";
        flag = true;
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
    if (state != -1)
        sqlQuery.bindValue(":state", state);
    if (sendingTime.year != -1)
        sqlQuery.bindValue(":sendingTime_Year", sendingTime.year);
    if (sendingTime.month != -1)
        sqlQuery.bindValue(":sendingTime_Month", sendingTime.month);
    if (sendingTime.day != -1)
        sqlQuery.bindValue(":sendingTime_Day", sendingTime.day);
    if (receivingTime.year != -1)
        sqlQuery.bindValue(":receivingTime_Year", receivingTime.year);
    if (receivingTime.month != -1)
        sqlQuery.bindValue(":receivingTime_Month", receivingTime.month);
    if (receivingTime.day != -1)
        sqlQuery.bindValue(":receivingTime_Day", receivingTime.day);
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

bool Database::modifyItemReceivingTime(const int id, const Time &receivingTime)
{
    bool flag1 = false, flag2 = false, flag3 = false;
    flag1 = modifyData("item", QString::number(id), "receivingTime_Year", receivingTime.year);
    flag2 = modifyData("item", QString::number(id), "receivingTime_Month", receivingTime.month);
    flag3 = modifyData("item", QString::number(id), "receivingTime_Day", receivingTime.day);
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
        return false;
    }
    else
    {
        qDebug() << "数据库删除id为 " << id << " 的项成功";
        return true;
    }
}

bool Database::deleteUser(const QString targetUsername) const
{
    if (!usernameSet.contains(targetUsername))
        return false;

    int type, balance;
    QString username, password, name, phoneNumber, address;
    char ch;
    QFile userFile1(userFileName), userFile2("../data/tempUsers.txt");
    if (!userFile1.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    if (!userFile2.open(QIODevice::ReadWrite | QIODevice ::Text))
    {
        qCritical() << "user文件打开失败";
        exit(1);
    }
    QTextStream stream1(&userFile1);
    QTextStream stream2(&userFile2);

    while (!stream1.atEnd())
    {
        stream1 >> username >> password >> type >> balance >> name >> phoneNumber >> address;
        stream1 >> ch; //吃一个回车
        qDebug() << username << password << type << balance << name << phoneNumber << address;
        if (username != targetUsername)
            stream2 << username << " " << password << " " << type << " " << balance << " " << name << " " << phoneNumber << " " << address << Qt::endl;
    }
    userFile1.close();
    userFile2.close();
    QDir dir;
    dir.remove(userFileName);
    dir.rename("../data/tempUsers.txt", userFileName);
    return true;
}
