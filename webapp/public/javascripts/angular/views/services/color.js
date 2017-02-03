define([], function() {
  function hex (c) {
    var s = "0123456789abcdef";
    var i = parseInt (c);
    if (i == 0 || isNaN (c))
      return "00";
    i = Math.round (Math.min (Math.max (0, i), 255));
    return s.charAt ((i - i % 16) / 16) + s.charAt (i % 16);
  }

  /* Convert an RGB triplet to a hex string */
  function convertToHex (rgb) {
    return hex(rgb[0]) + hex(rgb[1]) + hex(rgb[2]);
  }

  function trim (s) { return (s.charAt(0) == '#') ? s.substring(1, 7) : s }

  /**
   * It converts a hex string to an RGB triplet
   * 
   * @param {string} hex - Representation of color in Hexdecimal
   * @returns {number[]}
   */
  function convertToRGB (hex) {
    var color = [];
    color[0] = parseInt ((trim(hex)).substring (0, 2), 16);
    color[1] = parseInt ((trim(hex)).substring (2, 4), 16);
    color[2] = parseInt ((trim(hex)).substring (4, 6), 16);
    return color;
  }

  /**
   * It generates a gradient color between initial and end color
   * 
   * @example
   * var white = '#000000';
   * var black = '#ff0ff0';
   * var colors = generateColor(white, black, 10);
   * colors.forEach(function(color) {
   *   console.log(color);
   * });
   * 
   * @param {string} colorStart - Initial Hex color
   * @param {string} colorEnd - End Hex color
   * @param {number} colorCount - number of colors that will generate
   * @returns {string[]}
   */
  function generateColor(colorStart,colorEnd,colorCount){
    // The beginning of your gradient
    var start = convertToRGB (colorStart);    

    // The end of your gradient
    var end   = convertToRGB (colorEnd);    

    // The number of colors to compute
    var len = colorCount;

    //Alpha blending amount
    var alpha = 0.0;

    var output = [];
    
    for (i = 0; i < len; i++) {
      var c = [];
      alpha += (1.0/len);
      
      c[0] = start[0] * alpha + (1 - alpha) * end[0];
      c[1] = start[1] * alpha + (1 - alpha) * end[1];
      c[2] = start[2] * alpha + (1 - alpha) * end[2];

      output.push("#" + convertToHex(c));
      
    }
    
    return output; 
  }

  return function() {
    return {
      generateColor: generateColor,
      convertToRGB: convertToRGB,
      convertToHex: convertToHex,
      hex: hex
    };
  };
});