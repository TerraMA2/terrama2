#ifndef __TERRAMA2_UNITTEST_CORE_DATA_RETRIEVER_FTP_HPP__
#define __TERRAMA2_UNITTEST_CORE_DATA_RETRIEVER_FTP_HPP__


#include <terrama2/impl/DataRetrieverFTP.hpp>
#include "MockDataRetriever.hpp"
#include <QtTest>


class TsDataRetrieverFTP: public QObject
{
  Q_OBJECT

  private:
    QUrl url_;
   // std::string mask_;

  private slots:

    void initTestCase(){} // Run before all tests
    void cleanupTestCase(){} // Run after all tests

    void init(){ } //run before each test
    void cleanup(){ } //run before each test

    //******Test functions********

    void TestFailUriInvalid();
    void TestFailLoginInvalid();
    void TestFailMaskInvalid();
    void TestOkUriMaskAndLoginValid();

};

#endif //__TERRAMA2_UNITTEST_CORE_DATA_RETRIEVER_FTP_HPP__
