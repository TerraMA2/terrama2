define([], function() {
  function moduleLoader(moduleName, deps, onSuccess) {
    try {
      angular.module(moduleName);
      deps.push(moduleName);

      if (onSuccess && angular.isFunction(onSuccess)) {
        onSuccess();
      }
    } catch (err) {
      console.warn("Module " + moduleName + " is not loaded - " + err.toString());
    }
  }

  return moduleLoader;
})