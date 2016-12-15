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
    })

    .directive("terrama2List", terrama2ListDirective)
    .directive("terrama2ListItem", ["$compile", terrama2ListDirectiveItem]);

    /**
     * It defines a TerraMA² List auto-recursive directive. When a data has children instances, it auto call yourself to generate
     * sub items.
     * 
     * @usage
     * ### JS ###
     * $scope.myList = [
     *   {"name": "Item 1", "children": [{"name": "Sub Item 1.1"}, {"name": "Sub Item 1.2", "children": [{"name": "Sub Sub Item 1.2.1]}]},
     *   {"name": "Item 2", "children": [{"name": "Sub Item 2.1"}]}
     * ] // or ctrl.myList
     * 
     * ### HTML ###
     * <terrama2-list class="CSS_CLASSES" data="myList">
     * </terrama2-list>
     * 
     * @returns {angular.IDirective}
     */
    function terrama2ListDirective() {
      return {
        restrict: "E",
        replace: true,
        require: "^?terrama2ListItem",
        scope: {
          css: "=?class",
          data: "=",
          onItemClicked: "&"
        },
        template: "<ul ng-class=\"css\">" +
                    "<terrama2-list-item ng-repeat=\"item in data\" data=\"item\"></terrama2-list-item>" +
                  "</ul>",
      };
    }

    /**
     * It defines a TerraMA² List Item recursive directive. When a data has children instances, it auto call parent (terrama2-list) and make it again
     * 
     * @returns {angular.IDirective}
     */
    function terrama2ListDirectiveItem($compile) {
      return {
        restrict: "E",
        replace: true,
        scope: {
          onItemClicked: "&",
          data: "="
        },
        template: "<li><a href=\"javascript::void()\" ng-click=\"onClick(data)\">{{ data.name }}</a></li>",
        link: linkFn
      };

      function linkFn(scope, element, attrs) {
        if (!scope.data) {
          return;
        }

        scope.onClick = function(item) {
          scope.$emit("itemClicked", item);
        };

        if (angular.isArray(scope.data.children)) {
          element.addClass("dropdown-submenu");

          $compile("<terrama2-list class=\"dropdown-menu\" data=\"data.children\"></terrama2-list>")(scope, function(cloned, scope) {
            element.append(cloned);
          });
        }
      }
    }
} ());