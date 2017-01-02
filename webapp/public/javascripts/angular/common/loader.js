define([], function() {
  function moduleLoader(moduleName, deps) {
    try {
      angular.module(moduleName);
      deps.push(moduleName);
      return true;
    } catch (err) {
      console.warn("Module " + moduleName + " is not loaded - " + err.toString());
      return false;
    }
  }

  return moduleLoader;
})