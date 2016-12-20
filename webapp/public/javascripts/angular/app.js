"use strict";

var terrama2Application = angular.module("terrama2", ['i18n', 'terrama2.countries']);


/**
 * TerraMA2 Front end Log Decorator
 */
terrama2Application.config(["$provide", function($provide) {
  $provide.decorator("$log", [
    "$delegate",
    function $logDecorator($delegate) {
      var helper = function(context, msg) {
        // TODO: expand it. Use supplant/replace/format
        return "TerraMA² <" + context + "> [" + new Date().toString() + "]: "  + msg;
      };
      // Warn
      var warn = $delegate.warn;
      $delegate.warn = function() {
        var args = [].slice.call(arguments);
        args[0] = helper("warn", args[0]);

        warn.apply(null, args);
      };
      // debug
      var debug = $delegate.debug;
      $delegate.debug = function() {
        var args = [].slice.call(arguments);
        args[0] = helper("debug", args[0]);

        debug.apply(null, args);
      };

      return $delegate;
    }
  ]);
}]);

terrama2Application.controller("TerraMA2Controller", ['$scope', 'i18n', function($scope, i18n) {
  $scope.i18n = i18n;
}]);

// setting caches
terrama2Application.run(["$templateCache", "$rootScope", "$locale", function($templateCache, $rootScope, $locale) {
  // TerraMA2 Box
  $templateCache.put('box.html',
  '<div class="col-md-12" title="{{ titleHeader }}">' +
    '<div class="box box-default {{ boxType }}" title="{{ titleHeader }}">' +
      '<div class="box-header with-border">' +
        '<h3 class="box-title">{{ titleHeader }}</h3>' +
        '<div class="box-tools pull-right">' +
          '<button type="button" class="btn btn-box-tool terrama2-circle-button" style="margin-right: 15px;" data-toggle="tooltip" data-placement="bottom" title="{{ helper }}"><i class="fa fa-question"></i></button>' +
          '<button type="button" class="btn btn-box-tool" data-widget="collapse"><i ng-if="!collapsed" class="fa fa-minus"></i></button> ' +
        '</div>' +
      '</div>' +
      '<div class="box-body" id="targetTransclude">' +
      '</div>' +
    '</div>' +
  '</div>');

  $rootScope.locale = $locale.localeID;
}]);

terrama2Application.service("BaseService", ["$q", "$http", "$filter", "$parse", BaseService])

/**
 * TerraMA² Base service dao
 * 
 * @param {angular.IPromise} $q - Angular $q promiser
 * @param {angular.IHTTP} $http - Angular $http module
 * @param {angular.IFilter} $filter - Angular $filter module
 * @param {angular.IParse} $parse - Angular Parser module
 * 
 * @class BaseService
 */
function BaseService($q, $http, $filter, $parse) {
  this.$q = $q;
  this.$http = $http;
  this.$filter = $filter;
  this.$parse = $parse;

  var self = this;
  /**
   * TerraMA² base request URL. It performs $http operation from given request options
   * 
   * @param {string} url - URL to request
   * @param {string} method - HTTP method
   * @param {Object} options - HTTP options
   * @returns {ng.IPromise}
   */
  this.$request = function(url, method, options) {
    var defer = self.$q.defer();

    self.$http(Object.assign({
      url: url,
      method: method
    }, options)).success(function(data) {
      return defer.resolve(data);
    }).error(function(err) {
      return defer.reject(err);
    });

    return defer.promise;
  };
  /**
   * It applies a angular filter over a array with query restriction.
   * 
   * @param {Array<?>} model - An array of object to filter
   * @param {Object} query - A query restriction
   * @returns {Array<?>}
   */
  this.$list = function(model, query) {
    return self.$filter("filter")(model, query);
  };

  /**
   * It retrieves a first selement from model. If element found, return element. Otherwise, return null.
   * 
   * @param {Array<?>} model - An array of object to filter
   * @param {Object} query - A query restriction
   * @returns {?}
   */
  this.get = function(model, query) {
    var elements = self.$list(model, query);
    if (elements.length === 0) {
      return null;
    }
    return elements[0];
  };
}

