define([], function() {
  function Utility() {
    return {
      isNumber: isNumber,
      isObject: isObject,
      format: format,
      rgba2hex: rgba2hex
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

    function rgba2hex(r, g, b, a) {
      return "#" + (r | 1 << 8).toString(16).slice(1) + (g | 1 << 8).toString(16).slice(1) + (b | 1 << 8).toString(16).slice(1) + ((Math.round(255 * a)) | 1 << 8).toString(16).slice(1);
    }
  }

  return Utility;
});