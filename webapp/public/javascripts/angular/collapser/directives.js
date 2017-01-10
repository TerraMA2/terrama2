define([], function() {
  var moduleName = "terrama2.components.collapser";

  angular.module(moduleName, [])
    .filter("SmartFilter", ["$filter", function($filter) {
      /**
       * Angular isObject comparator
       */
      isObject = angular.isObject;
      /**
       * Angular isNumber comparator
       */
      isNumber = angular.isNumber;

      /**
       * Angular isArray comparator
       */
      isArray = angular.isArray;

    /**
     * A deep match object. It checks every key/object in target and match them from initial object.
     * It applies a auto recursive call when obj key is pointing to an another object.
     * 
     * @param {Object} obj - An javascript object with key/values to check.
     * @param {Object} target - An javascript object to be watched
     * @return {Boolean} a boolean condition of comparator.
     */
      function match(obj, target) {
        for(var key in obj) {
          if (obj.hasOwnProperty(key)) {
            if (isObject(target) && !target.hasOwnProperty(key)) {
              /** 
               * If element from array do not contain filter key, return it. 
               * It is important whenever you intend to filter object that do not contain key properly.
               */
              return target;
            }

            if (isObject(obj[key])) {
              switch(key) {
                case "$in":
                  return obj[key].indexOf(target) !== -1;
                  break;
                default:
                  return match(obj[key], target[key]);
              }
            }
            // TODO: add operators like $eq (Equals), $gt/$lt (Greater than/Less Than), etc.
            return target[key] === obj[key];
          } // end if obj.hasOwnProperty(key)
        }
      }

      /**
       * @param {any} input - A input value. It should be a list of values or even a object
       * @param {Object} objFilter - A filter object used. It contains special keys described above
       * @param {Object} objFilter.$depth - It defines how many levels should apply filter. It is important to avoid huge object iteration
       * @param {Object} objFilter.$key - It defines a default key to iterate over. Default: "children"
       */
      return function(input, objFilter) {
        if (!isObject(objFilter)) {
          throw new Error("SmartFilter requires a object to filter a collection, got " + objFilter);
        }

        if (!input || (isArray(input) && input.length === 0) || (Object.keys(input).length === 0)) {
          return input;
        }
        // retrieve max depth check
        var depth = objFilter.$depth;
        // remove $depth key to avoid extra iteration
        delete objFilter.$depth;
        // Checking depth and it is minor than 2 (operators level)
        if (isNumber(depth) && depth > 2) {
          return input;
        }

        // retrieving all matched values
        return input.filter(function(elm) {
          return match(objFilter, elm);
        });
      };
    }])
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
          level: "=?",
          expression: "="
        },
        controller: ["$scope", terrama2ListController],
        controllerAs: "vm",
        template: "<ul ng-class=\"css\">" +
                    "<terrama2-list-item ng-repeat=\"item in data | SmartFilter:vm.mergedFilter()\" data=\"item\"></terrama2-list-item>" +
                  "</ul>",
        link: linkFn
      };
      /**
       * It checks if it has parent and then add it as child in order to keep three level
       * @param {angular.IScope} scope - Directive scope
       * @param {angular.IElement} element - Directive selector
       * @param {angular.IAttributes} attrs - Directive selector attributes
       * @param {terrama2ListController} ctrl - Parent controller
       */
      function linkFn(scope, element, attrs, ctrl) {
        if (ctrl) {
          ctrl.addChild(scope);
        }
      }
      /**
       * It handles Directive behavior. It is used in children directives.
       * @class terrama2ListController
       */
      function terrama2ListController($scope) {
        var self = this;
        // defining default level if there is not. It is used only first iteration
        if (!$scope.level) {
          $scope.level = 0;
        }
        // children scopes cache
        var items = [];
        /**
         * Add a new child to cache
         * @param {angular.IScope} childScope - Directive child scope
         */
        this.addChild = function(childScope) {
          items.push(childScope);
        };
        /**
         * It gives a fullify object to filter
         * @returns {Object}
         */
        this.mergedFilter = function() {
          var output = angular.merge({}, self.getExpression());
          angular.merge(output, {$depth: self.getLevel()});
          return output;
        };
        /**
         * It retrieves a current expression validation
         * @returns {any}
         */
        this.getExpression = function() {
          return $scope.expression;
        };
        /**
         * Get current level of iteration
         * 
         * @return {number}
         */
        this.getLevel = function() {
          return $scope.level;
        };
      } // end terrama2ListController
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
        require: ["^terrama2List", "?terrama2ListItem"],
        scope: {
          data: "="
        },
        controller: ["$scope", terrama2ListItemController],
        controllerAs: "vm",
        template: "<li>" +
                    "<a href=\"javascript::void()\" ng-click=\"onClick(data)\" data-toggle=\"tooltip\" data-placement=\"top\" ng-attr-title=\"{{data.description}}\">{{ data.name }}</a>" +
                  "</li>",
        link: linkFn
      };

      function terrama2ListItemController($scope) {
        /**
         * Defines a list of child scopes
         * @type {angular.IScope[]}
         */
        var items = [];
        /**
         * Add a new child in the context.
         * 
         * @param {angular.IScope} childScope - Angular scope
         */
        this.addChild = function(childScope) {
          items.push(childScope);
        };
      } // end terrama2ListController

      /**
       * It handles directive post link. Used to identify if current scoped data has children element. If there is, auto-call parent
       * (terrama2-list) again giving array. 
       * 
       * @todo Remove harded-code used in style (dropdown-menu)
       * 
       * @param {angular.IScope}      scope - Angular Directive scope
       * @param {angular.IElement}    element - Directive Selector (jQlite)
       * @param {angular.IAttributes} attrs - Directive Scope Attributes 
       */
      function linkFn(scope, element, attrs, ctrls) {
        if (!scope.data) {
          return;
        }

        var selfCtrl = ctrls[1];
        var parentCtrl = ctrls[0];

        parentCtrl.addChild(scope);

        /**
         * Adding dynamically get level based parent
         * 
         * @returns {number} Current level
         */
        selfCtrl.getLevel = function() {
          return parentCtrl.getLevel() + 1;
        }

        selfCtrl.getExpression = function() {
          return parentCtrl.getExpression();
        };

        /**
         * Event handler that emit itemClicked to parent in order to detect scope clicked
         * 
         * @param {any} item - Select item
         */
        scope.onClick = function(item) {
          scope.$emit("itemClicked", item);
        };

        if (angular.isArray(scope.data.children)) {
          element.addClass("dropdown-submenu");

          $compile("<terrama2-list class=\"dropdown-menu\" data=\"data.children\" expression=\"vm.getExpression()\" level=\"vm.getLevel()\"></terrama2-list>")(scope, function(cloned, scope) {
            element.append(cloned);
          });
        }
      }
    }

  return moduleName;
});