/**
 * It parses a URI using HTML a tag.
 * 
 * @param {string} uriString - An URI
 * @returns {A}
 */
terrama2Application.factory("URIParser", function() {
  var parser = document.createElement('a');

  return function(uriString) {
    parser.href = uriString;
    return parser;
  };
});
/**
 * It parses a terrama2 date to a moment date object.
 * It requires "moment" library
 * @param {string} stringDate - A javascript string with date format
 * @return {Moment} a moment date object
 */
terrama2Application.factory("DateParser", function() {
  return function(stringDate) {
    return moment.parseZone(stringDate);
  };
});

terrama2Application.factory("MakeMetadata", function() {
  return function(metadataArray) {
    var output = {};
    metadataArray.forEach(function(meta) {
      output[meta.key] = meta.value;
    });
    return output;
  };
});

terrama2Application.factory("StringDiff", function() {
  return function(stringA, stringB) {
    var firstOccurance = stringB.indexOf(stringA);
    var output = null;
    if(firstOccurance === -1) {
      return output;   
    } else {
      var lengthA = stringA.length;
      if (firstOccurance === 0) {
        output = stringB.substring(lengthA);
      } else {
        output = stringB.substring(0, firstOccurance);
        output += stringB.substring(firstOccurance + lengthA);  
      }
      return output;
    }
  }
});

/**
 * It applies a string format with syntax: {0}, {1}, ...
 * 
 * @param {...string}
 * @returns {string}
 */
terrama2Application.factory("StringFormat", function() {
  return function() {
    var theString = arguments[0];
    
    // start with the second argument (i = 1)
    for (var i = 1; i < arguments.length; i++) {
      var regEx = new RegExp("\\{" + (i - 1) + "\\}", "gm");
      theString = theString.replace(regEx, arguments[i]);
    }
    
    return theString;
  };
});

/**
 * It parses a string into a object.
 * @example
 * var person = {name: "Person", address: {zip: 15478}};
 * console.log(MetaDotReader(person, 'address.zip'));
 * >> 15478
 */
terrama2Application.factory("MetaDotReader", function() {
  return function(object, value) {
    var parts = value.split('.');
    var output = null;
    for(var i = 0; i < parts.length; ++i) {
      output = object[parts[i]];
      object = output;
    }
    return output;
  };
});

// Helper for display invalid fields from form
terrama2Application.factory('FormHelper', function() {
  return function(form) {
    angular.forEach(form.$error, function (field) {
      angular.forEach(field, function(errorField){
        errorField.$setDirty();
      });
    });
  };
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
  }]);

terrama2Application.factory('$HttpSync', ['$http', '$cacheFactory',
  function ($http, $cacheFactory) {
    var cache = $cacheFactory('$HttpSync');

    var _wrap = function(promise) {
      return promise.then(function(response) {
        return response.data;
      });
    };

    return {
      get: function $httpOnce(url, options) {
        return _wrap(cache.get(url) || cache.put(url, $http.get(url, options)));
      },
      post: function $httpOnce(url, options) {
        return _wrap(cache.get(url) || cache.put(url, $http.post(url, options)));
      },
      put: function $httpOnce(url, options) {
        return _wrap(cache.get(url) || cache.put(url, $http.put(url, options)));
      }
    };
  }
]);

/**
 * It compares values two bind variables
 *  
 */
terrama2Application.directive('terrama2CompareTo', function() {
  return {
    restrict: 'A',
    require: 'ngModel',
    scope: {
      compare: '=terrama2CompareTo'
    },
    link: function(scope, element, attrs, ngModel) {
      ngModel.$validators.compareTo = function(modelValue) {
        return modelValue === scope.compare;
      };

      scope.$watch("compare", function() {
        ngModel.$validate();
      });
    }
  };
});

