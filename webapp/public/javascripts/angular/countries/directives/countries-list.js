define(function() {
  function terrama2Flag(i18n, $timeout, $rootScope, TerraMA2Countries, $log) {
    return {
      restrict: "E",
      templateUrl: "/javascripts/angular/countries/countries-tpl.html",
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
  }

  terrama2Flag.$inject = ["i18n", "$timeout", "$rootScope", "TerraMA2Countries", "$log"];

  return terrama2Flag;
})