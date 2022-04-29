/**
 * @file user.h
 * @author Haolin Yang
 * @brief 用户和用户管理
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 * @note 定义了用户和用户管理类
 * @note 用户类包括一个抽象的用户基类(其中getUserType函数为纯虚函数), 以及普通用户类(CUSTOMER)和管理员(ADMINISTRATOR)两个派生类.
 * @note 用户类中保存用户的余额.
 * @note 用户类的对象都是临时对象.
 * @note 用户管理类中定义了大多操作和鉴权.
 * @note 简单的成员函数直接在此文件给出实现.
 * @note 管理员不需要支持注册!!!
 */

#ifndef USER_H
#define USER_H

#define DEBUG

#include "database.h"
#include "item.h"
#include "time.h"

extern const int CUSTOMER;
extern const int ADMINISTRATOR;

extern const int RECEIVED;          //已签收
extern const int PENDING_REVEICING; //待签收

extern const int FRAGILE; //易碎物
extern const int BOOK;    //图书
extern const int NORMAL;  //普通快递

/**
 * @brief 用户基类
 * @note 纯虚类, 其中getUserType为纯虚函数.
 */
class User
{
public:
    User() = delete;

    /**
     * @brief 构造函数
     *
     * @param _balance 余额
     */

    /**
     * @brief 构造函数
     *
     * @param _username 用户名
     * @param _password 密码
     * @param _balance 余额
     * @param _name 姓名
     * @param _phoneNumber 电话号码
     * @param _address 地址
     */
    User(const QString &_username, const QString &_password, int _balance, const QString &_name, const QString &_phoneNumber, const QString &_address) : username(_username), password(_password), balance(_balance), type(-1), name(_name), phoneNumber(_phoneNumber), address(_address)
    {
#ifdef DEBUG
        qDebug() << "构造user";
#endif
    }

    /**
     * @brief 析构函数
     * @note 虚函数
     */
    virtual ~User()
    {
#ifdef DEBUG
        qDebug() << "析构user";
#endif
    };

    /**
     * @brief 获得用户名
     * @return const QString& 用户名
     */
    const QString &getUsername() const { return username; }

    /**
     * @brief 获得密码
     * @return const QString& 密码
     */
    const QString &getPassword() const { return password; }

    /**
     * @brief 获得用户余额
     * @return int 余额
     */
    int getBalance() const { return balance; }

    /**
     * @brief 获得用户类型
     * @return int 返回UserType
     */
    virtual int getUserType() const = 0;

    /**
     * @brief 获得姓名
     * @return const QString& 姓名
     */
    const QString &getName() const { return name; }

    /**
     * @brief 获得电话号码
     * @return const QString& 电话号码
     */
    const QString &getPhoneNumber() const { return phoneNumber; }

    /**
     * @brief 获得地址
     * @return const QString& 地址
     */
    const QString &getAddress() const { return address; }

    /**
     * @brief 改变余额
     * @param addend 余额增量
     */
    void addBalance(int addend) { balance += addend; }

protected:
    QString username;    //用户名
    QString password;    //用户密码
    int balance;         //余额
    int type;            //用户类型
    QString name;        //姓名
    QString phoneNumber; //电话号码
    QString address;     //地址
};

/**
 * @brief 用户类
 */
class Customer : public User
{
public:
    Customer() = delete;

    /**
     * @brief 构造函数
     *
     * @param _username 用户名
     * @param _password 密码
     * @param _balance 余额
     * @param _name 姓名
     * @param _phoneNumber 电话号码
     * @param _address 地址
     */
    Customer(const QString &_username, const QString &_password, int _balance, const QString &_name, const QString &_phoneNumber, const QString &_address) : User(_username, _password, _balance, _name, _phoneNumber, _address) { type = CUSTOMER; }

    /**
     * @brief 获得用户类
     * @return int 返回UserType
     * @note 1为CUSTOMER 2为ADMINISTRATOR
     */
    int getUserType() const override { return CUSTOMER; }
};

/**
 * @brief 管理员类
 */
class Administrator : public User
{
public:
    Administrator() = delete;

    /**
     * @brief 构造函数
     *
     * @param _username 用户名
     * @param _password 密码
     * @param _balance 余额
     * @param _type UserType
     * @param _name 姓名
     * @param _phoneNumber 电话号码
     * @param _address 地址
     */
    Administrator(const QString &_username, const QString &_password, int _balance, const QString &_name, const QString &_phoneNumber, const QString &_address) : User(_username, _password, _balance, _name, _phoneNumber, _address) { type = ADMINISTRATOR; }

    /**
     * @brief 获得用户类
     * @return int 返回UserType
     * @note 0为CUSTOMER 1为ADMINISTRATOR
     */
    int getUserType() const override { return ADMINISTRATOR; }
};

/**
 * @brief 用户管理类, 类似于工厂模式, 对象的创建和对象的使用分离.
 */
class UserManage
{
public:
    /**
     * @brief 禁止默认的构造函数
     * @note 用不上
     */
    UserManage() = delete;

    UserManage(Database *_db, ItemManage *_itemManage) : db(_db), itemManage(_itemManage) {}

    /**
     * @brief 注册
     *
     * @param username 用户名
     * @param password 密码
     * @param type 注册的账号类型 0为CUSTOMER, 2为EXPRESSMAN(Phase2开始出现)
     * @param name 姓名
     * @param phoneNumber 电话号码
     * @param address 地址
     * @return QString 如果注册成功，返回空串，否则返回错误信息.
     * @note ADMINISTRATOR不用支持注册.
     * @note register是关键字，不能作为函数名.
     */
    QString registerUser(const QString &username, const QString &password, int type, const QString &name, const QString &phoneNumber, const QString &address) const;

