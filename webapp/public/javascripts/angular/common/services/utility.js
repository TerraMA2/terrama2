define([], function() {
  function Utility() {
    return {
      isNumber: isNumber,
      isObject: isObject,
      format: format,
      rgba2hex: rgba2hex,
      hex2rgba: hex2rgba
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

    function hex2rgba(hexColor){
      if (hexColor){
        var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hexColor);
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16),
            a: parseInt(result[4], 16)
        } : null;
      }
      return null;
    }
  }

  return Utility;
});