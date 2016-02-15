module.exports = (gulp, plugins, paths)=> {
	console.log('paths.src.scripts', paths.src.scripts);
	console.log('paths.dest.scripts.name', paths.dest.scripts.name);
	return ()=> {
		return gulp.src(paths.src.scripts)
			.pipe(plugins.debug())
			.pipe(plugins.jshint())
			.pipe(plugins.jshint.reporter('jshint-stylish'))
			.pipe(plugins.concat(paths.dest.scripts.name))
			.pipe(plugins.uglify())
			.pipe(gulp.dest(paths.dest.scripts.dir));
	}
};