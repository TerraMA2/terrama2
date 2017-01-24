define([], function() {
  /**
   * It represents a angular services for handling style generation
   * 
   * @class StyleBuilder
   * @property {StyleConstants} StyleConstants - TerraMA² Style Constants
   * @property {StyleType} StyleConstants - TerraMA² Style Types
   */
  function StyleBuilder(StyleConstants, StyleType) {
    this.StyleType = StyleType;
    this.StyleConstants = StyleConstants;
  }

  StyleBuilder.prototype.make = function() {

  };

  StyleBuilder.$inject = ["StyleConstants", "StyleType"];

  return StyleBuilder;
});