/**
 * @file database.h
 * @author Haolin Yang
 * @brief 数据库类的声明
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 * @note 对于用户部分, 定义了插入用户(注册), 查询用户, 修改用户密码, 修改用户余额的接口.
 * @note 对于物品部分, 定义了插入物品, 查询物品(根据发送人/接收人/时间/快递单号即id), 修改物品信息, 删除物品的接口.
 * @note 额外定义了部分工具接口，有...
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QFile>
#include <QtSql>

#include "item.h"

class Item;
class Time;
class User;

const int CUSTOMER = 0;
const int ADMINISTRATOR = 1;
/**
 * @brief 数据库类
 */
class Database
{
public:
    QSet<QString> usernameSet; //用户名集合

    /**
     * @brief 删除默认构造函数
     */
    Database() = delete;

    /**
     * @brief 构造函数
     * @param connectionName 连接名称
     * @param fileName 文件名
     *
     * @note 检查是否存在user、item两个table，如果不存在某个表则创建；同时打开用户名文件，将用户名信息读取到usernameSet中。
     *
     */
    Database(const QString &connectionName, const QString &fileName);

    /**
     * @brief 插入用户条目
     *
     * @param username 用户名
     * @param password 密码
     * @param type 用户类型
     * @param balance 余额
     * @param name 姓名
     * @param phoneNumber 电话号码
     * @param address 地址
     */
    void insertUser(const QString &username, const QString &password, int type, int balance, const QString &name, const QString &phoneNumber, const QString &address);

    /**
     * @brief 根据用户名查询用户是否存在
     * @param username 用户名
     * @return true 查询到用户
     * @return false 没查询到用户
     */
    bool queryUserByName(const QString &username) const;

    /**
     * @brief 根据用户名查询用户是否存在且返回密码，用户类型，余额
     *
     * @param username 用户名
     * @param retPassword 返回密码
     * @param retType 返回UserType
     * @param retBalance 返回余额
     * @param retName 返回姓名
     * @param retPhoneNumber 返回电话号码
     * @param retAddress 返回地址
     * @return true 查询到用户
     * @return false 没查询到用户
     */
    bool queryUserByName(const QString &username, QString &retPassword, int &retType, int &retBalance, QString &retName, QString &retPhoneNumber, QString &retAddress) const;

    /**
     * @brief 获得用户名对应的余额
     * @param username
     * @return int 余额
     */
    int queryBalanceByName(const QString &username) const;

    /**
     * @brief 修改用户密码
     *
     * @param username 用户名
     * @param password 新密码
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyUserPassword(const QString &username, const QString &password) const;

    /**
     * @brief 修改用户余额
     *
     * @param username 用户名
     * @param balance 改后的余额
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyUserBalance(const QString &username, int balance) const;

    /**
     * @brief 查询表中主键的最大值
     * @param tableName 数据库表名
     * @return 返回最大主键允许的s值
     */
    int getDBMaxId(const QString &tableName) const;

    /**
     * @brief 插入物品
     *
     * @param id 主键
     * @param cost 总花费
     * @param type 物品类型
     * @param state 物品状态
     * @param sendingTime 寄送时间
     * @param receivingTime 接收时间
     * @param srcName 寄件用户的用户名
     * @param dstName 收件用户的用户名
     * @param description 物品描述
     */
    void insertItem(int id, int cost, int type, int state, const Time &sendingTime, const Time &receivingTime, const QString &srcName, const QString &dstName, const QString &description);

    /**
     * @brief 将数据库的Item查询结果转换成指向Item的指针
     * @param sqlQuery Item类的查询结果
     * @return QSharedPointer<Item> 一个指向新创建的Item类的指针
     */
    QSharedPointer<Item> query2Item(const QSqlQuery &sqlQuery) const;

    /**
     * @brief 根据条件查询物品
     * @param result 用于返回结果
     * @param id 物品单号
     * @param sendingTime 寄送时间
     * @param receivingTime 接收时间
     * @param srcName 寄件用户的用户名
     * @param dstName 收件用户的用户名
     * @return int 查到符合条件的数量
     */
    int queryItemByFilter(QList<QSharedPointer<Item>> &result, int id, const Time &sendingTime, const Time &receivingTime, const QString &srcName, const QString &dstName) const;

    /**
     * @brief 修改物品状态
     * @param id 物品单号
     * @param state 物品状态
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyItemState(const int id, const int state);

    /**
     * @brief 修改物品接收时间
     * @param id 物品单号
     * @param receivingTime 接收时间
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyItemReceivingTime(const int id, const Time receivingTime);

    /**
     * @brief 删除物品
     * @param id 物品单号
     * @return true 删除成功
     * @return false 删除失败
     */
    bool deleteItem(const int id) const;

private:
    QSqlDatabase db;    // SQLite数据库
    QFile userFile;     //永久存储用户名文件
    QTextStream stream; //用户读写用户名的stream

    /**
     * @brief 执行SQL语句
     * @param sqlQuery
     */
    static void exec(const QSqlQuery &sqlQuery);

    /**
     * @brief 通过数据库表名获得该表的主键
     * @param tableName 数据库表名
     * @return const QString& 主键
     *
     * @todo 是否可以不用static
     */
    static const QString &getPrimaryKeyByTableName(const QString &tableName);

    /**
     * @brief 修改数据库中某个记录的值，值为int类型，对应数据库的INT类型。
     * @param tableName 数据库表名
     * @param primaryKey 需要修改的记录的主键
     * @param key 需要修改的键
     * @param value 修改的值
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyData(const QString &tableName, const QString &primaryKey, const QString &key, int value) const;

    /**
     * @brief 修改数据库中某个记录的值，值为QString类型，对应数据库的TEXT类型。
     * @param tableName 数据库表名
     * @param primaryKey 需要修改的记录的主键
     * @param key 需要修改的键
     * @param value 修改的值
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyData(const QString &tableName, const QString &primaryKey, const QString &key, const QString value) const;
};

#endif