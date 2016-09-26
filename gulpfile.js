var gulp    = require('gulp');
var plugins = require('gulp-load-plugins')();
var paths = require('./gulp/paths.json');

gulp.on("error", function (err) { console.log("Oops: %s", err);});

// Concat & Minify & Lint Our JS
gulp.task('scripts', require('./gulp/tasks/scripts')(gulp, plugins, paths));

gulp.task('default', ['build']);
gulp.task('build', ['scripts']);