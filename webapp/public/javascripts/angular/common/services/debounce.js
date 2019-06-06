define(() => {
  /**
   * Debunce function as Angular Factory
   *
   * A debuncer act like a mediator to determine how long to wait in between calls of function
   *
   * @param {Function} $timeout
   * @param {any} $q
   */
  function debounce($timeout, $q) {
    return function wrapDebounce(func, wait, immediate) {
      let timeout;
      // Create a deferred object that will be resolved when we need to
      // actually call the func
      let deferred = $q.defer();
      return function() {
        let context = this;
        let args = arguments;
        let later = function() {
          timeout = null;
          if(!immediate) {
            deferred.resolve(func.apply(context, args));
            deferred = $q.defer();
          }
        };
        let callNow = immediate && !timeout;
        if ( timeout ) {
          $timeout.cancel(timeout);
        }
        timeout = $timeout(later, wait);
        if (callNow) {
          deferred.resolve(func.apply(context,args));
          deferred = $q.defer();
        }
        return deferred.promise;
      };
    };
  }

  debounce.$inject = ['$timeout','$q'];

  return debounce;
})