module.exports = function(grunt) {

  // Project configuration
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    banner: '/*! <%= pkg.name %> <%= pkg.version %> | (C) 2007, <%= grunt.template.today("yyyy") %> National Institute For Space Research (INPE) - Brazil | https://github.com/TerraMA2/terrama2/blob/master/LICENSE */',
    requirejs: {
      TerraMA2WebComponents: {
        options: {
          baseUrl: "javascripts",
          optimize: "none", // It does not minify
          out: function(text, sourceMapText) {
            var UglifyJS = require('uglify-es');
            uglified = UglifyJS.minify(text, { sourceMap: { content: sourceMapText, url: "/dist/TerraMA2WebComponents.min.js" } });

            grunt.file.write("dist/TerraMA2WebComponents.min.js", text /*uglified.code*/);
            grunt.file.write("dist/TerraMA2WebComponents.min.js.map", uglified.map);
          },
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
    copy: {
      files: {
        cwd: 'images',
        src: '**/*',
        dest: 'dist/images',
        expand: true
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
        src: ['README.md'],
        options: {
          configure: 'jsdoc/config.json',
          destination: 'docs'
        }
      }
    }
  });

  // Load the plugins.
  grunt.loadNpmTasks('grunt-contrib-requirejs');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-banner');
  grunt.loadNpmTasks('grunt-jsdoc');

  // Default tasks.
  grunt.registerTask('default', ['requirejs', 'cssmin', 'copy', 'usebanner:TerraMA2WebComponents', 'jsdoc']);

};