terrama2Application.directive('terrama2ShowErrors', function() {
  return {
    restrict: 'A',
    require: '^form',
    link: function(scope, el, attrs, formCtrl) {
      var inputEl = el[0].querySelector("[name]");

      var inputNgEl = angular.element(inputEl);

      var inputName = inputNgEl.attr('name');

      var _helper = function() {
        if (formCtrl[inputName].$dirty) {
          el.toggleClass('has-success', formCtrl[inputName].$valid);
          el.toggleClass('has-error', formCtrl[inputName].$invalid);
        }
        formCtrl[inputName].$setDirty();
      };

      // only apply the has-error class after the user leaves the text box
      inputNgEl.bind('keyup', function() {
        _helper();
      });

      inputNgEl.bind('change', function() {
        _helper();
      });

      scope.$on('formFieldValidation', function(formName) {
        formCtrl[inputName].$setDirty();
        _helper();
      });
    }
  };
});

/**
 * A generic component for displays a TerraMA² boxes.
 * 
 * @example
 * <terrama2-box title="'Data Provider'" css="{boxType: 'box-solid'}">
 *   <h1>Data Provider Registration</h1>
 * 
 *   <fieldset>
 *     ...
 *   </fieldset>
 * </terrama2-box>
 */
terrama2Application.directive('terrama2Box', function($parse, $templateCache) {
  return {
    restrict: 'E',
    transclude: true,
    templateUrl: 'box.html', // template cache
    scope: {
      titleHeader: '=title',
      helper: '=?helper',
      extra: '=?',
      css: '=?'
    },
    controller: function($scope) {
      $scope.css = $scope.css || {};

      $scope.boxType = "";
      if ($scope.css.boxType) {
        $scope.boxType = $scope.css.boxType;
      }
    },
    link: function(scope, element, attrs, ctrl, transclude) {
      var elm = element.find('#targetTransclude');

      transclude(scope.$parent, function(clone, scope) {
        elm.append(clone);
      });
    }
  }
});

terrama2Application.directive('terrama2BoxFooter', function() {
  return {
    // require: '^terrama2Box',
    transclude: true,
    template: '<div class="box-footer"></div>',
    scope: {
      onSubmit: '&'
    },
    link: function(scope, element, attrs, ctrl, transclude) {
      // create new child scope, then append the controller functions
      var self = scope.$parent.$new();

      transclude(self, function(clone, scope) {
        element.append(clone);
      });
    }
  };
});

terrama2Application.directive('terrama2Form', function() {
  return {
    restrict: 'E',
    transclude: true,
    template: '<form name="{{ formName }}" novalidate><div id="targetTransclude"></div></form>',
    link: {
      pre: function preLink(scope, element, attributes, controller, transclude) {
        scope.formName = attributes.formName;
      },
      post: function postLink(scope, element, attributes, ctrl, transclude){
        var elm = element.find('#targetTransclude');

        transclude(scope, function(clone, scp) {
          elm.append(clone);
        });

        scope.$on('formValidation', function() {
          scope.$emit('formStatus', scope.formName, scope[scope.formName].$valid);
        });
      }
    }
  };
});

terrama2Application.directive('terrama2BoxOverlay', function() {
  return {
    transclude: true,
    template: '<div class="overlay" ng-show="isChecking"><i class="fa fa-refresh fa-spin"></i></div>'
  };
});

terrama2Application.directive('terrama2Datetime', function($timeout) {
  return {
    restrict: 'A',
    require : 'ngModel',
    link: function(scope, element, attrs, ngModelCtrl) {
      var options = angular.extend({}, options, scope.$eval(attrs.options));
      scope.options = options;

      // Watchers
      scope.$watch('options', function (newValue) {
        var data = element.data('DateTimePicker');
        $.map(newValue, function (value, key) {
          data[key](value);
        });
      });

      ngModelCtrl.$render = function () {
        if (!!ngModelCtrl.$viewValue) {
          element.data('DateTimePicker').date(ngModelCtrl.$viewValue);
        } else {
          element.data('DateTimePicker').date(null);
        }
      };

      // Digesting scope
      element.on('dp.change', function (e) {
        $timeout(function () {
          console.log(element);
          if (!!e.date) {
            scope.$apply(function () {
              ngModelCtrl.$setViewValue(e.date);
            });
          }
        });
      });

      element.datetimepicker(options);

      $timeout(function () {
        if (!!ngModelCtrl.$viewValue) {
          if (!(ngModelCtrl.$viewValue instanceof moment)) {
            ngModelCtrl.$setViewValue(moment(scope.date));
          }
          element.data('DateTimePicker').date(ngModelCtrl.$viewValue);
        }
      });
    }
  };
});

