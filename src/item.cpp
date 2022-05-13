/**
 * @file item.cpp
 * @author Haolin Yang
 * @brief
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "../include/item.h"
#include "../include/database.h"

void Item::insertInfo2DB(Database *db)
{
    db->insertItem(id, cost, type, state, sendingTime, receivingTime, srcName, dstName, expressman, description);
}

ItemManage::ItemManage(Database *_db) : db(_db)
{
    total = db->getDBMaxId("item");
}

int ItemManage::insertItem(
    const int cost,
    const int state,
    const int type,
    const Time &sendingTime,
    const Time &receivingTime,
    const QString &srcName,
    const QString &dstName,
    const QString &expressman,
    const QString &description)
{
    qDebug() << "添加物品 ";
    QSharedPointer<Item> item;
    switch (type)
    {
    case FRAGILE:
        item = QSharedPointer<FragileItem>::create(++total, cost, state, sendingTime, receivingTime, srcName, dstName, expressman, description);
        break;
    case BOOK:
        item = QSharedPointer<Book>::create(++total, cost, state, sendingTime, receivingTime, srcName, dstName, expressman, description);
        break;
    case NORMAL:
        item = QSharedPointer<NormalItem>::create(++total, cost, state, sendingTime, receivingTime, srcName, dstName, expressman, description);
        break;
    }
    item->insertInfo2DB(db);
    return total;
}

int ItemManage::queryAll(QList<QSharedPointer<Item>> &result) const
{
    qDebug() << "查询所有物品";
    return db->queryItemByFilter(result, -1, -1, Time(-1, -1, -1), Time(-1, -1, -1), "", "", "");
}

int ItemManage::queryByFilter(QList<QSharedPointer<Item>> &result, const int id, const int state, const Time &sendingTime, const Time &receivingTime, const QString &srcName, const QString &dstName, const QString &expressman) const
{
    qDebug() << "按条件查询";
    return db->queryItemByFilter(result, id, state, sendingTime, receivingTime, srcName, dstName, expressman);
}

bool ItemManage::queryById(QSharedPointer<Item> &result, const int id) const
{
    QList<QSharedPointer<Item>> temp;
    if (db->queryItemByFilter(temp, id, -1, Time(-1, -1, -1), Time(-1, -1, -1), "", "", ""))
    {
        result = temp[0];
        return true;
    }
    else
        return false;
}

bool ItemManage::modifyState(const int id, const int state)
{
    return db->modifyItemState(id, state);
}

bool ItemManage::modifyReceivingTime(const int id, const Time &receivingTime)
{
    return db->modifyItemReceivingTime(id, receivingTime);
}

bool ItemManage::modifyExpressman(const int id, const QString &expressman)
{
    return db->modifyItemExpressman(id, expressman);
}

bool ItemManage::deleteItem(const int id) const
{
    qDebug() << "删除id为" << id << "的物品";
    return db->deleteItem(id);
}