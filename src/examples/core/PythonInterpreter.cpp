
#include <terrama2/core/interpreter/PythonInterpreter.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
  PyEval_InitThreads();
  Py_Initialize();
  PyEval_ReleaseLock();

  {
    terrama2::core::PythonInterpreter interpreter;
    interpreter.setDouble("teste", 10);
    interpreter.runScript("teste = 20");
    std::cout << "teste: " << *interpreter.getNumeric("teste") << std::endl;
  }

  PyEval_AcquireLock();
  Py_Finalize();
  return 0;
}