    /**
     * @brief 登录
     * @param username 用户名
     * @param password 密码
     * @param token 生成的凭据
     * @return QString 如果登录成功，返回空串，否则返回错误信息.
     */
    QString login(const QString &username, const QString &password, QJsonObject &token);

    /**
     * @brief 登出
     *
     * @param token 凭据
     * @return QString 如果登出成功，返回空串，否则返回错误信息.
     */
    QString logout(const QJsonObject &token);

    /**
     * @brief 更改密码
     *
     * @param token 凭据
     * @param newPassword 新密码
     * @return QString 如果更改成功，返回空串，否则返回错误信息.
     */
    QString changePassword(const QJsonObject &token, const QString &newPassword) const;

    /**
     * @brief 获取用户信息
     * @param token 凭据
     * @param ret 用户信息
     * @return 如果获取成功，返回空串，否则返回错误信息
     *
     * 用户信息的格式：
     * ```json
     * {
     *    "username": <字符串>,
     *    "type": <整数>,
     *    "balance": <整数>
     *    "name": <字符串>,
     *    "phonenumber": <字符串>,
     *    "address": <字符串>,
     * }
     * ```
     */
    QString getUserInfo(const QJsonObject &token, QJsonObject &ret) const;

    /**
     * @brief 获取用户信息
     * @param token 凭据
     * @param ret 用户信息数组
     * @return 如果获取成功，返回空串，否则返回错误信息
     *
     * 用户信息的格式：
     * ```json
     * {
     *    "username": <字符串>,
     *    "type": <整数>,
     *    "balance": <整数>
     *    "name": <字符串>,
     *    "phonenumber": <字符串>,
     *    "address": <字符串>,
     * }
     * ```
     */
    QString queryAllUserInfo(const QJsonObject &token, QJsonArray &ret) const;

    /**
     * @brief 更改余额(单用户)
     * @param token 凭据
     * @param addend 余额增量
     * @return QString 成功则返回返回空串，失败则返回错误信息.(用于phase3弹窗报错)
     */
    QString addBalance(const QJsonObject &token, int addend) const;

    /**
     * @brief 按照条件查询商品，条件以Json给出。
     * @param woken 用户鉴权
     * @param filter 条件
     * @param ret 查询结果
     * @return QString 查询成功则返回空串，否则返回错误信息
     *
     * @note 查询格式有:
     * 1. 查询所有符合条件的商品
     * ```json
     * {
     *      "type": 0
     *      可选："id" : <整数>,
     *      可选："sendingTime_Year" : <整数>,
     *      可选："sendingTime_Month" : <整数>,
     *      可选："sendingTime_Day" : <整数>,
     *      可选："receivingTime_Year" : <整数>,
     *      可选："receivingTime_Month" : <整数>,
     *      可选："receivingTime_Day" : <整数>,
     *      可选："dstName" : <字符串>
     * }
     * ```
     * 2. 按条件查询该用户发出的所有物品
     * ```json
     * {
     *      "type": 1,
     *      可选："id" : <整数>,
     *      可选："sendingTime_Year" : <整数>,
     *      可选："sendingTime_Month" : <整数>,
     *      可选："sendingTime_Day" : <整数>,
     *      可选："receivingTime_Year" : <整数>,
     *      可选："receivingTime_Month" : <整数>,
     *      可选："receivingTime_Day" : <整数>,
     *      可选："dstName" : <字符串>
     * }
     * ```
     * 2. 按条件查询该用户将收到的所有物品
     * ```json
     * {
     *      "type": 2,
     *      可选："id" : <整数>,
     *      可选："sendingTime_Year" : <整数>,
     *      可选："sendingTime_Month" : <整数>,
     *      可选："sendingTime_Day" : <整数>,
     *      可选："receivingTime_Year" : <整数>,
     *      可选："receivingTime_Month" : <整数>,
     *      可选："receivingTime_Day" : <整数>,
     *      可选："srcName" : <字符串>
     * }
     * ```
     */
    QString queryItem(const QJsonObject &token, const QJsonObject &filter, QJsonArray &ret) const;

    /**
     * @brief 发送快递物品
     * @param token 凭据
     * @param info 快递物品信息
     * @return QString
     * @note 物品信息格式:
     * ```json
     * {
     *      "dstName" : <字符串>
     *      "type" : <整数>
     *      "amount" : <整数>
     *      "description" : <字符串>
     * }
     */
    QString sendItem(const QJsonObject &token, const QJsonObject &info) const;

    /**
     * @brief 发送快递物品
     * @param token 凭据
     * @param info 快递物品信息
     * @return QString
     * @note 物品信息格式:
     * ```json
     * {
     *      "id" : <整数>
     * }
     */
    QString receiveItem(const QJsonObject &token, const QJsonObject &info) const;

private:
    QMap<QString, QSharedPointer<User>> userMap; //用户名到用户对象的映射.
    Database *db;                                //数据库
    ItemManage *itemManage;                      //物品管理类

    /**
     * @brief 用户鉴权
     * @param token 凭据
     * @return QString 鉴权成功则返回用户名，失败则返回空串.
     * @note 空串可用isEmpty判断.
     */
    QString verify(const QJsonObject &token) const;

    /**
     * @brief 转钱: 减少一个用户的余额，增加另一个用户的余额。
     * @param token 第一个用户（减去转移余额量的用户）的token
     * @param balance 转移余额量
     * @param srcUser 第二个用户（加上转移余额量的用户）的用户名
     * @return QString 转钱成功，返回空串，否则返回错误信息.
     * @note 转移余额量可以为负
     */
    QString transferBalance(const QJsonObject &token, int balance, const QString &dstUser) const;
};

#endif