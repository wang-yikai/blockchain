#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <QCoreApplication>
#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include <QTextStream>
#include <QFile>
#include <QCryptographicHash>
#include <QMutex>
#include <QMutexLocker>

#include <vector>

#include "Block.h"

#include <iostream>
//#include <string>
using namespace std;

template <typename T>
class Blockchain {
public:
    Blockchain()
    {
        QString path = QCoreApplication::applicationDirPath() + "/blockchain";
//        // cout << path << endl;

        QFile blockchain(path);

        if (!blockchain.open(QIODevice::ReadOnly)) {
            QMessageBox messageBox;
            messageBox.critical(0,"错误",("不能打开:\n" + path + "\n"));
            exit(1);
        }

    	// string line;
    	// while(getline(blockchain, line)) {
        // 	// cout << line << endl;
    	// }

        QTextStream blockStr(&blockchain);
        readFromStream(blockStr);

        blockchain.close();
    }

    Blockchain(const QByteArray& chainString) {
        // cerr << "In blockchain 2nd constr\n";
        if (!chainString.isEmpty()) {
//            // cerr << chainString.toStdString() << endl;
            QTextStream chainStr(chainString);
            readFromStream(chainStr, 0);
        }
    }

    Blockchain(const Blockchain<T>& otherChain): _nDifficulty(otherChain._nDifficulty),
        errors(otherChain.errors), _vChain(otherChain._vChain)  {
        // cerr << "In copy constructor!" << endl;
    }

    Blockchain& operator =(const Blockchain& rhs) {
        QMutexLocker locker(&mutex);
        if (this != &rhs) {
            _nDifficulty = rhs._nDifficulty;
            errors = rhs.errors;
            _vChain = rhs._vChain;
        }
        return *this;
    }

    QString addBlock(const T& data)
    {
        QMutexLocker locker(&mutex);
        Block<T> bNew(_vChain.size(), data);
        bNew.sPrevHash = _vChain.back().sHash;
        QString hash = bNew.mineBlock(_nDifficulty);
        _vChain.push_back(bNew);

        save(_vChain.size() - 1);
        return hash;
    }

    bool addBlocks(const QByteArray& text) {
        QMutexLocker locker(&mutex);
        QTextStream blockStr(text);
//        // cout << text.toStdString() << endl;

        //Block attributes
        quint64 ind;
        QString prevHash;
        quint64 datTime;
        QByteArray decode64;
        quint64 nonce;

        size_t counter = 0;

        //for checking
        QString hash;



        //gets the genesis block:
        while (!(blockStr >> ind >> prevHash >> datTime >> decode64 >> nonce).atEnd()) {
            // cerr << "In the while loop!" << endl;
            // cerr << "ind: " << ind << endl;
            // cerr << "prevHash: " << prevHash.toStdString() << endl;
            // cerr << "datTime: " << datTime << endl;
            // cerr << "decode64: " << decode64.toStdString() << endl;
            // cerr << "nonce: " << nonce << endl;
            if (prevHash == _vChain.back().sHash) {
                // cerr << "inside the if statement!\n";
                T dataIn = (QByteArray::fromBase64(decode64));
                 // cout << "decoded:\n" << QByteArray::fromBase64(decode64).toStdString() << endl;
                 // cout << ":end decode" << endl;

                Block<T> block(ind, prevHash, datTime, dataIn, nonce);
                blockStr >> hash;
                // cout << "hash: " << hash.toStdString() << endl;
                // cout << "block hash: " << block.sHash.toStdString() << endl;

                if (block.sHash != hash) {
                    // cerr << "In error statement!\n";
                    errors += "哈希在区块 #" + QString::number(ind) + " 不一致！\n";
                    save(_vChain.size() - counter);
                    return false;
                }
                _vChain.push_back(block);
                ++counter;
            }
            else {
                errors += "块 " + QString::number(ind) + " 无法连接到块 " + QString::number(ind - 1) + " !\n";
                save(_vChain.size() - counter);
                return false;
            }
        }

        save(_vChain.size() - counter);
        return true;
    }

    void save(size_t start = 0) const
    {
        QString path = QCoreApplication::applicationDirPath() + "/blockchain";
        QFile blockchain(path);

        if (start == 0) {
            if (!blockchain.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox messageBox;
                messageBox.critical(0,"错误",("不能打开:\n" + path + "\n"));
                exit(1);
            }
        }
        else {
            if (!blockchain.open(QIODevice::Append | QIODevice::Text)) {
                QMessageBox messageBox;
                messageBox.critical(0,"错误",("不能打开:\n" + path + "\n"));
                exit(1);
            }
        }

        QTextStream blockStr(&blockchain);

        for(size_t i = start; i < _vChain.size(); ++i) {
            const Block<T>* block = &_vChain[i];
            blockStr << block->getIndex() << " ";
    //            // cerr << block.getIndex() << endl;
            blockStr << block->sPrevHash << " ";
    //            // cerr << block.sPrevHash.toStdString() << endl;
            blockStr << ((quint64) block->getDatTime()) << " ";
    //            // cerr << ((quint64) block.getDatTime()) << endl;

    //            string data_str = ((QByteArray) block.getData()).data();
    //            blockStr << base64_encode(data_str, data_str.length()).c_str() << " ";
            blockStr << ((QByteArray) block->getData()).toBase64() << " ";

            blockStr << block->getNonce() << " ";
    //            // cerr << block.getNonce() << endl;
            blockStr << block->sHash << endl;
    //            // cerr << block.sHash.toStdString() << endl;
        }        

        blockchain.close();
    }

