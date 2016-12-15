(function() {
  angular.module("terrama2.components.collapser", ["terrama2"])
    /**
     * Defines a component for Collapse tree view on html lists
     * 
     * @todo Improve extensibility with Angular Element (restrict: "E")
     * 
     * @example
     * <ul terrama2-collapse>
     *   <li>
     *     <label class="tree-toogler">Click to collapse</label>
     *     <ul class="tree">
     *       <li>
     *         Value collapsed
     *       </li>
     *       ...
     *     </ul>
     *   </li>
     * </ul>
     */
    .directive("terrama2Collapse", function() {
      return {
        restrict: "A",
        link: terrama2CollapseLink
      };

      /**
       * It handles TerraMA² Collapse after Angular Compilation (post:link)
       * 
       * @param {angular.IScope} scope - Angular Scope directive
       * @param {angular.IElement} element - Angular Directive Element Selector
       * @param {angular.IAttributes} attrs - Angular Directive Element Attributes
       */
      function terrama2CollapseLink(scope, element, attrs) {
        // Find all children with label TODO: change it
        var items = element.find("label");

        items.click(function() {
          $(this).parent().children("ul.tree").toggle(300);
        });
      }
    });
} ());