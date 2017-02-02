define([], function() {
  function Utility() {
    return {
      isNumber: isNumber,
      format: format
    };

    function isNumber(arg) {
      return angular.isNumber(arg);
    }

    function format(str) {
      return str;
    }
  }

  return Utility;
});