define([], () => {
  class UiSelectMultipleController {
    constructor($scope) {
      this.$scope = $scope;
      if ($scope.form.options) {
        $scope.form.options = {};
      }

      /**
       * Extra options to configure Select Box
       */
      $scope.extraOptions = {
        scrollable: true,
        enableSearch: false,
      };

      /**
       * Defines list of selected options
       */
      $scope.selectedOptions = [];
      /**
       * Defines list of options to display
       */
      $scope.options = [];

      this.parseForm($scope.form);

      // Set values
    }

    parseForm(form) {
      if (!form)
        return;

      const inputSettings = form.extraSettings || {};
      this.$scope.extraOptions = Object.assign(this.$scope.extraOptions, inputSettings);

      // HTML
      this.$scope.htmlClasses = this.$scope.form.htmlClasses;

      this.parseItems(form.schema.items);
    }

    parseItems(items) {
      if (!items || !items instanceof Array)
        throw new Error("Schema items must be an array");

      items.forEach(el => this.$scope.options.push(el));
    }
  }
  UiSelectMultipleController.$inject = ['$scope'];

  return UiSelectMultipleController;
});