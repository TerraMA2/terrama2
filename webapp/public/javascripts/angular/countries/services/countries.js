define(function() {
  /**
   * It handles the available countries languages supported by TerraMA² 
   * @param {angular.IHttp} $http - Angular HTTP module
   * @param {angular.IQ} $q - Angular Promiser module
   * @returns {Object}
   */
  function TerraMA2Countries($http, $q) {
    /**
     * Remote host url
     * @type {string}
     */
    var targetUrl = "/javascripts/angular/countries/data.json";
    /**
     * It defines a cached countries
     * @private data
     * @type {Object[]}
     */
    var data = [];

    /**
     * It defines a selected country
     * @todo Validate it.
     * @type {Object}
     */
    var selected = null;

    return {
      /**
       * It initializes TerraMA2Countries factory to retrieve countries from remote host and cache them
       * 
       * @returns {angular.IPromise<Object[]>}
       */
      init: function() {
        var defer = $q.defer();
        $http.get(targetUrl, {})
          .then(function(output) {
            data = output.data;
            return defer.resolve(output);
          })
          .catch(function(err) {
            return defer.reject(err);
          });
        return defer.promise;
      },
      /**
       * It changes current url context. Once changed, it must be initialized again
       * 
       * @param {string} url - A target url to change
       */
      setUrl: function(url) {
        targetUrl = url || targetUrl;
      },
      /**
       * Retrieve cached countries
       * 
       * @returns {Object[]}
       */
      getData: function() {
        return data;
      },
      /**
       * It sets current selected country
       * @todo Validate it.
       * @param {string} country - A selected country in list.
       */
      select: function(country) {
        return $http.post("/languages", {locale: country});
      }
    };
  }

  TerraMA2Countries.$inject = ["$http", "$q"];

  return TerraMA2Countries;
});