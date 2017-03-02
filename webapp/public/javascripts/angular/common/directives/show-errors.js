define(
  function() {
    function terrama2ShowErrors() {
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
    } // end terrama2ShowErrors
    return terrama2ShowErrors;
  }
);