    bool equals(const Blockchain<T>& rhs) {
        QMutexLocker locker(&mutex);
        // cerr << "In equals...\n";
        if (rhs._vChain.size() < _vChain.size()) {
            // cerr << "First if statement, return false\n";
            return false;
        }

        for(size_t i = 0; i < _vChain.size(); ++i) {
            if (_vChain[i] != rhs._vChain[i]) {
                // cerr << "Second if statement, return false\n";
                return false;
            }
        }

        if (rhs._vChain.size() > _vChain.size()) {
            // cerr << "In equals, rhs > length\n";
            for(size_t i = _vChain.size(); i < rhs._vChain.size(); ++i) {
                _vChain.push_back(rhs._vChain[i]);
            }
        }

//        for (const Block<T>& b : _vChain) {
//            // cerr << b.getIndex() << endl;
//        }

        return true;
    }

    T viewAt(quint64 index) {
        QMutexLocker locker(&mutex);
        return _vChain[index].getData();
    }

    quint64 length() const {
        return (_vChain.size() > 0) ? _vChain.size() - 1 : 0;
    }

    QString getErrors() {
        QMutexLocker locker(&mutex);
        QString copy = errors;
        errors.clear();
        return copy;
    }

    QByteArray hash() const {
        QString path = QCoreApplication::applicationDirPath() + "/blockchain";
//        // cout << path << endl;

        QFile ifs(path);

        if (!ifs.open(QIODevice::ReadOnly)) {
            QMessageBox messageBox;
            messageBox.critical(0,"错误",("不能打开:\n" + path + "\n"));
            exit(1);
        }

        QByteArray content = ifs.readAll();
        ifs.close();

        return QCryptographicHash::hash(content, QCryptographicHash::Sha3_512).toHex();
    }

private:
    quint64 _nDifficulty = 2;
    QString errors;
    QMutex mutex;
    vector<Block<T>> _vChain;

    template <typename S>
    bool readFromStream(S& chainStr, bool flag = true) {
        //Block attributes
        quint64 ind;
        QString prevHash;
        quint64 datTime;
        QByteArray decode64;
        quint64 nonce;

        //for checking
        QString hash;

        //gets the genesis block:
        if(!(chainStr >> ind >> datTime >> decode64 >> nonce).atEnd()) {
//            T dataIn = (QByteArray(base64_decode(decode64.data()).c_str()));
            T dataIn = (QByteArray::fromBase64(decode64));
            // // cout << "decoded: " << base64_decode(decode64) << endl;
            // // cout << "line 49: " << dataIn << endl;

            Block<T> block(ind, prevHash, datTime, dataIn, nonce);
            chainStr >> hash;

            if (block.sHash != hash) {
                errors += "创世区块的哈希不一致！\n";
                // cerr << ind << endl;
                // cerr << block.getDatTime() << endl;
                // cerr << nonce << endl;
                // cerr << block.sHash.toStdString() << "\n" << hash.toStdString() << endl;

                if (flag) {
                    QMessageBox messageBox;
                    messageBox.critical(0,"错误","创世区块的哈希不一致！");
//                    exit(1);
                }

                return false;
            }

//            // cerr << ind << endl;
//            // cerr << block.getDatTime() << endl;
//            // cerr << nonce << endl;
//            // cerr << block.sHash.toStdString() << endl;

            _vChain.push_back(block);

            //gets other blocks
            while (!(chainStr >> ind >> prevHash >> datTime >> decode64 >> nonce).atEnd()) {
                if (prevHash == _vChain.back().sHash) {
    //                // cerr << decode64.toStdString() << endl;
    //                T dataIn = (QByteArray(base64_decode(decode64.data()).c_str()));
                    T dataIn = (QByteArray::fromBase64(decode64));

                    block = Block<T>(ind, prevHash, datTime, dataIn, nonce);
                    chainStr >> hash;
                    if (block.sHash != hash) {
                        errors += "哈希在区块 #" + QString::number(ind) + " 不一致！\n";
//                        // cerr << "Hash inconsistency at block " << ind << "!" << endl;
//                        // cerr << ind << endl;
//                        // cerr << block.sPrevHash.toStdString() << endl;
//                        // cerr << block.getDatTime() << endl;
//                        // cerr << nonce << endl;
//                        // cerr << block.sHash.toStdString() << "\n" << hash.toStdString() << endl;

                        if (flag) {
                            QMessageBox messageBox;
                            messageBox.critical(0,"错误", "哈希在区块 #" + QString::number(ind) + " 不一致！\n");
//                            exit(1);
                        }

                        return false;
                    }

//                    // cerr << ind << endl;
//                    // cerr << block.getDatTime() << endl;
//                    // cerr << nonce << endl;
//                    // cerr << block.sHash.toStdString() << endl;

                    _vChain.push_back(block);
                }
                else {
                    errors += "块 " + QString::number(ind) + " 无法连接到块 " + QString::number(ind - 1) + " !\n";
                    if (flag) {
                        QMessageBox messageBox;
                        messageBox.critical(0,"错误", "块 " + QString::number(ind) + " 无法连接到块 " + QString::number(ind - 1) + " !\n");
//                            exit(1);
                    }
                    return false;
                }
            }
        }
        else {
            _vChain.emplace_back(Block<T>(0, T(QByteArray("Genesis Block"))));
            return true;
        }

        return true;
    }
};

#endif
