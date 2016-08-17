'use strict';

angular.module("terrama2.services", ['terrama2'])
  .factory("DataProviderFactory", ["$http", function($http) {
    return {
      get: function() {
        return $http.get("/api/DataProvider", {});
      }
    };
  }])

  .factory("DataSeriesSemanticsFactory", ["$http", function($http) {
    var url = "/api/DataSeriesSemantics/";
    return {
      get: function(semanticsName, extra) {
        var data = extra instanceof Object ? extra : {};
        return $http({
          method: 'GET',
          url: url + semanticsName,
          params: data
        });
      },

      list: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      }
    };
  }])

  .factory("DataSeriesFactory", ["$http", "$HttpSync", function($http, $HttpSync) {
    var url = "/api/DataSeries";
    return {
      get: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      },

      post: function(dataSeriesObject) {
        return $HttpSync.post(url, dataSeriesObject);
      },

      put: function(dataSeriesId, dataSeriesObject) {
        return $HttpSync.put(url + "/" + dataSeriesId, dataSeriesObject);
      }
    };
  }])

  .factory("ServiceInstanceFactory", ["$http", function($http) {
    var url = "/api/Service";
    return {
      get: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      },

      post: function(serviceObject) {
        return $http.post(url, serviceObject);
      },

      put: function(serviceId, serviceObject) {
        return $http.put(url + "/" + serviceId, serviceObject);
      }
    };
  }])

  .factory("AnalysisFactory", ["$http", function($http) {
    var url = "/api/Analysis";
    return {
      get: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      },

      post: function(serviceObject) {
        return $http.post(url, serviceObject);
      },

      put: function(analysisId, serviceObject) {
        return $http.put(url + "/" + analysisId, serviceObject);
      }
    };
  }]).

  factory("Socket", function($rootScope) {
    var socket = io.connect(window.location.origin);

    return {
      on: function(name, callback) {
        socket.on(name, function() {
          var args = arguments;
          $rootScope.$apply(function() {
            callback.apply(socket, args);
          });
        });
      },

      emit: function (eventName, data, callback) {
        socket.emit(eventName, data, function () {
          var args = arguments;
          $rootScope.$apply(function () {
            if (callback) {
              callback.apply(socket, args);
            }
          });
        });
      },

      once: function(eventName, callback) {
        socket.once(eventName, function() {
          var args = arguments;
          $rootScope.$apply(function() {
            callback.apply(socket, args);
          });
        });
      }
    };
  })
  .factory("UserFactory", function($http) {
    var url = "/api/users/";
    return {
      get: function(userId, restriction) {
        var destination = url;
        if (userId) {
          destination += userId;
        }
        return $http({
          method: 'GET',
          url: destination,
          params: restriction instanceof Object ? restriction : {}
        });
      },
      put: function(userId, userObject) {
        return $http.put(url + userId, userObject || {});
      }
    };
  })

  .factory("UniqueNumber", function() {
    function generator() {
      this.length = 8;
      this.timestamp =+ new Date();

      var _getRandomInt = function( min, max ) {
        return Math.floor(Math.random() * ( max - min + 1 )) + min;
      }

      this.generate = function() {
        var ts = this.timestamp.toString();
        var parts = ts.split( "" ).reverse();
        var id = "";

        for( var i = 0; i < this.length; ++i ) {
          var index = _getRandomInt( 0, parts.length - 1 );
          id += parts[index];
        }

        return id;
      }
    }
    return function() {
      var unique = new generator();
      return unique.generate();
    };
  });
