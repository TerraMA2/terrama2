(function() {
  angular.module("terrama2.countries", ["terrama2"])
    .service("TerraMA2Countries", TerraMA2Countries)
    .directive("terrama2CountriesList", terrama2Flag);

    function TerraMA2Countries(BaseService) {
      this.BaseService = BaseService;
      this.targetUrl = "/javascripts/angular/countries/data.json";
      this.data = [];
    }

    TerraMA2Countries.prototype.setUrl = function(url) {
      this.targetUrl = url;
    };

    TerraMA2Countries.prototype.init = function() {
      return this.BaseService.$request(this.targetUrl, "GET");
    };

    TerraMA2Countries.prototype.getData = function() {
      return this.data;
    }

    function terrama2Flag(i18n, $timeout, $rootScope) {
      return {
        restrict: "E",
        templateUrl: "/javascripts/angular/countries/countries-tpl.html",
        controller: terrama2FlagController,
        controllerAs: "vm",
        link: function(scope, element, attrs, ctrl) {
          var input = $("#i18n-input-flags");
          
          // waiting for angular digest cycle
          $timeout(function() {
            input.intlTelInput({
              onlyCountries: ["br", "us", "fr", "es"]
            });

            input.on("countrychange", function(event, data) {
              var locales = ctrl.data;
              var locale = locales.find(function(value) {
                return value.id === data.iso2;
              });

              $rootScope.$apply(function() {
                $rootScope.locale = (locale || {}).locale || "en_US";
              });
            });
          });
        }
      }

      function terrama2FlagController(TerraMA2Countries) {
        var self = this;
        self.selected = null;
        TerraMA2Countries.init()
          .then(function(data) {
            self.data = data;
          })
          .catch(function(err) {
            self.data = [];
          });
      }
    }
} ());