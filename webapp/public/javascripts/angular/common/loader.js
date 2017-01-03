define([], function() {
  /**
   * It tries to perform angular module loading. Natively, angular throws a exception when a module is not available (injector error).
   * It checks if module name is instantiated. If there is, the required includes are ok turning out module available. Used for external deps
   * 
   * @param {string} moduleName - Angular Module name
   * @param {string[]} deps - Destination array where module given will be appended.
   * @returns {boolean} A bool condition of loading
   */
  function moduleLoader(moduleName, deps) {
    try {
      angular.module(moduleName);
      deps.push(moduleName);
      return true;
    } catch (err) {
      // console.warn("Module " + moduleName + " is not loaded - " + err.toString());
      return false;
    }
  }

  return moduleLoader;
})