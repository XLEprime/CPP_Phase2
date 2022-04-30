/**
 * @file item.h
 * @author Haolin Yang
 * @brief 物品和物品管理
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 * @note 定义了物品和物品管理类.
 * @note 物品类暂不需要继承(pahse2开始需要).
 * @note 物品类中包含两种状态:已签收，待签收(pahse2开始有三种)，包含6种属性:寄送时间、接收时间、寄件用户、收件用户、物品状态、物品描述
 */

#ifndef ITEM_H
#define ITEM_H

#define DEBUG

#include <QSharedPointer>
#include <QDebug>
#include "time.h"

const int PENDING_COLLECTING = 1; //待揽收
const int PENDING_REVEICING = 2;  //待签收
const int RECEIVED = 3;           //已签收

const int FRAGILE = 1; //易碎物
const int BOOK = 2;    //图书
const int NORMAL = 3;  //普通快递

const int FRAGILE_ITEM_PRICE = 8; //易碎品单价
const int BOOK_PRICE = 2;         //图书单价
const int NORMAL_ITEM_PRICE = 5;  //普通快递单价

class Database;
class Time;

/**
 * @brief 物品基类
 */
class Item
{
public:
    /**
     * @brief 删除默认构造函数
     * @note 用不上
     */
    Item() = delete;

    /**
     * @brief 构造函数
     *
     * @param _id 物品ID 主键
     * @param _state 物品状态
     * @param _sendingTime 寄送时间
     * @param _receivingTime 接收时间
     * @param _srcName 寄件用户的用户名
     * @param _dstName 收件用户的用户名
     * @param _expressman 快递员
     * @param _description 物品描述
     * @note 注意是否使用std::move
     */
    Item(int _id, int _cost, int _state, Time _sendingTime, Time _receivingTime, QString _srcName, QString _dstName, QString _expressman, QString _description) : id(_id), cost(_cost), state(_state), sendingTime(_sendingTime), receivingTime(_receivingTime), srcName(_srcName), dstName(_dstName), expressman(_expressman), description(_description)
    {
#ifdef DEBUG
        qDebug() << "构造item";
#endif
    }

    virtual ~Item()
    {
#ifdef DEBUG
        qDebug() << "析构item";
#endif
    }

    /**
     * @brief 获得物品id
     * @return int 物品id
     */
    int getId() const { return id; }

    /**
     * @brief 获得物品单价
     * @return int 物品单价
     */
    virtual int getPrice() const = 0;

    /**
     * @brief 获得物品总花费
     * @return int 物品总花费
     */
    int getCost() const { return cost; };

    /**
     * @brief 获得物品类型
     * @return int 物品类型
     */
    int getType() const { return type; };

    /**
     * @brief 获得物品状态
     * @return const int 物品状态
     */
    const int getState() const { return state; }

    /**
     * @brief 获得寄送时间
     * @return const Time& 寄送时间
     */
    const Time &getSendingTime() const { return sendingTime; }

    /**
     * @brief 获得接收时间
     * @return const Time& 接收时间
     */
    const Time &getReceivingTime() const { return receivingTime; }

    /**
     * @brief 获得寄件用户的用户名
     * @return const QString& 寄件用户的用户名
     */
    const QString &getSrcName() const { return srcName; }

    /**
     * @brief 获得收件用户的用户名
     * @return const QString& 收件用户的用户名
     */
    const QString &getDstName() const { return dstName; }

    /**
     * @brief 获得描述信息
     * @return const QString& 描述信息
     */
    const QString &getDescription() const { return description; }

    /**
     * @brief 插入快递信息到数据库中
     * @param db 数据库
     */
    void insertInfo2DB(Database *db);

protected:
    int id;              // 物品ID 主键
    int cost;            //总花费
    int type;            //物品种类
    int state;           //物品状态
    Time sendingTime;    //寄送时间
    Time receivingTime;  //接收时间
    QString srcName;     //寄件用户的用户名
    QString dstName;     //收件用户的用户名
    QString expressman;  //快递员
    QString description; //物品描述
};

//易碎品类
class FragileItem : public Item
{
public:
    FragileItem() = delete;

    /**
     * @brief 构造函数
     *
     * @param _id 物品ID 主键
     * @param _state 物品状态
     * @param _sendingTime 寄送时间
     * @param _receivingTime 接收时间
     * @param _srcName 寄件用户的用户名
     * @param _dstName 收件用户的用户名
     * @param _expressman 快递员
     * @param _description 物品描述
     * @note 注意是否使用std::move
     */
    FragileItem(int _id, int _cost, int _state, Time _sendingTime, Time _receivingTime, QString _srcName, QString _dstName, QString _expressman, QString _description) : Item(_id, _cost, _state, _sendingTime, _receivingTime, _srcName, _dstName, _expressman, _description) { type = FRAGILE; };

    /**
     * @brief 默认析构函数
     * @note 虚函数
     */
    virtual ~FragileItem() = default;

