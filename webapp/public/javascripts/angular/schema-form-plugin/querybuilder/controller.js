define([], () => {
  class QueryBuilderController {
    constructor($scope) {
      this.$scope = $scope;
    }

    init(model) {
      console.log(model);
    }

    onChange() {
      console.log(this);
    }
  }

  return QueryBuilderController;
});