//
//  Commerce.h
//  interface/src/commerce
//
//  Guard for safe use of Commerce (Wallet, Ledger) by authorized QML.
//
//  Created by Howard Stearns on 8/4/17.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once
#ifndef hifi_QmlCommerce_h
#define hifi_QmlCommerce_h

#include <OffscreenQmlDialog.h>

class QmlCommerce : public OffscreenQmlDialog {
    Q_OBJECT
    HIFI_QML_DECL

public:
    QmlCommerce(QQuickItem* parent = nullptr);

signals:
    void buyResult(const QString& failureMessage);
    // Balance and Inventory are NOT properties, because QML can't change them (without risk of failure), and 
    // because we can't scalably know of out-of-band changes (e.g., another machine interacting with the block chain).
    void balanceResult(int balance, const QString& failureMessage);
    void inventoryResult(QStringList inventory, const QString& failureMessage);

protected:
    Q_INVOKABLE void buy(const QString& assetId, int cost, const QString& buyerUsername = "");
    Q_INVOKABLE void balance();
    Q_INVOKABLE void inventory();
};

#endif // hifi_QmlCommerce_h
