angular.module("terrama2.services", [])
  .factory("DataProviderFactory", ["$http", function($http) {
    return {
      get: function() {
        return $http.get("/api/DataProvider", {});
      }
    }
  }])

  .factory("DataSeriesSemanticsFactory", ["$http", function($http) {
    return {
      get: function(dataFormat) {
        return $http.get("/api/DataSeriesSemantics", {dataFormat: dataFormat});
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