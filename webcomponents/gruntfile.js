module.exports = function(grunt) {

  // Project configuration
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    banner: '/*! <%= pkg.name %> <%= pkg.version %> | (C) 2007, <%= grunt.template.today("yyyy") %> National Institute For Space Research (INPE) - Brazil | https://github.com/TerraMA2/terrama2/blob/master/LICENSE */',
    requirejs: {
      TerraMA2WebComponents: {
        options: {
          baseUrl: "javascripts",
          out: "dist/TerraMA2WebComponents.min.js",
          preserveLicenseComments: false,
          paths: {
            TerraMA2WebComponentsPath: "src"
          },
          include: [
            'externals/almond',
            'TerraMA2WebComponentsPath/TerraMA2WebComponents',
            'TerraMA2WebComponentsPath/components/LayerExplorer.TerraMA2WebComponents',
            'TerraMA2WebComponentsPath/components/MapDisplay.TerraMA2WebComponents'
          ],
          wrap: {
            startFile: "javascripts/Wrap.TerraMA2WebComponents.start",
            endFile: "javascripts/Wrap.TerraMA2WebComponents.end"
          }
        }
      }
    },
    cssmin: {
      TerraMA2WebComponents: {
        files: [{
          expand: true,
          cwd: 'stylesheets',
          src: ['*.css'],
          dest: 'dist',
          ext: '.min.css'
        }]
      }
    },
    usebanner: {
      TerraMA2WebComponents: {
        options: {
          position: 'top',
          banner: '<%= banner %>',
          linebreak: true
        },
        files: {
          src: [ 'dist/*' ]
        }
      }
    },
    jsdoc : {
      TerraMA2WebComponents: {
        options: {
          configure: 'jsdoc/config.json'
        }
      }
    }
  });

  // Load the plugins.
  grunt.loadNpmTasks('grunt-contrib-requirejs');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-banner');
  grunt.loadNpmTasks('grunt-jsdoc');

  // Default tasks.
  grunt.registerTask('default', ['requirejs', 'cssmin', 'usebanner:TerraMA2WebComponents', 'jsdoc']);

};