terrama2Application.directive('formatDatetime', function ($window) {
  return {
    restrict: 'A',
    require: 'ngModel',
    link: function (scope, element, attrs, ngModel) {
      var moment = $window.moment;
      var fmt = scope.$eval(attrs.formatDatetime);
      
      ngModel.$formatters.push(formatter);
      ngModel.$parsers.push(parser);

      element.on('change', function (e) {
        var element = e.target;
        element.value = formatter(ngModel.$modelValue);
      });

      /**
       * It performs a parser string to check if it is a valid date
       * @param {string | Date} value - A date value
       * @return {boolean} 
       */
      function parser(value) {
        var m = moment(value);
        var valid = m.isValid();
        // ngModel.$setValidity('datetime', valid);
        if (valid) {
          return m.valueOf();
        } else {
          return value;
        }
      }

      /**
       * It performs a input formatter
       * @param {string | Date} value - A date value
       * @return {string}
       */
      function formatter(value) {
        var m = moment(value || null);
        var valid = m.isValid();
        if (valid) {
          return m.format(fmt || "LLLL");
        } else {
          return value;
        }
      }
    } //link
  };

}); //appDatetime

/**
 * Directive for handling Server errors and display them into input
 * 
 * @example
 * // HTML
 * <form name="providerForm">
 *   <input type="text" name="name" ng-model="provider.name" terrama2-server-errors="serverErrorsVar">
 * </form> 
 * 
 * // JS
 * ... // validation
 * $scope.serverErrorsVar = {"name": "Name is already taken"}
 */
terrama2Application.directive("terrama2ServerErrors", function($parse) {
  return {
    restrict: "A",
    require: "ngModel",
    link: function(scope, element, attrs, ngModel) {
      var inputNgEl = angular.element(element);
      var inputName = inputNgEl.attr('name');

      scope.$watch(function() {
        var errors = $parse(attrs.terrama2ServerErrors)(scope) || {};

        if (!errors[inputName]) { return true; }

        return errors[inputName].value !== ngModel.$viewValue;
      }, function(value) {
        ngModel.$setValidity("terrama2Error", value);
        // TODO: improve it
        if (!value) {
          element.trigger("change");
        }
        return;
      });
    }
  };
});

terrama2Application.directive('terrama2Button', function() {
  return {
    restrict: "E",
    template: "<button ng-class='class' ng-transclude></button>",
    scope: true,
    replace: true,
    transclude: true,
    link: function(scope, element, attrs, tranclude) {
      scope.class = attrs.class;
    }
  };
});

terrama2Application.directive("terrama2Content", function() {
  return {
    restrict: "E",
    transclude: true,
    template: "<div ng-class='divClass' ng-transclude></div>",
    link: function(scope, element, attrs) {
      scope.divClass = attrs.class || "row";
    }
  };
});

terrama2Application.directive('terrama2Fluid', function($window) {
  return {
    restrict: "A",
    link: function(scope, element, attrs) {
      var windowElement = angular.element($window);

      /**
       * Helper to handle window size and add/remove class depending resolution
       * 
       * @return {void}
       */
      function resizeComponent() {
        if ($window.pageYOffset > 200) {
          element.addClass("terrama2-fluid");
        } else {
          element.removeClass("terrama2-fluid");
        }
      }

      // Auto call to resize at first time
      resizeComponent();

      // Performs resize component on page scroll
      windowElement.bind('scroll', function() {
        return resizeComponent();
      });
    }
  };
});