
  angular.module("terrama2.countries", [])
    /**
     * Factory to retrieve available countries
     * 
     * @param {BaseService} BaseService - TerraMA² Base request service
     * @param {ng.IPromise} $q - Angular Promiser
     * @returns {Object}
     */
    .factory("TerraMA2Countries", ["$http", "$q", function TerraMA2Countries($http, $q) {
      /**
       * Remote host url
       * @type {string}
       */
      var targetUrl = BASE_URL + "javascripts/angular/countries/data.json";
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
         * @returns {ng.IPromise<Object[]>}
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
          return $http.post(BASE_URL + "languages", {locale: country});
        }
      };
    }])
    .directive("terrama2CountriesList", ["i18n", "$timeout", "$rootScope", "TerraMA2Countries", "$log", function terrama2Flag(i18n, $timeout, $rootScope, TerraMA2Countries, $log) {
      return {
        restrict: "E",
        templateUrl: BASE_URL + "javascripts/angular/countries/countries-tpl.html",
        controller: [terrama2FlagController],
        controllerAs: "vm",
        link: function(scope, element, attrs) {         
          // waiting for angular digest cycle
          var input = $("#i18n-input-flags");

          var initLocale = function(country) {
            $timeout(function() {
              input.intlTelInput({
                // performs a map operation over all supported countries (data.json)
                onlyCountries: TerraMA2Countries.getData().map(function(country) { return country.id; })
              });

              input.on("countrychange", function(event, data) {
                var found = TerraMA2Countries.getData().find(function(value) {
                  return value.id === data.iso2;
                });

                $rootScope.$apply(function() {
                  var country;
                  if (found) {
                    $rootScope.locale = found.locale;
                    country = found.id;
                  } else {
                    country = "us";
                    $rootScope.locale = "en_US";
                  }

                  TerraMA2Countries.select(country)
                    .then(function(response) {
                      $log.info("Locale changed");
                    })
                    .catch(function(err) {
                      $log.info("Could not change locale: " + err.toString());
                    });
                });
              });

              // setting session locale
              input.intlTelInput("setCountry", country);
            });
          };

          attrs.$observe("language", function(value) {
            if (!value) {
              return;
            }
            initLocale(value);
          });
        }
      };

      function terrama2FlagController() {
        var self = this;
        self.selected = null;
      }
    }])
    .run(["TerraMA2Countries", function(TerraMA2Countries) {
      TerraMA2Countries.init();
    }]);
