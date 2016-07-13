angular.module('terrama2.ace', ['terrama2'])
  .directive('terrama2Ace', function() {
    return {
      restrict: 'EA',
      require: '?ngModel',
      link: function(scope, element, attrs, ngModel) {
        var options = angular.extend({}, options, scope.$eval(attrs.terrama2Ace));

        window.ace.require("ace/ext/language_tools");

        var editor = window.ace.edit(element[0]);

        var session = editor.getSession();

        editor.setTheme("ace/theme/chrome");
        session.setMode("ace/mode/python");

        attrs.$observe('readonly', function (value) {
          editor.setReadOnly(!!value || value === '');
        });

        editor.setOptions({
          fontSize: "12pt",
          maxLines: 20,
          enableBasicAutocompletion: true,
          enableSnippets: true,
          enableLiveAutocompletion: false,
        })

        element.height(200);

        if (ngModel) {
          ngModel.$formatters.push(function (value) {
            if (angular.isUndefined(value) || value === null) {
              return '';
            }
            else if (angular.isObject(value) || angular.isArray(value)) {
              throw new Error('ui-ace cannot use an object or an array as a model');
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

        editor.getSession().setValue("");
        editor.getSession().on('change', function() {
          options.onChange(editor);
        });
      }
    }
  })
