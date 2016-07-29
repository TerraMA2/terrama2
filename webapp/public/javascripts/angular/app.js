var terrama2Application = angular.module("terrama2", ['i18n']);

// setting caches
terrama2Application.run(function($templateCache) {
  // TerraMA2 Box
  $templateCache.put('box.html',
  '<div class="col-md-12">' +
    '<div class="box box-default {{ boxType }}">' +
      '<div class="box-header with-border">' +
        '<h3 class="box-title">{{ title }}</h3>' +
        '<div class="box-tools pull-right">' +
          '<button type="button" class="btn btn-box-tool terrama2-circle-button" style="margin-right: 15px;" data-toggle="tooltip" data-placement="bottom" title="{{ helper }}"><i class="fa fa-question"></i></button>' +
          '<button type="button" class="btn btn-box-tool" data-widget="collapse"><i ng-if="!collapsed" class="fa fa-minus"></i></button> ' +
        '</div>' +
      '</div>' +
      '<div style=\""display: {{ collapsed ? "none" : "block" }};\"" class="box-body">' +
        '<div id="targetTransclude"></div>' +
      '</div>' +
    '</div>' +
  '</div>');
});

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
      })
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
    }
  }
]);

terrama2Application.directive('terrama2CompareTo', function() {
  return {
    restrict: 'A',
    require: 'ngModel',
    scope: {
      compare: '=terrama2CompareTo'
    },
    link: function(scope, element, attrs, ngModel) {
      ngModel.$validators.compareTo = function(modelValue) {
        return modelValue == scope.compare;
      };

      scope.$watch("compare", function() {
        ngModel.$validate();
      });
    }
  }
});

terrama2Application.directive('terrama2ShowErrors', function() {
  return {
    restrict: 'A',
    require: '^form',
    link: function(scope, el, attrs, formCtrl) {
      var inputEl   = el[0].querySelector("[name]");

      var inputNgEl = angular.element(inputEl);

      var inputName = inputNgEl.attr('name');

      var _helper = function() {
        if (!formCtrl[inputName].$dirty)
        el.toggleClass('has-success', formCtrl[inputName].$valid);
        el.toggleClass('has-error', formCtrl[inputName].$invalid);
        formCtrl[inputName].$setDirty();
      };

      // only apply the has-error class after the user leaves the text box
      inputNgEl.bind('blur', function() {
        _helper();
      });

      scope.$on('formFieldValidation', function(formName) {
        _helper();
      });
    }
  }
});

terrama2Application.directive('terrama2Box', function($parse, $templateCache) {
  return {
    restrict: 'E',
    transclude: true,
    templateUrl: 'box.html', // template cache
    scope: {
      title: '=title',
      helper: '=?helper',
      extra: '=?',
      css: '=?'
    },
    controller: function($scope) {
      $scope.css = $scope.css || {};

      $scope.boxType = "";
      if ($scope.css.boxType)
        $scope.boxType = $scope.css.boxType;
    },
    link: function(scope, element, attrs, ctrl, transclude) {
      var elm = element.find('#targetTransclude');

      transclude(scope.$parent, function(clone, scope) {
        elm.append(clone);
      })
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
  }
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
          elm.append(clone)
        });

        scope.$on('formValidation', function() {
          scope.$emit('formStatus', scope.formName, scope[scope.formName].$valid);
        })
      }
    }
  }
});

terrama2Application.directive('terrama2BoxOverlay', function() {
  return {
    transclude: true,
    template: '<div class="overlay" ng-show="isChecking"><i class="fa fa-refresh fa-spin"></i></div>'
  }
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
