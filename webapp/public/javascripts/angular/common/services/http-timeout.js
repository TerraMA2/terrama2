define(function() {
  function terrama2HttpTimeout($http, $q) {
    var _makeRequest = function(httpOptions, timeout) {
      var timeoutPromise = $q.defer(),
          result = $q.defer(),
          timedOut = false,
          httpRequest;

      // defining default timeout
      timeout = timeout || 2;

      setTimeout(function () {
        timedOut = true;
        timeoutPromise.resolve();
      }, (1000 * timeout));

      httpRequest = $http({
        method : httpOptions.method,
        url: httpOptions.url,
        data: httpOptions.data,
        cache: false,
        timeout: timeoutPromise.promise
      });

      httpRequest.then(function(response) {
        result.resolve(response.data);
      });

      httpRequest.catch(function(response) {
        if (timedOut) {
          result.reject({
            error: 'timeout',
            message: 'Request took longer than ' + timeout + ' seconds.'
          });
        } else {
          result.reject(response.data);
        }
      });

      return result.promise;
    };

    return function(httpOptions, timeout) {
      return _makeRequest(httpOptions, timeout);
    }
  }

  terrama2HttpTimeout.$inject = ['$http', '$q'];

  return terrama2HttpTimeout;
});