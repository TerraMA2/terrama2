define([], function() {
  function Utility() {
    return {
      isNumber: isNumber,
      isObject: isObject,
      format: format
    };

    function isObject(obj) {
      return angular.isObject(obj);
    }

    function isNumber(arg) {
      return angular.isNumber(arg);
    }

    function format(str) {
      return str;
    }
  }

  return Utility;
});