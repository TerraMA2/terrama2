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
})

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
