define([], function() {
  /**
   * It defines a set of utilities for handling angular form
   */
  function FormHelper() {
    return {
      dirty: dirty
    };
    /**
     * It performs a field state display
     */
    function dirty(form) {
      angular.forEach(form.$error, function (field) {
        angular.forEach(field, function(errorField){
          errorField.$setDirty();
        });
      });
    }
  }

  return FormHelper;
});