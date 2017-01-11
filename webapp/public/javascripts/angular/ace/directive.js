define([
  "TerraMA2WebApp/common/app"
], function(commonModule) {
  'use strict';
  var moduleName = "terrama2.ace";

  angular.module(moduleName, [commonModule])
    .directive('terrama2Ace', function() {
      return {
        restrict: 'EA',
        require: '?ngModel',
        link: function(scope, element, attrs, ngModel) {
          var options = angular.extend({}, options, scope.$eval(attrs.terrama2Ace));

          window.ace.require("ace/ext/language_tools");

          var editor = window.ace.edit(element[0]);

          var session = editor.getSession();

          var editorOptions = Object.assign({}, options);

          editor.setTheme("ace/theme/chrome");
          session.setMode(editorOptions.languageMode || "ace/mode/python");

          attrs.$observe('readonly', function (value) {
            editor.setReadOnly(!!value || value === '');
          }); 

          // defaults
          editorOptions.fontSize = options.fontSize || "12pt";
          editorOptions.maxLines = options.maxLines || 20;
          editorOptions.enableBasicAutocompletion = options.enableBasicAutocompletion || true;
          editorOptions.enableSnippets = options.enableSnippets || true;
          editorOptions.enableLiveAutocompletion = options.enableLiveAutocompletion || false;

          editor.setOptions(editorOptions);

          var triggerCallback = function() {
            var callback = arguments[0];
            var args = Array.prototype.slice.call(arguments, 1);

            if (angular.isDefined(callback)) {
              scope.$evalAsync(function () {
                if (angular.isFunction(callback)) {
                  callback(args);
                } else {
                  throw new Error('callback must be a function!!');
                }
              });
            }
          };

          var onChangeEvent = function (callback) {
            return function (e) {
              var newValue = session.getValue();

              if (ngModel && newValue !== ngModel.$viewValue && !scope.$$phase &&
                !scope.$root.$$phase) {
                scope.$evalAsync(function () {
                  ngModel.$setViewValue(newValue);
                });
              }

              triggerCallback(callback, e, editor);
            };
          }

          var onChange = onChangeEvent(options.onChange);

          element.height(options.height || 200);

          if (ngModel) {
            ngModel.$formatters.push(function (value) {
              if (angular.isUndefined(value) || value === null) {
                return '';
              }
              else if (angular.isObject(value) || angular.isArray(value)) {
                throw new Error('TerraMA2 ace cannot use an object as a model. Try setting a property.');
              }
              return value;
            });

            ngModel.$render = function () {
              session.setValue(ngModel.$viewValue);
            };
          }

          scope.$watch(function() {
            return [element[0].offsetWidth, element[0].offsetHeight];
          }, function() {
            editor.resize();
            editor.renderer.updateFull();
          }, true);

          session.setValue("");
          session.on('change', onChange);
        }
      };
    });
  
  return moduleName;
});