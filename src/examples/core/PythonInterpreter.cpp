
#include <terrama2/core/interpreter/PythonInterpreter.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
  PyEval_InitThreads();
  Py_Initialize();
  PyEval_ReleaseLock();

  {
    terrama2::core::PythonInterpreter interpreter;
    terrama2::core::PythonInterpreter interpreter2;
    interpreter.setDouble("teste", 10);
    std::cout << "== c++\n" << "teste: " << *interpreter.getNumeric("teste") << std::endl;
    interpreter.runScript("print('== python')\n"
                          "print(teste)\n"
                          "teste = 20");

    interpreter2.runScript("print('== python2')\n"
                           "print(teste)");

    std::cout << "== c++\n" << "teste: " << *interpreter.getNumeric("teste") << std::endl;
    interpreter.runScript("print('== python')\n"
                          "print(teste)");
  }

  {
    std::cout << "============================" << std::endl;
    std::cout << "New python state" << std::endl;
    terrama2::core::PythonInterpreter interpreter;
    interpreter.setDouble("teste", 10);
    std::cout << "c++\n" << "teste: " << *interpreter.getNumeric("teste") << std::endl;
    interpreter.runScript("print('python')\n"
                          "print(teste)\n"
                          "teste = 20");
    std::cout << "c++\n" << "teste: " << *interpreter.getNumeric("teste") << std::endl;
    interpreter.runScript("print('python')\n"
                          "print(teste)");
  }

  PyEval_AcquireLock();
  Py_Finalize();
  return 0;
}
