angular.module("terrama2.services", [])
  .factory("DataProviderFactory", ["$http", function($http) {
    return {
      get: function() {
        return $http.get("/api/DataProvider", {});
      }
    }
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
        return $http.get(url, extra instanceof Object ? extra : {});
      }
    }
  }])

  .factory("DataSeriesFactory", ["$http", function($http) {
    var url = "/api/DataSeries";
    return {
      get: function() {
        return $http.get(url, {});
      },
      
      post: function(dataSeriesObject) {
        return $http.post(url, dataSeriesObject);
      }
    }
  }]);