    /**
     * @brief 获得物品单价
     * @return int 物品单价
     */
    virtual int getPrice() const override { return FRAGILE_ITEM_PRICE; };
};

//图书类
class Book : public Item
{
public:
    Book() = delete;

    /**
     * @brief 构造函数
     *
     * @param _id 物品ID 主键
     * @param _state 物品状态
     * @param _sendingTime 寄送时间
     * @param _receivingTime 接收时间
     * @param _srcName 寄件用户的用户名
     * @param _dstName 收件用户的用户名
     * @param _expressman 快递员
     * @param _description 物品描述
     * @note 注意是否使用std::move
     */
    Book(int _id, int _cost, int _state, Time _sendingTime, Time _receivingTime, QString _srcName, QString _dstName, QString _expressman, QString _description) : Item(_id, _cost, _state, _sendingTime, _receivingTime, _srcName, _dstName, _expressman, _description) { type = BOOK; };

    /**
     * @brief 默认析构函数
     * @note 虚函数
     */
    virtual ~Book() = default;

    /**
     * @brief 获得物品单价
     * @return int 物品单价
     */
    virtual int getPrice() const override { return BOOK_PRICE; };
};

//普通快递类
class NormalItem : public Item
{
public:
    NormalItem() = delete;

    /**
     * @brief 构造函数
     *
     * @param _id 物品ID 主键
     * @param _state 物品状态
     * @param _sendingTime 寄送时间
     * @param _receivingTime 接收时间
     * @param _srcName 寄件用户的用户名
     * @param _dstName 收件用户的用户名
     * @param _expressman 快递员
     * @param _description 物品描述
     * @note 注意是否使用std::move
     */
    NormalItem(int _id, int _cost, int _state, Time _sendingTime, Time _receivingTime, QString _srcName, QString _dstName, QString _expressman, QString _description) : Item(_id, _cost, _state, _sendingTime, _receivingTime, _srcName, _dstName, _expressman, _description) { type = NORMAL; };

    /**
     * @brief 默认析构函数
     * @note 虚函数
     */
    virtual ~NormalItem() = default;

    /**
     * @brief 获得物品单价
     * @return int 物品单价
     */
    virtual int getPrice() const override { return NORMAL_ITEM_PRICE; };
};

/**
 * @brief 物品管理类
 */
class ItemManage
{
public:
    /**
     * @brief 删除默认构造函数
     * @note 用不上
     */
    ItemManage() = delete;

    /**
     * @brief 构造函数
     * @param _db 数据库的指针
     */
    ItemManage(Database *_db);

    /**
     * @brief 插入一个Item，会自动分配id.
     *
     * @param cost 快递花费
     * @param state 物品状态
     * @param type 物品状态
     * @param sendingTime 寄送时间
     * @param receivingTime 接收时间
     * @param srcName 寄件用户的用户名
     * @param dstName 收件用户的用户名
     * @param description 物品描述
     * @return int 为添加的快递分配的单号
     *
     * @note pahse1默认cost为15
     */
    int insertItem(
        const int cost,
        const int state,
        const int type,
        const Time &sendingTime,
        const Time &receivingTime,
        const QString &srcName,
        const QString &dstName,
        const QString &expressman,
        const QString &description);

    /**
     * @brief 查询所有物品
     * @param result 用于返回结果
     * @return int 查到符合条件的数量
     */
    int queryAll(QList<QSharedPointer<Item>> &result) const;

    /**
     * @brief 根据条件查询物品
     * @param result 用于返回结果
     * @param id 物品单号
     * @param sendingTime 寄送时间
     * @param receivingTime 接收时间
     * @param srcName 寄件用户的用户名
     * @param dstName 收件用户的用户名
     * @param expressman 快递员的用户名
     * @return int 查到符合条件的数量
     */
    int queryByFilter(QList<QSharedPointer<Item>> &result, const int id = -1, const Time &sendingTime = Time(-1, -1, -1), const Time &receivingTime = Time(-1, -1, -1), const QString &srcName = "", const QString &dstName = "", const QString &expressman = "") const;

    /**
     * @brief 根据条件查询物品
     * @param result 用于返回结果
     * @param id 物品单号
     * @return true 存在该物品
     * @return false 不存在该物品
     */
    bool queryById(QSharedPointer<Item> &result, const int id) const;

    /**
     * @brief 修改物品状态
     * @param id 物品单号
     * @param state 物品状态
     * @return true 修改成功
     * @return false 修改失败
     */
    bool modifyState(const int id, const int state);

    bool modifyReceivingTime(const int id, const Time &receivingTime);

    /**
     * @brief 从数据库中删除对应id的物品
     * @param id 物品单号
     * @return true 修改成功
     * @return false 修改失败
     */
    bool deleteItem(const int id) const;

private:
    Database *db; //数据库
    int total;    //物品ID允许的最大值
};
#endif