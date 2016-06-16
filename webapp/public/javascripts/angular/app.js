var terrama2Application = angular.module("terrama2", ['i18n']);

terrama2Application.factory("TryCaster", function() {
  return function(value) {
    if (isNaN(value))
      return value;
    return parseInt(value);
  }
});

// Helper for display invalid fields from form
terrama2Application.factory('FormHelper', function() {
  return function(form) {
    angular.forEach(form.$error, function (field) {
      angular.forEach(field, function(errorField){
        errorField.$setDirty();
      })
    });
  }
});

// Factory for handling HttpRequests with timeout specified.
terrama2Application.factory("$HttpTimeout", ['$http', '$q',
  function($http, $q) {
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

      httpRequest.success(function(data, status, headers, config) {
        result.resolve(data);
      });

      httpRequest.error(function(data, status, headers, config) {
        if (timedOut) {
          result.reject({
            error: 'timeout',
            message: 'Request took longer than ' + timeout + ' seconds.'
          });
        } else {
          result.reject(data);
        }
      });

      return result.promise;
    };

    return function(httpOptions, timeout) {
      return _makeRequest(httpOptions, timeout);
    }
  }])

terrama2Application.factory('$HttpSync', ['$http', '$cacheFactory',
  function ($http, $cacheFactory) {
    var cache = $cacheFactory('$HttpSync');

    var _wrap = function(promise) {
      return promise.then(function(response) {
        return response.data;
      })
    }

    return {
      get: function $httpOnce(url, options) {
        return _wrap(cache.get(url) || cache.put(url, $http.get(url, options)));
      },
      post: function $httpOnce(url, options) {
        return _wrap(cache.get(url) || cache.put(url, $http.post(url, options)));
      }
    }
  }
]);
