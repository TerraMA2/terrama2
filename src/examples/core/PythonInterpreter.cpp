
#include <terrama2/core/interpreter/PythonInterpreter.hpp>

int main(int argc, char* argv[])
{
  PyEval_InitThreads();
  Py_Initialize();
  PyEval_ReleaseLock();

  terrama2::core::PythonInterpreter interpreter;

  python::GILLock lock;
  Py_Finalize();

  return 0;
}
