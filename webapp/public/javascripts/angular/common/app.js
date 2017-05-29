define([
    "TerraMA2WebApp/common/services/index",
    "TerraMA2WebApp/common/controllers/i18n",
    "TerraMA2WebApp/common/directives/index"
  ],
  function(commonServicesModule, i18nController, commonDirectivesModule) {
    var moduleName = "terrama2.common";
    angular.module(moduleName, ["i18n", commonServicesModule, commonDirectivesModule])
      .config(["$provide", function($provide) {
        /** 
         * TODO: remove it when implemented in external lib (i18n-node-angular)
         * Overwritting i18n. It is important due i18n uses http success and http error. It is deprecated in angular 1.6.x
         */ 
        $provide.decorator("i18n", ["$delegate", "$http", "$rootScope", "$q", function($delegate, $http, $rootScope, $q) {
          var originalInit = $delegate.init;
          $delegate.init = function( locale ) {
            if (locale != this.userLanguage ) {
              if( this._localeLoadedDeferred ) {
                this._deferredStack.push( this._localeLoadedDeferred );
              }
              this._localeLoadedDeferred = $q.defer();
              this.loaded = false;
              this.userLanguage = locale;

              var service = this;

              $http( {
                method : "get",
                url    : BASE_URL + "i18n/" + locale,
                cache  : true
              }).then( function( response ) {
                $rootScope.i18n = response.data;
                service.loaded = true;
                service._localeLoadedDeferred.resolve( $rootScope.i18n );

                while( service._deferredStack.length ) {
                  service._deferredStack.pop().resolve( $rootScope.i18n );
                }

                $rootScope.$broadcast( "LOCALE_UPDATED", {userLanguage: service.userLanguage});
              } ).catch( function( response ) {
                service._localeLoadedDeferred.reject( response.data );

                while( service._deferredStack.length ) {
                  service._deferredStack.pop().reject( response.data );
                }
              } );
            }

            return this._localeLoadedDeferred.promise;
          };

          $delegate.__ = function( name ) {
            if( !$rootScope.i18n ) {
              return name;
            }

            var translation = $rootScope.i18n[ name ] || this.getTranslationObject( name );
            if( !translation ) {
              translation = name;
              $rootScope.i18n[ name ] = translation;
              // changing .success to .then to avoid errors in angular >= 1.6 versions
              $http.get( BASE_URL + "i18n/" + this.userLanguage + "/" + encodeURIComponent( name ) )
                .then( function(response) {
                  $rootScope.i18n[ name ] = response.data;
                });
            }

            // If an implementation of vsprintf is loaded and we have additional parameters,
            // try to perform the substitution and return the result.
            if( arguments.length > 1 && typeof( vsprintf ) == "function" ) {
              translation = vsprintf( translation, Array.prototype.slice.call( arguments, 1 ) );
            }

            return translation;
          };

          return $delegate;
        }]);
      }])
      .controller("TerraMA2Controller", i18nController)
      .run(["$rootScope", "$locale", function($rootScope, $locale) {
        $rootScope.locale = $locale.localeID;
      }]);
    return moduleName;
  });